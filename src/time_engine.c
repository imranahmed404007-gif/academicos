#include "academic_os.h"
#include <math.h>

static const char* MONTH_NAMES[] = {
    "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE",
    "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"
};

static const char* DAY_NAMES[] = {
    "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"
};

static bool s_has_simulated_time = false;
static time_t s_simulated_offset = 0;

void time_engine_set_simulated_time(int wday, int day, int month, int year, int hour, int minute, int sec) {
    (void)wday;
    s_has_simulated_time = true;
    struct tm sim_tm;
    memset(&sim_tm, 0, sizeof(struct tm));
    sim_tm.tm_mday = day;
    sim_tm.tm_mon  = month - 1;
    sim_tm.tm_year = year - 1900;
    sim_tm.tm_hour = hour;
    sim_tm.tm_min  = minute;
    sim_tm.tm_sec  = sec;
    sim_tm.tm_isdst = -1;
    time_t sim_t = mktime(&sim_tm);
    time_t now = time(NULL);
    s_simulated_offset = sim_t - now;
}

void time_engine_clear_simulated_time(void) {
    s_has_simulated_time = false;
    s_simulated_offset = 0;
}

bool time_engine_is_simulated(void) {
    return s_has_simulated_time;
}

struct tm time_get_localtime(void) {
    time_t now = time(NULL);
    if (s_has_simulated_time) {
        now += s_simulated_offset;
    }
    struct tm* tm_info = localtime(&now);
    if (tm_info) return *tm_info;
    
    struct tm fallback = {0};
    fallback.tm_year = 126; /* 2026 */
    fallback.tm_mon = 8;    /* September (0-indexed) */
    fallback.tm_mday = 25;
    fallback.tm_wday = 4;   /* Thursday */
    fallback.tm_hour = 22;
    fallback.tm_min = 42;
    fallback.tm_sec = 17;
    return fallback;
}

void time_format_date_banner(char* buffer, size_t max_len, const struct tm* timeinfo) {
    int wday = timeinfo->tm_wday >= 0 && timeinfo->tm_wday < 7 ? timeinfo->tm_wday : 0;
    int mon = timeinfo->tm_mon >= 0 && timeinfo->tm_mon < 12 ? timeinfo->tm_mon : 0;
    int day = timeinfo->tm_mday;
    int year = timeinfo->tm_year + 1900;

    snprintf(buffer, max_len, "%s • %02d %s %d", DAY_NAMES[wday], day, MONTH_NAMES[mon], year);
}

void time_format_clock(char* buffer, size_t max_len, const struct tm* timeinfo, bool is_24_hour, bool show_seconds) {
    int h = timeinfo->tm_hour;
    int m = timeinfo->tm_min;
    int s = timeinfo->tm_sec;

    if (is_24_hour) {
        if (show_seconds) {
            snprintf(buffer, max_len, "%02d:%02d:%02d", h, m, s);
        } else {
            snprintf(buffer, max_len, "%02d:%02d", h, m);
        }
    } else {
        const char* ampm = (h >= 12) ? "PM" : "AM";
        int h12 = h % 12;
        if (h12 == 0) h12 = 12;

        if (show_seconds) {
            snprintf(buffer, max_len, "%02d:%02d:%02d %s", h12, m, s, ampm);
        } else {
            snprintf(buffer, max_len, "%02d:%02d %s", h12, m, ampm);
        }
    }
}

void time_format_time_val(char* buffer, size_t max_len, int hour, int minute, bool is_24_hour) {
    if (is_24_hour) {
        snprintf(buffer, max_len, "%02d:%02d", hour, minute);
    } else {
        const char* ampm = (hour >= 12) ? "PM" : "AM";
        int h12 = hour % 12;
        if (h12 == 0) h12 = 12;
        snprintf(buffer, max_len, "%02d:%02d %s", h12, minute, ampm);
    }
}

