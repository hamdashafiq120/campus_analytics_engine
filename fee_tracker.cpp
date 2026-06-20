#include "fee_tracker.h"

#include "filehandler.h"
#include "student_ops.h"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

const std::string FEES_FILE = "fees.txt";
const std::vector<std::string> FEES_HEADER =
    {"fee_id", "roll_no", "semester", "total_fee", "amount_paid", "due_date",
     "payment_date", "payment_method", "status"};

int parseDay(const std::string& date);
int parseMonth(const std::string& date);
int parseYear(const std::string& date);
int daysInMonth(int month, int year);

int parseDay(const std::string& date) {
    std::istringstream iss(date.substr(0, 2));
    int d = 0;
    iss >> d;
    return d;
}

int parseMonth(const std::string& date) {
    std::istringstream iss(date.substr(3, 2));
    int m = 0;
    iss >> m;
    return m;
}

int parseYear(const std::string& date) {
    std::istringstream iss(date.substr(6, 4));
    int y = 0;
    iss >> y;
    return y;
}

bool isLeapYear(int year) {
    if (year % 400 == 0) return true;
    if (year % 100 == 0) return false;
    return (year % 4 == 0);
}

int daysInMonth(int month, int year) {
    int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    if (month >= 1 && month <= 12) {
        return days[month];
    }
    return 0;
}

bool isValidDate(const std::string& date) {
    if (date.length() != 10) {
        return false;
    }
    if (date[2] != '-' || date[5] != '-') {
        return false;
    }
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) {
            continue;
        }
        if (!isdigit(date[i])) {
            return false;
        }
    }

    int day = parseDay(date);
    int month = parseMonth(date);
    int year = parseYear(date);
    if (month < 1 || month > 12) {
        return false;
    }
    if (day < 1 || day > daysInMonth(month, year)) {
        return false;
    }
    if (year < 1) {
        return false;
    }
    return true;
}

int dateToDays(const std::string& date) {
    int day = parseDay(date);
    int month = parseMonth(date);
    int year = parseYear(date);
    int total = 0;
    for (int y = 1; y < year; y++) {
        total += isLeapYear(y) ? 366 : 365;
    }
    for (int m = 1; m < month; m++) {
        total += daysInMonth(m, year);
    }
    total += day;
    return total;
}

int daysBetween(const std::string& date1, const std::string& date2) {
    return dateToDays(date2) - dateToDays(date1);
}

bool recordPayment(const std::string& feeId, double amount, const std::string& paidDate,
                   const std::string& method) {
    if (!isValidDate(paidDate)) {
        return false;
    }
    if (amount <= 0.0) {
        return false;
    }

    std::vector<std::vector<std::string> > rows = readTXT(FEES_FILE);
    bool found = false;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() >= 9 && rows[i][0] == feeId) {
            std::istringstream paidIss(rows[i][4]);
            double paid = 0.0;
            paidIss >> paid;
            std::istringstream totalIss(rows[i][3]);
            double total = 0.0;
            totalIss >> total;

            paid += amount;
            if (paid > total) {
                return false;
            }

            std::ostringstream paidStr;
            paidStr << paid;
            rows[i][4] = paidStr.str();
            rows[i][6] = paidDate;
            rows[i][7] = method;

            if (paid >= total) {
                std::string dueDate = rows[i][5];
                if (daysBetween(dueDate, paidDate) > 0) {
                    rows[i][8] = "paid_late";
                } else {
                    rows[i][8] = "paid";
                }
            } else {
                rows[i][8] = "partial";
            }
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }
    return writeTXT(FEES_FILE, FEES_HEADER, rows);
}

double computeLateFine(const std::string& feeId) {
    std::vector<std::string> row = findRow(FEES_FILE, 0, feeId);
    if (row.size() < 7) {
        return 0.0;
    }

    std::string dueDate = row[5];
    std::string paidDate = row[6];
    if (!isValidDate(paidDate) || paidDate == "00-00-0000") {
        return 0.0;
    }

    int lateDays = daysBetween(dueDate, paidDate);
    if (lateDays <= 0) {
        return 0.0;
    }

    int weeks = lateDays / 7;
    std::istringstream iss(row[3]);
    double totalFee = 0.0;
    iss >> totalFee;
    return totalFee * 0.02 * weeks;
}

void generateReceipt(const std::string& feeId) {
    std::vector<std::string> row = findRow(FEES_FILE, 0, feeId);
    if (row.size() < 9) {
        std::cout << "Fee record not found.\n";
        return;
    }

    std::string roll = row[1];
    std::string name = getStudentName(roll);
    std::istringstream totalIss(row[3]);
    double totalFee = 0.0;
    totalIss >> totalFee;
    std::istringstream paidIss(row[4]);
    double amountPaid = 0.0;
    paidIss >> amountPaid;
    double fine = computeLateFine(feeId);
    double totalDue = totalFee + fine;
    double balance = totalDue - amountPaid;

    std::cout << "\n";
    std::cout << std::setfill('=') << std::setw(40) << "" << "\n";
    std::cout << std::setfill(' ') << std::left << std::setw(40) << "         FEE PAYMENT RECEIPT" << "\n";
    std::cout << std::setfill('=') << std::setw(40) << "" << "\n";
    std::cout << std::setfill('.') << std::left;
    std::cout << std::setw(20) << "Fee ID" << std::setfill(' ') << " " << feeId << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Roll No" << std::setfill(' ') << " " << roll << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Name" << std::setfill(' ') << " " << name << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Semester" << std::setfill(' ') << " " << row[2] << "\n";
    std::cout << std::setfill('-') << std::setw(40) << "" << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Tuition Fee" << std::setfill(' ')
              << std::fixed << std::setprecision(0) << " " << totalFee << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Late Fine" << std::setfill(' ') << " " << fine << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Total Due" << std::setfill(' ') << " " << totalDue << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Amount Paid" << std::setfill(' ') << " " << amountPaid << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Balance" << std::setfill(' ') << " " << balance << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Due Date" << std::setfill(' ') << " " << row[5] << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Payment Date" << std::setfill(' ') << " " << row[6] << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Method" << std::setfill(' ') << " " << row[7] << "\n";
    std::cout << std::setfill('.') << std::setw(20) << "Status" << std::setfill(' ') << " " << row[8] << "\n";
    std::cout << std::setfill('=') << std::setw(40) << "" << "\n\n";
}

std::vector<FeeDefaulter> getDefaulters(const std::string& currentDate) {
    std::vector<FeeDefaulter> defaulters;
    std::vector<std::vector<std::string> > rows = readTXT(FEES_FILE);

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() < 9) {
            continue;
        }
        std::istringstream totalIss(rows[i][3]);
        double total = 0.0;
        totalIss >> total;
        std::istringstream paidIss(rows[i][4]);
        double paid = 0.0;
        paidIss >> paid;
        double balance = total - paid;

        if (balance > 0 && daysBetween(rows[i][5], currentDate) > 0) {
            FeeDefaulter d;
            d.roll = rows[i][1];
            d.name = getStudentName(d.roll);
            d.outstanding = balance;
            int lateDays = daysBetween(rows[i][5], currentDate);
            d.weeksOverdue = lateDays / 7;
            defaulters.push_back(d);
        }
    }

    int n = (int)defaulters.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (defaulters[j].outstanding < defaulters[j + 1].outstanding) {
                FeeDefaulter temp = defaulters[j];
                defaulters[j] = defaulters[j + 1];
                defaulters[j + 1] = temp;
            }
        }
    }
    return defaulters;
}
