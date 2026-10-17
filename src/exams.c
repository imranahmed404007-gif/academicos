#include "academic_os.h"

static const char* clean_title(const char* title) {
    if (strncmp(title, "Term Final: ", 12) == 0) return title + 12;
    if (strncmp(title, "Midterm: ", 9) == 0) return title + 9;
    if (strncmp(title, "Lab Exam: ", 10) == 0) return title + 10;
    return title;
}

static const char* clean_notes(const char* notes) {
    /* If notes begins with e.g. "Term Final Date 1 (Monday). ", extract the core scope */
    const char* dot = strstr(notes, ". ");
    if (dot && (strncmp(notes, "Term Final Date", 15) == 0 || strncmp(notes, "Midterm Exam Date", 17) == 0)) {
        return dot + 2;
    }
    return notes;
}

static void print_exam_row(int index, const Exam* ex, const struct tm* now_tm, int style, bool is_24_hour) {
    const ColorPalette* pal = ui_get_palette();
    long diff = time_engine_calc_exam_diff(ex, now_tm);

    char cd_str[64];
    time_engine_format_countdown_str(diff, style, cd_str, sizeof(cd_str));

    char time_str[16];
    time_format_time_val(time_str, sizeof(time_str), ex->hour, ex->minute, is_24_hour);

    const char* type_color = pal->primary;
    if (ex->type == EXAM_TYPE_MIDTERM || ex->type == EXAM_TYPE_FINAL) {
        type_color = pal->danger;
    } else if (ex->type == EXAM_TYPE_LAB_EXAM) {
        type_color = pal->warning;
    } else if (ex->type == EXAM_TYPE_QUIZ || ex->type == EXAM_TYPE_CT) {
        type_color = pal->accent;
    }

    const char* display_title = clean_title(ex->title);
    const char* display_notes = clean_notes(ex->notes);

    /* 1. Header: [Index] Code • Title (Type) */
    if (index > 0) {
        printf("  %s[%d]%s %s%s%s • %s%s%s (%s%s%s)\n",
               pal->primary, index, ANSI_RESET,
               pal->primary, ex->course_code, ANSI_RESET,
               pal->text_bright, display_title, ANSI_RESET,
               type_color, exam_type_to_string(ex->type), ANSI_RESET);
    } else {
        printf("  %s%s%s • %s%s%s (%s%s%s)\n",
               pal->primary, ex->course_code, ANSI_RESET,
               pal->text_bright, display_title, ANSI_RESET,
               type_color, exam_type_to_string(ex->type), ANSI_RESET);
    }

    /* 2. Schedule: Date at Time • Room */
    printf("      Date      : %02d/%02d/%d at %s  •  Room %s\n",
           ex->day, ex->month, ex->year, time_str, ex->room);

    /* 3. Countdown */
    if (!ex->is_completed && ex->is_enabled) {
        if (diff > 0) {
            printf("      Countdown : %s%s%s\n", pal->warning, cd_str, ANSI_RESET);
        } else {
            printf("      Countdown : %sConcluded%s\n", pal->text_dim, ANSI_RESET);
        }
    }

    /* 4. Topics / Notes */
    if (strlen(display_notes) > 0) {
        printf("      Topics    : %s%s%s\n", pal->text_dim, display_notes, ANSI_RESET);
    }

    printf("\n");
}

void exams_display_upcoming_inline(const AppState* state) {
    const ColorPalette* pal = ui_get_palette();
    struct tm now_tm = time_get_localtime();

    printf("  %s┌── UPCOMING EXAMS (LIVE COUNTDOWNS) ──────────────────────────────────┐%s\n", pal->primary, ANSI_RESET);

    int count = 0;
    for (int i = 0; i < state->exam_count; i++) {
        const Exam* ex = &state->exams[i];
        if (ex->is_enabled && !ex->is_completed) {
            long diff = time_engine_calc_exam_diff(ex, &now_tm);
            if (diff >= 0) {
                count++;
                print_exam_row(count, ex, &now_tm, state->settings.countdown_style, state->settings.is_24_hour);
            }
        }
    }

    if (count == 0) {
        printf("    %sNo upcoming exams found! All assessments completed.%s\n\n", pal->text_dim, ANSI_RESET);
    }
    printf("  %s└───────────────────────────────────────────────────────────────────────┘%s\n\n", pal->primary, ANSI_RESET);
}

