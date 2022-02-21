#include "../include/text_object.hpp"
#include <regex>
#include <string>
#include <tuple>
#include <vector>
#include <algorithm>
#include <map>

std::string purgeComments(std::string text) {
  const std::regex separator(COMMENT_SEPARATORS);
  return std::sregex_iterator(text.begin(), text.end(), separator)->str();
};

std::vector<RawToken> splitRawTokens(std::string text, Address baseAddress, Location lineLocation) {
  const std::regex separators(RAW_TOKEN_SEPARATORS);
  std::vector<RawToken> rawTokens = std::vector<RawToken>{};

  text = purgeComments(text);
  auto begin = std::sregex_iterator(text.begin(), text.end(), separators);
  auto end = std::sregex_iterator();

  for (std::sregex_iterator i = begin; i != end; i++) {
    baseAddress.number++;
    RawToken rawToken = RawToken{i->str(), baseAddress, lineLocation};
    rawTokens.push_back(rawToken);
  }

  return rawTokens;
}

/*************************
 *        RawLine        *
 *************************/

RawLine::RawLine(std::string rawLineText, Address addr, Location loc) {
  text = rawLineText;
  address = addr;
  location = loc;
  currentRawTokenIndex = 0;
  rawTokens = splitRawTokens(rawLineText, addr, loc);
}

RawLine::RawLine(const RawLine& rLine) {
  text = rLine.text;
  address = rLine.address;
  location = rLine.location;
  currentRawTokenIndex = rLine.currentRawTokenIndex;
  rawTokens = rLine.rawTokens;
}

RawLine::RawLine() { }

/*
 * Return raw tokens parsed from field `text`.
 */
std::vector<RawToken> RawLine::getRawTokens() { return rawTokens; }

/*
 * Returns the current raw token.
 */
RawToken RawLine::getCurrentRawToken() { return rawTokens[currentRawTokenIndex]; };

/*
 * Updates current token to the next one.
 */
RawToken RawLine::nextRawToken() {
  int nextTokenIndex =
      isAtLastToken() ? currentRawTokenIndex : ++currentRawTokenIndex;

  return rawTokens[nextTokenIndex];
};

/*
 * Returns if the current token is the rawLine's last.
 */
bool RawLine::isAtLastToken() {
  return (currentRawTokenIndex + 1) == rawTokens.size();
};

/*
 * Return the address where the next rawLine should be.
 */
Address RawLine::nextRawLineAddress() {
  return nextAddress(rawTokens.back().address);
}

/*
 * Return the rawLine's raw unparsed text.
 */
std::string RawLine::getText() { return text; }

/*
 * Return the rawLine's address.
 */
Address RawLine::getAddress() { return address; }

/*
 * Return the rawLine's location.
 */
Location RawLine::getLocation() { return location; }

/***********************
 *        Token        *
 ***********************/

Token::Token(RawToken rToken) {
  rawToken = rToken;
  std::string treatedText = rToken.text;

  if(IS_LABEL_DEF(rToken.text))
    treatedText.pop_back();

  text = treatedText;
}

RawToken Token::getRawToken() { return rawToken; }

/****************************
 *        Instruction       *
 ****************************/

Instruction::Instruction(Token t) : Token::Token(t.getRawToken()) {};

/**********************
 *        Label       *
 **********************/

Label::Label(Token t, Address def) : Token::Token(t.getRawToken()) {
  definition = def;
};

/***********************
 *       Symbol        *
 ***********************/


Symbol::Symbol(Token t) : Token::Token(t.getRawToken()) {
  definition = Address{AddressType::Undefined, 0};
}

Symbol::Symbol(Token t, Address def) : Token::Token(t.getRawToken()) {
  definition = def;
}

Address Symbol::setDefinition(Address addr) {
  definition = addr;
  return definition;
}

bool Symbol::isDefined() { return definition.type != AddressType::Undefined; }

/*********************
 *       Value       *
 *********************/

Value::Value(Token t) : Token::Token(t.getRawToken()) {};
