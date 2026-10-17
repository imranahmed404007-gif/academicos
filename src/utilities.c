#include "academic_os.h"

/* ========================================================================= */
/* 1. GLOBAL SEARCH MODULE                                                   */
/* ========================================================================= */

static bool string_contains_ci(const char* haystack, const char* needle) {
    if (!haystack || !needle) return false;
    if (strlen(needle) == 0) return true;

    char h_lower[512] = "";
    char n_lower[128] = "";

    size_t h_len = strlen(haystack);
    if (h_len >= sizeof(h_lower)) h_len = sizeof(h_lower) - 1;
    for (size_t i = 0; i < h_len; i++) h_lower[i] = (char)tolower((unsigned char)haystack[i]);
    h_lower[h_len] = '\0';

    size_t n_len = strlen(needle);
    if (n_len >= sizeof(n_lower)) n_len = sizeof(n_lower) - 1;
    for (size_t i = 0; i < n_len; i++) n_lower[i] = (char)tolower((unsigned char)needle[i]);
    n_lower[n_len] = '\0';

    return strstr(h_lower, n_lower) != NULL;
}

void search_execute(const AppState* state, const char* query) {
    const ColorPalette* pal = ui_get_palette();
    int total_matches = 0;

    printf("\n%sSearch Results for '%s':%s\n", pal->primary, query, ANSI_RESET);
    printf("===============================================================\n");

    /* 1. Search Courses */
    int course_matches = 0;
    for (int i = 0; i < state->course_count; i++) {
        const Course* c = &state->courses[i];
        if (string_contains_ci(c->code, query) ||
            string_contains_ci(c->name, query) ||
            string_contains_ci(c->teacher, query) ||
            string_contains_ci(c->notes, query)) {
            if (course_matches == 0) printf("\n%s[COURSES]%s\n", pal->secondary, ANSI_RESET);
            printf("  • %s%-10s%s : %s (%.1f Cr) - %s%s%s\n",
                   pal->primary, c->code, ANSI_RESET, c->name, c->credit,
                   pal->text_dim, c->teacher, ANSI_RESET);
            course_matches++;
            total_matches++;
        }
    }

    /* 2. Search Routine */
    int routine_matches = 0;
    for (int i = 0; i < state->class_count; i++) {
        const RoutineClass* rc = &state->classes[i];
        if (string_contains_ci(rc->course_code, query) ||
            string_contains_ci(rc->course_name, query) ||
            string_contains_ci(rc->room, query) ||
            string_contains_ci(rc->teacher, query)) {
            if (routine_matches == 0) printf("\n%s[ROUTINE CLASSES]%s\n", pal->secondary, ANSI_RESET);
            printf("  • %s: %s%s%s (%s) %02d:%02d-%02d:%02d @ %s - %s\n",
                   day_to_string(rc->day_of_week),
                   pal->primary, rc->course_code, ANSI_RESET,
                   class_type_to_string(rc->class_type),
                   rc->start_hour, rc->start_minute, rc->end_hour, rc->end_minute,
                   rc->room, rc->teacher);
            routine_matches++;
            total_matches++;
        }
    }

    /* 3. Search Exams */
    int exam_matches = 0;
    for (int i = 0; i < state->exam_count; i++) {
        const Exam* ex = &state->exams[i];
        if (string_contains_ci(ex->course_code, query) ||
            string_contains_ci(ex->title, query) ||
            string_contains_ci(ex->room, query) ||
            string_contains_ci(ex->notes, query)) {
            if (exam_matches == 0) printf("\n%s[EXAMS & ASSESSMENTS]%s\n", pal->secondary, ANSI_RESET);
            printf("  • %s%s%s : %s [%s] on %02d/%02d/%d in %s\n",
                   pal->primary, ex->course_code, ANSI_RESET,
                   ex->title, exam_type_to_string(ex->type),
                   ex->day, ex->month, ex->year, ex->room);
            exam_matches++;
            total_matches++;
        }
    }

    /* 4. Search Syllabus */
    int syllabus_matches = 0;
    for (int i = 0; i < state->syllabus_count; i++) {
        const SyllabusTopic* top = &state->syllabus[i];
        bool matched = string_contains_ci(top->course_code, query) ||
                       string_contains_ci(top->title, query) ||
                       string_contains_ci(top->code_snippet, query);
        for (int s = 0; s < top->subtopic_count; s++) {
            if (string_contains_ci(top->subtopics[s].title, query) ||
                string_contains_ci(top->subtopics[s].key_points, query)) {
                matched = true;
                break;
            }
        }

        if (matched) {
            if (syllabus_matches == 0) printf("\n%s[SYLLABUS TOPICS]%s\n", pal->secondary, ANSI_RESET);
            printf("  • %s%s%s Ch %d: %s [%s]\n",
                   pal->primary, top->course_code, ANSI_RESET,
                   top->chapter_num, top->title, topic_status_to_string(top->status));
            syllabus_matches++;
            total_matches++;
        }
    }

    if (total_matches == 0) {
        printf("\n  %sNo matching items found for '%s'.%s\n", pal->text_dim, query, ANSI_RESET);
    } else {
        printf("\n===============================================================\n");
        printf("  Total matches found: %s%d%s\n", pal->success, total_matches, ANSI_RESET);
    }
    printf("\n");
}

