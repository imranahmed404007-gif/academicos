# 🎓 AcademicOS — Terminal Edition (KUET CSE)

An elegant, real-time academic terminal operating system written in **pure C** for **KUET CSE** students. Inspired by the Android AcademicOS concept and redesigned natively for modern terminals with UTF-8 box graphics, ANSI color palettes, live countdown timers, and binary data persistence.

```text
╔══════════════════════════════════════════════╗
║                 ACADEMIC OS                  ║
║                 KUET CSE                     ║
╠══════════════════════════════════════════════╣
║                                              ║
║  THURSDAY • 25 SEPTEMBER 2026                ║
║  Current Time: 10:42:17 PM                   ║
║                                              ║
║  ┌─────────────── TODAY ─────────────────┐   ║
║  │ No class running                      │   ║
║  │ Next: CSE 1107                        │   ║
║  │ Tomorrow • 09:00 AM • Room 301        │   ║
║  └───────────────────────────────────────┘   ║
║                                              ║
║  NEXT EXAM                                   ║
║  CSE 1107 Midterm                            ║
║  ⏱ 05 Days 12 Hours 21 Minutes              ║
║                                              ║
║  Classes Today : 3                           ║
║  Upcoming Exams: 5                           ║
║  Courses       : 6                           ║
║                                              ║
╠══════════════════════════════════════════════╣
║                                              ║
║  1. Dashboard (Live Mode)                    ║
║  2. Routine                                  ║
║  3. Exams                                    ║
║  4. Courses                                  ║
║  5. Syllabus                                 ║
║  6. Lecture Notes                            ║
║  7. GPA & Statistics                         ║
║  8. Search                                   ║
║  9. Settings                                 ║
║  0. Exit                                     ║
║                                              ║
╚══════════════════════════════════════════════╝
```

---

## ⚡ Core Features

### 1. 🖥️ Dashboard & Live Mode
- **Real-Time Clock**: Live digital clock updating every second in Live Mode (`[1]`).
- **Active Class Detection**: Detects currently running class with progress bar (`[██████░░░░] 60%`) and remaining minutes.
- **Next Class Hero**: Automatically schedules next class for today or tomorrow with room and instructor information.
- **Live Exam Countdown**: Precision day/hour/minute countdown to the next scheduled exam or assessment.
- **Quick Statistics**: Real-time counter of today's classes, pending exams, and courses.

### 2. 📅 Routine (Timetable)
- Full Sunday–Saturday timetable (KUET academic week starts Sunday).
- Supports Theory, Lab, Tutorial, and Seminar class types.
- Add, Edit, Delete, Duplicate, and Enable/Disable classes without losing data.
- Daily filtering and quick view of today's classes.

### 3. 📝 Exams & Assessments
- Supports Quizzes, Class Tests (CT), Midterms, Finals, Lab Exams, Vivas, Assignments, and Presentations.
- Live countdown calculation for each upcoming exam.
- Filters for Upcoming, Past, and All registered exams.
- Status toggle: Completed, Upcoming, or Disabled.

### 4. 📚 Courses Catalogue
- Preloaded with authentic KUET CSE 1st Year courses (e.g., `CSE 1100`, `CSE 1101`, `CSE 1102`, `CSE 1107`, `MATH 1107`, `PHY 1107`).
- Total degree credit calculations.
- Detailed course view linking timetable classes, syllabus chapters, and upcoming exams.

### 5. 📖 Syllabus (Hierarchy Tree)
- Visual tree-based hierarchy:
  ```text
  CSE 1107
   ├── Logic
   │    ├── Proposition [COMPLETED]
   │    ├── Truth Table [COMPLETED]
   │    └── Logical Equivalence [IN PROGRESS]
   │
   ├── Set Theory
   └── Relations
  ```
- Subtopic completion status tracking (`PENDING`, `IN PROGRESS`, `COMPLETED`).
- Embedded C code snippets and mathematical formulas.
- Progress summary bar displaying completion percentage across all courses.

### 6. 📝 Lecture Notes & Cards
- Concept cards, C code snippets, formulas, and cheat sheets.
- Course and lecture-based tagging.
- Syntax snippet preview inside double-line ASCII boxes.
- Export individual lecture notes to plain text files.

