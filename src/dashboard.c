#include "academic_os.h"


int dashboard_get_todays_classes(const AppState* state, const struct tm* now_tm, RoutineClass* out_classes, int max_out) {
    int current_wday = (now_tm->tm_wday >= 0 && now_tm->tm_wday < 7) ? now_tm->tm_wday : 0;
    int count = 0;

    for (int i = 0; i < state->class_count && count < max_out; i++) {
        const RoutineClass* rc = &state->classes[i];
        if (rc->is_enabled && rc->day_of_week == current_wday) {
            out_classes[count++] = *rc;
        }
    }

    /* Sort chronologically by start time */
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            int t1 = out_classes[i].start_hour * 60 + out_classes[i].start_minute;
            int t2 = out_classes[j].start_hour * 60 + out_classes[j].start_minute;
            if (t1 > t2) {
                RoutineClass temp = out_classes[i];
                out_classes[i] = out_classes[j];
                out_classes[j] = temp;
            }
        }
    }

    return count;
}

void dashboard_render(const AppState* state) {
    const ColorPalette* pal = ui_get_palette();
    struct tm now_tm = time_get_localtime();

    char date_str[64];
    time_format_date_banner(date_str, sizeof(date_str), &now_tm);

    char clock_str[32];
    time_format_clock(clock_str, sizeof(clock_str), &now_tm, state->settings.is_24_hour, state->settings.show_seconds);

    HeroClassState hero;
    time_engine_compute_hero_class(state, &now_tm, &hero);

    NextExamState next_exam;
    time_engine_compute_next_exam(state, &now_tm, &next_exam);

    DashboardStats stats;
    time_engine_compute_stats(state, &now_tm, &stats);

    /* Render Top Header */
    ui_draw_d_top(DASHBOARD_WIDTH);

    char title_buf[64];
    snprintf(title_buf, sizeof(title_buf), "%s%sACADEMIC OS%s", ANSI_BOLD, pal->primary, ANSI_RESET);
    ui_draw_d_line_aligned(DASHBOARD_WIDTH, title_buf, 0);

    char sub_buf[64];
    snprintf(sub_buf, sizeof(sub_buf), "%s%sKUET CSE%s", pal->text_bright, pal->secondary, ANSI_RESET);
    ui_draw_d_line_aligned(DASHBOARD_WIDTH, sub_buf, 0);

    ui_draw_d_sep(DASHBOARD_WIDTH);
    ui_draw_d_line(DASHBOARD_WIDTH, "");

    /* Date and Live Time */
    char line_buf[256];
    snprintf(line_buf, sizeof(line_buf), " %s%s%s", pal->text_bright, date_str, ANSI_RESET);
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

    snprintf(line_buf, sizeof(line_buf), " Current Time: %s%s%s%s", ANSI_BOLD, pal->primary, clock_str, ANSI_RESET);
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);
    ui_draw_d_line(DASHBOARD_WIDTH, "");

    /* TODAY Widget Box (Centered & Aligned within DASHBOARD_WIDTH) */
    char box_line[256];
    snprintf(box_line, sizeof(box_line), "%s┌───────────────── TODAY ──────────────────┐%s", pal->secondary, ANSI_RESET);
    ui_draw_d_line(DASHBOARD_WIDTH, box_line);

    char box_blank[256];
    snprintf(box_blank, sizeof(box_blank), "%s│                                          │%s", pal->secondary, ANSI_RESET);

    if (hero.status == HERO_STATUS_RUNNING) {
        ui_draw_d_line(DASHBOARD_WIDTH, box_blank);

        char raw1[128], pad1[128];
        snprintf(raw1, sizeof(raw1), "  RUNNING: %s • %.17s", hero.current_class.course_code, hero.current_class.course_name);
        ui_pad_to_visible_width(pad1, sizeof(pad1), raw1, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s%s%s %s│%s",
                 pal->secondary, ANSI_RESET, pal->success, pad1, ANSI_RESET, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        char raw2[128], pad2[128];
        snprintf(raw2, sizeof(raw2), "  Venue: Room %s • Ends %s", hero.current_class.room, hero.end_formatted);
        ui_pad_to_visible_width(pad2, sizeof(pad2), raw2, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s%s%s %s│%s",
                 pal->secondary, ANSI_RESET, pal->text_bright, pad2, ANSI_RESET, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        ui_draw_d_line(DASHBOARD_WIDTH, box_blank);

        char raw3[128], pad3[128], prog_bar[64];
        ui_make_progress_bar(prog_bar, sizeof(prog_bar), hero.progress_percentage, 12);
        snprintf(raw3, sizeof(raw3), "  Progress : %s", prog_bar);
        ui_pad_to_visible_width(pad3, sizeof(pad3), raw3, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s %s│%s",
                 pal->secondary, ANSI_RESET, pad3, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        char raw4[128], pad4[128];
        snprintf(raw4, sizeof(raw4), "  Time Left: %s", hero.formatted_remaining);
        ui_pad_to_visible_width(pad4, sizeof(pad4), raw4, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s%s%s %s│%s",
                 pal->secondary, ANSI_RESET, pal->warning, pad4, ANSI_RESET, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        ui_draw_d_line(DASHBOARD_WIDTH, box_blank);
    } else if (hero.status == HERO_STATUS_UPCOMING) {
        ui_draw_d_line(DASHBOARD_WIDTH, box_blank);

        char raw1[128], pad1[128];
        snprintf(raw1, sizeof(raw1), "  Status: No class running right now");
        ui_pad_to_visible_width(pad1, sizeof(pad1), raw1, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s%s%s %s│%s",
                 pal->secondary, ANSI_RESET, pal->text_dim, pad1, ANSI_RESET, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        ui_draw_d_line(DASHBOARD_WIDTH, box_blank);

        char raw2[128], pad2[128];
        snprintf(raw2, sizeof(raw2), "  NEXT: %s • %.18s", hero.current_class.course_code, hero.current_class.course_name);
        ui_pad_to_visible_width(pad2, sizeof(pad2), raw2, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s%s%s %s│%s",
                 pal->secondary, ANSI_RESET, pal->primary, pad2, ANSI_RESET, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        char raw3[128], pad3[128];
        snprintf(raw3, sizeof(raw3), "  Time: %s - %s • %s", hero.start_formatted, hero.end_formatted, hero.current_class.room);
        ui_pad_to_visible_width(pad3, sizeof(pad3), raw3, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s%s%s %s│%s",
                 pal->secondary, ANSI_RESET, pal->text_bright, pad3, ANSI_RESET, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        char raw4[128], pad4[128];
        if (strlen(hero.current_class.teacher) > 0) {
            snprintf(raw4, sizeof(raw4), "  Teacher: %.30s", hero.current_class.teacher);
        } else {
            snprintf(raw4, sizeof(raw4), "  Type: %s • Section B (B1)", class_type_to_string(hero.current_class.class_type));
        }
        ui_pad_to_visible_width(pad4, sizeof(pad4), raw4, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s%s%s %s│%s",
                 pal->secondary, ANSI_RESET, pal->text_dim, pad4, ANSI_RESET, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        ui_draw_d_line(DASHBOARD_WIDTH, box_blank);

        char raw5[128], pad5[128];
        int uh = (int)(hero.remaining_seconds / 3600);
        int um = (int)((hero.remaining_seconds % 3600) / 60);
        int us = (int)(hero.remaining_seconds % 60);
        if (uh > 0) {
            snprintf(raw5, sizeof(raw5), "  Starts in : %02dh %02dm %02ds", uh, um, us);
        } else {
            snprintf(raw5, sizeof(raw5), "  Starts in : %02dm %02ds", um, us);
        }
        ui_pad_to_visible_width(pad5, sizeof(pad5), raw5, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s%s%s %s│%s",
                 pal->secondary, ANSI_RESET, pal->warning, pad5, ANSI_RESET, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        ui_draw_d_line(DASHBOARD_WIDTH, box_blank);
    } else {
        ui_draw_d_line(DASHBOARD_WIDTH, box_blank);

        char raw1[128], pad1[128];
        snprintf(raw1, sizeof(raw1), "  Status: No class running right now");
        ui_pad_to_visible_width(pad1, sizeof(pad1), raw1, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s%s%s %s│%s",
                 pal->secondary, ANSI_RESET, pal->text_dim, pad1, ANSI_RESET, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        ui_draw_d_line(DASHBOARD_WIDTH, box_blank);

        char raw2[128], pad2[128];
        snprintf(raw2, sizeof(raw2), "  Schedule: Weekend / Study Day");
        ui_pad_to_visible_width(pad2, sizeof(pad2), raw2, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s%s%s %s│%s",
                 pal->secondary, ANSI_RESET, pal->primary, pad2, ANSI_RESET, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        char raw3[128], pad3[128];
        snprintf(raw3, sizeof(raw3), "  No classes scheduled for today");
        ui_pad_to_visible_width(pad3, sizeof(pad3), raw3, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s %s│%s",
                 pal->secondary, ANSI_RESET, pad3, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        ui_draw_d_line(DASHBOARD_WIDTH, box_blank);

        char raw4[128], pad4[128];
        snprintf(raw4, sizeof(raw4), "  Have a productive study session!");
        ui_pad_to_visible_width(pad4, sizeof(pad4), raw4, 40);
        snprintf(box_line, sizeof(box_line), "%s│%s %s%s%s %s│%s",
                 pal->secondary, ANSI_RESET, pal->text_dim, pad4, ANSI_RESET, pal->secondary, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, box_line);

        ui_draw_d_line(DASHBOARD_WIDTH, box_blank);
    }

    snprintf(box_line, sizeof(box_line), "%s└──────────────────────────────────────────┘%s", pal->secondary, ANSI_RESET);
    ui_draw_d_line(DASHBOARD_WIDTH, box_line);
    ui_draw_d_line(DASHBOARD_WIDTH, "");

    /* Classes Today Section (mentioning classes with time) */
    RoutineClass today_classes[MAX_CLASSES];
    int today_count = dashboard_get_todays_classes(state, &now_tm, today_classes, MAX_CLASSES);

    snprintf(line_buf, sizeof(line_buf), " %s%sClasses Today%s : %s%d%s",
             ANSI_BOLD, pal->text_bright, ANSI_RESET,
             pal->primary, today_count, ANSI_RESET);
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

    if (today_count > 0) {
        for (int i = 0; i < today_count; i++) {
            char s_time[16], e_time[16];
            time_format_time_val(s_time, sizeof(s_time), today_classes[i].start_hour, today_classes[i].start_minute, state->settings.is_24_hour);
            time_format_time_val(e_time, sizeof(e_time), today_classes[i].end_hour, today_classes[i].end_minute, state->settings.is_24_hour);

            snprintf(line_buf, sizeof(line_buf), "   %s•%s %-8s %s(%s - %s)%s",
                     pal->primary, ANSI_RESET,
                     today_classes[i].course_code,
                     pal->text_dim, s_time, e_time, ANSI_RESET);
            ui_draw_d_line(DASHBOARD_WIDTH, line_buf);
        }
    } else {
        snprintf(line_buf, sizeof(line_buf), "   %s(No classes scheduled today)%s", pal->text_dim, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, line_buf);
    }

    /* Space between Classes Today and NEXT EXAM */
    ui_draw_d_line(DASHBOARD_WIDTH, "");

    /* NEXT EXAM Widget (moved under 'classes today') */
    snprintf(line_buf, sizeof(line_buf), " %s%sNEXT EXAM%s", ANSI_BOLD, pal->text_bright, ANSI_RESET);
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

    if (next_exam.has_exam) {
        snprintf(line_buf, sizeof(line_buf), "  %s%s%s", pal->primary, next_exam.exam.title, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

        snprintf(line_buf, sizeof(line_buf), "  %sCountdown: %s%s", pal->warning, next_exam.formatted_countdown, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, line_buf);
    } else {
        snprintf(line_buf, sizeof(line_buf), "  %sNo upcoming exams scheduled%s", pal->text_dim, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, line_buf);
    }

    ui_draw_d_line(DASHBOARD_WIDTH, "");
    ui_draw_d_sep(DASHBOARD_WIDTH);
    ui_draw_d_line(DASHBOARD_WIDTH, "");

    /* REVIEW TODAY'S TOPICS Section */
    snprintf(line_buf, sizeof(line_buf), " %s%sREVIEW TODAY'S TOPICS%s", ANSI_BOLD, pal->primary, ANSI_RESET);
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

    if (today_count > 0) {
        for (int i = 0; i < today_count && i < 4; i++) {
            char time_buf[16];
            time_format_time_val(time_buf, sizeof(time_buf), today_classes[i].start_hour, today_classes[i].start_minute, state->settings.is_24_hour);

            char opt_label[16];
            snprintf(opt_label, sizeof(opt_label), "[R%d]", i + 1);

            char short_name[24];
            snprintf(short_name, sizeof(short_name), "%.16s", today_classes[i].course_name);

            snprintf(line_buf, sizeof(line_buf), "  %s%s%s %-8s %-8s %s• %-16s%s",
                     pal->warning, opt_label, ANSI_RESET,
                     time_buf,
                     today_classes[i].course_code,
                     pal->text_dim, short_name, ANSI_RESET);
            ui_draw_d_line(DASHBOARD_WIDTH, line_buf);
        }
        if (today_count > 4) {
            snprintf(line_buf, sizeof(line_buf), "  %s[Type 'R' to see all %d classes]%s", pal->text_dim, today_count, ANSI_RESET);
            ui_draw_d_line(DASHBOARD_WIDTH, line_buf);
        }
    } else {
        snprintf(line_buf, sizeof(line_buf), "  %sNo classes scheduled for today%s", pal->text_dim, ANSI_RESET);
        ui_draw_d_line(DASHBOARD_WIDTH, line_buf);
    }

    ui_draw_d_line(DASHBOARD_WIDTH, "");
    ui_draw_d_sep(DASHBOARD_WIDTH);
    ui_draw_d_line(DASHBOARD_WIDTH, "");

    /* Main Menu Navigation */
    snprintf(line_buf, sizeof(line_buf), " 1. Routine");
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

    snprintf(line_buf, sizeof(line_buf), " 2. Exams");
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

    snprintf(line_buf, sizeof(line_buf), " 3. Courses");
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

    snprintf(line_buf, sizeof(line_buf), " 4. GPA & Statistics");
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

    snprintf(line_buf, sizeof(line_buf), " 5. Search");
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

    snprintf(line_buf, sizeof(line_buf), " 6. Settings");
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

    snprintf(line_buf, sizeof(line_buf), " 0. Exit");
    ui_draw_d_line(DASHBOARD_WIDTH, line_buf);

    ui_draw_d_line(DASHBOARD_WIDTH, "");
    ui_draw_d_bottom(DASHBOARD_WIDTH);
}

void dashboard_live_mode(AppState* state) {
    ui_hide_cursor();
    clear_terminal();

    const ColorPalette* pal = ui_get_palette();

    while (1) {
        ui_move_cursor(1, 1);
        dashboard_render(state);

        printf("\n %s[LIVE CLOCK RUNNING] Press any key or 'q' to return to menu...%s   \n", pal->warning, ANSI_RESET);
        fflush(stdout);

        for (int i = 0; i < 10; i++) {
            if (platform_kbhit()) {
                platform_getch();
                ui_show_cursor();
                clear_terminal();
                return;
            }
            platform_sleep_ms(100);
        }
    }
}

void dashboard_review_class_topics(const AppState* state, const RoutineClass* rc) {
    clear_terminal();
    const ColorPalette* pal = ui_get_palette();

    char header_subtitle[256];
    snprintf(header_subtitle, sizeof(header_subtitle), "%s • %s", rc->course_name, rc->teacher);
    ui_print_header("TODAY'S CLASS TOPIC REVIEW", header_subtitle);

    /* 1. Class timing and location card */
    char start_time[16], end_time[16];
    time_format_time_val(start_time, sizeof(start_time), rc->start_hour, rc->start_minute, state->settings.is_24_hour);
    time_format_time_val(end_time, sizeof(end_time), rc->end_hour, rc->end_minute, state->settings.is_24_hour);

    printf("\n%s╔══ CLASS DETAILS ══════════════════════════════════════════════════╗%s\n", pal->primary, ANSI_RESET);
    printf("║ %sCourse%s : %-10s %sName%s : %-38.38s ║\n",
           pal->text_bright, ANSI_RESET, rc->course_code,
           pal->text_bright, ANSI_RESET, rc->course_name);
    printf("║ %sTime%s   : %s - %s  │  %sRoom%s: %-12s │ %sTeacher%s: %-14.14s ║\n",
           pal->text_bright, ANSI_RESET, start_time, end_time,
           pal->text_bright, ANSI_RESET, rc->room,
           pal->text_bright, ANSI_RESET, rc->teacher);
    if (_stricmp(rc->course_code, "CSE 1101") == 0) {
        printf("║ %sTeachers%s: Pre-Mid: Md Tajmilur Rahman │ Post-Mid: Ehsanul Karim  ║\n",
               pal->text_bright, ANSI_RESET);
        printf("║ %sCredits %s: 3.0 Hours/Week │ 3.0 Credits (Authentic Lecture Plan)  ║\n",
               pal->secondary, ANSI_RESET);
    }
    printf("%s╚═══════════════════════════════════════════════════════════════════╝%s\n", pal->primary, ANSI_RESET);

    /* 2. Today's Scheduled Topic(s) */
    printf("\n%s── TODAY'S SCHEDULED TOPIC ──────────────────────────────────────────────%s\n\n", pal->secondary, ANSI_RESET);

    struct tm now_tm = time_get_localtime();
    int cur_day = now_tm.tm_mday;
    int cur_mon = now_tm.tm_mon + 1;
    int cur_year = now_tm.tm_year + 1900;

    char date_full[32];
    char date_short[32];
    char date_nodigit[32];
    snprintf(date_full, sizeof(date_full), "%02d/%02d/%d", cur_day, cur_mon, cur_year);
    snprintf(date_short, sizeof(date_short), "%02d/%02d", cur_day, cur_mon);
    snprintf(date_nodigit, sizeof(date_nodigit), "%d/%02d", cur_day, cur_mon);

    int found_topics = 0;
    for (int i = 0; i < state->syllabus_count; i++) {
        const SyllabusTopic* top = &state->syllabus[i];
        if (_stricmp(top->course_code, rc->course_code) != 0) {
            continue;
        }

        bool date_matches = false;
        if (strstr(top->title, date_full) || strstr(top->title, date_short) || strstr(top->title, date_nodigit)) {
            date_matches = true;
        }

        for (int s = 0; s < top->subtopic_count; s++) {
            if (strstr(top->subtopics[s].key_points, date_full) ||
                strstr(top->subtopics[s].key_points, date_short) ||
                strstr(top->subtopics[s].key_points, date_nodigit) ||
                strstr(top->subtopics[s].title, date_full) ||
                strstr(top->subtopics[s].title, date_short)) {
                date_matches = true;
                break;
            }
        }

        if (date_matches) {
            found_topics++;

            const char* title_ptr = top->title;
            if (strncmp(title_ptr, "[Pre-Mid] ", 10) == 0) title_ptr += 10;
            else if (strncmp(title_ptr, "[Post-Mid] ", 11) == 0) title_ptr += 11;

            if (strncmp(title_ptr, "Lec ", 4) == 0) {
                const char* colon = strchr(title_ptr, ':');
                if (colon) {
                    title_ptr = colon + 1;
                    while (*title_ptr == ' ') title_ptr++;
                }
            }

            char clean_title[STR_NAME_LEN];
            strncpy(clean_title, title_ptr, sizeof(clean_title) - 1);
            clean_title[sizeof(clean_title) - 1] = '\0';
            char* ct_tag = strstr(clean_title, "[CT-");
            if (ct_tag) {
                *ct_tag = '\0';
                int len = strlen(clean_title);
                while (len > 0 && clean_title[len - 1] == ' ') {
                    clean_title[--len] = '\0';
                }
            }

            printf("  %s• Lecture %02d : %s%s\n", pal->text_bright, top->chapter_num, clean_title, ANSI_RESET);

            if (top->subtopic_count > 0 && top->subtopics[0].title[0] != '\0') {
                printf("    %sCoverage   %s: %s\n", pal->secondary, ANSI_RESET, top->subtopics[0].title);
            }
            if (top->subtopic_count > 0 && top->subtopics[0].key_points[0] != '\0') {
                printf("    %sSchedule   %s: %s\n", pal->secondary, ANSI_RESET, top->subtopics[0].key_points);
            }

            if (top->chapter_num == 13) {
                printf("    %sMilestone  %s: %s★ CLASS TEST 1 (CT-1)%s\n", pal->secondary, ANSI_RESET, pal->warning, ANSI_RESET);
            } else if (top->chapter_num == 29) {
                printf("    %sMilestone  %s: %s★ CLASS TEST 2 (CT-2)%s\n", pal->secondary, ANSI_RESET, pal->warning, ANSI_RESET);
            }

            if (strlen(top->code_snippet) > 0) {
                printf("\n    %sCode Example:%s\n", pal->text_bright, ANSI_RESET);
                const char* cp = top->code_snippet;
                char line_snip[256];
                while (*cp) {
                    int idx = 0;
                    while (*cp && *cp != '\n' && idx < (int)sizeof(line_snip) - 1) {
                        line_snip[idx++] = *cp++;
                    }
                    line_snip[idx] = '\0';
                    if (*cp == '\n') cp++;
                    printf("      %s%s%s\n", pal->text_dim, line_snip, ANSI_RESET);
                }
            }
            printf("\n");
        }
    }

    if (found_topics == 0) {
        printf("  %sNo specific curriculum topic registered for today's date (%02d/%02d/%d).%s\n\n",
               pal->text_dim, cur_day, cur_mon, cur_year, ANSI_RESET);
    }

    /* 3. Upcoming Assessments for this course */
    bool has_exam = false;
    for (int i = 0; i < state->exam_count; i++) {
        const Exam* ex = &state->exams[i];
        if (ex->is_enabled && !ex->is_completed && _stricmp(ex->course_code, rc->course_code) == 0) {
            long diff = time_engine_calc_exam_diff(ex, &now_tm);
            if (diff > 0) {
                if (!has_exam) {
                    printf("%s── UPCOMING ASSESSMENT ──────────────────────────────────────────%s\n", pal->warning, ANSI_RESET);
                    has_exam = true;
                }
                char cd_buf[64];
                time_engine_format_countdown_str(diff, state->settings.countdown_style, cd_buf, sizeof(cd_buf));
                printf("  %s• %s (%s)%s\n", pal->primary, ex->title, exam_type_to_string(ex->type), ANSI_RESET);
                printf("    %sDue in:%s %s%s%s (Room: %s, Marks: %.0f)\n",
                       pal->text_dim, ANSI_RESET, pal->warning, cd_buf, ANSI_RESET, ex->room, ex->total_marks);
            }
        }
    }

    printf("\n");
    ui_pause_key();
}

void dashboard_review_menu(AppState* state) {
    struct tm now_tm = time_get_localtime();
    RoutineClass today_classes[MAX_CLASSES];
    int count = dashboard_get_todays_classes(state, &now_tm, today_classes, MAX_CLASSES);

    if (count == 0) {
        clear_terminal();
        ui_print_header("REVIEW TODAY'S TOPICS", "Daily Curriculum Tracker");
        const ColorPalette* pal = ui_get_palette();
        printf("\n  %sNo classes are scheduled for today!%s\n\n", pal->text_dim, ANSI_RESET);
        ui_pause_key();
        return;
    }

    while (1) {
        clear_terminal();
        ui_print_header("REVIEW TODAY'S TOPICS", "Select a class to review curriculum topics");
        const ColorPalette* pal = ui_get_palette();

        printf("\n%sToday's Scheduled Classes:%s\n\n", pal->text_bright, ANSI_RESET);
        for (int i = 0; i < count; i++) {
            char time_buf[16];
            time_format_time_val(time_buf, sizeof(time_buf), today_classes[i].start_hour, today_classes[i].start_minute, state->settings.is_24_hour);
            printf("  [%d] %s%-9s%s (%s) • %-28s (Room: %s)\n",
                   i + 1,
                   pal->primary, today_classes[i].course_code, ANSI_RESET,
                   time_buf,
                   today_classes[i].course_name,
                   today_classes[i].room);
        }
        printf("  [0] Back to Main Dashboard\n\n");

        int sel = ui_input_int("Enter class number to review", 0, count, 1);
        if (sel == 0) break;
        dashboard_review_class_topics(state, &today_classes[sel - 1]);
    }
}