void exams_view_upcoming(const AppState* state) {
    ui_print_header("UPCOMING EXAMS", "Live Countdowns & Schedules");
    exams_display_upcoming_inline(state);
    ui_pause_key();
}

void exams_view_past(const AppState* state) {
    ui_print_header("PAST EXAMS", "Completed or Concluded Assessments");
    struct tm now_tm = time_get_localtime();

    int count = 0;
    for (int i = 0; i < state->exam_count; i++) {
        const Exam* ex = &state->exams[i];
        long diff = time_engine_calc_exam_diff(ex, &now_tm);
        if (ex->is_completed || diff < 0) {
            count++;
            print_exam_row(count, ex, &now_tm, state->settings.countdown_style, state->settings.is_24_hour);
        }
    }

    if (count == 0) {
        printf("  No past exams recorded.\n\n");
    }
    ui_pause_key();
}

void exams_view_all(const AppState* state) {
    ui_print_header("ALL EXAMS", "Catalogue of all Registered Exams");
    struct tm now_tm = time_get_localtime();

    for (int i = 0; i < state->exam_count; i++) {
        print_exam_row(i + 1, &state->exams[i], &now_tm, state->settings.countdown_style, state->settings.is_24_hour);
    }

    if (state->exam_count == 0) {
        printf("  No exams found!\n\n");
    }
    ui_pause_key();
}

void exams_view_term_finals(const AppState* state) {
    const ColorPalette* pal = ui_get_palette();
    ui_print_header("TERM FINAL EXAMINATIONS", "Official Academic Calendar Schedule");
    struct tm now_tm = time_get_localtime();

    printf("  %s%s%s\n", pal->primary, "================================================================================", ANSI_RESET);
    printf("  %s%-12s │ %-14s │ %-10s │ %-24s │ %s%s\n",
           pal->text_bright, "Date", "Day", "Course", "Title / Subject", "Remaining", ANSI_RESET);
    printf("  %s%s%s\n", pal->primary, "--------------------------------------------------------------------------------", ANSI_RESET);

    int count = 0;
    for (int i = 0; i < state->exam_count; i++) {
        const Exam* ex = &state->exams[i];
        if (ex->type == EXAM_TYPE_FINAL) {
            long diff = time_engine_calc_exam_diff(ex, &now_tm);
            char cd_str[64];
            time_engine_format_countdown_str(diff, 1, cd_str, sizeof(cd_str));

            char date_str[16];
            snprintf(date_str, sizeof(date_str), "%02d/%02d/%d", ex->day, ex->month, ex->year);

            const char* day_name = (ex->day == 9) ? "Monday" :
                                   (ex->day == 12) ? "Thursday" :
                                   (ex->day == 15) ? "Sunday" :
                                   (ex->day == 19) ? "Thursday" :
                                   (ex->day == 23) ? "Monday" : "";

            printf("  %-12s │ %-14s │ %s%-10s%s │ %-24s │ %s%s%s\n",
                   date_str, day_name,
                   pal->text_bright, ex->course_code, ANSI_RESET,
                   ex->course_name,
                   (diff > 0) ? pal->warning : pal->text_dim,
                   (diff > 0) ? cd_str : "Concluded",
                   ANSI_RESET);
            count++;
        }
    }
    printf("  %s%s%s\n\n", pal->primary, "================================================================================", ANSI_RESET);

    if (count == 0) {
        printf("  No Term Final examinations registered.\n\n");
    } else {
        printf("  %sNote: Exam preparation leave (PL) runs from 04/11/2026 to 08/11/2026.%s\n\n", pal->text_dim, ANSI_RESET);
    }
    ui_pause_key();
}

