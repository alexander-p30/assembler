#include "../include/analyze.hpp"

PreProcessorAnalyzer::PreProcessorAnalyzer(std::shared_ptr<PreProcessor> pp) {
  preProcessor = pp;
}

TwoPassAssemblerAnalyzer::TwoPassAssemblerAnalyzer(std::shared_ptr<TwoPassAssembler> asmer) {
  twoPassAssembler = asmer;
}

MacroAnalyzer::MacroAnalyzer(std::shared_ptr<PreProcessor> pp) : PreProcessorAnalyzer(pp) { }

std::vector<std::shared_ptr<Error>> MacroAnalyzer::analyze() {
  std::vector<Macro> macros = preProcessor->getMdt();
  std::vector<RawLine> codeLines = preProcessor->getLines();

  for(auto macro = macros.begin(); macro != macros.end(); ++macro) {
    if(!macro->hasEnd()) {
      std::shared_ptr<SyntacticalError> err{ new SyntacticalError(macro->definitionRawToken, "macro sem end.") };
      errors.push_back(err);
    }

    bool isMacroUsed = false;
    for(auto line = codeLines.begin(); line != codeLines.end() && !isMacroUsed; ++line) {
      std::vector<RawToken> lineTokens = line->getRawTokens();

      isMacroUsed += std::count_if(lineTokens.begin(), lineTokens.end(), [macro](RawToken rToken) {
          return rToken.text == macro->getLabel();
      });
    }

    if(!isMacroUsed) {
      std::shared_ptr<SemanticError> err{ 
        new SemanticError(macro->definitionRawToken, "macro nao utilizado.") 
      };
      errors.push_back(err);
    }
  }

  return errors;
}

DirectiveAnalyzer::DirectiveAnalyzer(std::shared_ptr<PreProcessor> pp) : PreProcessorAnalyzer(pp) { }

std::vector<std::shared_ptr<Error>> DirectiveAnalyzer::analyze() {
  std::vector<Equ> vals = preProcessor->getVals();
  std::vector<RawLine> codeLines = preProcessor->getLines();

  for(auto val = vals.begin(); val != vals.end(); ++val) {
    bool isValUsed = false;
    for(auto line = codeLines.begin(); line != codeLines.end() && !isValUsed; ++line) {
      std::vector<RawToken> lineTokens = line->getRawTokens();

      isValUsed += std::count_if(lineTokens.begin(), lineTokens.end(), [val](RawToken rToken) {
          return rToken.text == val->getLabel();
      });
    }

    if(!isValUsed) {
      std::shared_ptr<SemanticError> err{ 
        new SemanticError(val->rawToken, "rotulo com equ nao utilizado.") 
      };
      errors.push_back(err);
    }
  }

  std::vector<If> conditionals = preProcessor->getConditionals();
  for(auto conditional = conditionals.begin(); conditional != conditionals.end(); ++conditional) {
    std::string conditionalLabel = conditional->getLabel();

    bool isLabelDefined = std::count_if(vals.begin(), vals.end(), 
        [conditionalLabel](Equ val) {
        return val.getLabel() == conditionalLabel;
        });

    if(!isLabelDefined) {
      std::shared_ptr<SemanticError> err{ 
        new SemanticError(conditional->rawToken, "IF com rotulo nao declarado por EQU.") 
      };
      errors.push_back(err);
    }
  }

  return errors;
}

LexicalAnalyzer::LexicalAnalyzer(std::shared_ptr<PreProcessor> pp) : PreProcessorAnalyzer(pp) { }

std::vector<std::shared_ptr<Error>> LexicalAnalyzer::analyze() {
  return errors;
}

SyntacticalAnalyzer::SyntacticalAnalyzer(std::shared_ptr<TwoPassAssembler> asmer) : TwoPassAssemblerAnalyzer(asmer) { }

std::vector<std::shared_ptr<Error>> SyntacticalAnalyzer::analyze() {
  return errors;
}

SemanticAnalyzer::SemanticAnalyzer(std::shared_ptr<TwoPassAssembler> asmer) : TwoPassAssemblerAnalyzer(asmer) { }

std::vector<std::shared_ptr<Error>> SemanticAnalyzer::analyze() {
  return errors;
}

