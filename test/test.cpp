#include "../include/assemble.hpp"
#include "../include/parse.hpp"
#include "../include/text_object.hpp"
#include "../include/pre_process.hpp"
#include "../include/inspect.hpp"
#include <algorithm>
#include <iostream>
#include <memory>

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
  Parser p = Parser(FileData{"some_file.asm", text});
  PreProcessor pp = PreProcessor(p.getRawLines());
  TwoPassAssembler asmer = TwoPassAssembler(&pp, addr);

  /* std::vector<std::shared_ptr<Token>> t{}; */
  /* t.push_back(); */

  int i = 0;
  auto lines = asmer.getFirstPassProgramLines();
  std::for_each(lines.begin(), lines.end(), [&i](ProgramLine pl) {
    inspect(pl, 1);
  });

  return 0;
}
