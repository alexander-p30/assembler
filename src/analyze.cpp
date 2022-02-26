#include "../include/analyze.hpp"
#include <map>
#include <regex>

#define MAX_TOKEN_LENGTH 99
#define SYMBOL_FORMAT_REGEX "^[A-Z]{1}[A-Z_0-9]*"
#define VALUE_FORMAT_REGEX "^-?[0-9]*"

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

bool isAValidToken(std::shared_ptr<Token> token) {
  std::string tokenType = token->_name();

  if(tokenType == "SymbolDefinition" || tokenType == "Symbol") {
    bool appropriateLength = token->getText().size() <= MAX_TOKEN_LENGTH;
    bool allowedCharactersOnly = std::regex_match(token->getText(), std::regex(SYMBOL_FORMAT_REGEX));

    return appropriateLength && allowedCharactersOnly;
  } else if(tokenType == "Value") {
    bool numberOnly = std::regex_match(token->getText(), std::regex(VALUE_FORMAT_REGEX));
    return numberOnly;
  }


  return true;
}

LexicalAnalyzer::LexicalAnalyzer(std::shared_ptr<TwoPassAssembler> pp) : TwoPassAssemblerAnalyzer(pp) { }

std::vector<std::shared_ptr<Error>> LexicalAnalyzer::analyze() {
  std::vector<ProgramLine> lines = twoPassAssembler->getFirstPassProgramLines();

  for(auto line = lines.begin(); line != lines.end(); ++line) {
    std::vector<std::shared_ptr<Token>> tokens = line->getTokens();

    for(auto token = tokens.begin(); token != tokens.end(); ++token) {
      if(!isAValidToken(*token)) {
        std::shared_ptr<LexicalError> err{ 
          new LexicalError(*token, "token invalido.") 
        };
        errors.push_back(err);
      }
    }
  }

  return errors;
}

bool isSymbolDef(std::shared_ptr<Token> t) { return t->_name() == "SymbolDefinition"; }

SyntacticalAnalyzer::SyntacticalAnalyzer(std::shared_ptr<TwoPassAssembler> asmer) : TwoPassAssemblerAnalyzer(asmer) { }

std::shared_ptr<SyntacticalError> scanMultipleSymbolDefinitionError(
    std::vector<std::shared_ptr<Token>> tokens
    ) {
    int32_t symbolDefinitionCount = std::count_if(tokens.begin(), tokens.end(), 
        [](std::shared_ptr<Token> t) { return isSymbolDef(t); });

    if(symbolDefinitionCount > 1) {
      std::shared_ptr<Token> lastSymbolDef = *std::find_if(tokens.rbegin(), tokens.rend(), 
          [](std::shared_ptr<Token> t) { return isSymbolDef(t); });

      return std::shared_ptr<SyntacticalError>{
        new SyntacticalError(lastSymbolDef, "mais de um rotulo na mesma linha.")
      };
    }

  return nullptr;
}

std::shared_ptr<SyntacticalError> scanRepeatedSymbolDefinition(
    std::vector<std::shared_ptr<Token>> tokens
    ) {
    int32_t symbolDefinitionCount = std::count_if(tokens.begin(), tokens.end(), 
        [](std::shared_ptr<Token> t) { return isSymbolDef(t); });

    if(symbolDefinitionCount > 1) {
      std::shared_ptr<Token> lastSymbolDef = *std::find_if(tokens.rbegin(), tokens.rend(), 
          [](std::shared_ptr<Token> t) { return isSymbolDef(t); });

      return std::shared_ptr<SyntacticalError>{
        new SyntacticalError(lastSymbolDef, "mais de um rotulo na mesma linha.")
      };
    }

  return nullptr;
}