long time_engine_calc_exam_diff(const Exam* exam, const struct tm* now_tm) {
    struct tm exam_tm = {0};
    exam_tm.tm_year = exam->year - 1900;
    exam_tm.tm_mon  = exam->month - 1;
    exam_tm.tm_mday = exam->day;
    exam_tm.tm_hour = exam->hour;
    exam_tm.tm_min  = exam->minute;
    exam_tm.tm_sec  = 0;
    exam_tm.tm_isdst = -1;

    struct tm now_copy = *now_tm;
    time_t exam_time = mktime(&exam_tm);
    time_t current_time = mktime(&now_copy);

    if (exam_time == (time_t)-1 || current_time == (time_t)-1) {
        return -1;
    }
    return (long)(exam_time - current_time);
}

void time_engine_format_countdown_str(long diff_seconds, int style, char* buffer, size_t max_len) {
    if (diff_seconds <= 0) {
        snprintf(buffer, max_len, "Started / Overdue");
        return;
    }

    long days = diff_seconds / 86400;
    long rem = diff_seconds % 86400;
    long hours = rem / 3600;
    rem = rem % 3600;
    long minutes = rem / 60;
    long seconds = rem % 60;

    if (style == 1) { /* Compact */
        if (days > 0) {
            snprintf(buffer, max_len, "%ldd %02ldh %02ldm", days, hours, minutes);
        } else if (hours > 0) {
            snprintf(buffer, max_len, "%02ldh %02ldm %02lds", hours, minutes, seconds);
        } else {
            snprintf(buffer, max_len, "%02ldm %02lds", minutes, seconds);
        }
    } else { /* Detailed - matching the exact prompt format */
        snprintf(buffer, max_len, "%02ld Days %02ld Hours %02ld Minutes", days, hours, minutes);
    }
}

