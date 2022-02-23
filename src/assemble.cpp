#include "../include/assemble.hpp"
#include "../include/error.hpp"
#include <tuple>
#include <vector>
#include <memory>

bool rawLineHasEmptyLabelDef(RawLine * rawLine) {
  return rawLine->getRawTokens().size() == 1 && IS_LABEL_DEF(rawLine->getRawTokens().front().text);
}

bool isRawTokenSymbolDefinition(RawToken * t) { 
  return IS_LABEL_DEF(t->text); 
}

bool isRawTokenDirective(RawToken * t) {
  const std::vector<std::string> directives{"SPACE", "CONST"};

  return std::count(directives.begin(), directives.end(), t->text);
}

bool isRawTokenInstruction(RawToken * t) {
  const std::vector<std::string> instructions{"ADD", "SUB", "MULT", "DIV", "JMP",
    "JMPN", "JMPP", "JMPZ", "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP"};

  return std::count(instructions.begin(), instructions.end(), t->text);
}

bool isRawTokenValue(RawToken * t) { 
  return std::all_of(t->text.begin(), t->text.end(), ::isdigit); 
}

std::vector<std::shared_ptr<Token>> TwoPassAssembler::specializeRawTokens(std::vector<RawToken> rawTokens, Address * address) {
  std::vector<std::shared_ptr<Token>> tokens;

  for(auto rawT = rawTokens.begin(); rawT != rawTokens.end(); ++rawT) {
    RawToken * rawTPtr = &(*rawT);
    bool shoulUpdateAddress = false;

    if (isRawTokenSymbolDefinition(rawTPtr)) {
      std::shared_ptr<SymbolDefinition> sDef{ new SymbolDefinition(*rawT, *address)};
      symbolDefinitionTable.push_back(sDef);
      tokens.push_back(sDef);
      shoulUpdateAddress = true;
    } else if(isRawTokenInstruction(rawTPtr)) {
      std::shared_ptr<Instruction> instruction{ new Instruction(*rawT, *address)};
      tokens.push_back(instruction);
      shoulUpdateAddress = true;
    } else if(isRawTokenValue(rawTPtr)) {
      std::shared_ptr<Value> value{ new Value(*rawT)};
      tokens.push_back(value);
    } else if(isRawTokenDirective(rawTPtr)) {
      std::shared_ptr<Directive> directive{ new Directive(*rawT)};
      tokens.push_back(directive);
    } else {
      std::shared_ptr<Symbol> symbol{ new Symbol(*rawT, *address)};
      tokens.push_back(symbol);
      shoulUpdateAddress = true;
    }

    if(shoulUpdateAddress)
      address->number++;
  }

  return tokens;
}

ProgramLine::ProgramLine(std::vector<std::shared_ptr<Token>> lineTokens) { tokens = lineTokens; }

std::vector<std::shared_ptr<Token>> ProgramLine::getTokens() { return tokens; }

void ProgramLine::inspect(int indent) {
  std::string ind = std::string(indent * 2, ' ');
  std::cout << ind + "ProgramLine{\n";
  std::cout << ind + "  tokens: {\n";

  for(auto &t : this->getTokens()) {
    t->inspect(indent + 2);
    std::cout << "\n";
  }

  std::cout << ind + "  }\n";
  std::cout << ind + "}\n";
}

TwoPassAssembler::TwoPassAssembler(PreProcessor * p, Address baseAddress) {
  preProcessor = p;
  firstPassProgramLines = std::vector<ProgramLine>{};

  std::vector<RawLine> rawLines = preProcessor->getPreProcessedLines();
  Address currentAddress = baseAddress;

  for (auto rawLine = rawLines.begin(); rawLine != rawLines.end(); ++rawLine) {
    std::vector<RawToken> rawTokens = rawLine->getRawTokens();

    if (rawLineHasEmptyLabelDef(&(*rawLine)) && std::next(rawLine, 1) != rawLines.end()) {
      ++rawLine;
      std::vector<RawToken> nextLineTokens = rawLine->getRawTokens();
      rawTokens.insert(rawTokens.end(), nextLineTokens.begin(), nextLineTokens.end());
    }

    std::vector<std::shared_ptr<Token>> specializedTokens = specializeRawTokens(rawTokens, &baseAddress);
    firstPassProgramLines.push_back(ProgramLine(specializedTokens));
  }
}

std::vector<ProgramLine> TwoPassAssembler::getFirstPassProgramLines() { 
  return firstPassProgramLines; 
}

std::vector<ProgramLine> TwoPassAssembler::getSecondPassProgramLines() { 
  return secondPassProgramLines; 
}