### 7. 📊 GPA & Academic Statistics
- Official **KUET Credit-Weighted Grading Scale**:
  - `A+` (4.00) : 80% and above
  - `A`  (3.75) : 75% to <80%
  - `A-` (3.50) : 70% to <75%
  - `B+` (3.25) : 65% to <70%
  - `B`  (3.00) : 60% to <65%
  - `B-` (2.75) : 55% to <60%
  - `C+` (2.50) : 50% to <55%
  - `C`  (2.25) : 45% to <50%
  - `D`  (2.00) : 40% to <45%
  - `F`  (0.00) : <40%
- Credit-weighted term GPA and cumulative CGPA calculation.
- ASCII frequency histogram for grade distribution.
- **Target CGPA Simulator**: Predicts what average GPA you need in remaining credits to achieve your target graduation honors!

### 8. 🔎 Global Search
- Case-insensitive search across Courses, Routine, Exams, Syllabus, and Lecture Notes.

### 9. ⚙️ Settings & System Management
- 12-hour AM/PM vs 24-hour military clock toggle.
- Week-start configuration (Sunday vs Monday vs Saturday).
- Detailed vs Compact countdown style.
- 4 Terminal Color Themes: Cyan Tech, Emerald Matrix, Amber Warm, Monochrome.
- Full system backup and restore (`*.dat` binary archives).
- One-click reset to default authentic KUET CSE dataset.

---

## 🛠️ Architecture & Under the Hood

The application is built in **Pure C (C99 standard)** with zero external dependencies:

```text
             ACADEMIC OS
                  │
       ┌──────────┼──────────┐
       ↓          ↓          ↓
   Structures   Functions   Arrays
       ↓          ↓          ↓
    Pointers   Strings    Algorithms
       └──────────┼──────────┘
                  ↓
             File Handling
                  ↓
             *.dat files
```

- **Persistence Layer**: Structured binary files in `data/*.dat` (`courses.dat`, `routine.dat`, `exams.dat`, `syllabus.dat`, `notes.dat`, `grades.dat`, `settings.dat`).
- **Terminal Abstraction**: Windows Console Virtual Terminal Processing enabled via Windows API (`ENABLE_VIRTUAL_TERMINAL_PROCESSING`) with fallback for standard POSIX VT100 ANSI sequences.
- **Time Engine**: Real-time evaluation based on standard C `<time.h>` and `localtime()`.

---

## 🚀 Building & Running

### Using GCC (MinGW / Linux / macOS)
```bash
gcc -Wall -Wextra -std=c99 -Iinclude -O2 src/*.c -o academic_os
./academic_os
```

### Using Make
```bash
make
./academic_os
```
*(On Windows with MinGW, run `mingw32-make`)*

### Using CMake
```bash
mkdir build
cd build
cmake ..
cmake --build .
./academic_os
```

---

## 👥 3-Member Team Division & Git Workflow

### Branch Structure
| Branch Name | Owner | Primary Modules & Responsibilities |
| :--- | :--- | :--- |
| `main` | All / Team Lead | Production-ready, fully tested releases |
| `develop` | All Members | Shared integration branch for active merges |
| `feature/member1-ui-core` | Member 1 | `src/ui.c`, `src/dashboard.c`, `src/main.c`, Terminal VT100 / Box borders |
| `feature/member2-time-routine` | Member 2 | `src/time_engine.c`, `src/routine.c`, `src/exams.c`, Countdown math |
| `feature/member3-storage-syllabus` | Member 3 | `src/storage.c`, `src/courses.c`, `src/syllabus.c`, `src/utilities.c` |

### Recommended Git Workflow for Team Members
1. **Switch to your assigned branch**:
   ```bash
   git checkout feature/member1-ui-core   # or member2, member3
   ```
2. **Make your changes, test compile**:
   ```bash
   .\build.bat
   ```
3. **Commit your work**:
   ```bash
   git add src/
   git commit -m "feat(ui): implement responsive box borders"
   ```
4. **Integrate into `develop`**:
   ```bash
   git checkout develop
   git pull origin develop
   git merge feature/member1-ui-core
   ```

