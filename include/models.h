#ifndef MODELS_H
#define MODELS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Maximum Capacity Constants */
#define MAX_COURSES      32
#define MAX_CLASSES      64
#define MAX_EXAMS        64
#define MAX_SUBTOPICS    10
#define MAX_TOPICS       128
#define MAX_GRADES       32

/* String Buffer Lengths */
#define STR_CODE_LEN     16
#define STR_NAME_LEN     64
#define STR_ROOM_LEN     32
#define STR_TITLE_LEN    80
#define STR_SHORT_LEN    32
#define STR_MED_LEN      128
#define STR_LONG_LEN     256
#define STR_CONTENT_LEN  1024
#define STR_CODE_SNIP    512

/* Enum for Class Types */
typedef enum {
    CLASS_TYPE_THEORY = 0,
    CLASS_TYPE_LAB,
    CLASS_TYPE_TUTORIAL,
    CLASS_TYPE_SEMINAR
} ClassType;

/* Enum for Exam Classifications */
typedef enum {
    EXAM_TYPE_QUIZ = 0,
    EXAM_TYPE_CT,          /* Class Test */
    EXAM_TYPE_MIDTERM,
    EXAM_TYPE_FINAL,
    EXAM_TYPE_LAB_EXAM,
    EXAM_TYPE_VIVA,
    EXAM_TYPE_ASSIGNMENT,
    EXAM_TYPE_PRESENTATION,
    EXAM_TYPE_OTHER
} ExamType;

/* Enum for Syllabus Topic Status */
typedef enum {
    TOPIC_STATUS_PENDING = 0,
    TOPIC_STATUS_IN_PROGRESS,
    TOPIC_STATUS_COMPLETED
} TopicStatus;

/* Enum for Days of Week (0 = Sunday to match KUET Academic Week) */
typedef enum {
    DAY_SUNDAY = 0,
    DAY_MONDAY,
    DAY_TUESDAY,
    DAY_WEDNESDAY,
    DAY_THURSDAY,
    DAY_FRIDAY,
    DAY_SATURDAY
} DayOfWeek;

/* Course Model */
typedef struct {
    int id;
    char code[STR_CODE_LEN];        /* e.g., "CSE 1107" */
    char name[STR_NAME_LEN];        /* e.g., "Discrete Mathematics" */
    char teacher[STR_NAME_LEN];     /* e.g., "Prof. Dr. Aminur Rahman" */
    float credit;                   /* e.g., 3.0 */
    char color_code[STR_SHORT_LEN]; /* e.g., "Cyan", "Yellow", "Green" */
    char notes[STR_MED_LEN];        /* Short course description */
} Course;

/* Routine Class Model */
typedef struct {
    int id;
    int day_of_week;                /* 0=Sunday .. 6=Saturday */
    char course_code[STR_CODE_LEN]; /* Foreign key match */
    char course_name[STR_NAME_LEN];
    int start_hour;                 /* 24-hour format: 8..17 */
    int start_minute;               /* 0..59 */
    int end_hour;
    int end_minute;
    char room[STR_ROOM_LEN];        /* e.g., "Room 301", "Lab 2" */
    char teacher[STR_NAME_LEN];
    ClassType class_type;
    bool is_enabled;
} RoutineClass;

/* Exam Model */
typedef struct {
    int id;
    char course_code[STR_CODE_LEN];
    char course_name[STR_NAME_LEN];
    char title[STR_TITLE_LEN];       /* e.g., "Midterm Examination", "CT-1: Propositional Logic" */
    ExamType type;
    int year;                       /* e.g., 2026 */
    int month;                      /* 1..12 */
    int day;                        /* 1..31 */
    int hour;                       /* 0..23 */
    int minute;                     /* 0..59 */
    char room[STR_ROOM_LEN];
    float total_marks;
    float weightage;                /* Percentage weightage e.g. 20.0 */
    bool is_completed;
    bool is_enabled;
    char notes[STR_MED_LEN];
} Exam;

/* Syllabus Subtopic */
typedef struct {
    char title[STR_NAME_LEN];
    TopicStatus status;
    char key_points[STR_MED_LEN];
} SyllabusSubtopic;

/* Syllabus Chapter/Topic Model */
typedef struct {
    int id;
    char course_code[STR_CODE_LEN];
    int chapter_num;
    char title[STR_NAME_LEN];
    TopicStatus status;
    int subtopic_count;
    SyllabusSubtopic subtopics[MAX_SUBTOPICS];
    char code_snippet[STR_CODE_SNIP];
} SyllabusTopic;

/* Academic Grade Record for KUET GPA/CGPA */
typedef struct {
    char course_code[STR_CODE_LEN];
    char course_name[STR_NAME_LEN];
    float credit;
    float marks;                    /* Marks out of 100 */
    char grade_letter[4];           /* "A+", "A", "A-", "B+", etc. */
    float grade_point;              /* 4.00, 3.75, 3.50, etc. */
    int semester_num;               /* 1 for 1st Year 1st Term, etc. */
    bool is_completed;
} GradeRecord;

/* App Settings */
typedef struct {
    bool is_24_hour;                /* 12-hour AM/PM vs 24-hour military clock */
    int first_day_of_week;          /* 0=Sunday (KUET standard), 1=Monday */
    int countdown_style;            /* 0=Detailed ("05d 12h 21m 15s"), 1=Compact */
    int reminder_minutes;           /* Alert threshold before class (e.g., 10 mins) */
    int color_theme;                /* 0=Cyan Matrix, 1=Emerald, 2=Amber, 3=Monochrome */
    bool enable_colors;
    bool show_seconds;
} AppSettings;

/* Complete Runtime Academic OS Application State */
typedef struct {
    Course courses[MAX_COURSES];
    int course_count;

    RoutineClass classes[MAX_CLASSES];
    int class_count;

    Exam exams[MAX_EXAMS];
    int exam_count;

    SyllabusTopic syllabus[MAX_TOPICS];
    int syllabus_count;

    GradeRecord grades[MAX_GRADES];
    int grade_count;

    AppSettings settings;
} AppState;

/* Intuitive database alias for team recognition */
typedef AppState AcademicDatabase;
#define get_database get_app_state

/* Global state pointer accessor */
AppState* get_app_state(void);

/* Helper functions for enum strings */
const char* class_type_to_string(ClassType type);
const char* exam_type_to_string(ExamType type);
const char* topic_status_to_string(TopicStatus status);
const char* day_to_string(int day);
const char* day_to_short_string(int day);

#endif /* MODELS_H */
