#include "../include/pre_process.hpp"
#include <algorithm>
#include <tuple>
#include <iostream>

using std::cout;

Macro buildMacro(std::vector<RawLine>::iterator macroStart, std::vector<RawLine>::iterator macroEnd) {
  RawToken macroDefinition = macroStart->getRawTokens().front();
  std::vector<RawLine> innerCode{std::next(macroStart, 1), macroEnd};
  return Macro(macroDefinition, innerCode, macroEnd->getText() == "ENDMACRO");
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
  end = hasEnd;
}

std::string Macro::getLabel() { return label; }

std::vector<RawLine> Macro::expand() { return innerCode; }

/*******************
 *       If        *
 *******************/

If::If(RawToken rToken, RawLine code) {
  rawToken = std::make_shared<RawToken>(rToken);
  innerCode = code;
  label = rawToken->text;
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

PreProcessor::PreProcessor(std::vector<RawLine> l) {
  std::vector<RawLine> directiveFreeLines;

  for(auto rLine = l.begin(); rLine != l.end(); ++rLine) {
    std::vector<RawToken> lineRawTokens = rLine->getRawTokens();

    if(LINE_DEFINES_MACRO(rLine)) {
      auto macroEnd = std::find_if(rLine, l.end(), [](RawLine rl) { 
        return LINE_ENDS_MACRO(rl);
      });

      mdt.push_back(buildMacro(rLine, macroEnd));
      rLine = macroEnd;
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

  std::vector<RawLine> preProcessedLines;
  for(auto rLine = directiveFreeLines.begin(); rLine != directiveFreeLines.end(); ++rLine) {
    std::vector<RawToken> lineRawTokens = rLine->getRawTokens();

    if(LINE_DEFINES_CONDITIONAL(rLine)) {
      std::string conditionalLabel = lineRawTokens.back().text;
      auto conditionalVal = this->findVal(conditionalLabel);
      auto conditional = this->findConditional(conditionalLabel);

      if(conditional != conditionals.end() && conditionalVal != vals.end()) {
        conditional->setCond(conditionalVal->getValue());

        if(conditional->shouldExpand()) {
          preProcessedLines.push_back(conditional->expand());
        }
      } else {
        preProcessedLines.push_back(*rLine);
      }
    } else if(lineRawTokens.size() == 1) {
      std::string tokenLabel = lineRawTokens.front().text;
      auto macro = this->findMacro(tokenLabel);

      if(macro != mdt.end()) {
        std::vector<RawLine> macroCode = macro->expand();
        preProcessedLines.insert(preProcessedLines.end(), macroCode.begin(), macroCode.end());
      } else {
        preProcessedLines.push_back(*rLine);
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
      preProcessedLines.push_back(*rLine);
    }
  }

  lines = preProcessedLines;
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

std::vector<RawLine> PreProcessor::getPreProcessedLines() { return lines; }

std::vector<Macro> PreProcessor::getMdt() { return mdt; }

std::vector<If> PreProcessor::getConditionals() { return conditionals; }

std::vector<Equ> PreProcessor::getVals() { return vals; }

