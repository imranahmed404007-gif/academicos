#include "academic_os.h"

static int get_lecture_week(const char* course_code, int chapter_num) {
    if (_stricmp(course_code, "MATH 1107") == 0) {
        if (chapter_num <= 3) return 1;
        if (chapter_num <= 6) return 2;
        if (chapter_num <= 9) return 3;
        if (chapter_num <= 12) return 4;
        if (chapter_num <= 15) return 5;
        if (chapter_num <= 19) return 6;
        if (chapter_num <= 22) return 7;
        if (chapter_num <= 25) return 8;
        if (chapter_num <= 28) return 9;
        if (chapter_num <= 31) return 10;
        if (chapter_num <= 34) return 11;
        if (chapter_num <= 37) return 12;
        return 13;
    } else {
        if (chapter_num <= 3) return 1;
        if (chapter_num <= 6) return 2;
        if (chapter_num <= 9) return 3;
        if (chapter_num <= 12) return 4;
        if (chapter_num <= 15) return 5;
        if (chapter_num <= 18) return 6;
        if (chapter_num <= 21) return 7;
        if (chapter_num <= 24) return 8;
        if (chapter_num <= 27) return 9;
        if (chapter_num <= 30) return 10;
        if (chapter_num <= 33) return 11;
        if (chapter_num <= 36) return 12;
        return 13;
    }
}

