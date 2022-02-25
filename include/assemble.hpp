#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "parse.hpp"
#include "pre_process.hpp"
#include "text_object.hpp"
#include "error.hpp"
#include <memory>
#include <vector>

class ProgramLine {
  private:
    std::vector<std::shared_ptr<Token>> tokens;
  public:
    ProgramLine(std::vector<std::shared_ptr<Token>> lineTokens); 
    std::vector<std::shared_ptr<Token>> getTokens();
    void inspect(int indent);
};

class TwoPassAssembler {
  private:
    PreProcessor * preProcessor;
    std::vector<std::shared_ptr<SymbolDefinition>> symbolDefinitionTable;
    std::vector<ProgramLine> firstPassProgramLines;
    std::vector<ProgramLine> secondPassProgramLines;
    std::vector<std::vector<int32_t>> _asm;
    void assembleLine(ProgramLine programLine);
    void firstPass(Address baseAddress);
    void secondPass(Address baseAddress);
    std::vector<std::shared_ptr<Token>> specializeRawTokens(std::vector<RawToken> rawTokens, Address * address);
    std::vector<std::shared_ptr<SymbolDefinition>>::iterator findSymbolDefinition(std::string label);
  public:
    TwoPassAssembler(PreProcessor * p, Address baseAddress);
    std::vector<ProgramLine> getFirstPassProgramLines();
    std::vector<ProgramLine> getSecondPassProgramLines();
    std::vector<std::vector<int32_t>> getAsm();
};

#endif
