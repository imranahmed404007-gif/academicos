#include "academic_os.h"

const Course* courses_find_by_code(const AppState* state, const char* code) {
    for (int i = 0; i < state->course_count; i++) {
        if (_stricmp(state->courses[i].code, code) == 0) {
            return &state->courses[i];
        }
    }
    return NULL;
}

float courses_calculate_total_credits(const AppState* state) {
    float total = 0.0f;
    for (int i = 0; i < state->course_count; i++) {
        total += state->courses[i].credit;
    }
    return total;
}

void courses_display_catalogue_inline(const AppState* state) {
    const ColorPalette* pal = ui_get_palette();
    printf("  %s┌── REGISTERED COURSE CATALOGUE ───────────────────────────────────────────┐%s\n", pal->primary, ANSI_RESET);
    printf("    %-10s │ %-4s │ %-32s │ %-24s\n", "Code", "Cr", "Course Title", "Instructor");
    printf("    ───────────┼──────┼──────────────────────────────────┼─────────────────────────\n");

    for (int i = 0; i < state->course_count; i++) {
        const Course* c = &state->courses[i];
        printf("    %s%-10s%s │ %4.2f │ %-32s │ %s%s%s\n",
               pal->primary, c->code, ANSI_RESET,
               c->credit,
               c->name,
               pal->text_dim, c->teacher, ANSI_RESET);
    }

    printf("    ───────────┴──────┴──────────────────────────────────┴─────────────────────────\n");
    printf("    Total Courses: %s%d%s    Total Credits: %s%.2f%s\n",
           pal->text_bright, state->course_count, ANSI_RESET,
           pal->success, courses_calculate_total_credits(state), ANSI_RESET);
    printf("  %s└───────────────────────────────────────────────────────────────────────────┘%s\n\n", pal->primary, ANSI_RESET);
}

void courses_view_all(const AppState* state) {
    ui_print_header("COURSE CATALOGUE", "KUET CSE Academic Courses");
    courses_display_catalogue_inline(state);
    ui_pause_key();
}