void search_menu(const AppState* state) {
    ui_print_header("GLOBAL SEARCH", "Search Across All System Modules");
    printf("Select Search Preset or Custom Keyword:\n");
    printf("  1. CSE 1101 (Structured Programming)\n");
    printf("  2. CSE 1102 (Programming Lab)\n");
    printf("  3. CSE 1107 (Discrete Mathematics)\n");
    printf("  4. MATH 1107 (Differential & Integral Calculus)\n");
    printf("  5. PHY 1107 / 1108 (Physics)\n");
    printf("  6. HUM 1107 / 1108 (English & Humanities)\n");
    printf("  7. Class Tests & Term Final Exams\n");
    printf("  8. Custom Keyword Search\n");
    printf("  0. Cancel\n\n");

    int sel = ui_input_int("Choice (0-8)", 0, 8, 1);
    if (sel == 0) return;

    char query[STR_SHORT_LEN];
    if (sel == 1) snprintf(query, sizeof(query), "CSE 1101");
    else if (sel == 2) snprintf(query, sizeof(query), "CSE 1102");
    else if (sel == 3) snprintf(query, sizeof(query), "CSE 1107");
    else if (sel == 4) snprintf(query, sizeof(query), "MATH");
    else if (sel == 5) snprintf(query, sizeof(query), "PHY");
    else if (sel == 6) snprintf(query, sizeof(query), "HUM");
    else if (sel == 7) snprintf(query, sizeof(query), "Exam");
    else {
        ui_input_string("Enter search keyword", query, sizeof(query), "");
        if (strlen(query) == 0) {
            printf("Empty search query!\n");
            ui_pause_key();
            return;
        }
    }

    search_execute(state, query);
    ui_pause_key();
}

/* ========================================================================= */
/* 2. SETTINGS MODULE                                                        */
/* ========================================================================= */

void settings_toggle_clock_format(AppState* state) {
    state->settings.is_24_hour = !state->settings.is_24_hour;
    storage_save_settings();
    printf("\nClock format set to: %s%s%s\n",
           ANSI_GREEN, state->settings.is_24_hour ? "24-Hour (Military)" : "12-Hour (AM/PM)", ANSI_RESET);
    ui_pause_key();
}

void settings_change_countdown_style(AppState* state) {
    ui_print_header("COUNTDOWN STYLE", "Select Timer Format");
    printf("  0. Detailed: '05 Days 12 Hours 21 Minutes'\n");
    printf("  1. Compact : '05d 12h 21m'\n\n");

    state->settings.countdown_style = ui_input_int("Choice (0 or 1)", 0, 1, state->settings.countdown_style);
    storage_save_settings();
    printf("\n%sCountdown format updated!%s\n", ANSI_GREEN, ANSI_RESET);
    ui_pause_key();
}