std::shared_ptr<SyntacticalError> scanInstructionArgumentsError(
    std::vector<std::shared_ptr<Token>> tokens
    ) {
  std::map<std::string, int32_t> instructionArgumentCount = {
    {"ADD", 1}, {"SUB", 1}, {"MULT", 1}, {"DIV", 1}, {"JMP", 1}, {"JMPN", 1}, {"JMPP", 1}, 
    {"JMPZ", 1}, {"COPY", 2}, {"LOAD", 1}, {"STORE", 1}, {"INPUT", 1}, {"OUTPUT", 1}, {"STOP", 0}
  };

  for(auto token = tokens.begin(); token != tokens.end(); ++token) {
    std::shared_ptr<Token> t = *token;

    if(t->_name() != "Instruction")
      continue;

    std::vector<std::shared_ptr<Token>> lineInstructionSection(token, tokens.end());

    if(lineInstructionSection.size() != instructionArgumentCount[t->getText()] + 1)
      return std::shared_ptr<SyntacticalError>(
          new SyntacticalError(t, "instrucao com quantidade errada de operandos.")
        );

    return nullptr;
  }

  return nullptr;
}

bool SyntacticalAnalyzer::maybeAddError(std::shared_ptr<Error> err) {
  if(err != nullptr) {
    errors.push_back(err);
    return true;
  }

  return false;
}

std::vector<std::shared_ptr<Error>> SyntacticalAnalyzer::analyze() {
  std::vector<ProgramLine> lines = twoPassAssembler->getSecondPassProgramLines();

  for(auto line = lines.begin(); line != lines.end(); ++line) {
    std::vector<std::shared_ptr<Token>> tokens = line->getTokens();

    maybeAddError(scanMultipleSymbolDefinitionError(tokens));
    maybeAddError(scanInstructionArgumentsError(tokens));
  }

  return errors;
}

SemanticAnalyzer::SemanticAnalyzer(std::shared_ptr<TwoPassAssembler> asmer) : TwoPassAssemblerAnalyzer(asmer) { }

bool SemanticAnalyzer::maybeAddError(std::shared_ptr<Error> err) {
  if(err != nullptr) {
    errors.push_back(err);
    return true;
  }

  return false;
}

std::shared_ptr<SemanticError> scanMissingDefinition(std::shared_ptr<Token> token) {
  if(token->_name() != "Symbol")
    return nullptr;

  std::shared_ptr<Symbol> symbol = std::dynamic_pointer_cast<Symbol>(token);
  if(!(symbol->isDefined()))
    return std::shared_ptr<SemanticError>( new SemanticError(symbol, "simbolo nao declarado") );

  return nullptr;
}

std::shared_ptr<SemanticError> scanDuplicateSymbolDefinition(
    std::vector<std::shared_ptr<SymbolDefinition>>::reverse_iterator symbolDefIt,
    std::vector<std::shared_ptr<SymbolDefinition>> * symbolDefinitions
    ) {
  int32_t symbolDefOccurenceCount = std::count_if(symbolDefIt, symbolDefinitions->rend(), 
      [symbolDefIt](std::shared_ptr<SymbolDefinition> symbolDef) {
      return symbolDef->getText() == (*symbolDefIt)->getText();
      });

  if(symbolDefOccurenceCount > 1)
    return std::shared_ptr<SemanticError>( 
        new SemanticError(*symbolDefIt, "declaracao repetida.") 
      );

  return nullptr;
}

std::vector<std::shared_ptr<Error>> SemanticAnalyzer::analyze() {
  std::vector<ProgramLine> lines = twoPassAssembler->getSecondPassProgramLines();

  for(auto line = lines.begin(); line != lines.end(); ++line) {
    std::vector<std::shared_ptr<Token>> tokens = line->getTokens();

    for(auto token = tokens.begin(); token != tokens.end(); ++token)
      maybeAddError(scanMissingDefinition(*token));
  }

  std::vector<std::shared_ptr<SymbolDefinition>> symbolDefinitions = 
    twoPassAssembler->getSymbolDefinitionTable();

  for(auto symbolDef = symbolDefinitions.rbegin(); symbolDef != symbolDefinitions.rend(); ++symbolDef) {
    maybeAddError(scanDuplicateSymbolDefinition(symbolDef, &symbolDefinitions));
  }

  return errors;
}

