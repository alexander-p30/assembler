#ifndef ERROR_H
#define ERROR_H

#include "address.hpp"
#include "text_object.hpp"

enum class ErrorType { Lexical, Syntactical, Semantic };

class Error {
  public:
    ErrorType type;
    Token token;
};

class LexicalError : Error {
  public:
    LexicalError(Token t);
};

class SyntacticalError : Error {
  public:
    SyntacticalError(Token t);
};

class SemanticError : Error {
  public:
    SemanticError(Token t);
};

#endif
