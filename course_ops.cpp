#include "course_ops.h"

#include "filehandler.h"
#include "student_ops.h"

#include <sstream>

const std::string COURSES_FILE = "courses.txt";
const std::string ENROLLMENTS_FILE = "enrollments.txt";
const std::string GRADES_FILE = "grades.txt";
const std::vector<std::string> ENROLLMENT_HEADER =
    {"enrollment_id", "roll_no", "course_code", "semester", "enrollment_date", "status"};

bool courseExists(const std::string& courseCode) {
    return rowExists(COURSES_FILE, 0, courseCode);
}

std::vector<std::string> getCourseInfo(const std::string& courseCode) {
    return findRow(COURSES_FILE, 0, courseCode);
}

int getCourseCredits(const std::string& courseCode) {
    std::vector<std::string> course = getCourseInfo(courseCode);
    if (course.size() > 2) {
        std::istringstream iss(course[2]);
        int credits = 0;
        iss >> credits;
        return credits;
    }
    return 0;
}

bool isEnrolled(const std::string& roll, const std::string& courseCode,
                const std::string& semester) {
    std::vector<std::vector<std::string> > rows = readTXT(ENROLLMENTS_FILE);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 6 &&
            rows[i][1] == roll &&
            rows[i][2] == courseCode &&
            rows[i][3] == semester &&
            rows[i][5] == "active") {
            return true;
        }
    }
    return false;
}

int countEnrolledInCourse(const std::string& courseCode) {
    int count = 0;
    std::vector<std::vector<std::string> > rows = readTXT(ENROLLMENTS_FILE);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 6 && rows[i][2] == courseCode && rows[i][5] == "active") {
            count++;
        }
    }
    return count;
}

std::string nextEnrollmentId() {
    std::vector<std::vector<std::string> > rows = readTXT(ENROLLMENTS_FILE);
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
    oss << "E";
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

bool checkPrerequisite(const std::string& roll, const std::string& courseCode) {
    std::vector<std::string> course = getCourseInfo(courseCode);
    if (course.size() < 7) {
        return false;
    }
    std::string prereq = course[6];
    if (prereq == "NONE") {
        return true;
    }

    std::vector<std::vector<std::string> > grades = readTXT(GRADES_FILE);
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 9 &&
            grades[i][0] == roll &&
            grades[i][1] == prereq) {
            std::string letter = grades[i][8];
            if (letter != "F") {
                return true;
            }
        }
    }
    return false;
}

int getCreditLoad(const std::string& roll, const std::string& semester) {
    int totalCredits = 0;
    std::vector<std::vector<std::string> > enrollments = readTXT(ENROLLMENTS_FILE);
    std::vector<std::vector<std::string> > courses = readTXT(COURSES_FILE);

    for (int i = 0; i < (int)enrollments.size(); i++) {
        if (enrollments[i].size() >= 6 &&
            enrollments[i][1] == roll &&
            enrollments[i][3] == semester &&
            enrollments[i][5] == "active") {
            std::string code = enrollments[i][2];
            for (int j = 0; j < (int)courses.size(); j++) {
                if (courses[j].size() >= 3 && courses[j][0] == code) {
                    std::istringstream iss(courses[j][2]);
                    int credits = 0;
                    iss >> credits;
                    totalCredits += credits;
                    break;
                }
            }
        }
    }
    return totalCredits;
}

EnrollResult enrollStudent(const std::string& roll, const std::string& courseCode,
                           const std::string& semester) {
    if (!isStudentActive(roll)) {
        return ENROLL_STUDENT_INACTIVE;
    }
    if (!courseExists(courseCode)) {
        return ENROLL_COURSE_NOT_FOUND;
    }

    std::vector<std::string> course = getCourseInfo(courseCode);
    if (course.size() < 5) {
        return ENROLL_COURSE_NOT_FOUND;
    }

    std::istringstream capIss(course[4]);
    int capacity = 0;
    capIss >> capacity;
    int enrolled = countEnrolledInCourse(courseCode);
    if (enrolled >= capacity) {
        return ENROLL_NO_SEATS;
    }

    if (isEnrolled(roll, courseCode, semester)) {
        return ENROLL_ALREADY_ENROLLED;
    }

    int credits = getCourseCredits(courseCode);
    int currentLoad = getCreditLoad(roll, semester);
    if (currentLoad + credits > 21) {
        return ENROLL_CREDIT_OVERLOAD;
    }

    if (!checkPrerequisite(roll, courseCode)) {
        return ENROLL_PREREQ_FAILED;
    }

    std::vector<std::string> row;
    row.push_back(nextEnrollmentId());
    row.push_back(roll);
    row.push_back(courseCode);
    row.push_back(semester);
    row.push_back("15-01-2024");
    row.push_back("active");
    if (!appendTXT(ENROLLMENTS_FILE, row)) {
        return ENROLL_COURSE_NOT_FOUND;
    }

    std::vector<std::vector<std::string> > courses = readTXT(COURSES_FILE);
    std::vector<std::string> header =
        {"course_code", "course_name", "credit_hours", "instructor", "capacity", "enrolled", "prerequisite"};
    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].size() >= 6 && courses[i][0] == courseCode) {
            std::istringstream iss(courses[i][5]);
            int enr = 0;
            iss >> enr;
            enr++;
            std::ostringstream oss;
            oss << enr;
            courses[i][5] = oss.str();
            break;
        }
    }
    writeTXT(COURSES_FILE, header, courses);
    return ENROLL_SUCCESS;
}

bool hasAttendanceForEnrollment(const std::string& roll, const std::string& courseCode) {
    std::vector<std::vector<std::string> > logs = readTXT("attendance_log.txt");
    for (int i = 0; i < (int)logs.size(); i++) {
        if (logs[i].size() >= 3 && logs[i][1] == roll && logs[i][2] == courseCode) {
            return true;
        }
    }
    return false;
}

bool dropCourse(const std::string& roll, const std::string& courseCode,
                const std::string& semester) {
    if (hasAttendanceForEnrollment(roll, courseCode)) {
        return false;
    }

    std::vector<std::vector<std::string> > rows = readTXT(ENROLLMENTS_FILE);
    bool found = false;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 6 &&
            rows[i][1] == roll &&
            rows[i][2] == courseCode &&
            rows[i][3] == semester &&
            rows[i][5] == "active") {
            rows[i][5] = "dropped";
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }

    writeTXT(ENROLLMENTS_FILE, ENROLLMENT_HEADER, rows);

    std::vector<std::vector<std::string> > courses = readTXT(COURSES_FILE);
    std::vector<std::string> header =
        {"course_code", "course_name", "credit_hours", "instructor", "capacity", "enrolled", "prerequisite"};
    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].size() >= 6 && courses[i][0] == courseCode) {
            std::istringstream iss(courses[i][5]);
            int enr = 0;
            iss >> enr;
            if (enr > 0) {
                enr--;
            }
            std::ostringstream oss;
            oss << enr;
            courses[i][5] = oss.str();
            break;
        }
    }
    writeTXT(COURSES_FILE, header, courses);
    return true;
}

std::vector<std::string> listEnrolledStudents(const std::string& courseCode) {
    std::vector<std::string> rolls;
    std::vector<std::vector<std::string> > rows = readTXT(ENROLLMENTS_FILE);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 6 && rows[i][2] == courseCode && rows[i][5] == "active") {
            if (isStudentActive(rows[i][1])) {
                rolls.push_back(rows[i][1]);
            }
        }
    }
    return rolls;
}
