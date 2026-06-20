#include "grades.h"

#include "attendance.h"
#include "course_ops.h"
#include "filehandler.h"

#include <sstream>

const std::string GRADES_FILE = "grades.txt";
const std::vector<std::string> GRADES_HEADER =
    {"roll_no", "course_code", "semester", "quiz_avg", "assignment", "midterm", "final", "total", "letter_grade"};

double bestThreeOfFive(double quizzes[], int count) {
    if (count <= 0) {
        return 0.0;
    }
    if (count <= 3) {
        double sum = 0.0;
        for (int i = 0; i < count; i++) {
            sum += quizzes[i];
        }
        return sum / count;
    }

    double copy[5];
    int n = count;
    if (n > 5) {
        n = 5;
    }
    for (int i = 0; i < n; i++) {
        copy[i] = quizzes[i];
    }

    for (int pass = 0; pass < 2; pass++) {
        int lowestIdx = 0;
        for (int i = 1; i < n; i++) {
            if (copy[i] < copy[lowestIdx]) {
                lowestIdx = i;
            }
        }
        for (int i = lowestIdx; i < n - 1; i++) {
            copy[i] = copy[i + 1];
        }
        n--;
    }

    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += copy[i];
    }
    return sum / n;
}

double computeWeightedTotal(double quizAvg, double assignment, double midterm, double finalExam) {
    return quizAvg + assignment + (midterm / 40.0) * 30.0 + (finalExam / 60.0) * 50.0;
}

std::string getLetterGrade(double total) {
    if (total >= 85.0) return "A";
    if (total >= 80.0) return "B+";
    if (total >= 70.0) return "B";
    if (total >= 65.0) return "C+";
    if (total >= 60.0) return "C";
    if (total >= 50.0) return "D";
    return "F";
}

double getGPAPoints(const std::string& letter) {
    if (letter == "A") return 4.0;
    if (letter == "B+") return 3.5;
    if (letter == "B") return 3.0;
    if (letter == "C+") return 2.5;
    if (letter == "C") return 2.0;
    if (letter == "D") return 1.0;
    return 0.0;
}

std::string applyAttendancePenalty(const std::string& roll, const std::string& courseCode,
                                   const std::string& letterGrade) {
    double pct = getAttendancePct(roll, courseCode);
    if (pct < 75.0) {
        return "F";
    }
    return letterGrade;
}

bool enterMarks(const std::string& roll, const std::string& courseCode,
                const std::string& semester, double quizzes[], int quizCount,
                double assignment, double midterm, double finalExam) {
    if (quizCount < 0 || quizCount > 5) {
        return false;
    }
    for (int i = 0; i < quizCount; i++) {
        if (quizzes[i] < 0.0 || quizzes[i] > 10.0) {
            return false;
        }
    }
    if (assignment < 0.0 || assignment > 10.0) {
        return false;
    }
    if (midterm < 0.0 || midterm > 40.0) {
        return false;
    }
    if (finalExam < 0.0 || finalExam > 60.0) {
        return false;
    }

    double quizAvg = bestThreeOfFive(quizzes, quizCount);
    double total = computeWeightedTotal(quizAvg, assignment, midterm, finalExam);
    std::string letter = getLetterGrade(total);
    letter = applyAttendancePenalty(roll, courseCode, letter);

    std::vector<std::vector<std::string> > rows = readTXT(GRADES_FILE);
    bool updated = false;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 3 && rows[i][0] == roll && rows[i][1] == courseCode) {
            std::ostringstream q, a, m, f, t, l;
            q << quizAvg;
            a << assignment;
            m << midterm;
            f << finalExam;
            t << total;
            l << letter;
            if (rows[i].size() < 9) {
                rows[i].resize(9);
            }
            rows[i][2] = semester;
            rows[i][3] = q.str();
            rows[i][4] = a.str();
            rows[i][5] = m.str();
            rows[i][6] = f.str();
            rows[i][7] = t.str();
            rows[i][8] = letter;
            updated = true;
            break;
        }
    }

    if (!updated) {
        std::ostringstream q, a, m, f, t, l;
        q << quizAvg;
        a << assignment;
        m << midterm;
        f << finalExam;
        t << total;
        l << letter;
        std::vector<std::string> row;
        row.push_back(roll);
        row.push_back(courseCode);
        row.push_back(semester);
        row.push_back(q.str());
        row.push_back(a.str());
        row.push_back(m.str());
        row.push_back(f.str());
        row.push_back(t.str());
        row.push_back(letter);
        rows.push_back(row);
    }

    return writeTXT(GRADES_FILE, GRADES_HEADER, rows);
}

double computeGPA(const std::string& roll, const std::string& semester) {
    std::vector<std::vector<std::string> > grades = readTXT(GRADES_FILE);
    double weightedSum = 0.0;
    int totalCredits = 0;

    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 9 && grades[i][0] == roll && grades[i][2] == semester) {
            std::string letter = grades[i][8];
            int credits = getCourseCredits(grades[i][1]);
            double points = getGPAPoints(letter);
            weightedSum += points * credits;
            totalCredits += credits;
        }
    }

    if (totalCredits == 0) {
        return 0.0;
    }
    return weightedSum / totalCredits;
}

Stats computeClassStats(const std::string& courseCode, const std::string& semester) {
    Stats s;
    s.highest = 0.0;
    s.lowest = 100.0;
    s.mean = 0.0;
    s.median = 0.0;

    std::vector<double> scores;
    std::vector<std::vector<std::string> > grades = readTXT(GRADES_FILE);
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 9 &&
            grades[i][1] == courseCode &&
            grades[i][2] == semester) {
            std::istringstream iss(grades[i][7]);
            double total = 0.0;
            iss >> total;
            scores.push_back(total);
        }
    }

    int n = (int)scores.size();
    if (n == 0) {
        s.lowest = 0.0;
        return s;
    }

    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        if (scores[i] > s.highest) {
            s.highest = scores[i];
        }
        if (scores[i] < s.lowest) {
            s.lowest = scores[i];
        }
        sum += scores[i];
    }
    s.mean = sum / n;

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (scores[j] > scores[j + 1]) {
                double temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }

    if (n % 2 == 1) {
        s.median = scores[n / 2];
    } else {
        s.median = (scores[n / 2 - 1] + scores[n / 2]) / 2.0;
    }
    return s;
}

std::vector<std::string> getGradeRow(const std::string& roll, const std::string& courseCode) {
    std::vector<std::vector<std::string> > rows = readTXT(GRADES_FILE);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 2 && rows[i][0] == roll && rows[i][1] == courseCode) {
            return rows[i];
        }
    }
    return std::vector<std::string>();
}