void settings_change_color_theme(AppState* state) {
    ui_print_header("COLOR THEME", "Select Terminal Accent Palette");
    printf("  0. Cyan Tech (Default Cyberpunk)\n");
    printf("  1. Emerald Matrix (Hacker Green)\n");
    printf("  2. Amber Warm (Retro Terminal)\n");
    printf("  3. Monochrome (Clean High-Contrast White)\n\n");

    int t = ui_input_int("Select theme (0-3)", 0, 3, state->settings.color_theme);
    state->settings.color_theme = t;
    ui_set_theme(t);
    storage_save_settings();

    printf("\n%sTheme applied successfully!%s\n", ANSI_GREEN, ANSI_RESET);
    ui_pause_key();
}

void settings_toggle_seconds(AppState* state) {
    state->settings.show_seconds = !state->settings.show_seconds;
    storage_save_settings();
    printf("\nClock seconds display: %s%s%s\n",
           ANSI_GREEN, state->settings.show_seconds ? "ENABLED" : "DISABLED", ANSI_RESET);
    ui_pause_key();
}

void settings_reset_defaults(AppState* state) {
    (void)state;
    ui_print_header("RESET TO DEFAULTS", "Restore Authentic KUET CSE Data");
    printf("This will reset all courses, timetable, exams, and syllabus\n");
    printf("to the official authentic KUET CSE 1st Year Term-1 curriculum data.\n\n");

    bool confirm = ui_input_bool("Are you sure you want to reset? (Y/N)", false);
    if (confirm) {
        storage_reset_kuet_defaults();
        ui_set_theme(state->settings.color_theme);
        printf("\n%sSystem successfully reset to default KUET CSE dataset!%s\n", ANSI_GREEN, ANSI_RESET);
    } else {
        printf("Reset cancelled.\n");
    }
    ui_pause_key();
}

void settings_menu(AppState* state) {
    while (1) {
        ui_print_header("SYSTEM SETTINGS", "Preferences & Data Management");
        printf("  1. Clock Format: %s\n", state->settings.is_24_hour ? "24-Hour" : "12-Hour");
        printf("  2. Countdown Display: %s\n", state->settings.countdown_style == 0 ? "Detailed" : "Compact");
        printf("  3. Terminal Color Theme (Cyan / Matrix / Amber / Mono)\n");
        printf("  4. Toggle Clock Seconds Display: %s\n", state->settings.show_seconds ? "ON" : "OFF");
        printf("  5. Reset to Default KUET Data\n");
        printf("  0. Back to Main Menu\n\n");

        int choice = ui_input_int("Enter choice (0-5)", 0, 5, 1);
        switch (choice) {
            case 1: settings_toggle_clock_format(state); break;
            case 2: settings_change_countdown_style(state); break;
            case 3: settings_change_color_theme(state); break;
            case 4: settings_toggle_seconds(state); break;
            case 5: settings_reset_defaults(state); break;
            case 0: return;
        }
    }
}

/* ========================================================================= */
/* 3. GPA & STATISTICS MODULE                                                */
/* ========================================================================= */

void stats_marks_to_kuet_grade(float marks, char* out_letter, float* out_point) {
    if (marks >= 80.0f) {
        strcpy(out_letter, "A+");
        *out_point = 4.00f;
    } else if (marks >= 75.0f) {
        strcpy(out_letter, "A");
        *out_point = 3.75f;
    } else if (marks >= 70.0f) {
        strcpy(out_letter, "A-");
        *out_point = 3.50f;
    } else if (marks >= 65.0f) {
        strcpy(out_letter, "B+");
        *out_point = 3.25f;
    } else if (marks >= 60.0f) {
        strcpy(out_letter, "B");
        *out_point = 3.00f;
    } else if (marks >= 55.0f) {
        strcpy(out_letter, "B-");
        *out_point = 2.75f;
    } else if (marks >= 50.0f) {
        strcpy(out_letter, "C+");
        *out_point = 2.50f;
    } else if (marks >= 45.0f) {
        strcpy(out_letter, "C");
        *out_point = 2.25f;
    } else if (marks >= 40.0f) {
        strcpy(out_letter, "D");
        *out_point = 2.00f;
    } else {
        strcpy(out_letter, "F");
        *out_point = 0.00f;
    }
}

