#include "academic_os.h"
#include <stdarg.h>

#ifdef _WIN32
    static DWORD original_console_mode = 0;
    static UINT original_cp = 0;
    static UINT original_output_cp = 0;
#endif

static UITheme current_theme = THEME_CYAN_TECH;

/* Theme Color Palettes */
static const ColorPalette palettes[] = {
    /* THEME_CYAN_TECH */
    {
        .primary     = ANSI_BRIGHT_CYAN,
        .secondary   = ANSI_CYAN,
        .accent      = ANSI_BRIGHT_BLUE,
        .success     = ANSI_BRIGHT_GREEN,
        .warning     = ANSI_BRIGHT_YELLOW,
        .danger      = ANSI_BRIGHT_RED,
        .text_bright = ANSI_BRIGHT_WHITE,
        .text_dim    = ANSI_GRAY
    },
    /* THEME_EMERALD_MATRIX */
    {
        .primary     = ANSI_BRIGHT_GREEN,
        .secondary   = ANSI_GREEN,
        .accent      = ANSI_CYAN,
        .success     = ANSI_BRIGHT_GREEN,
        .warning     = ANSI_BRIGHT_YELLOW,
        .danger      = ANSI_BRIGHT_RED,
        .text_bright = ANSI_BRIGHT_WHITE,
        .text_dim    = ANSI_GRAY
    },
    /* THEME_AMBER_WARM */
    {
        .primary     = ANSI_BRIGHT_YELLOW,
        .secondary   = ANSI_YELLOW,
        .accent      = ANSI_BRIGHT_MAGENTA,
        .success     = ANSI_BRIGHT_GREEN,
        .warning     = ANSI_YELLOW,
        .danger      = ANSI_BRIGHT_RED,
        .text_bright = ANSI_BRIGHT_WHITE,
        .text_dim    = ANSI_GRAY
    },
    /* THEME_MONOCHROME */
    {
        .primary     = ANSI_BRIGHT_WHITE,
        .secondary   = ANSI_WHITE,
        .accent      = ANSI_WHITE,
        .success     = ANSI_BRIGHT_WHITE,
        .warning     = ANSI_WHITE,
        .danger      = ANSI_WHITE,
        .text_bright = ANSI_BRIGHT_WHITE,
        .text_dim    = ANSI_GRAY
    }
};

void platform_init_terminal(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        GetConsoleMode(hOut, &original_console_mode);
        DWORD mode = original_console_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, mode);
    }
    original_cp = GetConsoleCP();
    original_output_cp = GetConsoleOutputCP();
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void platform_cleanup_terminal(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE && original_console_mode != 0) {
        SetConsoleMode(hOut, original_console_mode);
    }
    if (original_cp != 0) SetConsoleCP(original_cp);
    if (original_output_cp != 0) SetConsoleOutputCP(original_output_cp);
#endif
    printf("%s", ANSI_RESET);
}

