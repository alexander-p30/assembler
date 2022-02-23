#include "../include/inspect.hpp"

void inspect(Parser p, int indent) {
  std::string ind = string(indent * 2, ' ');
  std::vector<RawLine> rawLines = p.getRawLines();
  cout << ind + "Parser{\n";
  cout << ind + "  fileText: " << "\"" << p.getText() << "\"" <<  ",\n"; 
  cout << ind + "  fileRawLines: {\n"; 
  for_each(rawLines.begin(), rawLines.end(),
           [indent](RawLine rawLine) { 
           inspect(rawLine, indent + 2); 
           });
  cout << ind + "  },\n  currentRawLine: \n"; 
  inspect(p.getCurrentRawLine(), indent + 1);
  cout << ind + "}\n"; 
}

void inspect(RawLine rawLine, int indent) {
  std::string ind = string(indent * 2, ' ');
  std::vector<RawToken> rawTokens = rawLine.getRawTokens();
  cout << ind + "RawLine{\n";
  cout << ind + "  text: " << "\"" << rawLine.getText() << "\"" <<  ",\n"; 
  cout << ind + "  rawTokens: {\n"; 
  for_each(rawTokens.begin(), rawTokens.end(),
           [indent](RawToken rawToken) { 
           inspect(rawToken, indent + 2); 
           cout << ",\n";
           });
  cout << ind + "  },\n";
  cout << ind + "  location: "; 
  inspect(rawLine.getLocation(), 0);
  cout << ",\n";
  cout << ind + "}\n"; 
}

void inspect(ProgramLine progLine, int indent) {
  std::string ind = string(indent * 2, ' ');
  std::vector<std::shared_ptr<Token>> tokens = progLine.getTokens();
  cout << ind + "ProgramLine{\n";
  cout << ind + "  tokens: {\n"; 
  for_each(tokens.begin(), tokens.end(),
           [indent](std::shared_ptr<Token> token) { 
           inspect(token, indent + 2); 
           cout << ",\n";
           });
  cout << ind + "  },\n";
  cout << ind + "}\n"; 
}

void inspect(std::shared_ptr<Token> t, int indent) {
  string ind = string(indent * 2, ' ');
  cout << ind + t->_name() << "{";
  cout << "text: " << t->getText() << ", ";
  auto addr = (std::shared_ptr<Addressable>) t;
  if(addr != nullptr) {
    cout << "address: ";
    inspect(addr->getAddress(), 0);
  }
  cout << "}";
}

void inspect(RawToken t, int indent) {
  string ind = string(indent * 2, ' ');
  cout << ind + "RawToken{";
  cout << "text: " << t.text << ", ";
  cout << "location: ";
  inspect(t.location, 0);
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

void inspect(Location l, int indent) {
  string ind = string(indent * 2, ' ');
  cout << ind + "Location{";
  cout << ind + "fileName: " << l.fileName << ", ";
  cout << ind + "lineNumber: " << l.lineNumber << ", ";
  cout << ind + "positionInLine: " << l.positionInLine;
  cout << ind + "}";
}