void time_engine_compute_hero_class(const AppState* state, const struct tm* now_tm, HeroClassState* out_hero) {
    memset(out_hero, 0, sizeof(HeroClassState));
    out_hero->status = HERO_STATUS_NO_SCHEDULE;

    int current_wday = now_tm->tm_wday; /* 0=Sunday */
    int current_minute_of_day = now_tm->tm_hour * 60 + now_tm->tm_min;
    int current_sec_of_minute = now_tm->tm_sec;

    /* 1. Check if a class is currently RUNNING */
    for (int i = 0; i < state->class_count; i++) {
        const RoutineClass* rc = &state->classes[i];
        if (!rc->is_enabled || rc->day_of_week != current_wday) continue;

        int class_start_min = rc->start_hour * 60 + rc->start_minute;
        int class_end_min   = rc->end_hour * 60 + rc->end_minute;

        if (current_minute_of_day >= class_start_min && current_minute_of_day < class_end_min) {
            out_hero->status = HERO_STATUS_RUNNING;
            out_hero->current_class = *rc;
            out_hero->is_today = true;
            strncpy(out_hero->day_name, "Today", sizeof(out_hero->day_name) - 1);

            time_format_time_val(out_hero->start_formatted, sizeof(out_hero->start_formatted), rc->start_hour, rc->start_minute, state->settings.is_24_hour);
            time_format_time_val(out_hero->end_formatted, sizeof(out_hero->end_formatted), rc->end_hour, rc->end_minute, state->settings.is_24_hour);

            int total_secs = (class_end_min - class_start_min) * 60;
            int elapsed_secs = (current_minute_of_day - class_start_min) * 60 + current_sec_of_minute;
            int rem_secs = total_secs - elapsed_secs;
            if (rem_secs < 0) rem_secs = 0;

            out_hero->remaining_seconds = rem_secs;
            out_hero->progress_percentage = (total_secs > 0) ? ((float)elapsed_secs / total_secs) * 100.0f : 0.0f;
            int rh = rem_secs / 3600;
            int rm = (rem_secs % 3600) / 60;
            int rs = rem_secs % 60;
            if (rh > 0) {
                snprintf(out_hero->formatted_remaining, sizeof(out_hero->formatted_remaining), "%02dh %02dm %02ds left", rh, rm, rs);
            } else {
                snprintf(out_hero->formatted_remaining, sizeof(out_hero->formatted_remaining), "%02dm %02ds left", rm, rs);
            }
            return;
        }
    }

    /* 2. Check for UPCOMING class TODAY */
    const RoutineClass* next_today = NULL;
    int min_diff = 999999;
    for (int i = 0; i < state->class_count; i++) {
        const RoutineClass* rc = &state->classes[i];
        if (!rc->is_enabled || rc->day_of_week != current_wday) continue;

        int class_start_min = rc->start_hour * 60 + rc->start_minute;
        if (class_start_min > current_minute_of_day) {
            int diff = class_start_min - current_minute_of_day;
            if (diff < min_diff) {
                min_diff = diff;
                next_today = rc;
            }
        }
    }

    if (next_today) {
        out_hero->status = HERO_STATUS_UPCOMING;
        out_hero->current_class = *next_today;
        out_hero->is_today = true;
        strncpy(out_hero->day_name, "Today", sizeof(out_hero->day_name) - 1);
        time_format_time_val(out_hero->start_formatted, sizeof(out_hero->start_formatted), next_today->start_hour, next_today->start_minute, state->settings.is_24_hour);
        time_format_time_val(out_hero->end_formatted, sizeof(out_hero->end_formatted), next_today->end_hour, next_today->end_minute, state->settings.is_24_hour);

        int start_sec = (next_today->start_hour * 60 + next_today->start_minute) * 60;
        int now_sec = current_minute_of_day * 60 + current_sec_of_minute;
        int diff_sec = start_sec - now_sec;
        if (diff_sec < 0) diff_sec = 0;
        out_hero->remaining_seconds = diff_sec;
        int uh = diff_sec / 3600;
        int um = (diff_sec % 3600) / 60;
        int us = diff_sec % 60;
        if (uh > 0) {
            snprintf(out_hero->formatted_remaining, sizeof(out_hero->formatted_remaining), "Starts in %02dh %02dm %02ds", uh, um, us);
        } else {
            snprintf(out_hero->formatted_remaining, sizeof(out_hero->formatted_remaining), "Starts in %02dm %02ds", um, us);
        }
        return;
    }

    /* 3. Check subsequent days for next scheduled class */
    for (int day_offset = 1; day_offset <= 7; day_offset++) {
        int check_day = (current_wday + day_offset) % 7;
        const RoutineClass* earliest_that_day = NULL;
        int earliest_min = 999999;

        for (int i = 0; i < state->class_count; i++) {
            const RoutineClass* rc = &state->classes[i];
            if (!rc->is_enabled || rc->day_of_week != check_day) continue;

            int class_start_min = rc->start_hour * 60 + rc->start_minute;
            if (class_start_min < earliest_min) {
                earliest_min = class_start_min;
                earliest_that_day = rc;
            }
        }

        if (earliest_that_day) {
            out_hero->status = HERO_STATUS_UPCOMING;
            out_hero->current_class = *earliest_that_day;
            out_hero->is_today = false;

            if (day_offset == 1) {
                strncpy(out_hero->day_name, "Tomorrow", sizeof(out_hero->day_name) - 1);
            } else {
                strncpy(out_hero->day_name, DAY_NAMES[check_day], sizeof(out_hero->day_name) - 1);
            }

            time_format_time_val(out_hero->start_formatted, sizeof(out_hero->start_formatted), earliest_that_day->start_hour, earliest_that_day->start_minute, state->settings.is_24_hour);
            time_format_time_val(out_hero->end_formatted, sizeof(out_hero->end_formatted), earliest_that_day->end_hour, earliest_that_day->end_minute, state->settings.is_24_hour);

            int start_sec = (earliest_that_day->start_hour * 60 + earliest_that_day->start_minute) * 60;
            int now_sec = current_minute_of_day * 60 + current_sec_of_minute;
            long diff_sec = (long)day_offset * 86400L + (long)(start_sec - now_sec);
            if (diff_sec < 0) diff_sec = 0;
            out_hero->remaining_seconds = diff_sec;

            long cd_days = diff_sec / 86400L;
            long cd_hours = (diff_sec % 86400L) / 3600L;
            long cd_mins = (diff_sec % 3600L) / 60L;
            long cd_secs = diff_sec % 60L;

            if (cd_days > 0) {
                snprintf(out_hero->formatted_remaining, sizeof(out_hero->formatted_remaining), "Starts in %ldd %02ldh %02ldm", cd_days, cd_hours, cd_mins);
            } else if (cd_hours > 0) {
                snprintf(out_hero->formatted_remaining, sizeof(out_hero->formatted_remaining), "Starts in %02ldh %02ldm %02lds", cd_hours, cd_mins, cd_secs);
            } else {
                snprintf(out_hero->formatted_remaining, sizeof(out_hero->formatted_remaining), "Starts in %02ldm %02lds", cd_mins, cd_secs);
            }
            return;
        }
    }

    /* No classes found at all */
    out_hero->status = HERO_STATUS_NO_SCHEDULE;
}

