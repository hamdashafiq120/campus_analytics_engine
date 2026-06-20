#ifndef STUDENT_OPS_H
#define STUDENT_OPS_H

#include <string>
#include <vector>

struct Student {
    std::string roll;
    std::string name;
    std::string dept;
    std::string semester;
    double cgpa;
    std::string status;
};

bool addStudent(const std::string& roll, const std::string& name,
                const std::string& dept, const std::string& semester, double cgpa);
std::vector<std::string> searchByRoll(const std::string& roll);
std::vector<std::vector<std::string> > searchByName(const std::string& namePart);
bool updateStudent(const std::string& roll, int fieldIndex, const std::string& newValue);
bool softDelete(const std::string& roll);
std::vector<Student> listActiveStudents();
std::vector<std::vector<std::string> > searchByNamePrefix(const std::string& prefix);
std::string getStudentName(const std::string& roll);
bool isStudentActive(const std::string& roll);

#endif