void courses_display_detailed_plan(const AppState* state, const char* course_code, bool is_pre_mid) {
    const ColorPalette* pal = ui_get_palette();
    const Course* c = courses_find_by_code(state, course_code);

    char title_buf[64];
    snprintf(title_buf, sizeof(title_buf), "%s TERM COURSE PLAN", is_pre_mid ? "PRE-MID" : "POST-MID");
    ui_print_header(title_buf, "Official KUET CSE Curriculum");

    /* Institutional Header Card */
    printf(" %s╔═════════════════════════════════════════════════════════════════════════╗%s\n", pal->primary, ANSI_RESET);
    printf(" %s║ KHULNA UNIVERSITY OF ENGINEERING & TECHNOLOGY (KUET)                   ║%s\n", pal->text_bright, ANSI_RESET);
    printf(" %s║ Department of Computer Science and Engineering                         ║%s\n", pal->secondary, ANSI_RESET);
    printf(" %s╠═════════════════════════════════════════════════════════════════════════╣%s\n", pal->primary, ANSI_RESET);
    printf("   %sCourse Code%s    : %s%-12s%s (%.2f Credits, 3.0 Hours/Week)\n",
           pal->secondary, ANSI_RESET, pal->primary, course_code, ANSI_RESET, c ? c->credit : 3.0f);
    printf("   %sCourse Title%s   : %s%s%s\n",
           pal->secondary, ANSI_RESET, pal->text_bright, c ? c->name : "Structured Programming", ANSI_RESET);

    if (is_pre_mid) {
        printf("   %sTerm Period%s    : %sPre-Mid Term (Lectures 01 to 18)%s\n", pal->secondary, ANSI_RESET, pal->text_bright, ANSI_RESET);
        printf("   %sCourse Teacher%s : %sMd Tajmilur Rahman%s, Lecturer, Department of CSE\n", pal->secondary, ANSI_RESET, pal->text_bright, ANSI_RESET);
        printf("   %sMilestone%s      : %sClass Test 1 (26/07 Sec B | 27/07 Sec A)%s\n", pal->secondary, ANSI_RESET, pal->warning, ANSI_RESET);
        printf("   %sMid-Term Exam%s  : %s18/08/2026 to 02/09/2026%s (PL: 13/08 to 17/08)\n", pal->secondary, ANSI_RESET, pal->warning, ANSI_RESET);
        printf("   %sReference Books%s: 1. Let Us C (Yashwant Kanetkar)\n", pal->secondary, ANSI_RESET);
        printf("                      2. Teach Yourself C (Herbert Schildt)\n");
        printf("                      3. Programming in ANSI C (E. Balagurusamy)\n");
    } else {
        printf("   %sTerm Period%s    : %sPost-Mid Term (Lectures 19 to 39)%s\n", pal->secondary, ANSI_RESET, pal->text_bright, ANSI_RESET);
        printf("   %sCourse Teacher%s : %sEhsanul Karim%s, Lecturer (part-time), Department of CSE\n", pal->secondary, ANSI_RESET, pal->text_bright, ANSI_RESET);
        printf("   %sMilestone%s      : %sClass Test 2 (04/10 Sec B | 05/10 Sec A)%s\n", pal->secondary, ANSI_RESET, pal->warning, ANSI_RESET);
        printf("   %sTerm Final Exam%s: %s09/11/2026 to 23/11/2026%s (PL: 04/11 to 08/11)\n", pal->secondary, ANSI_RESET, pal->warning, ANSI_RESET);
        printf("   %sReference Books%s: 1. Programming in ANSI C (E. Balagurusamy)\n", pal->secondary, ANSI_RESET);
        printf("                      2. Teach Yourself C (Herbert Schildt)\n");
        printf("                      3. Let Us C (Yashwant Kanetkar)\n");
    }
    printf(" %s╚═════════════════════════════════════════════════════════════════════════╝%s\n\n", pal->primary, ANSI_RESET);

    /* Gather matching topics */
    int matched_topics[MAX_TOPICS];
    int count = 0;
    for (int i = 0; i < state->syllabus_count; i++) {
        if (_stricmp(state->syllabus[i].course_code, course_code) == 0) {
            bool matches_term = is_pre_mid ?
                (strstr(state->syllabus[i].title, "[Pre-Mid]") != NULL || state->syllabus[i].chapter_num <= 18) :
                (strstr(state->syllabus[i].title, "[Post-Mid]") != NULL || state->syllabus[i].chapter_num >= 19);

            if (matches_term) {
                matched_topics[count++] = i;
            }
        }
    }

    if (count == 0) {
        printf("  %sNo registered syllabus topics found for this course term.%s\n\n", pal->text_dim, ANSI_RESET);
        ui_pause_key();
        return;
    }

    int current_week = -1;
    for (int i = 0; i < count; i++) {
        const SyllabusTopic* top = &state->syllabus[matched_topics[i]];

        /* Calculate Week */
        int week_num = 1;
        if (top->chapter_num <= 3) week_num = 1;
        else if (top->chapter_num <= 6) week_num = 2;
        else if (top->chapter_num <= 9) week_num = 3;
        else if (top->chapter_num <= 12) week_num = 4;
        else if (top->chapter_num <= 15) week_num = 5;
        else if (top->chapter_num <= 18) week_num = 6;
        else if (top->chapter_num <= 21) week_num = 7;
        else if (top->chapter_num <= 24) week_num = 8;
        else if (top->chapter_num <= 27) week_num = 9;
        else if (top->chapter_num <= 30) week_num = 10;
        else if (top->chapter_num <= 33) week_num = 11;
        else if (top->chapter_num <= 36) week_num = 12;
        else week_num = 13;

        if (week_num != current_week) {
            current_week = week_num;
            printf(" %s─────────────────────────────────────────────────────────────────────────%s\n", pal->secondary, ANSI_RESET);
            printf("  %sWEEK %02d%s\n", pal->primary, week_num, ANSI_RESET);
            printf(" %s─────────────────────────────────────────────────────────────────────────%s\n", pal->secondary, ANSI_RESET);
        }

        /* Clean display title */
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
            printf("    %sMilestone  %s: %s★ CLASS TEST 1 (CT-1) - Recursion & Pre-Mid Basics%s\n",
                   pal->secondary, ANSI_RESET, pal->warning, ANSI_RESET);
        } else if (top->chapter_num == 29) {
            printf("    %sMilestone  %s: %s★ CLASS TEST 2 (CT-2) - Structures & User Defined Types%s\n",
                   pal->secondary, ANSI_RESET, pal->warning, ANSI_RESET);
        } else if (top->chapter_num == 38) {
            printf("    %sMilestone  %s: %s★ TERM FINAL EXAM REVISION (Pre-Mid Fundamentals)%s\n",
                   pal->secondary, ANSI_RESET, pal->warning, ANSI_RESET);
        } else if (top->chapter_num == 39) {
            printf("    %sMilestone  %s: %s★ TERM FINAL EXAM REVISION (Post-Mid Advanced C)%s\n",
                   pal->secondary, ANSI_RESET, pal->warning, ANSI_RESET);
        }

        printf("\n");
    }

    /* Term Examination Footer */
    printf(" %s╔═════════════════════════════════════════════════════════════════════════╗%s\n", pal->primary, ANSI_RESET);
    if (is_pre_mid) {
        printf(" %s║ PRE-MID TERM EXAMINATION MILESTONES & TIMELINE                          ║%s\n", pal->text_bright, ANSI_RESET);
        printf(" %s╠═════════════════════════════════════════════════════════════════════════╣%s\n", pal->primary, ANSI_RESET);
        printf("   • %sClass Test 1 (CT-1)%s         : 26/07/2026 (Sec B) │ 27/07/2026 (Sec A)\n", pal->text_bright, ANSI_RESET);
        printf("   • %sPreparatory Leave (PL)%s      : 13/08/2026 to 17/08/2026 (5 Days)\n", pal->text_bright, ANSI_RESET);
        printf("   • %sMid-Term Examination Period%s : 18/08/2026 to 02/09/2026\n", pal->text_bright, ANSI_RESET);
        printf("   • %sExam Dates%s                  : 18/08, 22/08, 25/08, 29/08, and 02/09\n", pal->text_bright, ANSI_RESET);
    } else {
        printf(" %s║ POST-MID TERM EXAMINATION MILESTONES & TIMELINE                         ║%s\n", pal->text_bright, ANSI_RESET);
        printf(" %s╠═════════════════════════════════════════════════════════════════════════╣%s\n", pal->primary, ANSI_RESET);
        printf("   • %sClass Test 2 (CT-2)%s         : 04/10/2026 (Sec B) │ 05/10/2026 (Sec A)\n", pal->text_bright, ANSI_RESET);
        printf("   • %sPreparatory Leave (PL)%s      : 04/11/2026 to 08/11/2026 (5 Days)\n", pal->text_bright, ANSI_RESET);
        printf("   • %sTerm Final Examination Period%s: 09/11/2026 to 23/11/2026\n", pal->text_bright, ANSI_RESET);
        printf("   • %sExam Dates%s                  : 09/11, 12/11, 16/11, 20/11, and 24/11\n", pal->text_bright, ANSI_RESET);
    }
    printf(" %s╚═════════════════════════════════════════════════════════════════════════╝%s\n\n", pal->primary, ANSI_RESET);

    ui_pause_key();
}

