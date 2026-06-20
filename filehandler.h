#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <vector>

std::vector<std::vector<std::string> > readTXT(const std::string& filename);
bool writeTXT(const std::string& filename,
              const std::vector<std::string>& header,
              const std::vector<std::vector<std::string> >& rows);
bool appendTXT(const std::string& filename, const std::vector<std::string>& row);
std::vector<std::string> findRow(const std::string& filename,
                                 int colIndex,
                                 const std::string& value);
bool rowExists(const std::string& filename, int colIndex, const std::string& value);

#endif