void stats_display_gpa_inline(const AppState* state) {
    const ColorPalette* pal = ui_get_palette();
    printf("  %s┌── CURRENT GPA & TERM ACADEMIC REPORT ──────────────────────────────────────┐%s\n", pal->primary, ANSI_RESET);

    if (state->grade_count == 0) {
        printf("    %sNo course grades recorded yet.%s\n", pal->text_dim, ANSI_RESET);
        printf("  %s└───────────────────────────────────────────────────────────────────────────┘%s\n\n", pal->primary, ANSI_RESET);
        return;
    }

    printf("    %-10s │ %-4s │ %-6s │ %-5s │ %-6s │ %s\n", "Course", "Cr", "Marks", "Grade", "Point", "Course Title");
    printf("    ───────────┼──────┼────────┼───────┼────────┼──────────────────────────────────\n");

    float total_weighted_points = 0.0f;
    float total_credits = 0.0f;
    float earned_credits = 0.0f;

    for (int i = 0; i < state->grade_count; i++) {
        const GradeRecord* gr = &state->grades[i];
        printf("    %s%-10s%s │ %4.2f │ %6.1f │ %s%-5s%s │ %6.2f │ %s\n",
               pal->primary, gr->course_code, ANSI_RESET,
               gr->credit, gr->marks,
               pal->text_bright, gr->grade_letter, ANSI_RESET,
               gr->grade_point,
               gr->course_name);

        total_credits += gr->credit;
        total_weighted_points += (gr->credit * gr->grade_point);
        if (gr->grade_point > 0.0f) {
            earned_credits += gr->credit;
        }
    }

    float gpa = (total_credits > 0.0f) ? (total_weighted_points / total_credits) : 0.0f;

    printf("    ───────────┴──────┴────────┴───────┴────────┴──────────────────────────────────\n");
    printf("    Credits: %s%.2f%s Attempted, %s%.2f%s Earned   Current GPA: %s%s%.2f%s / 4.00\n",
           pal->text_bright, total_credits, ANSI_RESET,
           pal->success, earned_credits, ANSI_RESET,
           ANSI_BOLD, pal->primary, gpa, ANSI_RESET);

    const char* standing = (gpa >= 3.75f) ? "★ First Class with Honours" :
                           (gpa >= 3.00f) ? "First Class" :
                           (gpa >= 2.25f) ? "Second Class" : "Academic Probation Warning";
    printf("    Standing: %s%s%s\n", (gpa >= 3.0f ? pal->success : pal->warning), standing, ANSI_RESET);
    printf("  %s└───────────────────────────────────────────────────────────────────────────┘%s\n\n", pal->primary, ANSI_RESET);
}

void stats_view_gpa_report(const AppState* state) {
    ui_print_header("GPA & CGPA REPORT", "Official KUET Credit-Weighted System");
    stats_display_gpa_inline(state);
    ui_pause_key();
}

void stats_view_distribution_chart(const AppState* state) {
    const ColorPalette* pal = ui_get_palette();
    ui_print_header("GRADE DISTRIBUTION", "ASCII Frequency Histogram");

    int count_aplus = 0, count_a = 0, count_aminus = 0;
    int count_bplus = 0, count_b = 0, count_bminus = 0;
    int count_c = 0, count_d = 0, count_f = 0;

    for (int i = 0; i < state->grade_count; i++) {
        const char* l = state->grades[i].grade_letter;
        if (strcmp(l, "A+") == 0) count_aplus++;
        else if (strcmp(l, "A") == 0) count_a++;
        else if (strcmp(l, "A-") == 0) count_aminus++;
        else if (strcmp(l, "B+") == 0) count_bplus++;
        else if (strcmp(l, "B") == 0) count_b++;
        else if (strcmp(l, "B-") == 0) count_bminus++;
        else if (strcmp(l, "C+") == 0 || strcmp(l, "C") == 0) count_c++;
        else if (strcmp(l, "D") == 0) count_d++;
        else count_f++;
    }

    struct {
        const char* label;
        int count;
        const char* col;
    } bars[] = {
        {"A+ (4.00)", count_aplus, pal->primary},
        {"A  (3.75)", count_a,     pal->secondary},
        {"A- (3.50)", count_aminus, pal->accent},
        {"B+ (3.25)", count_bplus, pal->warning},
        {"B  (3.00)", count_b,     pal->warning},
        {"B- (2.75)", count_bminus, pal->warning},
        {"C  (2.25+)", count_c,    pal->danger},
        {"D  (2.00)", count_d,     pal->danger},
        {"F  (0.00)", count_f,     pal->danger}
    };

    printf("\n");
    for (int i = 0; i < 9; i++) {
        printf("  %s%-11s%s │ ", bars[i].col, bars[i].label, ANSI_RESET);
        for (int b = 0; b < bars[i].count * 3; b++) {
            printf("%s█%s", bars[i].col, ANSI_RESET);
        }
        printf(" (%d)\n", bars[i].count);
    }
    printf("\n");
    ui_pause_key();
}

