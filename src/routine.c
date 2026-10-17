#include "academic_os.h"

void routine_display_day_classes(const AppState* state, int day_of_week) {
    const ColorPalette* pal = ui_get_palette();
    printf("\n%s=== %s TIMETABLE ===%s\n", pal->primary, day_to_string(day_of_week), ANSI_RESET);

    int count = 0;
    for (int i = 0; i < state->class_count; i++) {
        const RoutineClass* rc = &state->classes[i];
        if (rc->day_of_week != day_of_week) continue;
        count++;

        char start_t[16], end_t[16];
        time_format_time_val(start_t, sizeof(start_t), rc->start_hour, rc->start_minute, state->settings.is_24_hour);
        time_format_time_val(end_t, sizeof(end_t), rc->end_hour, rc->end_minute, state->settings.is_24_hour);

        const char* type_color = (rc->class_type == CLASS_TYPE_LAB) ? pal->warning : pal->accent;
        const char* status_str = rc->is_enabled ? "" : " (CANCELLED/DISABLED)";
        const char* status_col = rc->is_enabled ? pal->text_bright : pal->text_dim;

        printf("  %s[%d]%s %s%-8s%s │ %-9s - %-9s │ %s%-8s%s │ %-12s │ %s%s%s\n",
               pal->text_dim, rc->id, ANSI_RESET,
               pal->primary, rc->course_code, ANSI_RESET,
               start_t, end_t,
               type_color, class_type_to_string(rc->class_type), ANSI_RESET,
               rc->room,
               status_col, rc->course_name, status_str);
        if (strlen(rc->teacher) > 0) {
            printf("       Instructor: %s%s%s\n", pal->text_dim, rc->teacher, ANSI_RESET);
        }
    }

    if (count == 0) {
        printf("  %s(No classes scheduled for %s)%s\n", pal->text_dim, day_to_string(day_of_week), ANSI_RESET);
    }
}

void routine_view_today(const AppState* state) {
    struct tm now_tm = time_get_localtime();
    ui_print_header("TODAY'S ROUTINE", day_to_string(now_tm.tm_wday));
    routine_display_day_classes(state, now_tm.tm_wday);
    ui_pause_key();
}

void routine_view_week(const AppState* state) {
    ui_print_header("WEEKLY TIMETABLE", "KUET CSE Academic Routine");
    int first_day = state->settings.first_day_of_week;
    for (int i = 0; i < 7; i++) {
        int d = (first_day + i) % 7;
        routine_display_day_classes(state, d);
    }
    ui_pause_key();
}

void routine_view_by_day(const AppState* state) {
    ui_print_header("VIEW BY DAY", "Select Day of Week");
    for (int i = 0; i < 7; i++) {
        printf("  %d. %s\n", i + 1, day_to_string(i));
    }
    int choice = ui_input_int("\nEnter day number (1-7)", 1, 7, 1);
    routine_display_day_classes(state, choice - 1);
    ui_pause_key();
}