static void courses_view_schedule_and_exams(const AppState* state, const Course* c) {
    const ColorPalette* pal = ui_get_palette();

    char title_buf[64];
    snprintf(title_buf, sizeof(title_buf), "%s TIMETABLE & EXAMS", c->code);
    ui_print_header(title_buf, c->name);

    printf("\n%s╔═════════════════════════════════════════════════════════════════════╗%s\n", pal->primary, ANSI_RESET);
    printf("  %sCourse Code%s    : %s%-12s%s (%.2f Credits)\n", pal->secondary, ANSI_RESET, pal->primary, c->code, ANSI_RESET, c->credit);
    printf("  %sCourse Title%s   : %s%s%s\n", pal->secondary, ANSI_RESET, pal->text_bright, c->name, ANSI_RESET);
    printf("  %sFaculty/Teacher%s: %s\n", pal->secondary, ANSI_RESET, c->teacher[0] ? c->teacher : "Not Specified");
    printf("  %sOverview%s       : %s\n", pal->secondary, ANSI_RESET, c->notes[0] ? c->notes : "1st Year 1st Term");
    printf("%s╚═════════════════════════════════════════════════════════════════════╝%s\n\n", pal->primary, ANSI_RESET);

    /* 1. Associated Timetable Classes */
    printf("%s── SCHEDULED TIMETABLE CLASSES ──────────────────────────────────────────%s\n\n", pal->secondary, ANSI_RESET);
    int cls_count = 0;
    for (int i = 0; i < state->class_count; i++) {
        if (_stricmp(state->classes[i].course_code, c->code) == 0) {
            printf("  • %-9s : %02d:%02d - %02d:%02d  │ %-7s │ Room: %s\n",
                   day_to_string(state->classes[i].day_of_week),
                   state->classes[i].start_hour, state->classes[i].start_minute,
                   state->classes[i].end_hour, state->classes[i].end_minute,
                   class_type_to_string(state->classes[i].class_type),
                   state->classes[i].room);
            cls_count++;
        }
    }
    if (cls_count == 0) {
        printf("  %s(No weekly classes registered for this course)%s\n", pal->text_dim, ANSI_RESET);
    }
    printf("\n");

    /* 2. Associated Scheduled Exams */
    printf("%s── SCHEDULED ASSESSMENTS & EXAMS ────────────────────────────────────────%s\n\n", pal->secondary, ANSI_RESET);
    int ex_count = 0;
    for (int i = 0; i < state->exam_count; i++) {
        if (_stricmp(state->exams[i].course_code, c->code) == 0) {
            printf("  • %-36s │ %02d/%02d/%d │ %-10s │ %s\n",
                   state->exams[i].title,
                   state->exams[i].day, state->exams[i].month, state->exams[i].year,
                   exam_type_to_string(state->exams[i].type),
                   state->exams[i].room);
            ex_count++;
        }
    }
    if (ex_count == 0) {
        printf("  %s(No upcoming exams scheduled for this course)%s\n", pal->text_dim, ANSI_RESET);
    }
    printf("\n");
    ui_pause_key();
}