void stats_manage_grades(AppState* state) {
    ui_print_header("MANAGE GRADES", "Record / Update Term Results");
    if (state->course_count == 0) {
        printf("  No courses registered in catalogue.\n\n");
        ui_pause_key();
        return;
    }

    printf("Select Course:\n");
    for (int i = 0; i < state->course_count; i++) {
        printf("  %d. %s (%s, %.2f Cr)\n", i + 1, state->courses[i].code, state->courses[i].name, state->courses[i].credit);
    }
    printf("  0. Back\n\n");

    int c_choice = ui_input_int("Choice (0 to cancel)", 0, state->course_count, 1);
    if (c_choice == 0) return;

    const Course* course = &state->courses[c_choice - 1];

    int idx = -1;
    for (int i = 0; i < state->grade_count; i++) {
        if (_stricmp(state->grades[i].course_code, course->code) == 0) {
            idx = i;
            break;
        }
    }

    printf("\nSelect Grade / Performance for %s:\n", course->code);
    printf("  1. A+ (80%%+ / 4.00 GP - Outstanding)\n");
    printf("  2. A  (75%%-79%% / 3.75 GP - Excellent)\n");
    printf("  3. A- (70%%-74%% / 3.50 GP - Very Good)\n");
    printf("  4. B+ (65%%-69%% / 3.25 GP - Good)\n");
    printf("  5. B  (60%%-64%% / 3.00 GP - Satisfactory)\n");
    printf("  6. Custom Marks Input\n");
    int g_sel = ui_input_int("Choice (1-6)", 1, 6, 1);

    float marks = 80.0f;
    if (g_sel == 1) marks = 85.0f;
    else if (g_sel == 2) marks = 77.0f;
    else if (g_sel == 3) marks = 72.0f;
    else if (g_sel == 4) marks = 67.0f;
    else if (g_sel == 5) marks = 62.0f;
    else marks = ui_input_float("Marks obtained (0-100)", 0.0f, 100.0f, 80.0f);

    if (idx != -1) {
        state->grades[idx].marks = marks;
        stats_marks_to_kuet_grade(marks, state->grades[idx].grade_letter, &state->grades[idx].grade_point);
        printf("\nUpdated Grade for %s: %s (%.2f GP)\n", course->code, state->grades[idx].grade_letter, state->grades[idx].grade_point);
    } else {
        if (state->grade_count >= MAX_GRADES) {
            printf("%sGrades record limit reached!%s\n", ANSI_RED, ANSI_RESET);
            ui_pause_key();
            return;
        }
        GradeRecord gr;
        memset(&gr, 0, sizeof(GradeRecord));
        snprintf(gr.course_code, sizeof(gr.course_code), "%s", course->code);
        snprintf(gr.course_name, sizeof(gr.course_name), "%s", course->name);
        gr.credit = course->credit;
        gr.marks = marks;
        stats_marks_to_kuet_grade(gr.marks, gr.grade_letter, &gr.grade_point);
        gr.semester_num = 1;
        gr.is_completed = true;

        state->grades[state->grade_count++] = gr;
        printf("\nSaved Grade for %s: %s (%.2f GP)\n", course->code, gr.grade_letter, gr.grade_point);
    }

    storage_save_grades();
    printf("\n%sGrade saved successfully!%s\n", ANSI_GREEN, ANSI_RESET);
    ui_pause_key();
}