void routine_add_class(AppState* state) {
    if (state->class_count >= MAX_CLASSES) {
        printf("%sRoutine storage is full!%s\n", ANSI_RED, ANSI_RESET);
        ui_pause_key();
        return;
    }

    ui_print_header("ADD CLASS", "Create New Timetable Entry");
    RoutineClass nc;
    memset(&nc, 0, sizeof(RoutineClass));

    int max_id = 0;
    for (int i = 0; i < state->class_count; i++) {
        if (state->classes[i].id > max_id) max_id = state->classes[i].id;
    }
    nc.id = max_id + 1;

    /* 1. Day Selection */
    printf("Select Day:\n");
    for (int i = 0; i <= 4; i++) {
        printf("  %d. %s\n", i + 1, day_to_string(i));
    }
    nc.day_of_week = ui_input_int("Choice (1-5)", 1, 5, 1) - 1;

    /* 2. Course Selection */
    printf("\nSelect Course:\n");
    for (int i = 0; i < state->course_count; i++) {
        printf("  %d. %s (%s)\n", i + 1, state->courses[i].code, state->courses[i].name);
    }
    int c_choice = ui_input_int("Choice", 1, state->course_count, 1);
    snprintf(nc.course_code, sizeof(nc.course_code), "%s", state->courses[c_choice - 1].code);
    snprintf(nc.course_name, sizeof(nc.course_name), "%s", state->courses[c_choice - 1].name);

    /* 3. Class Type Selection */
    printf("\nClass Type:\n  1. Theory\n  2. Lab / Sessional\n");
    int type_sel = ui_input_int("Choice (1-2)", 1, 2, 1);
    nc.class_type = (type_sel == 1) ? CLASS_TYPE_THEORY : CLASS_TYPE_LAB;

    /* 4. Routine Period Selection */
    printf("\nSelect Class Period / Slot:\n");
    printf("  1. Period 1 (08:00 AM - 08:50 AM)\n");
    printf("  2. Period 2 (08:50 AM - 09:40 AM)\n");
    printf("  3. Period 3 (09:40 AM - 10:30 AM)\n");
    printf("  4. Period 4 (10:40 AM - 11:30 AM)\n");
    printf("  5. Period 5 (11:30 AM - 12:20 PM)\n");
    printf("  6. Period 6 (12:20 PM - 01:10 PM)\n");
    printf("  7. Afternoon Lab Slot (02:30 PM - 05:00 PM)\n");
    printf("  8. Extended Lab Slot  (02:30 PM - 05:30 PM)\n");
    printf("  9. Late Afternoon     (03:20 PM - 05:00 PM)\n");
    int slot_sel = ui_input_int("Choice (1-9)", 1, 9, 4);

    switch (slot_sel) {
        case 1: nc.start_hour = 8;  nc.start_minute = 0;  nc.end_hour = 8;  nc.end_minute = 50; break;
        case 2: nc.start_hour = 8;  nc.start_minute = 50; nc.end_hour = 9;  nc.end_minute = 40; break;
        case 3: nc.start_hour = 9;  nc.start_minute = 40; nc.end_hour = 10; nc.end_minute = 30; break;
        case 4: nc.start_hour = 10; nc.start_minute = 40; nc.end_hour = 11; nc.end_minute = 30; break;
        case 5: nc.start_hour = 11; nc.start_minute = 30; nc.end_hour = 12; nc.end_minute = 20; break;
        case 6: nc.start_hour = 12; nc.start_minute = 20; nc.end_hour = 13; nc.end_minute = 10; break;
        case 7: nc.start_hour = 14; nc.start_minute = 30; nc.end_hour = 17; nc.end_minute = 0;  break;
        case 8: nc.start_hour = 14; nc.start_minute = 30; nc.end_hour = 17; nc.end_minute = 30; break;
        case 9: nc.start_hour = 15; nc.start_minute = 20; nc.end_hour = 17; nc.end_minute = 0;  break;
    }

    /* 5. Room Selection */
    printf("\nSelect Room / Venue:\n");
    printf("  1. CSE 101 (Main Theory Room)\n");
    printf("  2. CSE 102 (Thursday Theory Room)\n");
    printf("  3. CSE 104 (Programming Lab)\n");
    printf("  4. Physics Lab\n");
    printf("  5. Language Lab\n");
    int def_room = (nc.class_type == CLASS_TYPE_LAB) ? 3 : (nc.day_of_week == 4) ? 2 : 1;
    int room_sel = ui_input_int("Choice (1-5)", 1, 5, def_room);
    if (room_sel == 1) snprintf(nc.room, sizeof(nc.room), "CSE 101");
    else if (room_sel == 2) snprintf(nc.room, sizeof(nc.room), "CSE 102");
    else if (room_sel == 3) snprintf(nc.room, sizeof(nc.room), "CSE 104");
    else if (room_sel == 4) snprintf(nc.room, sizeof(nc.room), "Physics Lab");
    else snprintf(nc.room, sizeof(nc.room), "Language Lab");

    nc.teacher[0] = '\0';
    nc.is_enabled = true;

    state->classes[state->class_count++] = nc;
    storage_save_routine();

    printf("\n%sClass added successfully without typing!%s\n", ANSI_GREEN, ANSI_RESET);
    ui_pause_key();
}

void routine_delete_class(AppState* state) {
    ui_print_header("DELETE CLASS", "Remove Timetable Entry");
    if (state->class_count == 0) {
        printf("  No classes registered to delete.\n\n");
        ui_pause_key();
        return;
    }

    printf("Select Class to delete:\n");
    for (int i = 0; i < state->class_count; i++) {
        RoutineClass* c = &state->classes[i];
        printf("  %2d. %-9s %02d:%02d-%02d:%02d : %-8s (%s)\n",
               i + 1, day_to_string(c->day_of_week),
               c->start_hour, c->start_minute, c->end_hour, c->end_minute,
               c->course_code, c->room);
    }
    printf("   0. Cancel\n\n");

    int choice = ui_input_int("Enter choice (0 to cancel)", 0, state->class_count, 0);
    if (choice == 0) return;

    int idx = choice - 1;
    bool confirm = ui_input_bool("Are you sure you want to delete this class? (Y/N)", false);
    if (confirm) {
        for (int i = idx; i < state->class_count - 1; i++) {
            state->classes[i] = state->classes[i + 1];
        }
        state->class_count--;
        storage_save_routine();
        printf("\n%sClass deleted.%s\n", ANSI_GREEN, ANSI_RESET);
    } else {
        printf("\nDeletion cancelled.\n");
    }
    ui_pause_key();
}

void routine_menu(AppState* state) {
    while (1) {
        struct tm now_tm = time_get_localtime();
        char banner_sub[64];
        snprintf(banner_sub, sizeof(banner_sub), "KUET CSE Timetable • Today is %s", day_to_string(now_tm.tm_wday));
        ui_print_header("ROUTINE MANAGEMENT", banner_sub);

        /* 1. Today's Routine is seen directly in this section */
        routine_display_day_classes(state, now_tm.tm_wday);
        printf("\n");

        /* 2. Other options are under today's routine */
        const ColorPalette* pal = ui_get_palette();
        printf("%s--- Other Options ---%s\n", pal->secondary, ANSI_RESET);
        printf("  1. Full Weekly Schedule\n");
        printf("  2. Filter by Specific Day\n");
        printf("  3. Add New Class\n");
        printf("  4. Delete Class\n");
        printf("  0. Back to Main Menu\n\n");

        int choice = ui_input_int("Enter choice (0-4)", 0, 4, 1);
        switch (choice) {
            case 1: routine_view_week(state); break;
            case 2: routine_view_by_day(state); break;
            case 3: routine_add_class(state); break;
            case 4: routine_delete_class(state); break;
            case 0: return;
        }
    }
}
