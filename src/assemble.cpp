#include "../include/assemble.hpp"
#include "../include/error.hpp"
#include <tuple>
#include <vector>

bool rawLineHasEmptyLabelDef(RawLine * rawLine) {
  return rawLine->getRawTokens().size() == 1 && IS_LABEL_DEF(rawLine->getRawTokens().front().text);
}

bool isRawTokenLabel(RawToken * t) { return t->text.back() == ':'; }

bool isRawTokenInstruction(RawToken * t) {
  const std::vector<std::string> instructions{"ADD", "SUB", "MULT", "DIV", "JMP",
    "JMPN", "JMPP", "JMPZ", "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP"};

  return std::count(instructions.begin(), instructions.end(), t->text);
}

bool isRawTokenValue(RawToken * t) { 
  return std::all_of(t->text.begin(), t->text.end(), ::isdigit); 
}

std::vector<Token> specializeRawTokens(std::vector<RawToken> rawTokens, Address baseAddress) {
  std::vector<Token> tokens;
  Address currentAddress = baseAddress;

  for(auto rawT = rawTokens.begin(); rawT != rawTokens.end(); ++rawT) {
    RawToken * rawTPtr = &(*rawT);

    if (isRawTokenLabel(rawTPtr)) {
      tokens.push_back(Label(*rawT, currentAddress));
    } else if(isRawTokenInstruction(rawTPtr)) {
      tokens.push_back(Instruction(*rawT));
    } else if(isRawTokenValue(rawTPtr)) {
      tokens.push_back(Value(*rawT));
    } else {
      tokens.push_back(Symbol(*rawT));
    }
  }

  return tokens;
}

ProgramLine::ProgramLine(std::vector<Token> lineTokens) { tokens = lineTokens; }

TwoPassAssembler::TwoPassAssembler(PreProcessor * p, Address baseAddress) {
  preProcessor = p;
  std::vector<RawLine> rawLines = preProcessor->getPreProcessedLines();
  programLines = std::vector<ProgramLine>{};

  for (auto rawLine = rawLines.begin(); rawLine != rawLines.end(); ++rawLine) {
    std::vector<RawToken> rawTokens = rawLine->getRawTokens();

    if (rawLineHasEmptyLabelDef(&(*rawLine)) && std::next(rawLine, 1) != rawLines.end()) {
      ++rawLine;
      std::vector<RawToken> nextLineTokens = rawLine->getRawTokens();
      rawTokens.insert(rawTokens.end(), nextLineTokens.begin(), nextLineTokens.end());
    }

    specializeRawTokens(rawTokens, baseAddress);
  }
}
