#include "../include/text_object.hpp"
#include <regex>
#include <string>
#include <tuple>
#include <vector>
#include <algorithm>
#include <map>

void inspect_addr(AddressType t, int indent) {
  std::string ind = std::string(indent * 2, ' ');
  switch (t) {
  case AddressType::Absolute:
    std::cout << ind + "Absolute" << ", ";
    break;
  case AddressType::Relative:
    std::cout << ind + "Relative" << ", ";
    break;
  case AddressType::Undefined:
    std::cout << ind + "Undefined" << ", ";
    break;
  }
}

void inspect_addr(Address a, int indent) {
  std::string ind = std::string(indent * 2, ' ');
  std::cout << "Address{";
  std::cout << "type: ";
  inspect_addr(a.type, 0);
  std::cout << "number: " << a.number;
  std::cout << "}";
}

std::string purgeComments(std::string text) {
  const std::regex separator(COMMENT_SEPARATORS);
  return std::sregex_iterator(text.begin(), text.end(), separator)->str();
};

std::vector<RawToken> splitRawTokens(std::string text, Location location) {
  const std::regex separators(RAW_TOKEN_SEPARATORS);
  std::vector<RawToken> rawTokens = std::vector<RawToken>{};

  text = purgeComments(text);
  auto begin = std::sregex_iterator(text.begin(), text.end(), separators);
  auto end = std::sregex_iterator();

  for (std::sregex_iterator i = begin; i != end; i++) {
    RawToken rawToken = RawToken{i->str(), location};
    rawTokens.push_back(rawToken);
    location.positionInLine++;
  }

  return rawTokens;
}

/*************************
 *        RawLine        *
 *************************/

RawLine::RawLine(std::string rawLineText, Location loc) {
  text = rawLineText;
  location = loc;
  currentRawTokenIndex = 0;
  rawTokens = splitRawTokens(rawLineText, loc);
}

RawLine::RawLine(const RawLine& rLine) {
  text = rLine.text;
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
 * Return updates the RawLine's raw token vector with the parameter.
 */
std::vector<RawToken> RawLine::setRawTokens(std::vector<RawToken> newTokens) {
  rawTokens = newTokens;
  return newTokens; 
}

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
 * Return the rawLine's raw unparsed text.
 */
std::string RawLine::getText() { return text; }

/*
 * Return the rawLine's location.
 */
Location RawLine::getLocation() { return location; }

/*****************************
 *        Addressable        *
 *****************************/

Addressable::Addressable(Address addr) { address = addr; }

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

/****************************
 *        Instruction       *
 ****************************/

Instruction::Instruction(RawToken t, Address addr) : Token(t), Addressable(addr) {
  defineInstructionAddress();
};

void Instruction::defineInstructionAddress() {
  const std::vector<std::string> instructions{"ADD", "SUB", "MULT", "DIV", "JMP",
    "JMPN", "JMPP", "JMPZ", "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP"};

  int32_t index = std::find(instructions.begin(), instructions.end(), 
      this->text) - instructions.begin();

  address.type = AddressType::Absolute;
  address.number = index + 1;
}

void Instruction::inspect(int indent) {  
  std::string ind = std::string(indent * 2, ' ');
  std::cout << ind;
  std::cout << this->_name() << "{";
  std::cout << "text: " << this->getText() << ", ";
  std::cout << "address: ";
  inspect_addr(this->getAddress(), 0);
  std::cout << "}";
}

/*********************************
 *        SymbolDefinition       *
 *********************************/

SymbolDefinition::SymbolDefinition(RawToken t, Address addr) : Token(t), Addressable(addr) {
  address.type = AddressType::Relative;
};

std::string SymbolDefinition::getLabel() { return text; }

Address SymbolDefinition::getDefinition() { return address; }

void SymbolDefinition::inspect(int indent) {
  std::string ind = std::string(indent * 2, ' ');
  std::cout << ind;
  std::cout << this->_name() << "{";
  std::cout << "text: " << this->getText() << ", ";
  std::cout << "address: ";
  inspect_addr(this->getAddress(), 0);
  std::cout << "}";

}

/***********************
 *       Symbol        *
 ***********************/


Symbol::Symbol(RawToken t, Address addr) : Token(t), Addressable(addr) {
  address.type = AddressType::Relative;
  definition = Address{AddressType::Undefined, 0};
}

Address Symbol::setDefinition(Address addr) {
  address = addr;
  return address;
}

bool Symbol::isDefined() { return address.type != AddressType::Undefined; }

void Symbol::inspect(int indent) {  
  std::string ind = std::string(indent * 2, ' ');
  std::cout << ind;
  std::cout << this->_name() << "{";
  std::cout << "text: " << this->getText() << ", ";
  std::cout << "address: ";
  inspect_addr(this->getAddress(), 0);
  std::cout << "}";
}

/*************************
 *       Directive       *
 *************************/

Directive::Directive(RawToken t) : Token(t) { };

void Directive::inspect(int indent) {  
  std::string ind = std::string(indent * 2, ' ');
  std::cout << ind;
  std::cout << this->_name() << "{";
  std::cout << "text: " << this->getText();
  std::cout << "}";
}

/*********************
 *       Value       *
 *********************/

Value::Value(RawToken t) : Token(t) { };

void Value::inspect(int indent) {  
  std::string ind = std::string(indent * 2, ' ');
  std::cout << ind;
  std::cout << this->_name() << "{";
  std::cout << "text: " << this->getText();
  std::cout << "}";
}
