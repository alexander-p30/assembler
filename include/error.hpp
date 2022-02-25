#ifndef ERROR_H
#define ERROR_H

#include "address.hpp"
#include "text_object.hpp"
#include <memory>

enum class ErrorType { Lexical, Syntactical, Semantic, Undefined };

class Error {
  protected:
    std::string errorTypeString();
    std::string locationToString(Location l);
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

class LexicalError : public Error {
  public:
    LexicalError(std::shared_ptr<RawToken> t, std::string description);
    std::string message();
};

class SyntacticalError : public Error {
  private:
    std::string tokenType;
  public:
    SyntacticalError(std::shared_ptr<Token> t, std::string description);
    SyntacticalError(std::shared_ptr<RawToken> t, std::string description);
    std::string message();
};

class SemanticError : public Error {
  private:
    std::string tokenType;
  public:
    SemanticError(std::shared_ptr<Token> t, std::string description);
    SemanticError(std::shared_ptr<RawToken> t, std::string description);
    std::string message();
};

#endif
