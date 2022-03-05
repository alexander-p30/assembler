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
  return std::string{"LINHA " + std::to_string(l.lineNumber) +
                     ", token na posicao " + std::to_string(l.positionInLine)};
}

std::string Error::errorTypeString() {
  switch (type) {
  case ErrorType::Undefined:
    return "DESCONHECIDO";
  case ErrorType::Lexical:
    return "LEXICO";
  case ErrorType::Syntactical:
    return "SINTATICO";
  case ErrorType::Semantic:
    return "SEMANTICO";
  }

  return "";
}

std::string Error::baseMessage(Location l) {
  return std::string{locationToString(l) + " causou erro " + errorTypeString() +
                     ": "};
}

LexicalError::LexicalError(std::shared_ptr<Token> t, std::string desc)
    : Error(t, desc) {
  type = ErrorType::Lexical;
}

Location Error::getLocation() {
  if(rawToken != nullptr) {
    return rawToken->location;
  }

  return token->getRawToken().location;
}

std::string LexicalError::message() {
  return std::string{baseMessage(token->getRawToken().location) + 
    '"' + token->getText() + "\" " + description};
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
  tokenType = "Token";
}

SemanticError::SemanticError(std::shared_ptr<RawToken> t,
                                   std::string desc)
    : Error(t, desc) {
  type = ErrorType::Semantic;
  tokenType = "RawToken";
}

std::string SemanticError::message() {
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
