# Campus Analytics Engine

A multi-file, menu-driven data analytics system for university student management. Built for the BS Artificial Intelligence Programming Fundamentals course.

## Features

- **Student Management** — Add, search (by roll/name/prefix), update, soft-delete, list active students (selection sort)
- **Course Management** — Enroll/drop with prerequisite, seat, and credit-load (max 21) validation
- **Attendance** — Mark sessions (P/A/L), compute percentage, shortage list, undo last session, daily sheet
- **Grades** — Enter marks, weighted totals, letter grades, semester GPA, class statistics, attendance penalty
- **Fee Tracker** — Record payments, late fines (manual date arithmetic), receipts, defaulter list (bubble sort)
- **Reports** — Merit list, attendance/fee defaulters, semester results, department summary, export to file

## Requirements

- C++ compiler with C++11 support (g++ or clang++)
- `make` (optional, for easy compilation)

## Setup & Compilation

```bash
git clone <your-repo-url>
cd campus_analytics_engine
make
```

Or compile manually:

```bash
g++ -std=c++11 -Wall -o campus_analytics \
    main.cpp filehandler.cpp student_ops.cpp course_ops.cpp \
    attendance.cpp grades.cpp fee_tracker.cpp reports.cpp
```

## Running

```bash
./campus_analytics
```

Ensure all `.txt` data files are in the same directory as the executable.

## Data Files

| File | Description |
|------|-------------|
| `students.txt` | Student records (roll, name, dept, semester, CGPA, status) |
| `courses.txt` | Course catalog with prerequisites and capacity |
| `enrollments.txt` | Student course enrollments |
| `attendance_log.txt` | Per-session attendance records |
| `fees.txt` | Fee and payment records |
| `grades.txt` | Grade records (created/updated at runtime) |

## Sample Run

```
========================================
     CAMPUS ANALYTICS ENGINE v1.0
========================================

=== Main Menu ===
1. Student Management
2. Course Management
3. Attendance Management
4. Grades Management
5. Fee Management
6. Reports
0. Exit
Choice: 6

=== Reports ===
1. Merit List
2. Attendance Defaulters
...
Choice: 1

+------+------------+---------------------------+------------+
| Rank | Roll No    | Name                      | CGPA       |
+------+------------+---------------------------+------------+
|    1 | BSAI-23-006| Sana Pervez               |       3.90 |
...
```

## Project Structure

```
campus_analytics_engine/
├── main.cpp              # Entry point, 3-level menu
├── filehandler.h/.cpp    # File I/O (read, write, append, find)
├── student_ops.h/.cpp    # Student CRUD operations
├── course_ops.h/.cpp     # Enrollment and prerequisites
├── attendance.h/.cpp     # Attendance tracking
├── grades.h/.cpp         # Grades and GPA
├── fee_tracker.h/.cpp    # Fee payments and fines
├── reports.h/.cpp        # Formatted reports
├── Makefile
├── README.md
└── *.txt                 # Data files
```

## Constraints Followed

- No `class` keyword (structs only)
- No STL algorithms (`<algorithm>`)
- No `<map>`, `<set>`, `<unordered_map>`
- No `<ctime>` — date arithmetic done manually
- Manual sorting (selection sort, bubble sort)
- Multi-file architecture with cross-module function calls

## Author

BS AI — Programming Fundamentals Project
