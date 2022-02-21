#include "../include/parse.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <vector>
#include <sstream>

bool rawLineHasEmptyLabelDef(RawLine rawLine) {
  return rawLine.getRawTokens().size() == 1 && IS_LABEL_DEF(rawLine.getRawTokens().front().text);
}

std::vector<RawLine> splitRawLines(std::string text, Address baseAddress, Location baseLocation) {
  const std::regex separators("[^\n]+");
  std::vector<RawLine> rawLines = std::vector<RawLine>{};
  std::vector<RawLine> rawLinesWithCorrectedLabelDefs = std::vector<RawLine>{};

  auto begin = std::sregex_iterator(text.begin(), text.end(), separators);
  auto end = std::sregex_iterator();

  Address currentAddress = baseAddress;
  Location currentLocation = baseLocation;

  for (std::sregex_iterator i = begin; i != end; i++) {
    RawLine rawLine = RawLine(i->str(), currentAddress, currentLocation);

    auto nextIterator = std::next(i, 1);
    if (rawLineHasEmptyLabelDef(rawLine) && nextIterator != end) {
      std::string labelAndDefinition = i->str() + " " + nextIterator->str();
      rawLine = RawLine(labelAndDefinition, currentAddress, currentLocation);
      rawLines.push_back(rawLine);
      i++;
    } else {
      rawLines.push_back(rawLine);
    }

    currentAddress = rawLine.nextRawLineAddress();
    currentLocation.lineNumber++;
  }

  return rawLines;
}

/***********************
 *       Parser        *
 ***********************/

Parser::Parser(FileData fData, Address startAddress) {
  Location startLocation = Location{fData.fileName, 1};
  std::string text = fData.fileContent;
  std::for_each(text.begin(), text.end(), [](char &c) { c = ::toupper(c); });
  fileText = text;
  fileRawLines = splitRawLines(text, startAddress, startLocation);
  currentRawLineIndex = 0;
  currentRawToken = fileRawLines[currentRawLineIndex].getCurrentRawToken();
}

/*
 * Returns the rawLine currently being parsed.
 */
RawLine Parser::getCurrentRawLine() { return fileRawLines[currentRawLineIndex]; }

/*
 * Returns the rawLine next rawLine to be parsed.
 */
RawLine Parser::nextRawLine() { return fileRawLines[++currentRawLineIndex]; }

/*
 * Returns the current token in the rawLine being parsed.
 */
RawToken Parser::getCurrentRawToken() { return fileRawLines[currentRawLineIndex].getCurrentRawToken(); }

/*
 * Returns the parser's text.
 */
std::string Parser::getText() { return fileText; }

/*
 * Return the parser's rawLines.
 */
std::vector<RawLine> Parser::getRawLines() { return fileRawLines; }