void exams_add(AppState* state) {
    if (state->exam_count >= MAX_EXAMS) {
        printf("%sExam storage limit reached!%s\n", ANSI_RED, ANSI_RESET);
        ui_pause_key();
        return;
    }

    ui_print_header("ADD EXAM", "Schedule New Assessment");
    Exam ne;
    memset(&ne, 0, sizeof(Exam));

    int max_id = 0;
    for (int i = 0; i < state->exam_count; i++) {
        if (state->exams[i].id > max_id) max_id = state->exams[i].id;
    }
    ne.id = max_id + 1;

    /* 1. Select Course from registered courses */
    printf("Select Course:\n");
    for (int i = 0; i < state->course_count; i++) {
        printf("  %d. %s (%s)\n", i + 1, state->courses[i].code, state->courses[i].name);
    }
    int c_choice = ui_input_int("Choice", 1, state->course_count, 1);
    snprintf(ne.course_code, sizeof(ne.course_code), "%s", state->courses[c_choice - 1].code);
    snprintf(ne.course_name, sizeof(ne.course_name), "%s", state->courses[c_choice - 1].name);

    /* 2. Select Exam Type */
    printf("\nSelect Exam Type:\n");
    printf("  1. Class Test (CT)\n");
    printf("  2. Midterm Examination\n");
    printf("  3. Term Final Examination\n");
    printf("  4. Lab Exam / Practical Assessment\n");
    int type_sel = ui_input_int("Choice (1-4)", 1, 4, 1);

    if (type_sel == 1) {
        ne.type = EXAM_TYPE_CT;
        printf("\nSelect CT Number:\n  1. CT-1\n  2. CT-2\n  3. CT-3\n  4. CT-4\n");
        int ct_num = ui_input_int("Choice (1-4)", 1, 4, 1);
        snprintf(ne.title, sizeof(ne.title), "CT-%d: %s", ct_num, ne.course_code);
    } else if (type_sel == 2) {
        ne.type = EXAM_TYPE_MIDTERM;
        snprintf(ne.title, sizeof(ne.title), "Midterm: %s", ne.course_name);
    } else if (type_sel == 3) {
        ne.type = EXAM_TYPE_FINAL;
        snprintf(ne.title, sizeof(ne.title), "Term Final: %s", ne.course_name);
    } else {
        ne.type = EXAM_TYPE_LAB_EXAM;
        snprintf(ne.title, sizeof(ne.title), "Lab Exam: %s", ne.course_name);
    }

    /* 3. Date Selection */
    struct tm now = time_get_localtime();
    ne.year = now.tm_year + 1900;

    printf("\nSelect Month:\n");
    const char* months[] = {"January", "February", "March", "April", "May", "June",
                            "July", "August", "September", "October", "November", "December"};
    for (int i = 0; i < 12; i++) {
        printf("  %2d. %s\n", i + 1, months[i]);
    }
    ne.month = ui_input_int("Choice (1-12)", 1, 12, now.tm_mon + 1);

    int max_days = 31;
    if (ne.month == 2) max_days = 28;
    else if (ne.month == 4 || ne.month == 6 || ne.month == 9 || ne.month == 11) max_days = 30;
    ne.day = ui_input_int("Day of Month", 1, max_days, now.tm_mday);

    /* 4. Select Exam Starting Time */
    printf("\nSelect Exam Starting Time:\n");
    printf("  1. 10:00 AM (Morning Slot)\n");
    printf("  2. 02:00 PM (Afternoon Slot)\n");
    printf("  3. 02:30 PM (Lab Slot)\n");
    printf("  4. 08:50 AM (First Period)\n");
    int time_sel = ui_input_int("Choice (1-4)", 1, 4, 1);
    if (time_sel == 1) { ne.hour = 10; ne.minute = 0; }
    else if (time_sel == 2) { ne.hour = 14; ne.minute = 0; }
    else if (time_sel == 3) { ne.hour = 14; ne.minute = 30; }
    else { ne.hour = 8; ne.minute = 50; }

    /* 5. Select Venue / Room */
    printf("\nSelect Venue / Room:\n");
    printf("  1. CSE 101 (Main Theory Room)\n");
    printf("  2. CSE 102 (Thursday Room)\n");
    printf("  3. CSE 104 (Programming Lab)\n");
    printf("  4. Physics Lab\n");
    printf("  5. Language Lab\n");
    int room_sel = ui_input_int("Choice (1-5)", 1, 5, 1);
    if (room_sel == 1) snprintf(ne.room, sizeof(ne.room), "CSE 101");
    else if (room_sel == 2) snprintf(ne.room, sizeof(ne.room), "CSE 102");
    else if (room_sel == 3) snprintf(ne.room, sizeof(ne.room), "CSE 104");
    else if (room_sel == 4) snprintf(ne.room, sizeof(ne.room), "Physics Lab");
    else snprintf(ne.room, sizeof(ne.room), "Language Lab");

    ne.total_marks = (ne.type == EXAM_TYPE_FINAL) ? 60.0f : (ne.type == EXAM_TYPE_MIDTERM) ? 30.0f : 20.0f;
    ne.weightage = (ne.type == EXAM_TYPE_FINAL) ? 40.0f : (ne.type == EXAM_TYPE_MIDTERM) ? 30.0f : 15.0f;

    ne.is_completed = false;
    ne.is_enabled = true;

    state->exams[state->exam_count++] = ne;
    storage_save_exams();

    printf("\n%sExam scheduled successfully without typing!%s\n", ANSI_GREEN, ANSI_RESET);
    ui_pause_key();
}

