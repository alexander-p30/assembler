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

class Analyzer {
  public:
    virtual std::vector<Error> analyze() = 0;
};

class MacroAnalyzer : public Analyzer {
  public:
    MacroAnalyzer(std::vector<ProgramLine> program);
};

class DirectiveAnalyzer : public Analyzer {
  public:
    DirectiveAnalyzer(std::vector<ProgramLine> program);
};

class LexicalAnalyzer : public Analyzer {
  public:
    LexicalAnalyzer(std::vector<ProgramLine> program);
};

class SyntacticalAnalyzer : public Analyzer {
  public:
    SyntacticalAnalyzer(std::vector<ProgramLine> program);
};

class SemanticalAnalyzer : public Analyzer {
  public:
    SemanticalAnalyzer(std::vector<ProgramLine> program);
};

typedef std::vector<Analyzer> analysis;

class TwoPassAssembler {
  private:
    PreProcessor * preProcessor;
    std::vector<std::shared_ptr<SymbolDefinition>> symbolDefinitionTable;
    std::vector<ProgramLine> firstPassProgramLines;
    std::vector<ProgramLine> secondPassProgramLines;
    std::vector<std::shared_ptr<Token>> specializeRawTokens(std::vector<RawToken> rawTokens, Address * address);
    std::vector<std::shared_ptr<SymbolDefinition>>::iterator findSymbolDefinition(std::string label);
  public:
    TwoPassAssembler(PreProcessor * p, Address baseAddress);
    std::vector<ProgramLine> getFirstPassProgramLines();
    std::vector<ProgramLine> getSecondPassProgramLines();
};

#endif
