#include "academic_os.h"

static bool get_dashboard_choice(const AppState* state, char* out_buf, size_t max_len) {
#ifdef _WIN32
    if (!_isatty(_fileno(stdin))) {
        clear_terminal();
        dashboard_render(state);
        printf("\nEnter choice: ");
        fflush(stdout);
        if (!fgets(out_buf, (int)max_len, stdin)) return false;
        return true;
    }
#else
    if (!isatty(fileno(stdin))) {
        clear_terminal();
        dashboard_render(state);
        printf("\nEnter choice: ");
        fflush(stdout);
        if (!fgets(out_buf, (int)max_len, stdin)) return false;
        return true;
    }
#endif

    size_t len = 0;
    out_buf[0] = '\0';

    clear_terminal();
    ui_move_cursor(1, 1);
    dashboard_render(state);
    printf("\nEnter choice: ");
    fflush(stdout);

    struct tm last_tm = time_get_localtime();

    while (1) {
        struct tm now_tm = time_get_localtime();
        if (now_tm.tm_sec != last_tm.tm_sec) {
            last_tm = now_tm;
            ui_move_cursor(1, 1);
            dashboard_render(state);
            printf("\nEnter choice: %s\033[K", out_buf);
            fflush(stdout);
        }

        if (platform_kbhit()) {
            int ch = platform_getch();
            if (ch == 0 || ch == 224) {
                platform_getch();
                continue;
            }

            if (ch == '\r' || ch == '\n') {
                out_buf[len] = '\0';
                putchar('\n');
                return true;
            } else if (ch == '\b' || ch == 127) {
                if (len > 0) {
                    len--;
                    out_buf[len] = '\0';
                    printf("\b \b");
                    fflush(stdout);
                }
            } else if (ch == 3) { /* Ctrl+C */
                return false;
            } else if (ch >= 32 && ch <= 126) {
                if (len < max_len - 1) {
                    out_buf[len++] = (char)ch;
                    out_buf[len] = '\0';
                    putchar(ch);
                    fflush(stdout);
                }
            }
        }

        platform_sleep_ms(50);
    }
}

int main(int argc, char* argv[]) {
    /* Initialize terminal environment (UTF-8, VT100 / ANSI escape sequences) */
    ui_init();

    /* Load persistent binary state or seed default authentic KUET CSE dataset */
    storage_init_all();
    AppState* state = get_app_state();

    /* Automatically upgrade to full authentic 39-lecture dataset if older partial dataset detected */
    if (state->syllabus_count < 20) {
        storage_reset_kuet_defaults();
    }

    /* CLI flags: reset or demo */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--reset") == 0 || strcmp(argv[i], "-r") == 0) {
            storage_reset_kuet_defaults();
        } else if (strcmp(argv[i], "--demo") == 0 || strcmp(argv[i], "--demo-running") == 0 || strcmp(argv[i], "-d") == 0) {
            time_engine_set_simulated_time(0, 27, 9, 2026, 9, 45, 15);
        }
    }

    ui_set_theme(state->settings.color_theme);

    bool running = true;
    while (running) {
        char choice_str[32];
        if (!get_dashboard_choice(state, choice_str, sizeof(choice_str))) {
            break;
        }

        /* Clean whitespace */
        char* p = choice_str;
        while (*p == ' ' || *p == '\t') p++;

        int choice = -1;
        if (isdigit((unsigned char)*p)) {
            choice = atoi(p);
        } else if (*p == 'q' || *p == 'Q') {
            choice = 0;
        } else if (*p == 'r' || *p == 'R' || *p == 't' || *p == 'T') {
            if (*(p + 1) >= '1' && *(p + 1) <= '9') {
                int r_idx = *(p + 1) - '1';
                struct tm now_tm = time_get_localtime();
                RoutineClass today_classes[MAX_CLASSES];
                int today_count = dashboard_get_todays_classes(state, &now_tm, today_classes, MAX_CLASSES);
                if (r_idx >= 0 && r_idx < today_count) {
                    dashboard_review_class_topics(state, &today_classes[r_idx]);
                }
                continue;
            } else {
                dashboard_review_menu(state);
                continue;
            }
        }

        switch (choice) {
            case 1:
                routine_menu(state);
                break;
            case 2:
                exams_menu(state);
                break;
            case 3:
                courses_menu(state);
                break;
            case 4:
                stats_menu(state);
                break;
            case 5:
                search_menu(state);
                break;
            case 6:
            case 7:
            case 8: /* Support backward compatibility */
                settings_menu(state);
                ui_set_theme(state->settings.color_theme);
                break;
            case 0:
                running = false;
                break;
            default:
                /* Simply redraw or ignore invalid */
                break;
        }
    }

    /* Save on exit */
    storage_save_all();

    clear_terminal();
    const ColorPalette* pal = ui_get_palette();
    printf("\n%s╔══════════════════════════════════════════════╗%s\n", pal->primary, ANSI_RESET);
    printf("  %sACADEMIC OS - KUET CSE%s\n", pal->text_bright, ANSI_RESET);
    printf("  %sAll records and settings saved successfully.%s\n", pal->secondary, ANSI_RESET);
    printf("  %sHave a productive study session!%s\n", pal->text_dim, ANSI_RESET);
    printf("%s╚══════════════════════════════════════════════╝%s\n\n", pal->primary, ANSI_RESET);

    platform_cleanup_terminal();
    return 0;
}
