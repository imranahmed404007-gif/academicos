#ifndef ACADEMIC_OS_H
#define ACADEMIC_OS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <conio.h>
    #include <io.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <sys/select.h>
#endif

#include "models.h"

/* ========================================================================= */
/* 1. CONSTANTS & ANSI COLOR CODES                                           */
/* ========================================================================= */

#define ANSI_RESET        "\033[0m"
#define ANSI_BOLD         "\033[1m"
#define ANSI_DIM          "\033[2m"
#define ANSI_UNDERLINE    "\033[4m"

#define ANSI_BLACK        "\033[30m"
#define ANSI_RED          "\033[31m"
#define ANSI_GREEN        "\033[32m"
#define ANSI_YELLOW       "\033[33m"
#define ANSI_BLUE         "\033[34m"
#define ANSI_MAGENTA      "\033[35m"
#define ANSI_CYAN         "\033[36m"
#define ANSI_WHITE        "\033[37m"
#define ANSI_GRAY         "\033[90m"

#define ANSI_BRIGHT_RED   "\033[91m"
#define ANSI_BRIGHT_GREEN "\033[92m"
#define ANSI_BRIGHT_YELLOW "\033[93m"
#define ANSI_BRIGHT_BLUE  "\033[94m"
#define ANSI_BRIGHT_MAGENTA "\033[95m"
#define ANSI_BRIGHT_CYAN  "\033[96m"
#define ANSI_BRIGHT_WHITE "\033[97m"

/* Double-line Box Drawing Characters */
#define BOX_D_TL    "╔"
#define BOX_D_TR    "╗"
#define BOX_D_BL    "╚"
#define BOX_D_BR    "╝"
#define BOX_D_H     "═"
#define BOX_D_V     "║"
#define BOX_D_LT    "╠"
#define BOX_D_RT    "╣"
#define BOX_D_TT    "╦"
#define BOX_D_BT    "╩"
#define BOX_D_CROSS "╬"

/* Single-line Box Drawing Characters */
#define BOX_S_TL    "┌"
#define BOX_S_TR    "┐"
#define BOX_S_BL    "└"
#define BOX_S_BR    "┘"
#define BOX_S_H     "─"
#define BOX_S_V     "│"
#define BOX_S_LT    "├"
#define BOX_S_RT    "┤"
#define BOX_S_TT    "┬"
#define BOX_S_BT    "┴"
#define BOX_S_CROSS "┼"

/* Tree View Symbols */
#define TREE_BRANCH " ├── "
#define TREE_CORNER " └── "
#define TREE_PIPE   " │   "
#define TREE_SPACE  "     "

/* UI Frame Widths */
#define UI_MAIN_WIDTH    50
#define UI_INNER_WIDTH   44
#define DASHBOARD_WIDTH  48

/* ========================================================================= */
/* 2. UI & TERMINAL DEFINITIONS                                              */
/* ========================================================================= */

typedef enum {
    THEME_CYAN_TECH = 0,
    THEME_EMERALD_MATRIX,
    THEME_AMBER_WARM,
    THEME_MONOCHROME
} UITheme;

typedef struct {
    const char* primary;
    const char* secondary;
    const char* accent;
    const char* success;
    const char* warning;
    const char* danger;
    const char* text_bright;
    const char* text_dim;
} ColorPalette;

/* Platform terminal helpers */
void platform_init_terminal(void);
void platform_cleanup_terminal(void);
void clear_terminal(void);
bool platform_kbhit(void);
int  platform_getch(void);
void platform_sleep_ms(int ms);

/* UI & Drawing Functions */
void ui_init(void);
const ColorPalette* ui_get_palette(void);
void ui_set_theme(int theme_id);
void ui_clear_screen(void);
void ui_move_cursor(int row, int col);
void ui_hide_cursor(void);
void ui_show_cursor(void);

int  ui_calc_visible_width(const char* str);
void ui_pad_to_visible_width(char* dest, size_t dest_size, const char* src, int target_width);

void ui_draw_d_top(int width);
void ui_draw_d_sep(int width);
void ui_draw_d_bottom(int width);
void ui_draw_d_line(int width, const char* text);
void ui_draw_d_line_aligned(int width, const char* text, int align);

void ui_draw_s_top(int width, const char* title);
void ui_draw_s_sep(int width);
void ui_draw_s_bottom(int width);
void ui_draw_s_line(int width, const char* text);

void ui_make_progress_bar(char* out_buf, size_t buf_len, float percentage, int bar_width);
bool ui_input_string(const char* prompt, char* dest, size_t max_len, const char* default_val);
int  ui_input_int(const char* prompt, int min_val, int max_val, int default_val);
float ui_input_float(const char* prompt, float min_val, float max_val, float default_val);
bool ui_input_bool(const char* prompt, bool default_val);
void ui_pause_key(void);
void ui_print_badge(const char* text, const char* color);
void ui_print_header(const char* title, const char* subtitle);

/* ========================================================================= */
/* 3. TIME ENGINE DEFINITIONS                                                */
/* ========================================================================= */

typedef enum {
    HERO_STATUS_RUNNING,
    HERO_STATUS_UPCOMING,
    HERO_STATUS_ALL_COMPLETED_TODAY,
    HERO_STATUS_NO_CLASSES_TODAY,
    HERO_STATUS_NO_SCHEDULE
} HeroClassStatus;

