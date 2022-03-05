#include "../include/assemble.hpp"
#include "../include/error.hpp"
#include <tuple>
#include <vector>
#include <memory>
#include <sstream>

#define VALUE_FORMAT_REGEX "^-?[0-9]*"

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
  return std::regex_match(t->text, std::regex(VALUE_FORMAT_REGEX)); 
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
      shoulUpdateAddress = true;
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

void TwoPassAssembler::firstPass(Address currentAddress) {
  std::vector<RawLine> rawLines = preProcessor->getPreProcessedLines();

  for (auto rawLine = rawLines.begin(); rawLine != rawLines.end(); ++rawLine) {
    std::vector<RawToken> rawTokens = rawLine->getRawTokens();

    if (rawLineHasEmptyLabelDef(&(*rawLine)) && std::next(rawLine, 1) != rawLines.end()) {
      ++rawLine;
      std::vector<RawToken> nextLineTokens = rawLine->getRawTokens();
      rawTokens.insert(rawTokens.end(), nextLineTokens.begin(), nextLineTokens.end());
    }

    std::vector<std::shared_ptr<Token>> specializedTokens = specializeRawTokens(rawTokens, &currentAddress);
    firstPassProgramLines.push_back(ProgramLine(specializedTokens));
  }
}

std::vector<std::shared_ptr<SymbolDefinition>>::iterator TwoPassAssembler::findSymbolDefinition(
    std::string label
    ) {
  return std::find_if(symbolDefinitionTable.begin(), symbolDefinitionTable.end(), 
      [label](std::shared_ptr<SymbolDefinition> sDef) {
      return sDef->getText() == label;
      });
}

void TwoPassAssembler::secondPass(Address currentAddress) {
  for(auto programLine = firstPassProgramLines.begin(); programLine != firstPassProgramLines.end(); ++programLine) {
    /* std::cout << "\nFIRST THING\n"; */
    std::vector<std::shared_ptr<Token>> tokens = programLine->getTokens();
    std::vector<std::shared_ptr<Token>> newTokens{};

    for(auto tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt) {
      std::shared_ptr<Token> token = (*tokenIt);
      std::string tokenType = token->_name();

      if(tokenType == "Symbol") {
        std::shared_ptr<Symbol> symbol = std::dynamic_pointer_cast<Symbol>(token);
        auto symbolDefinition = findSymbolDefinition(symbol->getText());

        if(symbolDefinition != symbolDefinitionTable.end())
          symbol->setDefinition((*symbolDefinition)->getDefinition());

        auto symbolDup = std::make_shared<Symbol>(symbol->getRawToken(), symbol->getAddress());
        newTokens.push_back(symbol);
      } else {
        newTokens.push_back(token);
      }
    }

    ProgramLine newProgramLine = ProgramLine(newTokens);
    secondPassProgramLines.push_back(newProgramLine);
    assembleLine(newProgramLine);
  }
}

void _assemble_error(std::vector<int32_t> * x) { x->push_back(0); };

void TwoPassAssembler::assembleLine(ProgramLine programLine) {
  std::vector<std::shared_ptr<Token>> tokens = programLine.getTokens();
  std::vector<int32_t> lineAsm{};

  for(auto tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt) {
    std::shared_ptr<Token> token = (*tokenIt);
    std::string tokenType = token->_name();

    if(tokenType == "SymbolDefinition") {
      auto nextTokenIt = std::next(tokenIt, 1);
      if(nextTokenIt != tokens.end() && (*nextTokenIt)->_name() == "Directive") {
        std::string tokenDirective = (*nextTokenIt)->getText();

        if(tokenDirective == "CONST") {
          auto tokenValue = std::next(nextTokenIt, 1);

          if(tokenValue != tokens.end() && (*tokenValue)->_name() == "Value") {
            lineAsm.push_back(std::stoi((*tokenValue)->getText()));
          } else if(tokenValue != tokens.end()) {
            _assemble_error(&lineAsm);
          }
          tokenIt = tokenValue;
        } else if(tokenDirective == "SPACE") {
          lineAsm.push_back(0);
          tokenIt = nextTokenIt;
        }
      }
    } else if(tokenType == "Symbol") {
      std::shared_ptr<Symbol> symbol = std::dynamic_pointer_cast<Symbol>(token);
      if(symbol->isDefined()) {
        lineAsm.push_back(symbol->getDefinition().number);
      } else {
        _assemble_error(&lineAsm);
      }
    } else if(tokenType == "Instruction") {
      std::shared_ptr<Instruction> instruction = std::dynamic_pointer_cast<Instruction>(token);
      lineAsm.push_back(instruction->getAddress().number);
    } else if(tokenType == "Value") {
      lineAsm.push_back(std::stoi(token->getText()));
    } else if(tokenType == "Directive") {
      _assemble_error(&lineAsm);
    }
  }

  _asm.push_back(lineAsm);
}

TwoPassAssembler::TwoPassAssembler(std::shared_ptr<PreProcessor> p, Address baseAddress) {
  preProcessor = p;
  firstPass(baseAddress);
  secondPass(baseAddress);
}

std::vector<std::vector<int32_t>> TwoPassAssembler::getAsm() { return _asm; }

std::string TwoPassAssembler::getAsmText() {
  std::vector<std::string> asmCodes;

  for(auto asmLine = _asm.begin(); asmLine != _asm.end(); ++asmLine) {
    for(auto asmCode = asmLine->begin(); asmCode != asmLine->end(); ++asmCode) {
      asmCodes.push_back(std::to_string(*asmCode));
    }
  }

  std::ostringstream asmText;
  const char * const delim = " ";

  std::copy(
      asmCodes.begin(), 
      asmCodes.end(), 
      std::ostream_iterator<std::string>(asmText, delim)
      );

  return asmText.str();
}

std::vector<ProgramLine> TwoPassAssembler::getFirstPassProgramLines() { 
  return firstPassProgramLines; 
}

std::vector<ProgramLine> TwoPassAssembler::getSecondPassProgramLines() { 
  return secondPassProgramLines; 
}

std::vector<std::shared_ptr<SymbolDefinition>> TwoPassAssembler::getSymbolDefinitionTable() { 
  return symbolDefinitionTable; 
}
