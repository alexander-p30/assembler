#include "../include/parse.hpp"
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <vector>
#include <sstream>

Address nextAddress(Address a) { return Address{a.type, NEXT(a)}; }

bool lineHasEmptyLabelDef(Line line) {
  return line.getRawTokens().size() == 1 && IS_LABEL_DEF(line.getRawTokens().front().text);
}

std::vector<Line> splitLines(std::string text, Address baseAddress) {
  const std::regex separators("[^\n]+");
  std::vector<Line> lines = std::vector<Line>{};
  std::vector<Line> linesWithCorrectedLabelDefs = std::vector<Line>{};

  auto begin = std::sregex_iterator(text.begin(), text.end(), separators);
  auto end = std::sregex_iterator();

  Address currentAddress = baseAddress;

  for (std::sregex_iterator i = begin; i != end; i++) {
    Line line = Line(i->str(), currentAddress);

    auto nextIterator = std::next(i, 1);
    if (lineHasEmptyLabelDef(line) && nextIterator != end) {
      std::string labelAndDefinition = i->str() + " " + nextIterator->str();
      line = Line(labelAndDefinition, currentAddress);
      lines.push_back(line);
      i++;
    } else {
      lines.push_back(line);
    }

    currentAddress = line.nextLineAddress();
  }

  return lines;
}
  
std::string purgeComments(std::string text) {
  const std::regex separator(COMMENT_SEPARATORS);
  return std::sregex_iterator(text.begin(), text.end(), separator)->str();
};

std::vector<RawToken> splitRawTokens(std::string text, Address baseAddress) {
  const std::regex separators(RAW_TOKEN_SEPARATORS);
  std::vector<RawToken> rawTokens = std::vector<RawToken>{};

  text = purgeComments(text);
  auto begin = std::sregex_iterator(text.begin(), text.end(), separators);
  auto end = std::sregex_iterator();

  for (std::sregex_iterator i = begin; i != end; i++) {
    baseAddress.number++;
    RawToken rawToken = RawToken{i->str(), baseAddress};
    rawTokens.push_back(rawToken);
  }

  return rawTokens;
}

/**********************
 *        Line        *
 **********************/

Line::Line(std::string lineText, Address addr) {
  text = lineText;
  address = addr;
  currentRawTokenIndex = 0;
  rawTokens = splitRawTokens(lineText, addr);
}

/*
 * Return raw tokens parsed from field `text`.
 */
std::vector<RawToken> Line::getRawTokens() { return rawTokens; }

/*
 * Returns the current raw token.
 */
RawToken Line::getCurrentRawToken() { return rawTokens[currentRawTokenIndex]; };

/*
 * Updates current token to the next one.
 */
RawToken Line::nextRawToken() {
  int nextTokenIndex =
      isAtLastToken() ? currentRawTokenIndex : ++currentRawTokenIndex;

  return rawTokens[nextTokenIndex];
};

/*
 * Returns if the current token is the line's last.
 */
bool Line::isAtLastToken() {
  return (currentRawTokenIndex + 1) == rawTokens.size();
};

/*
 * Return the address where the next line should be.
 */
Address Line::nextLineAddress() {
  return nextAddress(rawTokens.back().address);
}

/*
 * Return the line's raw unparsed text.
 */
std::string Line::getText() { return text; }

/*
 * Return the line's address.
 */
Address Line::getAddress() { return address; }

/***********************
 *       Parser        *
 ***********************/

Parser::Parser(std::string text, Address startAddress) {
  fileText = text;
  fileLines = splitLines(text, startAddress);
  currentLineIndex = 0;
  currentRawToken = fileLines[currentLineIndex].getCurrentRawToken();
}

/*
 * Returns the line currently being parsed.
 */
Line Parser::getCurrentLine() { return fileLines[currentLineIndex]; }

/*
 * Returns the line next line to be parsed.
 */
Line Parser::nextLine() { return fileLines[++currentLineIndex]; }

/*
 * Returns the current token in the line being parsed.
 */
RawToken Parser::getCurrentRawToken() { return fileLines[currentLineIndex].getCurrentRawToken(); }

std::string Parser::getText() { return fileText; }
std::vector<Line> Parser::getLines() { return fileLines; }
