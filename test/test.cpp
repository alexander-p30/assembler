#include "../include/parse.hpp"
#include "../include/text_object.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

void inspect(Parser p, int x);
void inspect(Line l, int x);
void inspect(RawToken t, int x);
void inspect(Address a, int x);
void inspect(AddressType t, int x);

void inspect(Parser p, int indent) {
  std::string ind = string(indent * 2, ' ');
  std::vector<Line> lines = p.getLines();
  cout << ind + "Parser{\n";
  cout << ind + "  fileText: " << "\"" << p.getText() << "\"" <<  ",\n"; 
  cout << ind + "  fileLines: {\n"; 
  for_each(lines.begin(), lines.end(),
           [indent](Line line) { 
           inspect(line, indent + 2); 
           });
  cout << ind + "  },\n  currentLine: \n"; 
  inspect(p.getCurrentLine(), indent + 1);
  cout << ind + "}\n"; 
}

void inspect(Line line, int indent) {
  std::string ind = string(indent * 2, ' ');
  std::vector<RawToken> rawTokens = line.getRawTokens();
  cout << ind + "Line{\n";
  cout << ind + "  text: " << "\"" << line.getText() << "\"" <<  ",\n"; 
  cout << ind + "  rawTokens: {\n"; 
  for_each(rawTokens.begin(), rawTokens.end(),
           [indent](RawToken rawToken) { 
           inspect(rawToken, indent + 2); 
           cout << ",\n";
           });
  cout << ind + "  },\n";
  cout << ind + "  address: "; 
  inspect(line.getAddress(), 0);
  cout << ",\n";
  cout << ind + "  nextLineAddress: "; 
  inspect(line.nextLineAddress(), 0);
  cout << ",\n";
  cout << ind + "}\n"; 
}

void inspect(RawToken t, int indent) {
  string ind = string(indent * 2, ' ');
  cout << ind + "RawToken{";
  cout << "text: " << t.text << ", ";
  cout << "address: ";
  inspect(t.address, 0);
  cout << "}";
}

void inspect(Address a, int indent) {
  string ind = string(indent * 2, ' ');
  cout << "Address{";
  cout << "type: ";
  inspect(a.type, 0);
  cout << "number: " << a.number;
  cout << "}";
}

void inspect(AddressType t, int indent) {
  string ind = string(indent * 2, ' ');
  switch (t) {
  case AddressType::Absolute:
    cout << ind + "Absolute" << ", ";
    break;
  case AddressType::Relative:
    cout << ind + "Relative" << ", ";
    break;
  case AddressType::Undefined:
    cout << ind + "Undefined" << ", ";
    break;
  }
}

int main() {
  std::string text = "\tLABEL: COPY A, B\nLABEL2:\nADD B";
  Address addr = Address{AddressType::Absolute, 0};
  Parser p = Parser(text, addr);

  inspect(p, 1);

  return 0;
}
