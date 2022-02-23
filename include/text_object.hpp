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
 * and a location, with no behaviour really attached
 * to it.
 */
struct RawToken {
  std::string text;
  Location location;
};

/*
 * RawLine class consists of a collection of raw tokens.
 */
class RawLine {
private:
  int32_t currentRawTokenIndex;
  std::string text;
  std::vector<RawToken> rawTokens;
  Location location;

public:
  RawLine(std::string rawLineText, Location loc);
  RawLine(const RawLine& rLine);
  RawLine();
  std::vector<RawToken> getRawTokens();
  std::vector<RawToken> setRawTokens(std::vector<RawToken> newTokens);
  RawToken getCurrentRawToken();
  RawToken nextRawToken();
  bool isAtLastToken();
  std::string getText();
  Location getLocation();
};

enum class TokenType { Instruction, Directive, Symbol, Value };

class Addressable {
  protected:
    Address address;
  public:
    Addressable(Address addr);
    virtual Address getAddress() { return address; }
};

class Token {
  protected:
    std::string text;
    RawToken rawToken;

  public:
    Token(RawToken t);
    virtual RawToken getRawToken() { return rawToken; };
    virtual std::string getText() { return text; };
    virtual std::string _name() = 0;
};

class Instruction : public Token, Addressable {
  public:
    Instruction(RawToken t, Address addr);
    std::string _name() { return "Instruction"; }
};

class SymbolDefinition : public Token, Addressable {
  private:
    Address definition;
  public:
    SymbolDefinition(RawToken t, Address addr);
    std::string getLabel();
    Address getDefinition();
    std::string _name() { return "SymbolDefinition"; }
};

class Symbol : public Token, Addressable {
  private:
    Address definition;
  public:
    Symbol(RawToken t, Address addr);
    Address setDefinition(Address addr);
    bool isDefined();
    std::string _name() { return "Symbol"; }
};

class Directive : public Token {
  public:
    Directive(RawToken t);
    std::string _name() { return "Directive"; }
};

class Value : public Token {
  public:
    Value(RawToken t);
    std::string _name() { return "Value"; }
};

#endif