void syllabus_render_tree(const AppState* state, const char* course_code) {
    const ColorPalette* pal = ui_get_palette();
    printf("\n%s%s - %s CURRICULUM TREE%s\n", pal->primary, course_code,
           (_stricmp(course_code, "MATH 1107") == 0) ? "CALCULUS" : "STRUCTURED PROGRAMMING",
           ANSI_RESET);

    int course_topics[MAX_TOPICS];
    int count = 0;
    for (int i = 0; i < state->syllabus_count; i++) {
        if (_stricmp(state->syllabus[i].course_code, course_code) == 0) {
            course_topics[count++] = i;
        }
    }

    if (count == 0) {
        printf("  %s(No syllabus topics defined for %s)%s\n\n", pal->text_dim, course_code, ANSI_RESET);
        return;
    }

    bool shown_pre_mid = false;
    bool shown_post_mid = false;
    int last_week = -1;

    for (int i = 0; i < count; i++) {
        int idx = course_topics[i];
        const SyllabusTopic* top = &state->syllabus[idx];
        int cur_week = get_lecture_week(course_code, top->chapter_num);

        if (strstr(top->title, "[Pre-Mid]") && !shown_pre_mid) {
            shown_pre_mid = true;
            printf(" %s╔══ PART 1: PRE-MID TERM (Weeks 01 - 06 • Lectures 01 to %s) ═══════════════╗%s\n",
                   pal->primary, (_stricmp(course_code, "MATH 1107") == 0) ? "19" : "18", ANSI_RESET);
            if (_stricmp(course_code, "MATH 1107") == 0) {
                printf(" %s║ Teacher: Md. Dulal Hossain, Assistant Professor, Dept. of Mathematics      ║%s\n", pal->text_bright, ANSI_RESET);
                printf(" %s║ Milestones: Class Test 1 (22/07) • Mid-Term Exam (22/08/2026, CSE 101)    ║%s\n", pal->secondary, ANSI_RESET);
            } else {
                printf(" %s║ Teacher: Md Tajmilur Rahman, Lecturer, Department of CSE                  ║%s\n", pal->text_bright, ANSI_RESET);
                printf(" %s║ Milestones: Class Test 1 (26/07) • Mid-Term Exam (18/08 to 02/09)          ║%s\n", pal->secondary, ANSI_RESET);
            }
            printf(" %s╚═══════════════════════════════════════════════════════════════════════════╝%s\n\n", pal->primary, ANSI_RESET);
        } else if (strstr(top->title, "[Post-Mid]") && !shown_post_mid) {
            shown_post_mid = true;
            printf("\n %s╔══ MID-TERM EXAMINATIONS & PREPARATORY LEAVE (PL) ══════════════════════════╗%s\n", pal->warning, ANSI_RESET);
            printf(" %s║ PL: 13/08/2026 to 17/08/2026 • Mid-Term Exams: 18/08 to 02/09/2026 (30%%)   ║%s\n", pal->text_bright, ANSI_RESET);
            printf(" %s╚═══════════════════════════════════════════════════════════════════════════╝%s\n\n", pal->warning, ANSI_RESET);

            printf(" %s╔══ PART 2: POST-MID TERM (Weeks 07 - 13 • Lectures %s to 39) ═══════════════╗%s\n",
                   pal->primary, (_stricmp(course_code, "MATH 1107") == 0) ? "20" : "19", ANSI_RESET);
            if (_stricmp(course_code, "MATH 1107") == 0) {
                printf(" %s║ Teacher: Mst. Munny Khatun, Lecturer, Department of Mathematics           ║%s\n", pal->text_bright, ANSI_RESET);
                printf(" %s║ Milestones: Class Test 2 (04/10) • Term Final Exam (12/11/2026, CSE 101)    ║%s\n", pal->secondary, ANSI_RESET);
            } else {
                printf(" %s║ Teacher: Ehsanul Karim, Lecturer, Department of CSE                       ║%s\n", pal->text_bright, ANSI_RESET);
                printf(" %s║ Milestones: Class Test 2 (04/10) • Term Final Exam (09/11 to 23/11)        ║%s\n", pal->secondary, ANSI_RESET);
            }
            printf(" %s╚═══════════════════════════════════════════════════════════════════════════╝%s\n\n", pal->primary, ANSI_RESET);
        }

        /* Print Week grouping header when week changes */
        if (cur_week != last_week) {
            last_week = cur_week;
            printf("\n  %s┌── WEEK %02d %s────────────────────────────────────────────────┐%s\n",
                   pal->accent, cur_week,
                   (cur_week <= 6) ? "(Mid-Term Phase)" : "(Term Final Phase)",
                   ANSI_RESET);
        }

        bool is_last_topic = (i == count - 1 || (shown_pre_mid && !shown_post_mid && i < count - 1 && strstr(state->syllabus[course_topics[i + 1]].title, "[Post-Mid]")));

        const char* branch = is_last_topic ? " └── " : " ├── ";
        const char* pipe   = is_last_topic ? "     " : " │   ";

        const char* stat_col = (top->status == TOPIC_STATUS_COMPLETED) ? pal->success :
                               (top->status == TOPIC_STATUS_IN_PROGRESS) ? pal->warning : pal->text_dim;

        printf("%s%s%s%s %s[%s]%s\n",
               pal->secondary, branch, ANSI_RESET,
               top->title,
               stat_col, topic_status_to_string(top->status), ANSI_RESET);

        for (int s = 0; s < top->subtopic_count; s++) {
            bool is_last_sub = (s == top->subtopic_count - 1);
            const char* sub_branch = is_last_sub ? "└── " : "├── ";
            const SyllabusSubtopic* sub = &top->subtopics[s];

            const char* sub_col = (sub->status == TOPIC_STATUS_COMPLETED) ? pal->success :
                                  (sub->status == TOPIC_STATUS_IN_PROGRESS) ? pal->warning : pal->text_dim;

            printf("%s%s%s%s%s %s[%s]%s\n",
                   pal->secondary, pipe, sub_branch, ANSI_RESET,
                   sub->title,
                   sub_col, topic_status_to_string(sub->status), ANSI_RESET);

            if (strlen(sub->key_points) > 0) {
                const char* sub_pipe = is_last_sub ? "    " : "│   ";
                printf("%s%s%s%s• %s%s\n",
                       pal->secondary, pipe, sub_pipe, pal->text_dim,
                       sub->key_points, ANSI_RESET);
            }
        }

        if (strlen(top->code_snippet) > 0) {
            printf("%s%s[Has Code Example: View via Menu Option 3]%s\n",
                   pal->secondary, pipe, ANSI_RESET);
        }

        if (!is_last_topic) {
            printf("%s │%s\n", pal->secondary, ANSI_RESET);
        }
    }
    printf("\n");
}

