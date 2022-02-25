#include "../include/error.hpp"
#include <memory>

Error::Error(std::shared_ptr<Token> t, std::string desc) {
  type = ErrorType::Undefined;
  token = t;
  description = desc;
}

Error::Error(std::shared_ptr<RawToken> t, std::string desc) {
  type = ErrorType::Undefined;
  rawToken = t;
  description = desc;
}

std::string Error::locationToString(Location l) {
  return std::string{"Linha " + std::to_string(l.lineNumber) +
                     ", token na posicao " + std::to_string(l.positionInLine)};
}

std::string Error::errorTypeString() {
  switch (type) {
  case ErrorType::Undefined:
    return "desconhecido";
  case ErrorType::Lexical:
    return "lexico";
  case ErrorType::Syntactical:
    return "sintatico";
  case ErrorType::Semantic:
    return "semantico";
  }

  return "";
}

std::string Error::baseMessage(Location l) {
  return std::string{locationToString(l) + " causou erro " + errorTypeString() +
                     ": "};
}

LexicalError::LexicalError(std::shared_ptr<RawToken> t, std::string desc)
    : Error(t, desc) {
  type = ErrorType::Lexical;
}

std::string LexicalError::message() {
  return std::string{baseMessage(rawToken->location) + 
    '"' + rawToken->text + "\" " + description};
}

SyntacticalError::SyntacticalError(std::shared_ptr<Token> t, std::string desc)
    : Error(t, desc) {
  type = ErrorType::Syntactical;
  tokenType = "Token";
}

SyntacticalError::SyntacticalError(std::shared_ptr<RawToken> t,
                                   std::string desc)
    : Error(t, desc) {
  type = ErrorType::Syntactical;
  tokenType = "RawToken";
}

std::string SyntacticalError::message() {
  std::string txt;
  Location l;

  if(tokenType == "RawToken") {
    txt = rawToken->text;
    l = rawToken->location;
  } else if(tokenType == "Token") {
    txt = token->getText();
    l = token->getRawToken().location;
  }

  return std::string{baseMessage(l) + '"' + txt + "\" " + description};
}

SemanticError::SemanticError(std::shared_ptr<Token> t, std::string desc)
    : Error(t, desc) {
  type = ErrorType::Semantic;
}

std::string SemanticError::message() {
  return std::string{baseMessage(token->getRawToken().location) + 
    '"' + token->getText() + "\" " + description};
}
