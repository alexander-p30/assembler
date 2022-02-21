#ifndef TEXT_OBJECT_H
#define TEXT_OBJECT_H

#include <regex>
#include <string>
#include <vector>
#include "address.hpp"

#define BLANK_LINE_REGEX "^(?:[\t ]*(?:\r?\n|\r))+"
#define COMMENT_SEPARATORS "[^;]+"
#define RAW_TOKEN_SEPARATORS "[^ ,\t]+"
#define IS_LABEL_DEF(rawText) rawText.back() == ':'

/*
 * A raw token as the aggregation of a piece of text
 * and an address, with no behaviour really attached
 * to it.
 */
struct RawToken {
  std::string text;
  Address address;
  Location location;
};

/*
 * RawLine class consists of a collection of raw tokens
 * and an address.
 */
class RawLine {
private:
  int32_t currentRawTokenIndex;
  std::string text;
  std::vector<RawToken> rawTokens;
  Address address;
  Location location;

public:
  RawLine(std::string rawLineText, Address addr, Location loc);
  RawLine(const RawLine& rLine);
  RawLine();
  std::vector<RawToken> getRawTokens();
  RawToken getCurrentRawToken();
  RawToken nextRawToken();
  bool isAtLastToken();
  Address nextRawLineAddress();
  std::string getText();
  Address getAddress();
  Location getLocation();
};

enum class TokenType { Instruction, Directive, Symbol, Value };

class Token {
  protected:
    Address address;
    std::string text;
    RawToken rawToken;

  public:
    Token(RawToken rawToken);
    RawToken getRawToken();
};

class Instruction : public Token {
  public:
    Instruction(Token t);
};

class Label : public Token {
  private:
    Address definition;
  public:
    Label(Token t, Address def);
};

class Symbol : public Token {
  private:
    Address definition;
  public:
    Symbol(Token t);
    Symbol(Token t, Address def);
    Address setDefinition(Address addr);
    bool isDefined();
};

class Value : public Token {
  public:
    Value(Token t);
};

#endif
