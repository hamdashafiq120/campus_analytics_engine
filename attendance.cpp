#include "attendance.h"

#include "course_ops.h"
#include "filehandler.h"
#include "student_ops.h"

#include <cctype>
#include <iostream>
#include <iomanip>
#include <sstream>

const std::string ATTENDANCE_FILE = "attendance_log.txt";
const std::vector<std::string> ATTENDANCE_HEADER =
    {"log_id", "roll_no", "course_code", "session_date", "status"};

static std::vector<std::vector<std::string> > attendanceBackup;
static bool hasBackup = false;

std::string nextLogId() {
    std::vector<std::vector<std::string> > rows = readTXT(ATTENDANCE_FILE);
    int maxId = 0;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() > 0 && rows[i][0].length() > 1) {
            std::string numPart = rows[i][0].substr(1);
            std::istringstream iss(numPart);
            int id = 0;
            iss >> id;
            if (id > maxId) {
                maxId = id;
            }
        }
    }
    maxId++;
    std::ostringstream oss;
    oss << "L";
    if (maxId < 100000) {
        oss << "0";
    }
    if (maxId < 10000) {
        oss << "0";
    }
    if (maxId < 1000) {
        oss << "0";
    }
    if (maxId < 100) {
        oss << "0";
    }
    if (maxId < 10) {
        oss << "0";
    }
    oss << maxId;
    return oss.str();
}

bool markAttendance(const std::string& courseCode, const std::string& date) {
    std::vector<std::string> students = listEnrolledStudents(courseCode);
    if (students.size() == 0) {
        return false;
    }

    attendanceBackup = readTXT(ATTENDANCE_FILE);
    hasBackup = true;

    for (int i = 0; i < (int)students.size(); i++) {
        std::string roll = students[i];
        std::string name = getStudentName(roll);
        char status = ' ';
        while (status != 'P' && status != 'A' && status != 'L') {
            std::cout << roll << " - " << name << " [P/A/L]: ";
            std::cin >> status;
            status = (char)toupper(status);
            if (status != 'P' && status != 'A' && status != 'L') {
                std::cout << "Invalid. Enter P, A, or L.\n";
            }
        }

        std::vector<std::string> row;
        row.push_back(nextLogId());
        row.push_back(roll);
        row.push_back(courseCode);
        row.push_back(date);
        std::string statusStr(1, status);
        row.push_back(statusStr);
        if (!appendTXT(ATTENDANCE_FILE, row)) {
            writeTXT(ATTENDANCE_FILE, ATTENDANCE_HEADER, attendanceBackup);
            hasBackup = false;
            return false;
        }
    }
    return true;
}

double getAttendancePct(const std::string& roll, const std::string& courseCode) {
    std::vector<std::vector<std::string> > rows = readTXT(ATTENDANCE_FILE);
    int present = 0;
    int late = 0;
    int total = 0;

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 5 &&
            rows[i][1] == roll &&
            rows[i][2] == courseCode) {
            total++;
            if (rows[i][4] == "P") {
                present++;
            } else if (rows[i][4] == "L") {
                late++;
            }
        }
    }

    if (total == 0) {
        return 0.0;
    }
    return ((present + 0.5 * late) / total) * 100.0;
}

std::vector<std::vector<std::string> > getShortageList(const std::string& courseCode) {
    std::vector<std::vector<std::string> > shortage;
    std::vector<std::string> students = listEnrolledStudents(courseCode);
    for (int i = 0; i < (int)students.size(); i++) {
        double pct = getAttendancePct(students[i], courseCode);
        if (pct < 75.0) {
            std::vector<std::string> entry;
            entry.push_back(students[i]);
            entry.push_back(getStudentName(students[i]));
            entry.push_back(courseCode);
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << pct;
            entry.push_back(oss.str());
            shortage.push_back(entry);
        }
    }
    return shortage;
}

bool undoLastSession() {
    if (!hasBackup) {
        return false;
    }
    writeTXT(ATTENDANCE_FILE, ATTENDANCE_HEADER, attendanceBackup);
    hasBackup = false;
    return true;
}

void printDailySheet(const std::string& courseCode, const std::string& date) {
    std::vector<std::string> students = listEnrolledStudents(courseCode);
    std::cout << "\n+------------+---------------------------+----------+\n";
    std::cout << "| Roll No    | Name                      | Status   |\n";
    std::cout << "+------------+---------------------------+----------+\n";

    for (int i = 0; i < (int)students.size(); i++) {
        std::string roll = students[i];
        std::string name = getStudentName(roll);
        std::string status = "-";
        std::vector<std::vector<std::string> > rows = readTXT(ATTENDANCE_FILE);
        for (int j = 0; j < (int)rows.size(); j++) {
            if (rows[j].size() >= 5 &&
                rows[j][1] == roll &&
                rows[j][2] == courseCode &&
                rows[j][3] == date) {
                status = rows[j][4];
                break;
            }
        }
        std::cout << "| " << std::setw(10) << std::left << roll
                  << " | " << std::setw(25) << name
                  << " | " << std::setw(8) << status << " |\n";
    }
    std::cout << "+------------+---------------------------+----------+\n";
    std::cout << "Course: " << courseCode << "  Date: " << date << "\n\n";
}