void exams_delete(AppState* state) {
    ui_print_header("DELETE EXAM", "Remove Assessment");
    if (state->exam_count == 0) {
        printf("  No exams registered to delete.\n\n");
        ui_pause_key();
        return;
    }

    printf("Select Exam to delete:\n");
    for (int i = 0; i < state->exam_count; i++) {
        Exam* ex = &state->exams[i];
        printf("  %2d. [%-8s] %-28s (%02d/%02d/%d) %s\n",
               i + 1, ex->course_code, ex->title, ex->day, ex->month, ex->year,
               ex->is_completed ? "[Done]" : "[Upcoming]");
    }
    printf("   0. Cancel\n\n");

    int choice = ui_input_int("Enter choice (0 to cancel)", 0, state->exam_count, 0);
    if (choice == 0) return;

    int idx = choice - 1;
    bool confirm = ui_input_bool("Are you sure you want to delete this exam? (Y/N)", false);
    if (confirm) {
        for (int i = idx; i < state->exam_count - 1; i++) {
            state->exams[i] = state->exams[i + 1];
        }
        state->exam_count--;
        storage_save_exams();
        printf("\n%sExam deleted successfully.%s\n", ANSI_GREEN, ANSI_RESET);
    }
    ui_pause_key();
}

void exams_menu(AppState* state) {
    while (1) {
        ui_print_header("EXAMS & ASSESSMENTS", "Countdowns & Schedule");
        exams_display_upcoming_inline(state);

        const ColorPalette* pal = ui_get_palette();
        printf("  %s--- Other Options ---%s\n", pal->secondary, ANSI_RESET);
        printf("  1. Term Final Examination Schedule (Official Dates)\n");
        printf("  2. View All Registered Exams\n");
        printf("  3. Add New Exam / Assessment\n");
        printf("  4. Delete Exam\n");
        printf("  0. Back to Main Menu\n\n");

        int choice = ui_input_int("Enter choice (0-4)", 0, 4, 1);
        switch (choice) {
            case 1: exams_view_term_finals(state); break;
            case 2: exams_view_all(state); break;
            case 3: exams_add(state); break;
            case 4: exams_delete(state); break;
            case 0: return;
        }
    }
}
