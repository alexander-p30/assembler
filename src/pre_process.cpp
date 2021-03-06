#include "../include/pre_process.hpp"
#include <algorithm>
#include <iterator>
#include <tuple>
#include <sstream>

#define LINE_DEFINES_MACRO(line) line->getRawTokens().size() == 2 && line->getRawTokens()[1].text == "MACRO"
#define LINE_ENDS_MACRO(line) line.getRawTokens().size() == 1 && line.getRawTokens()[0].text == "ENDMACRO"
#define LINE_DEFINES_CONDITIONAL(line) line->getRawTokens().size() == 2 && line->getRawTokens()[0].text == "IF"
#define LINE_DEFINES_EQU(line) line->getRawTokens().size() == 3 && line->getRawTokens()[1].text == "EQU"

Macro buildMacro(std::vector<RawLine>::iterator macroStart, 
    std::vector<RawLine>::iterator macroEnd, bool hasEnd) {
  RawToken macroDefinition = macroStart->getRawTokens().front();
  std::vector<RawLine> innerCode{std::next(macroStart, 1), macroEnd};
  return Macro(macroDefinition, innerCode, hasEnd);
}

Equ buildEqu(RawLine rawLine) {
  RawToken rToken = rawLine.getRawTokens()[0];
  int32_t value = std::stoi(rawLine.getRawTokens()[2].text);
  return Equ(rToken, value);
}

If buildConditional(RawLine conditionalLine, RawLine codeLine) {
  RawToken rawToken = conditionalLine.getRawTokens()[1];
  return If(rawToken, codeLine);
}

/***********************
 *        Macro        *
 ***********************/

Macro::Macro(RawToken def, std::vector<RawLine> code, bool hasEnd) {
  innerCode = code;
  label = def.text;
  label.pop_back();
  definitionRawToken = std::make_shared<RawToken>(def);
  end = hasEnd;
}

bool Macro::hasEnd() { return end; }

std::string Macro::getLabel() { return label; }

std::vector<RawLine> Macro::expand() { return innerCode; }

/*******************
 *       If        *
 *******************/

If::If(RawToken rToken, RawLine code) {
  rawToken = std::make_shared<RawToken>(rToken);
  innerCode = code;
  label = rawToken->text;
  cond = false;
}

bool If::setCond(int32_t val) { return cond = val; }

bool If::shouldExpand() { return cond; }

std::string If::getLabel() { return label; }

RawLine If::expand() { return innerCode; }

/*********************
 *        Equ        *
 *********************/

Equ::Equ(RawToken rToken, int32_t v) {
  rawToken = std::make_shared<RawToken>(rToken);
  label = rawToken->text;
  label.pop_back();
  value = v;
  token = RawToken{std::to_string(v), Location{"", 0, 0}};
}

int32_t Equ::getValue() { return value; }

std::string Equ::getLabel() { return label; }

RawToken Equ::expand() { return token; }

/*******************************
 *         PreProcessor        *
 *******************************/

