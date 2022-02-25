#include "../include/analyze.hpp"

PreProcessorAnalyzer::PreProcessorAnalyzer(std::shared_ptr<PreProcessor> pp) {
  preProcessor = pp;
}

TwoPassAssemblerAnalyzer::TwoPassAssemblerAnalyzer(std::shared_ptr<TwoPassAssembler> asmer) {
  twoPassAssembler = asmer;
}

MacroAnalyzer::MacroAnalyzer(std::shared_ptr<PreProcessor> pp) : PreProcessorAnalyzer(pp) { }

DirectiveAnalyzer::DirectiveAnalyzer(std::shared_ptr<PreProcessor> pp) : PreProcessorAnalyzer(pp) { }

LexicalAnalyzer::LexicalAnalyzer(std::shared_ptr<PreProcessor> pp) : PreProcessorAnalyzer(pp) { }

SyntacticalAnalyzer::SyntacticalAnalyzer(std::shared_ptr<TwoPassAssembler> asmer) : TwoPassAssemblerAnalyzer(asmer) { }

SemanticAnalyzer::SemanticAnalyzer(std::shared_ptr<TwoPassAssembler> asmer) : TwoPassAssemblerAnalyzer(asmer) { }

