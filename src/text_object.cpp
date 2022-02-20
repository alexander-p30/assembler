#include "../include/text_object.hpp"
#include <regex>
#include <string>
#include <tuple>
#include <vector>
#include <algorithm>
#include <map>

#define TOKEN_TYPE_POSITION 0
#define VALIDATOR_POSITION 1

typedef bool (*validatorFunction)(Token t);
typedef std::tuple<TokenType, validatorFunction> validatorFunctionMapping;

TokenType getLineStartTokenType(RawToken rawToken) {
  std::map<TokenType, validatorFunction> validators;
  validators[TokenType::Label] = Label::validate;
  validators[TokenType::Instruction] = Instruction::validate;

  Token t = Token(rawToken);

  auto matchingValidator = std::find_if(
      validators.begin(),
      validators.end(),
      [t](std::pair<TokenType, validatorFunction> validator) { validator.second(t); }
    );

  if(matchingValidator == validators.end()) {
    // throw something
  }

  return matchingValidator->first;
}

TokenType getLineTokenType(RawToken rawToken) {
  const std::vector<validatorFunctionMapping> validatorFunctionMappings =
      std::vector<validatorFunctionMapping>{
        std::make_tuple(TokenType::Directive, Directive::validate),
        std::make_tuple(TokenType::Instruction, Instruction::validate),
        std::make_tuple(TokenType::Label, Label::validate),
        std::make_tuple(TokenType::Integer, Integer::validate),
        std::make_tuple(TokenType::Symbol, Symbol::validate)
      };
  return TokenType::Symbol;
}

TokenType Token::resolveType(Parser parser) {
  RawToken rawToken = parser.getCurrentRawToken();

  return parser.isBegginingOfLine() ? getLineStartTokenType(rawToken)
                                    : getLineTokenType(rawToken);
}

Token::Token(Parser parser) {
  address = parser.address;
  text = parser.text;
  type = resolveType(parser);
}

class Token : public virtual TextObject {
private:
  TokenType type;

  TokenType resolveType(Parser parser) {}

public:
};

class Label : public Token {
private:
  bool validate(Token t) { return t.text.back() == ':'; }

public:
  Label(Token t) : Token(t.text, t.memoryAddress) {
    if (validate(t))
      text = t.text;
  }
};
