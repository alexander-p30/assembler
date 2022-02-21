#include "../include/parse.hpp"
#include "../include/text_object.hpp"
#include "../include/pre_process.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

void inspect(Parser p, int x);
void inspect(RawLine l, int x);
void inspect(RawToken t, int x);
void inspect(Address a, int x);
void inspect(AddressType t, int x);
void inspect(Location a, int x);

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
  cout << ind + "  address: "; 
  inspect(rawLine.getAddress(), 0);
  cout << ",\n";
  cout << ind + "  location: "; 
  inspect(rawLine.getLocation(), 0);
  cout << ",\n";
  cout << ind + "  nextRawLineAddress: "; 
  inspect(rawLine.nextRawLineAddress(), 0);
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

void inspect(Location l, int indent) {
  string ind = string(indent * 2, ' ');
  cout << ind + "Location{";
  cout << ind + "fileName: " << l.fileName << ", ";
  cout << ind + "lineNumber: " << l.lineNumber;
  cout << ind + "}";
}

int main() {
  /* std::string text = "\tLABEL: COPY A, B\nLABEL2:\nADD B"; */

  /* std::string text = "mul_n: macro \n\ */
/* MULT N\n\ */
/* STORE N\n\ */
/* ENDMACRO\n\ */
/* INPUT N\n\ */
/* ADD X\n\ */
/* SUB A\n\ */
/* LOAD N\n\ */
/* FAT: SUB ONE\n\ */
/* JMPZ FIM\n\ */
/* STORE AUX\n\ */
/* MUL_N \n\ */
/* LOAD AUX\n\ */
/* JMP FAT\n\ */
/* FIM: OUTPUT N\n\ */
/* STOP\n\ */
/* X: EQU 5\n\ */
/* A: EQU -100\n\ */
/* AUX: SPACE\n\ */
/* N: SPACE\n\ */
/* ONE: CONST 1"; */

  std::string text = "mul_n: macro \n\
MULT N\n\
STORE N\n\
ENDMACRO\n\
INPUT N\n\
ADD X\n\
SUB A\n\
MUL_N\n\
FAT: SUB ONE\n\
JMPZ FIM\n\
STORE AUX\n\
IF X\n\
JMP TRUE\n\
IF A\n\
JMP FALSE\n\
FIM: OUTPUT N\n\
STOP\n\
X: EQU 5\n\
A: EQU 0\n\
AUX: SPACE\n\
N: SPACE\n\
ONE: CONST 1";

  Address addr = Address{AddressType::Absolute, 0};
  Parser p = Parser(FileData{"some_file.asm", text}, addr);
  PreProcessor pp = PreProcessor(p.getRawLines());

  int i = 0;
  auto lines = pp.getPreProcessedLines();
  std::for_each(lines.begin(), lines.end(), [&i](RawLine rl) {
    inspect(rl, 1);
  });

  return 0;
}
