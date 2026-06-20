#ifndef FEE_TRACKER_H
#define FEE_TRACKER_H

#include <string>
#include <vector>

struct FeeDefaulter {
    std::string roll;
    std::string name;
    double outstanding;
    int weeksOverdue;
};

bool isValidDate(const std::string& date);
int daysBetween(const std::string& date1, const std::string& date2);
bool recordPayment(const std::string& feeId, double amount, const std::string& paidDate,
                   const std::string& method);
double computeLateFine(const std::string& feeId);
void generateReceipt(const std::string& feeId);
std::vector<FeeDefaulter> getDefaulters(const std::string& currentDate);

#endif