void courses_view_detail(const AppState* state) {
    const ColorPalette* pal = ui_get_palette();
    ui_print_header("COURSE DETAILS & PLANS", "Select Course to Inspect");

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

    int c_choice = ui_input_int("Choice (0 to cancel)", 0, state->course_count, 1);
    if (c_choice == 0) return;

    const Course* c = &state->courses[c_choice - 1];

    while (1) {
        ui_clear_screen();

        printf("%s╔═════════════════════════════════════════════════════════════════════╗%s\n", pal->primary, ANSI_RESET);
        printf("  %sCourse Code%s    : %s%-12s%s (%.2f Credits)\n", pal->secondary, ANSI_RESET, pal->primary, c->code, ANSI_RESET, c->credit);
        printf("  %sCourse Title%s   : %s%s%s\n", pal->secondary, ANSI_RESET, pal->text_bright, c->name, ANSI_RESET);
        printf("  %sFaculty/Teacher%s: %s\n", pal->secondary, ANSI_RESET, c->teacher[0] ? c->teacher : "Not Specified");
        printf("  %sOverview%s       : %s\n", pal->secondary, ANSI_RESET, c->notes[0] ? c->notes : "1st Year 1st Term");
        printf("%s╚═════════════════════════════════════════════════════════════════════╝%s\n\n", pal->primary, ANSI_RESET);

        printf("  1. Pre-Mid Term Course Plan (Detailed)\n");
        printf("  2. Post-Mid Term Course Plan (Detailed)\n");
        printf("  3. View Timetable Classes & Exam Schedule\n");
        printf("  0. Back to Courses Menu\n\n");

        int opt = ui_input_int("Enter choice (0-3)", 0, 3, 1);
        if (opt == 1) {
            courses_display_detailed_plan(state, c->code, true);
        } else if (opt == 2) {
            courses_display_detailed_plan(state, c->code, false);
        } else if (opt == 3) {
            courses_view_schedule_and_exams(state, c);
        } else {
            break;
        }
    }
}

