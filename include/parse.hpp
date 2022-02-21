#ifndef PARSE_H
#define PARSE_H

#include <cstdint>
#include <regex>
#include <string>
#include <vector>
#include "text_object.hpp"
#include "address.hpp"

struct FileData {
  std::string fileName;
  std::string fileContent;
};

/*
 * Parses class provides a file-traversing functionality
 * through raw string tokens and addresses
 */
class Parser {
private:
  std::string fileText;
  std::vector<RawLine> fileRawLines;
  int32_t currentRawLineIndex;
  RawToken currentRawToken;

public:
  Parser(FileData fData, Address startAddress);
  RawLine getCurrentRawLine();
  RawLine nextRawLine();
  RawToken getCurrentRawToken();
  std::string getText();
  std::vector<RawLine> getRawLines();
};

#endif
