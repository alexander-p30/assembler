#include "../include/assemble.hpp"
#include "../include/error.hpp"
#include <tuple>
#include <vector>

bool rawLineHasEmptyLabelDef(RawLine rawLine) {
  return rawLine.getRawTokens().size() == 1 && IS_LABEL_DEF(rawLine.getRawTokens().front().text);
}

bool isRawTokenInstruction(RawToken t) {
  const std::vector<std::string> instructions{"ADD", "SUB", "MULT", "DIV", "JMP",
    "JMPN", "JMPP", "JMPZ", "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP"};
  auto tokenTextInInstructions = std::find(instructions.begin(), instructions.end(), t.text);
  return tokenTextInInstructions != instructions.end();
}

std::vector<Token> specializeRawTokens(std::vector<RawToken> rawTokens) {
  std::vector<Token> tokens;

  for(auto rawT = rawTokens.begin(); rawT != rawTokens.end(); ++rawT) {
    if (isRawTokenLabel(*rawT)) {
      
    } else if(isRawTokenInstruction(*rawT)) {
      tokens.push_back(Instruction(rawT));
    } else if(isRawTokenMacro(rawT)) {
      // collapse macro
    } else if(isRawTokenDirective
  }

  switch (rawTokens.size()) {
  case 1:
    RawToken rawT = rawTokens.front();
    Token t = Token(rawT);

    if(isRawTokenInstruction(rawT)) {
      tokens.push_back(Instruction(t));
    } else {
      tokens.push_back(t);
    }

    break;
  case 2:
  case 3:
  case 4:
  }
}

ProgramLine::ProgramLine(std::vector<Token> lineTokens) { tokens = lineTokens; }

TwoPassAssembler::TwoPassAssembler(Parser p) {
  parser = p;
  std::vector<RawLine> rawLines = parser.getRawLines();
  programLines = std::vector<ProgramLine>{};

  for (auto rawLine = rawLines.begin(); rawLine != rawLines.end(); ++rawLine) {
    std::vector<RawToken> rawTokens = rawLine->getRawTokens();

    if (rawLineHasEmptyLabelDef(*rawLine) && std::next(rawLine, 1) != rawLines.end()) {
      ++rawLine;
      std::vector<RawToken> nextLineTokens = rawLine->getRawTokens();
      rawTokens.insert(rawTokens.end(), nextLineTokens.begin(), nextLineTokens.end());
    }

    specializeRawTokens(rawTokens);
  }
}
