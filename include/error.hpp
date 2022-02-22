#ifndef ERROR_H
#define ERROR_H

#include "address.hpp"
#include "text_object.hpp"
#include <memory>

enum class ErrorType { Lexical, Syntactical, Semantic };

class Error {
  public:
    ErrorType type;
    std::shared_ptr<Token> token;
};

class LexicalError : Error {
  public:
    LexicalError(std::shared_ptr<Token> t);
};

class SyntacticalError : Error {
  public:
    SyntacticalError(std::shared_ptr<Token> t);
};

class SemanticError : Error {
  public:
    SemanticError(std::shared_ptr<Token> t);
};

#endif
