#ifndef ERROR_H
#define ERROR_H

#include "address.hpp"
#include "text_object.hpp"
#include <memory>

enum class ErrorType { Lexical, Syntactical, Semantic, Undefined };

class Error {
  private:
    std::string errorTypeString();
    std::string locationToString(Location l);
  protected:
    std::string baseMessage(Location l);
  public:
    Error(std::shared_ptr<Token> t, std::string description);
    Error(std::shared_ptr<RawToken> t, std::string description);
    ErrorType type;
    std::shared_ptr<Token> token;
    std::shared_ptr<RawToken> rawToken;
    std::string description;
    virtual std::string message() = 0;
};

class LexicalError : Error {
  public:
    LexicalError(std::shared_ptr<RawToken> t, std::string description);
    std::string message();
};

class SyntacticalError : Error {
  private:
    std::string tokenType;
  public:
    SyntacticalError(std::shared_ptr<Token> t, std::string description);
    SyntacticalError(std::shared_ptr<RawToken> t, std::string description);
    std::string message();
};

class SemanticError : Error {
  public:
    SemanticError(std::shared_ptr<Token> t, std::string description);
    std::string message();
};

#endif