void courses_add(AppState* state) {
    if (state->course_count >= MAX_COURSES) {
        printf("%sCourse catalogue is full!%s\n", ANSI_RED, ANSI_RESET);
        ui_pause_key();
        return;
    }

    ui_print_header("ADD COURSE", "Register New Course");
    printf("Select Course to add:\n");
    printf("  1. CSE 1200 - Analytical Programming (1.50 Cr)\n");
    printf("  2. CSE 1201 - Data Structures (3.00 Cr)\n");
    printf("  3. CSE 1202 - Data Structures Sessional (1.50 Cr)\n");
    printf("  4. MATH 1207 - Mathematics II (Calculus & ODE) (3.00 Cr)\n");
    printf("  5. CHEM 1107 - Chemistry (3.00 Cr)\n");
    printf("  6. Custom Course Entry\n");
    printf("  0. Cancel\n\n");

    int sel = ui_input_int("Choice (0-6)", 0, 6, 1);
    if (sel == 0) return;

    Course nc;
    memset(&nc, 0, sizeof(Course));

    int max_id = 0;
    for (int i = 0; i < state->course_count; i++) {
        if (state->courses[i].id > max_id) max_id = state->courses[i].id;
    }
    nc.id = max_id + 1;

    if (sel == 1) {
        snprintf(nc.code, sizeof(nc.code), "CSE 1200");
        snprintf(nc.name, sizeof(nc.name), "Analytical Programming");
        nc.credit = 1.50f;
    } else if (sel == 2) {
        snprintf(nc.code, sizeof(nc.code), "CSE 1201");
        snprintf(nc.name, sizeof(nc.name), "Data Structures");
        nc.credit = 3.00f;
    } else if (sel == 3) {
        snprintf(nc.code, sizeof(nc.code), "CSE 1202");
        snprintf(nc.name, sizeof(nc.name), "Data Structures Sessional");
        nc.credit = 1.50f;
    } else if (sel == 4) {
        snprintf(nc.code, sizeof(nc.code), "MATH 1207");
        snprintf(nc.name, sizeof(nc.name), "Mathematics II");
        nc.credit = 3.00f;
    } else if (sel == 5) {
        snprintf(nc.code, sizeof(nc.code), "CHEM 1107");
        snprintf(nc.name, sizeof(nc.name), "Chemistry");
        nc.credit = 3.00f;
    } else {
        ui_input_string("Course Code", nc.code, sizeof(nc.code), "CSE 1109");
        ui_input_string("Course Name", nc.name, sizeof(nc.name), "Algorithms");
        nc.credit = ui_input_float("Credit Hours", 0.5f, 6.0f, 3.0f);
    }
    nc.teacher[0] = '\0';
    nc.notes[0] = '\0';

    state->courses[state->course_count++] = nc;
    storage_save_courses();

    printf("\n%sCourse '%s' added successfully!%s\n", ANSI_GREEN, nc.code, ANSI_RESET);
    ui_pause_key();
}

void courses_delete(AppState* state) {
    ui_print_header("DELETE COURSE", "Remove Course from Catalogue");
    if (state->course_count == 0) {
        printf("  No courses registered to delete.\n\n");
        ui_pause_key();
        return;
    }

    printf("Select Course to delete:\n");
    for (int i = 0; i < state->course_count; i++) {
        printf("  %d. %s - %s (%.2f Cr)\n", i + 1, state->courses[i].code, state->courses[i].name, state->courses[i].credit);
    }
    printf("  0. Cancel\n\n");

    int c_choice = ui_input_int("Choice (0 to cancel)", 0, state->course_count, 0);
    if (c_choice == 0) return;

    int idx = c_choice - 1;
    bool confirm = ui_input_bool("Are you sure you want to delete this course? (Y/N)", false);
    if (confirm) {
        for (int i = idx; i < state->course_count - 1; i++) {
            state->courses[i] = state->courses[i + 1];
        }
        state->course_count--;
        storage_save_courses();
        printf("\n%sCourse deleted successfully.%s\n", ANSI_GREEN, ANSI_RESET);
    }
    ui_pause_key();
}

void courses_menu(AppState* state) {
    while (1) {
        ui_print_header("COURSES MANAGEMENT", "Catalogue & Curriculum");
        courses_display_catalogue_inline(state);

        const ColorPalette* pal = ui_get_palette();
        printf("  %s--- Other Options ---%s\n", pal->secondary, ANSI_RESET);
        printf("  1. View Detailed Course Plans (Pre-Mid / Post-Mid / Schedule)\n");
        printf("  2. Add New Course\n");
        printf("  3. Delete Course\n");
        printf("  0. Back to Main Menu\n\n");

        int choice = ui_input_int("Enter choice (0-3)", 0, 3, 1);
        switch (choice) {
            case 1: courses_view_detail(state); break;
            case 2: courses_add(state); break;
            case 3: courses_delete(state); break;
            case 0: return;
        }
    }
}