void time_engine_compute_next_exam(const AppState* state, const struct tm* now_tm, NextExamState* out_next_exam) {
    memset(out_next_exam, 0, sizeof(NextExamState));
    out_next_exam->has_exam = false;

    long min_diff = 999999999L;
    const Exam* closest_exam = NULL;

    for (int i = 0; i < state->exam_count; i++) {
        const Exam* ex = &state->exams[i];
        if (!ex->is_enabled || ex->is_completed) continue;

        long diff = time_engine_calc_exam_diff(ex, now_tm);
        if (diff > 0 && diff < min_diff) {
            min_diff = diff;
            closest_exam = ex;
        }
    }

    if (closest_exam) {
        out_next_exam->has_exam = true;
        out_next_exam->exam = *closest_exam;
        out_next_exam->total_diff_seconds = min_diff;

        out_next_exam->days = min_diff / 86400;
        long rem = min_diff % 86400;
        out_next_exam->hours = rem / 3600;
        rem = rem % 3600;
        out_next_exam->minutes = rem / 60;
        out_next_exam->seconds = rem % 60;

        time_engine_format_countdown_str(min_diff, state->settings.countdown_style, out_next_exam->formatted_countdown, sizeof(out_next_exam->formatted_countdown));
        
        char time_str[32];
        time_format_time_val(time_str, sizeof(time_str), closest_exam->hour, closest_exam->minute, state->settings.is_24_hour);
        snprintf(out_next_exam->formatted_datetime, sizeof(out_next_exam->formatted_datetime), "%02d/%02d/%d at %s", closest_exam->day, closest_exam->month, closest_exam->year, time_str);
    }
}

void time_engine_compute_stats(const AppState* state, const struct tm* now_tm, DashboardStats* out_stats) {
    memset(out_stats, 0, sizeof(DashboardStats));

    out_stats->total_courses_count = state->course_count;

    /* Classes today */
    int wday = now_tm->tm_wday;
    for (int i = 0; i < state->class_count; i++) {
        if (state->classes[i].is_enabled && state->classes[i].day_of_week == wday) {
            out_stats->classes_today_count++;
        }
    }

    /* Upcoming exams */
    for (int i = 0; i < state->exam_count; i++) {
        if (state->exams[i].is_enabled && !state->exams[i].is_completed) {
            long diff = time_engine_calc_exam_diff(&state->exams[i], now_tm);
            if (diff > 0) {
                out_stats->upcoming_exams_count++;
            }
        }
    }

    /* Topics count */
    out_stats->total_topics_count = state->syllabus_count;
    for (int i = 0; i < state->syllabus_count; i++) {
        if (state->syllabus[i].status == TOPIC_STATUS_COMPLETED) {
            out_stats->completed_topics_count++;
        }
    }
}
