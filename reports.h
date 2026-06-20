#ifndef REPORTS_H
#define REPORTS_H

#include <string>

void printMeritList();
void printAttendanceDefaulters();
void printFeeDefaulters(const std::string& currentDate);
void printSemesterResult(const std::string& semester);
void printDepartmentSummary();
void exportReportToFile(int reportChoice, const std::string& filename,
                        const std::string& currentDate, const std::string& semester);

#endif
