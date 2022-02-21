#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "parse.hpp"
#include "pre_process.hpp"
#include "text_object.hpp"
#include "error.hpp"
#include <vector>

class ProgramLine {
  private:
    std::vector<Token> tokens;
  public:
    ProgramLine(std::vector<Token> lineTokens); 
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
    PreProcessor preProcessor;
    std::vector<ProgramLine> programLines;
  public:
    TwoPassAssembler(Parser p);
};

#endif
