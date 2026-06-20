#include "reports.h"

#include "attendance.h"
#include "course_ops.h"
#include "fee_tracker.h"
#include "filehandler.h"
#include "grades.h"
#include "student_ops.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

void printMeritList() {
    std::vector<std::vector<std::string> > rows = readTXT("students.txt");
    std::vector<std::vector<std::string> > active;

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 6 && rows[i][5] == "active") {
            active.push_back(rows[i]);
        }
    }

    int n = (int)active.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            std::istringstream iss1(active[j][4]);
            std::istringstream iss2(active[j + 1][4]);
            double cgpa1 = 0.0, cgpa2 = 0.0;
            iss1 >> cgpa1;
            iss2 >> cgpa2;
            if (cgpa1 < cgpa2) {
                std::vector<std::string> temp = active[j];
                active[j] = active[j + 1];
                active[j + 1] = temp;
            }
        }
    }

    std::cout << "\n+------+------------+---------------------------+------------+\n";
    std::cout << "| Rank | Roll No    | Name                      | CGPA       |\n";
    std::cout << "+------+------------+---------------------------+------------+\n";
    for (int i = 0; i < n; i++) {
        std::cout << "| " << std::setw(4) << std::right << (i + 1)
                  << " | " << std::setw(10) << std::left << active[i][0]
                  << " | " << std::setw(25) << active[i][1]
                  << " | " << std::setw(10) << std::right << active[i][4] << " |\n";
    }
    std::cout << "+------+------------+---------------------------+------------+\n\n";
}

void printAttendanceDefaulters() {
    std::vector<std::string> courses;
    std::vector<std::vector<std::string> > courseRows = readTXT("courses.txt");
    for (int i = 0; i < (int)courseRows.size(); i++) {
        if (courseRows[i].size() > 0) {
            courses.push_back(courseRows[i][0]);
        }
    }

    std::cout << "\n+------------+---------------------------+----------+------------+\n";
    std::cout << "| Roll No    | Name                      | Course   | Attend %   |\n";
    std::cout << "+------------+---------------------------+----------+------------+\n";

    bool any = false;
    for (int c = 0; c < (int)courses.size(); c++) {
        std::vector<std::vector<std::string> > shortage = getShortageList(courses[c]);
        for (int i = 0; i < (int)shortage.size(); i++) {
            any = true;
            std::cout << "| " << std::setw(10) << std::left << shortage[i][0]
                      << " | " << std::setw(25) << shortage[i][1]
                      << " | " << std::setw(8) << shortage[i][2]
                      << " | " << std::setw(10) << shortage[i][3] << " |\n";
        }
    }
    if (!any) {
        std::cout << "| (none)     |                           |          |            |\n";
    }
    std::cout << "+------------+---------------------------+----------+------------+\n\n";
}

void printFeeDefaulters(const std::string& currentDate) {
    std::vector<FeeDefaulter> defaulters = getDefaulters(currentDate);
    std::cout << "\n+------------+---------------------------+------------+------------+\n";
    std::cout << "| Roll No    | Name                      | Outstanding| Weeks Late |\n";
    std::cout << "+------------+---------------------------+------------+------------+\n";

    if (defaulters.size() == 0) {
        std::cout << "| (none)     |                           |            |            |\n";
    } else {
        for (int i = 0; i < (int)defaulters.size(); i++) {
            std::cout << "| " << std::setw(10) << std::left << defaulters[i].roll
                      << " | " << std::setw(25) << defaulters[i].name
                      << " | " << std::setw(10) << std::fixed << std::setprecision(0)
                      << defaulters[i].outstanding
                      << " | " << std::setw(10) << defaulters[i].weeksOverdue << " |\n";
        }
    }
    std::cout << "+------------+---------------------------+------------+------------+\n\n";
}