void syllabus_browse_tree(const AppState* state) {
    ui_print_header("SYLLABUS TREE BROWSER", "Hierarchical Course Curriculum");
    if (state->course_count == 0) {
        printf("  No courses registered.\n\n");
        ui_pause_key();
        return;
    }
    printf("Select Course:\n");
    for (int i = 0; i < state->course_count; i++) {
        printf("  %d. %s (%s)\n", i + 1, state->courses[i].code, state->courses[i].name);
    }
    printf("  0. Back\n\n");
    int c_sel = ui_input_int("Choice (0 to cancel)", 0, state->course_count, 1);
    if (c_sel == 0) return;

    syllabus_render_tree(state, state->courses[c_sel - 1].code);
    ui_pause_key();
}

void syllabus_toggle_subtopic(AppState* state) {
    ui_print_header("UPDATE TOPIC STATUS", "Track Curriculum Progress");
    if (state->course_count == 0) {
        printf("  No courses registered.\n\n");
        ui_pause_key();
        return;
    }
    printf("Select Course:\n");
    for (int i = 0; i < state->course_count; i++) {
        printf("  %d. %s (%s)\n", i + 1, state->courses[i].code, state->courses[i].name);
    }
    printf("  0. Back\n\n");
    int c_sel = ui_input_int("Choice (0 to cancel)", 0, state->course_count, 1);
    if (c_sel == 0) return;

    const char* code = state->courses[c_sel - 1].code;
    int matches[MAX_TOPICS];
    int count = 0;
    for (int i = 0; i < state->syllabus_count; i++) {
        if (_stricmp(state->syllabus[i].course_code, code) == 0) {
            printf("  %d. Ch %d: %s [%s]\n", count + 1, state->syllabus[i].chapter_num, state->syllabus[i].title, topic_status_to_string(state->syllabus[i].status));
            matches[count++] = i;
        }
    }

    if (count == 0) {
        printf("\n%sNo topics found for %s.%s\n", ANSI_RED, code, ANSI_RESET);
        ui_pause_key();
        return;
    }

    int sel = ui_input_int("Select topic number", 1, count, 1);
    SyllabusTopic* top = &state->syllabus[matches[sel - 1]];

    printf("\nTarget Topic: %s\n", top->title);
    printf("  1. Update Topic Status\n");
    printf("  2. Update Subtopic Status\n");
    int op = ui_input_int("Choice (1-2)", 1, 2, 1);

    if (op == 1) {
        printf("\nSelect Status:\n  1. Pending\n  2. In Progress\n  3. Completed\n");
        int st = ui_input_int("Choice (1-3)", 1, 3, 3);
        top->status = (st == 1) ? TOPIC_STATUS_PENDING : (st == 2) ? TOPIC_STATUS_IN_PROGRESS : TOPIC_STATUS_COMPLETED;
    } else {
        if (top->subtopic_count == 0) {
            printf("No subtopics available!\n");
            ui_pause_key();
            return;
        }
        for (int s = 0; s < top->subtopic_count; s++) {
            printf("  %d. %s [%s]\n", s + 1, top->subtopics[s].title, topic_status_to_string(top->subtopics[s].status));
        }
        int sub_sel = ui_input_int("Select subtopic", 1, top->subtopic_count, 1) - 1;
        printf("\nSelect Status:\n  1. Pending\n  2. In Progress\n  3. Completed\n");
        int st = ui_input_int("Choice (1-3)", 1, 3, 3);
        top->subtopics[sub_sel].status = (st == 1) ? TOPIC_STATUS_PENDING : (st == 2) ? TOPIC_STATUS_IN_PROGRESS : TOPIC_STATUS_COMPLETED;
    }

    storage_save_syllabus();
    printf("\n%sStatus updated successfully!%s\n", ANSI_GREEN, ANSI_RESET);
    ui_pause_key();
}

