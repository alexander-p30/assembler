#ifndef TEXT_OBJECT_H
#define TEXT_OBJECT_H

#include <regex>
#include <string>
#include <vector>
#include "parse.hpp"

# define DIRECTIVES std::vector<std::string>{"CONST", "SPACE"}
# define INSTRUCTIONS std::vector<std::string>{"COPY", "ADD"}

class TextObject {
  public:
    Address address;
    std::string text;
};

enum class TokenType { Label, Instruction, Directive, Symbol, Integer };

class Token : public virtual TextObject {
  private:
    TokenType type;
    TokenType resolveType(Parser parser);
    TokenType resolveType(RawToken rawToken);

  public:
    Token(Parser parser);
    Token(RawToken rawToken);
    static bool validate(Token t);
};

class Label : public Token {
  public:
    Label(Token t); 
};

enum class DirectiveType { PreProcessment, Regular };

class Directive : public Token {
  private:
    DirectiveType type;
    DirectiveType resolveType;
  public:
    Directive(Token t);
    static bool validate(Token t);
};

class Instruction : public Token {
  public:
    Instruction(Token t);
    static bool validate(Token t);
};

class Symbol : public Token {
  public:
    Symbol(Token t);
    static bool validate(Token t);
};

class Integer : public Token {
  public:
    Integer(Token t);
    static bool validate(Token t);
};

#endif