void printSemesterResult(const std::string& semester) {
    std::vector<Student> students = listActiveStudents();
    std::cout << "\n";
    std::cout << "================================================================\n";
    std::cout << "              SEMESTER " << semester << " RESULT SHEET                  \n";
    std::cout << "================================================================\n";
    std::cout << std::left << std::setw(12) << "Roll"
              << std::setw(22) << "Name"
              << std::setw(8) << "Course"
              << std::setw(8) << "Grade"
              << std::setw(8) << "GPA"
              << std::setw(12) << "Attendance"
              << "\n";
    std::cout << "----------------------------------------------------------------\n";

    for (int i = 0; i < (int)students.size(); i++) {
        double gpa = computeGPA(students[i].roll, semester);
        std::ostringstream gpaStr;
        gpaStr << std::fixed << std::setprecision(2) << gpa;

        std::vector<std::vector<std::string> > enrollments = readTXT("enrollments.txt");
        bool hasCourse = false;
        for (int e = 0; e < (int)enrollments.size(); e++) {
            if (enrollments[e].size() >= 6 &&
                enrollments[e][1] == students[i].roll &&
                enrollments[e][3] == semester &&
                enrollments[e][5] == "active") {
                hasCourse = true;
                std::string course = enrollments[e][2];
                std::vector<std::string> gradeRow = getGradeRow(students[i].roll, course);
                std::string letter = "-";
                if (gradeRow.size() >= 9) {
                    letter = gradeRow[8];
                }
                double pct = getAttendancePct(students[i].roll, course);
                std::ostringstream pctStr;
                pctStr << std::fixed << std::setprecision(1) << pct << "%";

                std::cout << std::setw(12) << students[i].roll
                          << std::setw(22) << students[i].name
                          << std::setw(8) << course
                          << std::setw(8) << letter
                          << std::setw(8) << gpaStr.str()
                          << std::setw(12) << pctStr.str()
                          << "\n";
            }
        }
        if (!hasCourse) {
            std::cout << std::setw(12) << students[i].roll
                      << std::setw(22) << students[i].name
                      << std::setw(8) << "-"
                      << std::setw(8) << "-"
                      << std::setw(8) << gpaStr.str()
                      << std::setw(12) << "-"
                      << "\n";
        }
    }
    std::cout << "================================================================\n\n";
}

void printDepartmentSummary() {
    std::vector<std::vector<std::string> > rows = readTXT("students.txt");

    std::string deptNames[20];
    int deptCounts[20];
    double deptCgpaSum[20];
    int deptPassCount[20];
    int deptTotal[20];
    int deptCount = 0;

    for (int i = 0; i < 20; i++) {
        deptCounts[i] = 0;
        deptCgpaSum[i] = 0.0;
        deptPassCount[i] = 0;
        deptTotal[i] = 0;
    }

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 6 || rows[i][5] != "active") {
            continue;
        }
        std::string dept = rows[i][2];
        int idx = -1;
        for (int d = 0; d < deptCount; d++) {
            if (deptNames[d] == dept) {
                idx = d;
                break;
            }
        }
        if (idx == -1 && deptCount < 20) {
            idx = deptCount;
            deptNames[deptCount] = dept;
            deptCount++;
        }
        if (idx >= 0) {
            deptCounts[idx]++;
            std::istringstream iss(rows[i][4]);
            double cgpa = 0.0;
            iss >> cgpa;
            deptCgpaSum[idx] += cgpa;
            deptTotal[idx]++;
            if (cgpa >= 2.0) {
                deptPassCount[idx]++;
            }
        }
    }

    std::cout << "\n+---------------------------+--------+------------+------------+\n";
    std::cout << "| Department                | Count  | Avg CGPA   | Pass Rate  |\n";
    std::cout << "+---------------------------+--------+------------+------------+\n";

    for (int d = 0; d < deptCount; d++) {
        double avgCgpa = deptTotal[d] > 0 ? deptCgpaSum[d] / deptTotal[d] : 0.0;
        double passRate = deptTotal[d] > 0 ?
            (100.0 * deptPassCount[d] / deptTotal[d]) : 0.0;
        std::ostringstream avgStr, passStr;
        avgStr << std::fixed << std::setprecision(2) << avgCgpa;
        passStr << std::fixed << std::setprecision(1) << passRate << "%";

        std::cout << "| " << std::setw(25) << std::left << deptNames[d]
                  << " | " << std::setw(6) << std::right << deptCounts[d]
                  << " | " << std::setw(10) << avgStr.str()
                  << " | " << std::setw(10) << passStr.str() << " |\n";
    }
    std::cout << "+---------------------------+--------+------------+------------+\n\n";
}

void exportReportToFile(int reportChoice, const std::string& filename,
                        const std::string& currentDate, const std::string& semester) {
    std::streambuf* oldBuf = std::cout.rdbuf();
    std::ofstream outFile(filename.c_str());
    if (!outFile.is_open()) {
        std::cout << "Could not open file for export.\n";
        return;
    }
    std::cout.rdbuf(outFile.rdbuf());

    if (reportChoice == 1) {
        printMeritList();
    } else if (reportChoice == 2) {
        printAttendanceDefaulters();
    } else if (reportChoice == 3) {
        printFeeDefaulters(currentDate);
    } else if (reportChoice == 4) {
        printSemesterResult(semester);
    } else if (reportChoice == 5) {
        printDepartmentSummary();
    }

    std::cout.rdbuf(oldBuf);
    outFile.close();
    std::cout << "Report exported to " << filename << "\n";
}