void stats_target_cgpa_simulator(const AppState* state) {
    const ColorPalette* pal = ui_get_palette();
    ui_print_header("TARGET CGPA SIMULATOR", "Predict Required Future Performance");

    float curr_total_points = 0.0f;
    float curr_total_credits = 0.0f;
    for (int i = 0; i < state->grade_count; i++) {
        curr_total_credits += state->grades[i].credit;
        curr_total_points += (state->grades[i].credit * state->grades[i].grade_point);
    }

    float current_cgpa = (curr_total_credits > 0.0f) ? (curr_total_points / curr_total_credits) : 0.0f;
    printf("  Current Completed Credits: %s%.2f%s\n", pal->text_bright, curr_total_credits, ANSI_RESET);
    printf("  Current CGPA             : %s%.2f%s\n\n", pal->primary, current_cgpa, ANSI_RESET);

    printf("Select Target CGPA Goal:\n");
    printf("  1. 4.00 (Exceptional / Top Rank)\n");
    printf("  2. 3.85 (Honours / High Distinction)\n");
    printf("  3. 3.75 (First Class with Distinction)\n");
    printf("  4. 3.50 (First Class)\n");
    printf("  5. Custom Target Input\n");
    int goal_sel = ui_input_int("Choice (1-5)", 1, 5, 2);
    float target_cgpa = 3.85f;
    if (goal_sel == 1) target_cgpa = 4.00f;
    else if (goal_sel == 2) target_cgpa = 3.85f;
    else if (goal_sel == 3) target_cgpa = 3.75f;
    else if (goal_sel == 4) target_cgpa = 3.50f;
    else target_cgpa = ui_input_float("Enter your Target CGPA", 2.0f, 4.00f, 3.80f);

    float remaining_credits = ui_input_float("Remaining Credits in Degree/Term", 1.0f, 150.0f, 15.0f);

    float total_degree_credits = curr_total_credits + remaining_credits;
    float needed_total_points = target_cgpa * total_degree_credits;
    float needed_future_points = needed_total_points - curr_total_points;
    float needed_gpa = needed_future_points / remaining_credits;

    printf("\n%s--- SIMULATION RESULT ---%s\n", pal->secondary, ANSI_RESET);
    if (needed_gpa > 4.00f) {
        printf("  %sTarget CGPA %.2f is mathematically impossible.%s\n", pal->danger, target_cgpa, ANSI_RESET);
        printf("  (Requires a GPA of %s%.2f%s in remaining credits, but max possible is 4.00)\n",
               pal->danger, needed_gpa, ANSI_RESET);
    } else if (needed_gpa <= 0.0f) {
        printf("  %sCongratulations! You have already surpassed this target!%s\n", pal->success, ANSI_RESET);
    } else {
        printf("  To achieve Target CGPA: %s%.2f%s\n", pal->text_bright, target_cgpa, ANSI_RESET);
        printf("  Required Average GPA   : %s%s%.2f%s in next %.1f credits\n",
               ANSI_BOLD, (needed_gpa > 3.75f ? pal->warning : pal->success), needed_gpa, ANSI_RESET, remaining_credits);

        if (needed_gpa >= 3.75f) {
            printf("  Strategy: Aim for %sA+ and A grades%s in all major 3.0 credit theory courses.\n", pal->warning, ANSI_RESET);
        } else if (needed_gpa >= 3.00f) {
            printf("  Strategy: Consistent %sA- and B+ grades%s will achieve your target.\n", pal->primary, ANSI_RESET);
        } else {
            printf("  Strategy: Moderate performance will secure this target easily.\n");
        }
    }
    printf("\n");
    ui_pause_key();
}

void stats_menu(AppState* state) {
    while (1) {
        ui_print_header("GPA & STATISTICS", "KUET Grading & Performance");
        stats_display_gpa_inline(state);

        const ColorPalette* pal = ui_get_palette();
        printf("  %s--- Other Options ---%s\n", pal->secondary, ANSI_RESET);
        printf("  1. Grade Distribution Chart (ASCII Histogram)\n");
        printf("  2. Record / Update Course Grades\n");
        printf("  3. Target CGPA Simulator\n");
        printf("  0. Back to Main Menu\n\n");

        int choice = ui_input_int("Enter choice (0-3)", 0, 3, 1);
        switch (choice) {
            case 1: stats_view_distribution_chart(state); break;
            case 2: stats_manage_grades(state); break;
            case 3: stats_target_cgpa_simulator(state); break;
            case 0: return;
        }
    }
}