void syllabus_view_code_snippet(const AppState* state) {
    const ColorPalette* pal = ui_get_palette();
    ui_print_header("TOPIC CODE SNIPPET", "C Programming & Concept Implementation");

    if (state->course_count == 0) {
        printf("  No courses registered.\n\n");
        ui_pause_key();
        return;
    }
    printf("Select Course:\n");
    for (int i = 0; i < state->course_count; i++) {
        printf("  %d. %s (%s)\n", i + 1, state->courses[i].code, state->courses[i].name);
    }
    printf("  0. Back\n\n");
    int c_sel = ui_input_int("Choice (0 to cancel)", 0, state->course_count, 1);
    if (c_sel == 0) return;

    const char* code = state->courses[c_sel - 1].code;
    int matches[MAX_TOPICS];
    int count = 0;
    for (int i = 0; i < state->syllabus_count; i++) {
        if (_stricmp(state->syllabus[i].course_code, code) == 0 && strlen(state->syllabus[i].code_snippet) > 0) {
            printf("  %d. %s\n", count + 1, state->syllabus[i].title);
            matches[count++] = i;
        }
    }

    if (count == 0) {
        printf("  No code snippets available for %s.\n", code);
        ui_pause_key();
        return;
    }

    int choice = ui_input_int("Select topic number", 1, count, 1);
    const SyllabusTopic* top = &state->syllabus[matches[choice - 1]];

    printf("\n%s─── Snippet: %s ──────────────────────────────%s\n", pal->primary, top->title, ANSI_RESET);
    printf("%s%s%s\n", pal->text_bright, top->code_snippet, ANSI_RESET);
    printf("%s─────────────────────────────────────────────────────────%s\n\n", pal->primary, ANSI_RESET);

    ui_pause_key();
}

void syllabus_display_progress_inline(const AppState* state) {
    const ColorPalette* pal = ui_get_palette();
    printf("  %s┌── SYLLABUS PROGRESS & COMPLETION RATES ───────────────────────────────────┐%s\n", pal->primary, ANSI_RESET);

    for (int c = 0; c < state->course_count; c++) {
        const Course* course = &state->courses[c];
        int total = 0;
        int completed = 0;

        for (int i = 0; i < state->syllabus_count; i++) {
            if (_stricmp(state->syllabus[i].course_code, course->code) == 0) {
                total++;
                if (state->syllabus[i].status == TOPIC_STATUS_COMPLETED) completed++;
            }
        }

        float pct = (total > 0) ? ((float)completed / total) * 100.0f : 0.0f;
        char bar[64];
        ui_make_progress_bar(bar, sizeof(bar), pct, 12);

        printf("    %s%-10s%s │ %-26s │ %s (%d/%d topics)\n",
               pal->primary, course->code, ANSI_RESET,
               course->name,
               bar, completed, total);
    }
    printf("  %s└───────────────────────────────────────────────────────────────────────────┘%s\n\n", pal->primary, ANSI_RESET);
}

void syllabus_show_completion_summary(const AppState* state) {
    ui_print_header("SYLLABUS PROGRESS", "Course Completion Rates");
    syllabus_display_progress_inline(state);
    ui_pause_key();
}

void syllabus_menu(AppState* state) {
    while (1) {
        ui_print_header("SYLLABUS BROWSER", "Topics & Learning Progression");
        syllabus_display_progress_inline(state);

        const ColorPalette* pal = ui_get_palette();
        printf("  %s--- Other Options ---%s\n", pal->secondary, ANSI_RESET);
        printf("  1. Browse Course Syllabus Tree (Lectures & Topics)\n");
        printf("  2. Update Topic / Subtopic Progress Tracker\n");
        printf("  3. View Topic C Code Snippet / Key Formulas\n");
        printf("  0. Back to Main Menu\n\n");

        int choice = ui_input_int("Enter choice (0-3)", 0, 3, 1);
        switch (choice) {
            case 1: syllabus_browse_tree(state); break;
            case 2: syllabus_toggle_subtopic(state); break;
            case 3: syllabus_view_code_snippet(state); break;
            case 0: return;
        }
    }
}