typedef struct {
    HeroClassStatus status;
    RoutineClass current_class;
    float progress_percentage;
    long remaining_seconds;
    char formatted_remaining[STR_SHORT_LEN];
    char start_formatted[STR_SHORT_LEN];
    char end_formatted[STR_SHORT_LEN];
    char day_name[STR_SHORT_LEN];
    bool is_today;
} HeroClassState;

typedef struct {
    bool has_exam;
    Exam exam;
    long days;
    long hours;
    long minutes;
    long seconds;
    long total_diff_seconds;
    char formatted_countdown[STR_MED_LEN];
    char formatted_datetime[STR_MED_LEN];
} NextExamState;

typedef struct {
    int classes_today_count;
    int upcoming_exams_count;
    int total_courses_count;
    int completed_topics_count;
    int total_topics_count;
} DashboardStats;

struct tm time_get_localtime(void);
void time_engine_set_simulated_time(int wday, int day, int month, int year, int hour, int minute, int sec);
void time_engine_clear_simulated_time(void);
bool time_engine_is_simulated(void);

void time_format_date_banner(char* buffer, size_t max_len, const struct tm* timeinfo);
void time_format_clock(char* buffer, size_t max_len, const struct tm* timeinfo, bool is_24_hour, bool show_seconds);
void time_format_time_val(char* buffer, size_t max_len, int hour, int minute, bool is_24_hour);

void time_engine_compute_hero_class(const AppState* state, const struct tm* now_tm, HeroClassState* out_hero);
void time_engine_compute_next_exam(const AppState* state, const struct tm* now_tm, NextExamState* out_next_exam);
void time_engine_compute_stats(const AppState* state, const struct tm* now_tm, DashboardStats* out_stats);
long time_engine_calc_exam_diff(const Exam* exam, const struct tm* now_tm);
void time_engine_format_countdown_str(long diff_seconds, int style, char* buffer, size_t max_len);

/* ========================================================================= */
/* 4. STORAGE & PERSISTENCE DEFINITIONS                                      */
/* ========================================================================= */

bool storage_init_all(void);
bool storage_save_all(void);
bool storage_save_courses(void);
bool storage_load_courses(void);
bool storage_save_routine(void);
bool storage_load_routine(void);
bool storage_save_exams(void);
bool storage_load_exams(void);
bool storage_save_syllabus(void);
bool storage_load_syllabus(void);
bool storage_save_grades(void);
bool storage_load_grades(void);
bool storage_save_settings(void);
bool storage_load_settings(void);
void storage_reset_kuet_defaults(void);

/* ========================================================================= */
/* 5. DASHBOARD & MENUS                                                      */
/* ========================================================================= */

/* Dashboard */
void dashboard_render(const AppState* state);
void dashboard_live_mode(AppState* state);
int  dashboard_get_todays_classes(const AppState* state, const struct tm* now_tm, RoutineClass* out_classes, int max_out);
void dashboard_review_class_topics(const AppState* state, const RoutineClass* rc);
void dashboard_review_menu(AppState* state);

/* Routine */
void routine_menu(AppState* state);
void routine_view_today(const AppState* state);
void routine_view_week(const AppState* state);
void routine_view_by_day(const AppState* state);
void routine_add_class(AppState* state);
void routine_delete_class(AppState* state);

/* Courses */
void courses_menu(AppState* state);
void courses_view_all(const AppState* state);
void courses_view_detail(const AppState* state);
void courses_display_detailed_plan(const AppState* state, const char* course_code, bool is_pre_mid);
void courses_add(AppState* state);
void courses_delete(AppState* state);
const Course* courses_find_by_code(const AppState* state, const char* code);
float courses_calculate_total_credits(const AppState* state);

/* Exams */
void exams_menu(AppState* state);
void exams_view_upcoming(const AppState* state);
void exams_view_all(const AppState* state);
void exams_view_term_finals(const AppState* state);
void exams_add(AppState* state);
void exams_delete(AppState* state);

/* Syllabus */
void syllabus_menu(AppState* state);
void syllabus_browse_tree(const AppState* state);
void syllabus_render_tree(const AppState* state, const char* course_code);
void syllabus_toggle_subtopic(AppState* state);
void syllabus_view_code_snippet(const AppState* state);
void syllabus_show_completion_summary(const AppState* state);

/* Utilities: Search, Settings, GPA & Statistics */
void search_menu(const AppState* state);
void search_execute(const AppState* state, const char* query);

void settings_menu(AppState* state);
void settings_toggle_clock_format(AppState* state);
void settings_change_countdown_style(AppState* state);
void settings_change_color_theme(AppState* state);
void settings_toggle_seconds(AppState* state);
void settings_reset_defaults(AppState* state);

void stats_menu(AppState* state);
void stats_marks_to_kuet_grade(float marks, char* out_letter, float* out_point);
void stats_view_gpa_report(const AppState* state);
void stats_view_distribution_chart(const AppState* state);
void stats_manage_grades(AppState* state);
void stats_target_cgpa_simulator(const AppState* state);

#endif /* ACADEMIC_OS_H */
