#include <iostream>
#include <sstream>
#include <string>
#include <limits>

#include "attendance.h"
#include "course_ops.h"
#include "fee_tracker.h"
#include "grades.h"
#include "reports.h"
#include "student_ops.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

void clearInput() {
    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool readIntInRange(const string& prompt, int minVal, int maxVal, int& out) {
    while (true) {
        cout << prompt;
        if (cin >> out) {
            clearInput();
            if (out >= minVal && out <= maxVal) {
                return true;
            }
            cout << "Invalid input. Value must be between " << minVal
                 << " and " << maxVal << ". Try again.\n";
        } else {
            clearInput();
            cout << "Invalid input. Enter a whole number between " << minVal
                 << " and " << maxVal << ". Try again.\n";
        }
    }
}

bool readDoubleInRange(const string& prompt, double minVal, double maxVal, double& out) {
    while (true) {
        cout << prompt;
        if (cin >> out) {
            clearInput();
            if (out >= minVal && out <= maxVal) {
                return true;
            }
            cout << "Invalid input. Value must be between " << minVal
                 << " and " << maxVal << ". Try again.\n";
        } else {
            clearInput();
            cout << "Invalid input. Enter a number between " << minVal
                 << " and " << maxVal << ". Try again.\n";
        }
    }
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    clearInput();
    cin.get();
}

void printEnrollResult(EnrollResult result) {
    switch (result) {
        case ENROLL_SUCCESS:
            cout << "Enrollment successful.\n";
            break;
        case ENROLL_STUDENT_INACTIVE:
            cout << "Error: Student is inactive.\n";
            break;
        case ENROLL_COURSE_NOT_FOUND:
            cout << "Error: Course not found.\n";
            break;
        case ENROLL_NO_SEATS:
            cout << "Error: No seats available.\n";
            break;
        case ENROLL_ALREADY_ENROLLED:
            cout << "Error: Already enrolled.\n";
            break;
        case ENROLL_CREDIT_OVERLOAD:
            cout << "Error: Credit load exceeds 21 hours.\n";
            break;
        case ENROLL_PREREQ_FAILED:
            cout << "Error: Prerequisite not met.\n";
            break;
    }
}

void showPrefixMatches(const std::string& prefix) {
    std::vector<std::vector<std::string> > results = searchByNamePrefix(prefix);
    std::cout << "\n--- Matches for \"" << prefix << "\" (" << results.size() << ") ---\n";
    for (int i = 0; i < (int)results.size(); i++) {
        std::cout << results[i][0] << " - " << results[i][1] << "\n";
    }
    std::cout << "---------------------------\n";
}

void searchAsYouType() {
    string prefix = "";
    cout << "Search as you type (type letters, Enter=done, Backspace=delete):\n";
    clearInput();
    showPrefixMatches(prefix);

    while (true) {
        cout << "Prefix: \"" << prefix << "\" | Next key: ";
        char c = 0;
        if (!cin.get(c)) {
            break;
        }
        if (c == '\n') {
            break;
        }
        if (c == 127 || c == '\b') {
            if (prefix.length() > 0) {
                prefix = prefix.substr(0, prefix.length() - 1);
            }
        } else {
            prefix += c;
        }
        showPrefixMatches(prefix);
    }
}

void studentMenu() {
    int choice = 0;
    do {
        cout << "\n=== Student Management ===\n";
        cout << "1. Add Student\n";
        cout << "2. Search by Roll Number\n";
        cout << "3. Search by Name\n";
        cout << "4. Search As You Type\n";
        cout << "5. Update Student\n";
        cout << "6. Soft Delete Student\n";
        cout << "7. List Active Students\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> choice;
        clearInput();

        if (choice == 1) {
            string roll, name, dept, semester;
            double cgpa;
            cout << "Roll (BSAI-YY-XXX): ";
            cin >> roll;
            cout << "Name: ";
            clearInput();
            std::getline(cin, name);
            cout << "Department: ";
            std::getline(cin, dept);
            cout << "Semester: ";
            cin >> semester;
            cout << "CGPA: ";
            cin >> cgpa;
            clearInput();
            if (addStudent(roll, name, dept, semester, cgpa)) {
                cout << "Student added.\n";
            } else {
                cout << "Failed to add student.\n";
            }
        } else if (choice == 2) {
            string roll;
            cout << "Roll: ";
            cin >> roll;
            clearInput();
            std::vector<std::string> row = searchByRoll(roll);
            if (row.size() == 0) {
                cout << "Not found.\n";
            } else {
                cout << row[0] << ", " << row[1] << ", " << row[2]
                     << ", Sem " << row[3] << ", CGPA " << row[4]
                     << ", " << row[5] << "\n";
            }
        } else if (choice == 3) {
            string part;
            cout << "Name substring: ";
            clearInput();
            std::getline(cin, part);
            std::vector<std::vector<std::string> > results = searchByName(part);
            for (int i = 0; i < (int)results.size(); i++) {
                cout << results[i][0] << " - " << results[i][1] << "\n";
            }
        } else if (choice == 4) {
            searchAsYouType();
        } else if (choice == 5) {
            string roll, value;
            int field;
            cout << "Roll: ";
            cin >> roll;
            cout << "Field (1=name, 2=dept, 3=semester, 4=cgpa, 5=status): ";
            cin >> field;
            cout << "New value: ";
            cin >> value;
            clearInput();
            if (updateStudent(roll, field, value)) {
                cout << "Updated.\n";
            } else {
                cout << "Update failed.\n";
            }
        } else if (choice == 6) {
            string roll;
            cout << "Roll: ";
            cin >> roll;
            clearInput();
            if (softDelete(roll)) {
                cout << "Student deactivated.\n";
            } else {
                cout << "Delete failed.\n";
            }
        } else if (choice == 7) {
            std::vector<Student> list = listActiveStudents();
            cout << "\n+------------+---------------------------+------------+\n";
            cout << "| Roll No    | Name                      | CGPA       |\n";
            cout << "+------------+---------------------------+------------+\n";
            for (int i = 0; i < (int)list.size(); i++) {
                cout << "| " << list[i].roll << " | " << list[i].name
                     << " | " << list[i].cgpa << " |\n";
            }
            cout << "+------------+---------------------------+------------+\n";
        }
        pauseScreen();
    } while (choice != 0);
}

void courseMenu() {
    int choice = 0;
    do {
        cout << "\n=== Course Management ===\n";
        cout << "1. Enroll Student\n";
        cout << "2. Drop Course\n";
        cout << "3. View Credit Load\n";
        cout << "4. Check Prerequisite\n";
        cout << "5. List Enrolled Students\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> choice;
        clearInput();

        if (choice == 1) {
            string roll, code, sem;
            cout << "Roll: ";
            cin >> roll;
            cout << "Course code: ";
            cin >> code;
            cout << "Semester: ";
            cin >> sem;
            clearInput();
            printEnrollResult(enrollStudent(roll, code, sem));
        } else if (choice == 2) {
            string roll, code, sem;
            cout << "Roll: ";
            cin >> roll;
            cout << "Course code: ";
            cin >> code;
            cout << "Semester: ";
            cin >> sem;
            clearInput();
            if (dropCourse(roll, code, sem)) {
                cout << "Course dropped.\n";
            } else {
                cout << "Drop failed (attendance exists or not enrolled).\n";
            }
        } else if (choice == 3) {
            string roll, sem;
            cout << "Roll: ";
            cin >> roll;
            cout << "Semester: ";
            cin >> sem;
            clearInput();
            cout << "Credit load: " << getCreditLoad(roll, sem) << "\n";
        } else if (choice == 4) {
            string roll, code;
            cout << "Roll: ";
            cin >> roll;
            cout << "Course code: ";
            cin >> code;
            clearInput();
            cout << (checkPrerequisite(roll, code) ? "Prerequisite met.\n" : "Prerequisite NOT met.\n");
        } else if (choice == 5) {
            string code;
            cout << "Course code: ";
            cin >> code;
            clearInput();
            std::vector<std::string> rolls = listEnrolledStudents(code);
            for (int i = 0; i < (int)rolls.size(); i++) {
                cout << rolls[i] << " - " << getStudentName(rolls[i]) << "\n";
            }
        }
        pauseScreen();
    } while (choice != 0);
}

void attendanceMenu() {
    int choice = 0;
    do {
        cout << "\n=== Attendance Management ===\n";
        cout << "1. Mark Attendance\n";
        cout << "2. View Attendance %\n";
        cout << "3. Attendance Shortage List\n";
        cout << "4. Undo Last Session\n";
        cout << "5. Daily Attendance Sheet\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> choice;
        clearInput();

        if (choice == 1) {
            string code, date;
            cout << "Course code: ";
            cin >> code;
            cout << "Date (DD-MM-YYYY): ";
            cin >> date;
            clearInput();
            if (markAttendance(code, date)) {
                cout << "Attendance marked.\n";
            } else {
                cout << "Failed to mark attendance.\n";
            }
        } else if (choice == 2) {
            string roll, code;
            cout << "Roll: ";
            cin >> roll;
            cout << "Course code: ";
            cin >> code;
            clearInput();
            cout << "Attendance: " << getAttendancePct(roll, code) << "%\n";
        } else if (choice == 3) {
            string code;
            cout << "Course code: ";
            cin >> code;
            clearInput();
            std::vector<std::vector<std::string> > list = getShortageList(code);
            for (int i = 0; i < (int)list.size(); i++) {
                cout << list[i][0] << " " << list[i][1]
                     << " " << list[i][2] << " " << list[i][3] << "%\n";
            }
        } else if (choice == 4) {
            if (undoLastSession()) {
                cout << "Last session undone.\n";
            } else {
                cout << "No backup available.\n";
            }
        } else if (choice == 5) {
            string code, date;
            cout << "Course code: ";
            cin >> code;
            cout << "Date (DD-MM-YYYY): ";
            cin >> date;
            clearInput();
            printDailySheet(code, date);
        }
        pauseScreen();
    } while (choice != 0);
}

void gradesMenu() {
    int choice = 0;
    do {
        cout << "\n=== Grades Management ===\n";
        cout << "1. Enter Marks\n";
        cout << "2. Compute GPA\n";
        cout << "3. Class Statistics\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> choice;
        clearInput();

        if (choice == 1) {
            string roll, code, sem;
            double quizzes[5];
            int quizCount = 0;
            double assignment = 0.0, midterm = 0.0, finalExam = 0.0;

            cout << "Roll: ";
            cin >> roll;
            cout << "Course code: ";
            cin >> code;
            cout << "Semester: ";
            cin >> sem;
            clearInput();

            readIntInRange("Number of quizzes (0-5): ", 0, 5, quizCount);
            for (int i = 0; i < quizCount; i++) {
                std::ostringstream quizPrompt;
                quizPrompt << "Quiz " << (i + 1) << " (0-10): ";
                readDoubleInRange(quizPrompt.str(), 0.0, 10.0, quizzes[i]);
            }
            readDoubleInRange("Assignment (0-10): ", 0.0, 10.0, assignment);
            readDoubleInRange("Midterm (0-40): ", 0.0, 40.0, midterm);
            readDoubleInRange("Final (0-60): ", 0.0, 60.0, finalExam);

            if (enterMarks(roll, code, sem, quizzes, quizCount, assignment, midterm, finalExam)) {
                std::vector<std::string> gradeRow = getGradeRow(roll, code);
                double pct = getAttendancePct(roll, code);
                if (gradeRow.size() >= 9) {
                    cout << "Marks saved. Total: " << gradeRow[7]
                         << "  Grade: " << gradeRow[8] << "\n";
                    if (pct < 75.0) {
                        cout << "Attendance is " << pct
                             << "% (below 75%). Grade overridden to F.\n";
                    }
                } else {
                    cout << "Marks saved.\n";
                }
            } else {
                cout << "Invalid marks.\n";
            }
        } else if (choice == 2) {
            string roll, sem;
            cout << "Roll: ";
            cin >> roll;
            cout << "Semester: ";
            cin >> sem;
            clearInput();
            cout << "GPA: " << computeGPA(roll, sem) << "\n";
        } else if (choice == 3) {
            string code, sem;
            cout << "Course code: ";
            cin >> code;
            cout << "Semester: ";
            cin >> sem;
            clearInput();
            Stats s = computeClassStats(code, sem);
            cout << "Highest: " << s.highest << "\n";
            cout << "Lowest: " << s.lowest << "\n";
            cout << "Mean: " << s.mean << "\n";
            cout << "Median: " << s.median << "\n";
        }
        pauseScreen();
    } while (choice != 0);
}

void feesMenu() {
    int choice = 0;
    do {
        cout << "\n=== Fee Management ===\n";
        cout << "1. Record Payment\n";
        cout << "2. Compute Late Fine\n";
        cout << "3. Generate Receipt\n";
        cout << "4. List Defaulters\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> choice;
        clearInput();

        if (choice == 1) {
            string feeId, date, method;
            double amount;
            cout << "Fee ID: ";
            cin >> feeId;
            cout << "Amount: ";
            cin >> amount;
            cout << "Payment date (DD-MM-YYYY): ";
            cin >> date;
            cout << "Method: ";
            cin >> method;
            clearInput();
            if (recordPayment(feeId, amount, date, method)) {
                cout << "Payment recorded.\n";
            } else {
                cout << "Payment failed.\n";
            }
        } else if (choice == 2) {
            string feeId;
            cout << "Fee ID: ";
            cin >> feeId;
            clearInput();
            cout << "Late fine: " << computeLateFine(feeId) << "\n";
        } else if (choice == 3) {
            string feeId;
            cout << "Fee ID: ";
            cin >> feeId;
            clearInput();
            generateReceipt(feeId);
        } else if (choice == 4) {
            string date;
            cout << "Current date (DD-MM-YYYY): ";
            cin >> date;
            clearInput();
            std::vector<FeeDefaulter> list = getDefaulters(date);
            for (int i = 0; i < (int)list.size(); i++) {
                cout << list[i].roll << " " << list[i].name
                     << " Rs." << list[i].outstanding
                     << " (" << list[i].weeksOverdue << " weeks)\n";
            }
        }
        pauseScreen();
    } while (choice != 0);
}

void reportsMenu() {
    int choice = 0;
    do {
        cout << "\n=== Reports ===\n";
        cout << "1. Merit List\n";
        cout << "2. Attendance Defaulters\n";
        cout << "3. Fee Defaulters\n";
        cout << "4. Semester Result Sheet\n";
        cout << "5. Department Summary\n";
        cout << "6. Export Report to File\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> choice;
        clearInput();

        if (choice == 1) {
            printMeritList();
        } else if (choice == 2) {
            printAttendanceDefaulters();
        } else if (choice == 3) {
            string currentDate;
            cout << "Current date (DD-MM-YYYY): ";
            cin >> currentDate;
            clearInput();
            printFeeDefaulters(currentDate);
        } else if (choice == 4) {
            string semester;
            cout << "Semester: ";
            cin >> semester;
            clearInput();
            printSemesterResult(semester);
        } else if (choice == 5) {
            printDepartmentSummary();
        } else if (choice == 6) {
            int reportChoice;
            string filename;
            string currentDate = "15-04-2024";
            string semester = "2";
            cout << "Report (1=Merit, 2=Attendance, 3=Fee, 4=Semester, 5=Dept): ";
            cin >> reportChoice;
            cout << "Output filename: ";
            cin >> filename;
            if (reportChoice == 3) {
                cout << "Current date (DD-MM-YYYY): ";
                cin >> currentDate;
            } else if (reportChoice == 4) {
                cout << "Semester: ";
                cin >> semester;
            }
            clearInput();
            exportReportToFile(reportChoice, filename, currentDate, semester);
        }
        pauseScreen();
    } while (choice != 0);
}

int main() {
    int choice = 0;
    cout << "========================================\n";
    cout << "     CAMPUS ANALYTICS ENGINE v1.0       \n";
    cout << "========================================\n";

    do {
        cout << "\n=== Main Menu ===\n";
        cout << "1. Student Management\n";
        cout << "2. Course Management\n";
        cout << "3. Attendance Management\n";
        cout << "4. Grades Management\n";
        cout << "5. Fee Management\n";
        cout << "6. Reports\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        clearInput();

        if (choice == 1) {
            studentMenu();
        } else if (choice == 2) {
            courseMenu();
        } else if (choice == 3) {
            attendanceMenu();
        } else if (choice == 4) {
            gradesMenu();
        } else if (choice == 5) {
            feesMenu();
        } else if (choice == 6) {
            reportsMenu();
        } else if (choice == 0) {
            cout << "Goodbye!\n";
        } else {
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}