PreProcessor::PreProcessor(std::vector<RawLine> l, bool shouldProcessMacros) {
  std::vector<RawLine> directiveFreeLines;
  lines = l;

  for(auto rLine = l.begin(); rLine != l.end(); ++rLine) {
    std::vector<RawToken> lineRawTokens = rLine->getRawTokens();

    if(LINE_DEFINES_MACRO(rLine) && shouldProcessMacros) {
      auto macroEnd = std::find_if(rLine, l.end(), [](RawLine rl) { 
        return LINE_ENDS_MACRO(rl);
      });

      bool hasEnd = macroEnd != l.end() && macroEnd->getText() == "ENDMACRO";

      Macro macro = buildMacro(rLine, macroEnd, hasEnd);
      mdt.push_back(macro);


      if(macro.hasEnd()) {
        rLine = macroEnd;
      }
    } else if(LINE_DEFINES_EQU(rLine)) {
      vals.push_back(buildEqu(*rLine));
    } else if(LINE_DEFINES_CONDITIONAL(rLine)) {
      directiveFreeLines.push_back(*rLine);

      RawLine conditionalCodeLine = *(std::next(rLine, 1));
      conditionals.push_back(buildConditional(*rLine, conditionalCodeLine));

      rLine = std::next(rLine, 1);
    } else {
      directiveFreeLines.push_back(*rLine);
    }
  }

  std::vector<RawLine> preProcessedLinesAux;
  for(auto rLine = directiveFreeLines.begin(); rLine != directiveFreeLines.end(); ++rLine) {
    std::vector<RawToken> lineRawTokens = rLine->getRawTokens();

    if(LINE_DEFINES_CONDITIONAL(rLine)) {
      std::string conditionalLabel = lineRawTokens.back().text;
      auto conditionalVal = this->findVal(conditionalLabel);
      auto conditional = this->findConditional(conditionalLabel);

      if(conditional != conditionals.end() && conditionalVal != vals.end()) {
        conditional->setCond(conditionalVal->getValue());

        if(conditional->shouldExpand()) {
          preProcessedLinesAux.push_back(conditional->expand());
        }
      }
    } else if(lineRawTokens.size() == 1) {
      std::string tokenLabel = lineRawTokens.front().text;
      auto macro = this->findMacro(tokenLabel);

      if(macro != mdt.end()) {
        std::vector<RawLine> macroCode = macro->expand();
        preProcessedLinesAux.insert(preProcessedLinesAux.end(), macroCode.begin(), macroCode.end());
      } else {
        preProcessedLinesAux.push_back(*rLine);
      }
    } else {
      std::vector<Equ> v = vals;

      for(auto rToken = lineRawTokens.begin(); rToken != lineRawTokens.end(); ++rToken) {
        auto rTokenVal = this->findVal(rToken->text);
        if(rTokenVal != vals.end()) {
          (*rToken) = rTokenVal->expand();
        }
      }

      rLine->setRawTokens(lineRawTokens);
      preProcessedLinesAux.push_back(*rLine);
    }
  }

  preProcessedLines = preProcessedLinesAux;
}

std::vector<Macro>::iterator PreProcessor::findMacro(std::string tokenLabel) {
  return std::find_if(mdt.begin(), mdt.end(), [tokenLabel](Macro m) {
      return m.getLabel() == tokenLabel;
      }); 
}

std::vector<If>::iterator PreProcessor::findConditional(std::string conditionalLabel) {
   return std::find_if(conditionals.begin(), conditionals.end(), 
          [conditionalLabel](If cond) {
          return cond.getLabel() == conditionalLabel;
      }); 
}

std::vector<Equ>::iterator PreProcessor::findVal(std::string valToken) {
  return std::find_if(vals.begin(), vals.end(), [valToken](Equ e) { 
      return e.getLabel() == valToken; 
      });
}

std::vector<RawLine> PreProcessor::getLines() { return lines; }

std::vector<RawLine> PreProcessor::getPreProcessedLines() { return preProcessedLines; }

std::string PreProcessor::getPreProcessedText() {
  std::vector<std::string> preProcessedWords;

  for(auto line = preProcessedLines.begin(); line != preProcessedLines.end(); ++line) {
    std::vector<RawToken> tokens = line->getRawTokens();

    for(auto t = tokens.begin(); t != tokens.end(); ++t) {
      preProcessedWords.push_back(t->text);
    }

    preProcessedWords.push_back("\n");
  }

  std::ostringstream preProcessedText;
  const char * const delim = " ";

  std::copy(
      preProcessedWords.begin(), 
      preProcessedWords.end(), 
      std::ostream_iterator<std::string>(preProcessedText, delim)
      );

  return preProcessedText.str();
}

std::vector<Macro> PreProcessor::getMdt() { return mdt; }

std::vector<If> PreProcessor::getConditionals() { return conditionals; }

std::vector<Equ> PreProcessor::getVals() { return vals; }