void clear_terminal(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[H\033[J");
    fflush(stdout);
#endif
}

bool platform_kbhit(void) {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
#endif
}

int platform_getch(void) {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

void platform_sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void ui_init(void) {
    platform_init_terminal();
}

const ColorPalette* ui_get_palette(void) {
    return &palettes[current_theme];
}

void ui_set_theme(int theme_id) {
    if (theme_id >= 0 && theme_id <= 3) {
        current_theme = (UITheme)theme_id;
    }
}

void ui_clear_screen(void) {
    clear_terminal();
}

void ui_move_cursor(int row, int col) {
    printf("\033[%d;%dH", row, col);
}

void ui_hide_cursor(void) {
    printf("\033[?25l");
    fflush(stdout);
}

void ui_show_cursor(void) {
    printf("\033[?25h");
    fflush(stdout);
}

/* Helper to calculate visible display width of a UTF-8 string (ignores ANSI codes) */
int ui_calc_visible_width(const char* str) {
    int len = 0;
    bool in_escape = false;
    for (size_t i = 0; str[i] != '\0';) {
        if (str[i] == '\033') {
            in_escape = true;
            i++;
        } else if (in_escape) {
            if (str[i] == 'm' || str[i] == 'H' || str[i] == 'J') {
                in_escape = false;
            }
            i++;
        } else {
            unsigned char c = (unsigned char)str[i];
            if (c < 0x80) {
                len++;
                i++;
            } else if ((c & 0xE0) == 0xC0) {
                len++;
                i += 2;
            } else if ((c & 0xF0) == 0xE0) {
                /* 3-byte UTF-8, box characters are 1 terminal column wide */
                len++;
                i += 3;
            } else if ((c & 0xF8) == 0xF0) {
                len += 2; /* wide emoji */
                i += 4;
            } else {
                i++;
            }
        }
    }
    return len;
}

void ui_pad_to_visible_width(char* dest, size_t dest_size, const char* src, int target_width) {
    snprintf(dest, dest_size, "%s", src);
    int current_w = ui_calc_visible_width(dest);
    int needed = target_width - current_w;
    while (needed > 0 && strlen(dest) + 1 < dest_size) {
        strcat(dest, " ");
        needed--;
    }
}

void ui_draw_d_top(int width) {
    const ColorPalette* pal = ui_get_palette();
    printf("%s%s", pal->primary, BOX_D_TL);
    for (int i = 0; i < width - 2; i++) {
        printf("%s", BOX_D_H);
    }
    printf("%s%s\n", BOX_D_TR, ANSI_RESET);
}

void ui_draw_d_sep(int width) {
    const ColorPalette* pal = ui_get_palette();
    printf("%s%s", pal->primary, BOX_D_LT);
    for (int i = 0; i < width - 2; i++) {
        printf("%s", BOX_D_H);
    }
    printf("%s%s\n", BOX_D_RT, ANSI_RESET);
}

void ui_draw_d_bottom(int width) {
    const ColorPalette* pal = ui_get_palette();
    printf("%s%s", pal->primary, BOX_D_BL);
    for (int i = 0; i < width - 2; i++) {
        printf("%s", BOX_D_H);
    }
    printf("%s%s\n", BOX_D_BR, ANSI_RESET);
}

void ui_draw_d_line(int width, const char* text) {
    ui_draw_d_line_aligned(width, text, -1);
}

void ui_draw_d_line_aligned(int width, const char* text, int align) {
    const ColorPalette* pal = ui_get_palette();
    int inner_w = width - 4; /* Accounting for borders and 1 space each side */
    int text_w = ui_calc_visible_width(text);
    int pad_left = 1;
    int pad_right = 1;

    if (text_w > inner_w) {
        /* Truncate logically if needed */
        inner_w = text_w;
    }

    if (align == 0) { /* Center */
        int extra = inner_w - text_w;
        pad_left = 1 + (extra / 2);
        pad_right = 1 + (extra - (extra / 2));
    } else if (align == 1) { /* Right */
        int extra = inner_w - text_w;
        pad_left = 1 + extra;
        pad_right = 1;
    } else { /* Left */
        pad_left = 1;
        pad_right = 1 + (inner_w - text_w);
    }

    printf("%s%s%s", pal->primary, BOX_D_V, ANSI_RESET);
    for (int i = 0; i < pad_left; i++) putchar(' ');
    printf("%s", text);
    for (int i = 0; i < pad_right; i++) putchar(' ');
    printf("%s%s%s\n", pal->primary, BOX_D_V, ANSI_RESET);
}

void ui_draw_s_top(int width, const char* title) {
    const ColorPalette* pal = ui_get_palette();
    printf("%s%s", pal->secondary, BOX_S_TL);
    if (title && strlen(title) > 0) {
        int title_len = ui_calc_visible_width(title);
        int rem = width - 4 - title_len;
        int left_bars = rem / 2;
        int right_bars = rem - left_bars;
        for (int i = 0; i < left_bars; i++) printf("%s", BOX_S_H);
        printf(" %s%s%s ", pal->text_bright, title, pal->secondary);
        for (int i = 0; i < right_bars; i++) printf("%s", BOX_S_H);
    } else {
        for (int i = 0; i < width - 2; i++) printf("%s", BOX_S_H);
    }
    printf("%s%s\n", BOX_S_TR, ANSI_RESET);
}

void ui_draw_s_sep(int width) {
    const ColorPalette* pal = ui_get_palette();
    printf("%s%s", pal->secondary, BOX_S_LT);
    for (int i = 0; i < width - 2; i++) printf("%s", BOX_S_H);
    printf("%s%s\n", BOX_S_RT, ANSI_RESET);
}

void ui_draw_s_bottom(int width) {
    const ColorPalette* pal = ui_get_palette();
    printf("%s%s", pal->secondary, BOX_S_BL);
    for (int i = 0; i < width - 2; i++) printf("%s", BOX_S_H);
    printf("%s%s\n", BOX_S_BR, ANSI_RESET);
}

void ui_draw_s_line(int width, const char* text) {
    const ColorPalette* pal = ui_get_palette();
    int inner_w = width - 4;
    int text_w = ui_calc_visible_width(text);
    int pad_right = inner_w - text_w;
    if (pad_right < 0) pad_right = 0;

    printf("%s%s%s %s", pal->secondary, BOX_S_V, ANSI_RESET, text);
    for (int i = 0; i < pad_right + 1; i++) putchar(' ');
    printf("%s%s%s\n", pal->secondary, BOX_S_V, ANSI_RESET);
}

void ui_make_progress_bar(char* out_buf, size_t buf_len, float percentage, int bar_width) {
    if (percentage < 0.0f) percentage = 0.0f;
    if (percentage > 100.0f) percentage = 100.0f;

    int filled = (int)((percentage / 100.0f) * bar_width);
    if (filled > bar_width) filled = bar_width;

    char bar_str[128] = "";
    strcat(bar_str, "[");
    for (int i = 0; i < filled; i++) {
        strcat(bar_str, "=");
    }
    for (int i = filled; i < bar_width; i++) {
        strcat(bar_str, ".");
    }
    strcat(bar_str, "]");

    snprintf(out_buf, buf_len, "%s %3.0f%%", bar_str, percentage);
}

bool ui_input_string(const char* prompt, char* dest, size_t max_len, const char* default_val) {
    const ColorPalette* pal = ui_get_palette();
    char buffer[256];

    if (default_val && strlen(default_val) > 0) {
        printf("%s%s [%s%s%s]: %s", pal->primary, prompt, pal->text_dim, default_val, pal->primary, pal->text_bright);
    } else {
        printf("%s%s: %s", pal->primary, prompt, pal->text_bright);
    }

    if (!fgets(buffer, sizeof(buffer), stdin)) {
        printf("%s\n", ANSI_RESET);
        return false;
    }
    printf("%s", ANSI_RESET);

    /* Strip newline */
    size_t len = strlen(buffer);
    if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
        buffer[len - 1] = '\0';
    }
    if (len > 1 && buffer[len - 2] == '\r') {
        buffer[len - 2] = '\0';
    }

    if (strlen(buffer) == 0 && default_val) {
        strncpy(dest, default_val, max_len - 1);
        dest[max_len - 1] = '\0';
    } else {
        strncpy(dest, buffer, max_len - 1);
        dest[max_len - 1] = '\0';
    }
    return true;
}

int ui_input_int(const char* prompt, int min_val, int max_val, int default_val) {
    char default_str[32];
    char dest[32];
    snprintf(default_str, sizeof(default_str), "%d", default_val);

    while (true) {
        if (!ui_input_string(prompt, dest, sizeof(dest), default_str)) {
            return default_val;
        }
        int val = atoi(dest);
        if (val >= min_val && val <= max_val) {
            return val;
        }
        printf("%sError: Value must be between %d and %d.%s\n", ANSI_RED, min_val, max_val, ANSI_RESET);
    }
}

float ui_input_float(const char* prompt, float min_val, float max_val, float default_val) {
    char default_str[32];
    char dest[32];
    snprintf(default_str, sizeof(default_str), "%.2f", default_val);

    while (true) {
        if (!ui_input_string(prompt, dest, sizeof(dest), default_str)) {
            return default_val;
        }
        float val = (float)atof(dest);
        if (val >= min_val && val <= max_val) {
            return val;
        }
        printf("%sError: Value must be between %.2f and %.2f.%s\n", ANSI_RED, min_val, max_val, ANSI_RESET);
    }
}

bool ui_input_bool(const char* prompt, bool default_val) {
    char dest[16];
    const char* def_str = default_val ? "Y" : "N";
    ui_input_string(prompt, dest, sizeof(dest), def_str);
    if (dest[0] == 'y' || dest[0] == 'Y' || dest[0] == '1') return true;
    if (dest[0] == 'n' || dest[0] == 'N' || dest[0] == '0') return false;
    return default_val;
}

void ui_pause_key(void) {
    const ColorPalette* pal = ui_get_palette();
    printf("\n%sPress ENTER to continue...%s", pal->text_dim, ANSI_RESET);
    while (getchar() != '\n');
}

void ui_print_badge(const char* text, const char* color) {
    printf("%s[%s]%s", color, text, ANSI_RESET);
}

void ui_print_header(const char* title, const char* subtitle) {
    const ColorPalette* pal = ui_get_palette();
    ui_clear_screen();
    ui_draw_d_top(UI_MAIN_WIDTH);
    
    char title_buf[128];
    snprintf(title_buf, sizeof(title_buf), "%s%s%s", pal->primary, title, ANSI_RESET);
    ui_draw_d_line_aligned(UI_MAIN_WIDTH, title_buf, 0);

    if (subtitle && strlen(subtitle) > 0) {
        char sub_buf[128];
        snprintf(sub_buf, sizeof(sub_buf), "%s%s%s", pal->text_dim, subtitle, ANSI_RESET);
        ui_draw_d_line_aligned(UI_MAIN_WIDTH, sub_buf, 0);
    }
    ui_draw_d_sep(UI_MAIN_WIDTH);
}
