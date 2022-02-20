#include <string>
#include <iostream>

class Token {
  public:
    std::string text;
    static bool validate(std::string t);
};

class Parser {
  public:
    Token (*constructor)(std::string);

    Parser(Token (*construc)(std::string)) {
      constructor = construc;
    }
};

class Label : public Token {
  public:
    Label(std::string t) {
      text = t;
    }

    static bool validate(std::string t) {
      return t.back() == ':';
    }
};

int main() {
  Parser p = Parser(Label.getDeclaredConstructor())

  return 0;
}
