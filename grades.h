#ifndef GRADES_H
#define GRADES_H

#include <string>
#include <vector>

struct Stats {
    double highest;
    double lowest;
    double mean;
    double median;
};

double bestThreeOfFive(double quizzes[], int count);
double computeWeightedTotal(double quizAvg, double assignment, double midterm, double finalExam);
std::string getLetterGrade(double total);
double getGPAPoints(const std::string& letter);
bool enterMarks(const std::string& roll, const std::string& courseCode,
                const std::string& semester, double quizzes[], int quizCount,
                double assignment, double midterm, double finalExam);
double computeGPA(const std::string& roll, const std::string& semester);
Stats computeClassStats(const std::string& courseCode, const std::string& semester);
std::string applyAttendancePenalty(const std::string& roll, const std::string& courseCode,
                                   const std::string& letterGrade);
std::vector<std::string> getGradeRow(const std::string& roll, const std::string& courseCode);

#endif
