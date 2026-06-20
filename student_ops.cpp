#include "student_ops.h"

#include "filehandler.h"

#include <cctype>
#include <sstream>

const std::string STUDENTS_FILE = "students.txt";
const std::vector<std::string> STUDENT_HEADER =
    {"roll_no", "name", "department", "semester", "cgpa", "status"};

bool validateRollFormat(const std::string& roll) {
    if (roll.length() != 11) {
        return false;
    }
    if (roll.substr(0, 5) != "BSAI-") {
        return false;
    }
    if (roll[7] != '-') {
        return false;
    }
    for (int i = 5; i < 7; i++) {
        if (!isdigit(roll[i])) {
            return false;
        }
    }
    for (int i = 8; i < 11; i++) {
        if (!isdigit(roll[i])) {
            return false;
        }
    }
    return true;
}

bool nameHasNoDigits(const std::string& name) {
    for (int i = 0; i < (int)name.length(); i++) {
        if (isdigit(name[i])) {
            return false;
        }
    }
    return name.length() > 0;
}

bool addStudent(const std::string& roll, const std::string& name,
                const std::string& dept, const std::string& semester, double cgpa) {
    if (!validateRollFormat(roll)) {
        return false;
    }
    if (rowExists(STUDENTS_FILE, 0, roll)) {
        return false;
    }
    if (!nameHasNoDigits(name)) {
        return false;
    }
    if (cgpa < 0.0 || cgpa > 4.0) {
        return false;
    }

    std::ostringstream cgpaStr;
    cgpaStr << cgpa;

    std::vector<std::string> row;
    row.push_back(roll);
    row.push_back(name);
    row.push_back(dept);
    row.push_back(semester);
    row.push_back(cgpaStr.str());
    row.push_back("active");
    return appendTXT(STUDENTS_FILE, row);
}

std::vector<std::string> searchByRoll(const std::string& roll) {
    return findRow(STUDENTS_FILE, 0, roll);
}

bool nameContainsPart(const std::string& name, const std::string& namePart) {
    if ((int)namePart.length() == 0) {
        return true;
    }
    if ((int)namePart.length() > (int)name.length()) {
        return false;
    }
    for (int i = 0; i <= (int)name.length() - (int)namePart.length(); i++) {
        if (name.substr(i, namePart.length()) == namePart) {
            return true;
        }
    }
    return false;
}

std::vector<std::vector<std::string> > searchByName(const std::string& namePart) {
    std::vector<std::vector<std::string> > results;
    std::vector<std::vector<std::string> > rows = readTXT(STUDENTS_FILE);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() > 1 && nameContainsPart(rows[i][1], namePart)) {
            results.push_back(rows[i]);
        }
    }
    return results;
}

std::vector<std::vector<std::string> > searchByNamePrefix(const std::string& prefix) {
    std::vector<std::vector<std::string> > results;
    std::vector<std::vector<std::string> > rows = readTXT(STUDENTS_FILE);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() > 1 && rows[i][5] == "active") {
            std::string name = rows[i][1];
            if ((int)prefix.length() <= (int)name.length()) {
                if (name.substr(0, prefix.length()) == prefix) {
                    results.push_back(rows[i]);
                }
            }
        }
    }
    return results;
}

bool updateStudent(const std::string& roll, int fieldIndex, const std::string& newValue) {
    if (fieldIndex == 0) {
        return false;
    }
    std::vector<std::vector<std::string> > rows = readTXT(STUDENTS_FILE);
    bool found = false;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() > 0 && rows[i][0] == roll) {
            if (fieldIndex == 4) {
                std::istringstream iss(newValue);
                double cgpa = 0.0;
                iss >> cgpa;
                if (cgpa < 0.0 || cgpa > 4.0) {
                    return false;
                }
            }
            if (fieldIndex == 1 && !nameHasNoDigits(newValue)) {
                return false;
            }
            if (fieldIndex < (int)rows[i].size()) {
                rows[i][fieldIndex] = newValue;
            }
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }
    return writeTXT(STUDENTS_FILE, STUDENT_HEADER, rows);
}

bool softDelete(const std::string& roll) {
    return updateStudent(roll, 5, "inactive");
}

std::vector<Student> listActiveStudents() {
    std::vector<Student> students;
    std::vector<std::vector<std::string> > rows = readTXT(STUDENTS_FILE);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 6 && rows[i][5] == "active") {
            Student s;
            s.roll = rows[i][0];
            s.name = rows[i][1];
            s.dept = rows[i][2];
            s.semester = rows[i][3];
            std::istringstream iss(rows[i][4]);
            iss >> s.cgpa;
            s.status = rows[i][5];
            students.push_back(s);
        }
    }

    int n = (int)students.size();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (students[j].roll < students[minIdx].roll) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            Student temp = students[i];
            students[i] = students[minIdx];
            students[minIdx] = temp;
        }
    }
    return students;
}

std::string getStudentName(const std::string& roll) {
    std::vector<std::string> row = searchByRoll(roll);
    if (row.size() > 1) {
        return row[1];
    }
    return "";
}

bool isStudentActive(const std::string& roll) {
    std::vector<std::string> row = searchByRoll(roll);
    if (row.size() >= 6) {
        return row[5] == "active";
    }
    return false;
}
