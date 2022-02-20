#ifndef PARSE_H
#define PARSE_H

#include <cstdint>
#include <regex>
#include <string>
#include <vector>

#define BLANK_LINE_REGEX "^(?:[\t ]*(?:\r?\n|\r))+"
#define COMMENT_SEPARATORS "[^;]+"
#define RAW_TOKEN_SEPARATORS "[^ ,\t]+"
#define NEXT(address) address.number + 1
#define IS_LABEL_DEF(rawText) rawText.back() == ':'

enum class AddressType { Absolute, Relative, Undefined };

/*
 * An address composed of a number and a type, which
 * can be one of the following: Absoltue, Relative or
 * Undefined.
 */
struct Address {
  AddressType type;
  int32_t number;
};

Address nextAddress(Address a);

/*
 * A raw token as the aggregation of a piece of text
 * and an address, with no behaviour really attached
 * to it.
 */
struct RawToken {
  std::string text;
  Address address;
};

/*
 * Line class consists of a collection of raw tokens
 * and an address.
 */
class Line {
private:
  int32_t currentRawTokenIndex;
  std::string text;
  std::vector<RawToken> rawTokens;
  Address address;

public:
  Line(std::string lineText, Address addr);
  std::vector<RawToken> getRawTokens();
  RawToken getCurrentRawToken();
  RawToken nextRawToken();
  bool isAtLastToken();
  Address nextLineAddress();
  std::string getText();
  Address getAddress();
};

/*
 * Parses class provides a file-traversing functionality
 * through raw string tokens and addresses
 */
class Parser {
private:
  std::string fileText;
  std::vector<Line> fileLines;
  int32_t currentLineIndex;
  RawToken currentRawToken;

public:
  Parser(std::string text, Address startAddress);
  Line getCurrentLine();
  Line nextLine();
  RawToken getCurrentRawToken();
  std::string getText();
  std::vector<Line> getLines();
};

#endif
