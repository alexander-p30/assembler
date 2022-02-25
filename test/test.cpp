#include "../include/assemble.hpp"
#include "../include/inspect.hpp"
#include "../include/error.hpp"
#include "../include/parse.hpp"
#include "../include/pre_process.hpp"
#include "../include/text_object.hpp"
#include <fstream>
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
std::ifstream t("code1.asm");
std::stringstream buffer;
buffer << t.rdbuf();

std::string text = buffer.str();

std::cout << text << std::endl; 

  Address addr = Address{AddressType::Absolute, 0};
  Parser p = Parser(FileData{"some_file.asm", text});
  PreProcessor pp = PreProcessor(p.getRawLines());
  TwoPassAssembler asmer = TwoPassAssembler(&pp, addr);

  int i = 0;
  auto lines = pp.getPreProcessedLines();
  std::for_each(lines.begin(), lines.end(), [&i](RawLine pl) {
      inspect(pl, 0);
  });

  /* std::cout << std::endl; */

  /* SemanticError err = SemanticError(lines.front().getTokens().front(), std::string{"nao definido."}); */
  /* auto rtoken = std::make_shared<RawToken>(pp.getPreProcessedLines().back().getRawTokens().front()); */
  /* LexicalError err2 = LexicalError(rtoken, std::string{"token invalido."}); */
  /* rtoken = std::make_shared<RawToken>(pp.getPreProcessedLines()[7].getRawTokens().front()); */
  /* SyntacticalError err3 = SyntacticalError(rtoken, "MACRO sem ENDMACRO."); */
  /* SyntacticalError err4 = SyntacticalError(lines[3].getTokens().front(), "label dupla na linha"); */
  /* std::cout << err.message() << std::endl; */
  /* std::cout << err2.message() << std::endl; */
  /* std::cout << err3.message() << std::endl; */
  /* std::cout << err4.message() << std::endl; */

  /* std::cout << std::endl; */

  /* auto _asmLines = asmer.getAsm(); */
  /* std::for_each(_asmLines.begin(), _asmLines.end(), [&i](std::vector<int32_t> _asmLine) { */
  /*   std::for_each(_asmLine.begin(), _asmLine.end(), [&i](int32_t _asm) { */
  /*     std::cout << _asm << " "; */
  /*   }); */
  /* }); */

  return 0;
}
