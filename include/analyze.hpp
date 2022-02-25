#ifndef ANALYZE_H
#define ANALYZE_H

#include "error.hpp"
#include "assemble.hpp"
#include "pre_process.hpp"

/**************
 * Interfaces *
 **************/

class Analyzer {
  protected:
    std::vector<std::shared_ptr<Error>> errors;
  public:
    virtual std::vector<std::shared_ptr<Error>> analyze() = 0;
};

class PreProcessorAnalyzer : public Analyzer {
  protected:
    std::shared_ptr<PreProcessor> preProcessor;
  public:
    PreProcessorAnalyzer(std::shared_ptr<PreProcessor> _preProcessor);
};

class TwoPassAssemblerAnalyzer : public Analyzer {
  protected:
    std::shared_ptr<TwoPassAssembler> twoPassAssembler;
  public:
    TwoPassAssemblerAnalyzer(std::shared_ptr<TwoPassAssembler> _twoPassAssembler);
};

/*******************
 * Implementations *
 *******************/

class MacroAnalyzer : public PreProcessorAnalyzer {
  public:
    MacroAnalyzer(std::shared_ptr<PreProcessor> _preProcessor);
    std::vector<std::shared_ptr<Error>> analyze();
};

class DirectiveAnalyzer : public PreProcessorAnalyzer {
  public:
    DirectiveAnalyzer(std::shared_ptr<PreProcessor> _preProcessor);
    std::vector<std::shared_ptr<Error>> analyze();
};

class LexicalAnalyzer : public PreProcessorAnalyzer {
  public:
    LexicalAnalyzer(std::shared_ptr<PreProcessor> _preProcessor);
    std::vector<std::shared_ptr<Error>> analyze();
};

class SyntacticalAnalyzer : public TwoPassAssemblerAnalyzer {
  public:
    SyntacticalAnalyzer(std::shared_ptr<TwoPassAssembler> _asmer);
    std::vector<std::shared_ptr<Error>> analyze();
};

class SemanticAnalyzer : public TwoPassAssemblerAnalyzer {
  public:
    SemanticAnalyzer(std::shared_ptr<TwoPassAssembler> _asmer);
    std::vector<std::shared_ptr<Error>> analyze();
};

#endif

