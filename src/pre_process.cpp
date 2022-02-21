#include "../include/pre_process.hpp"
#include <tuple>

Macro::Macro(std::string lab, std::vector<RawLine> code) {
  innerCode = code;
  label = lab;
}

std::string Macro::getLabel() { return label; }

std::vector<RawLine> Macro::expand() { return innerCode; }

If::If(bool cond, RawLine code) {
  innerCode = code;
  condition = cond;
}

bool If::shouldExpand() { return condition; }

RawLine If::expand() { return innerCode; }

Equ::Equ(std::string lab, int32_t v) {
  label = lab;
  value = v;
  token = RawToken{std::to_string(v), Address{AddressType::Absolute, 0}, Location{"", 0}};
}

std::string Equ::getLabel() { return label; }

RawToken Equ::expand() { return token; }

Macro buildMacro(std::vector<RawLine>::iterator macroStart, std::vector<RawLine>::iterator macroEnd) {
  std::string label = macroStart->getRawTokens()[0].text;
  label.pop_back();
  std::vector<RawLine> innerCode{std::next(macroStart, 1), macroEnd};
  return Macro(label, innerCode);
}

Equ buildEqu(RawLine rawLine) {
  std::string label = rawLine.getRawTokens()[0].text;
  int32_t value = std::stoi(rawLine.getRawTokens()[2].text);
  return Equ(label, value);
}

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
    } else {
      directiveFreeLines.push_back(*rLine);
    }
  }

  std::vector<RawLine> preProcessedLines;
  for(auto rLine = directiveFreeLines.begin(); rLine != directiveFreeLines.end(); ++rLine) {
    std::vector<RawToken> lineRawTokens = rLine->getRawTokens();

    if(lineRawTokens.size() == 1) {
      std::string tokenLabel = lineRawTokens.front().text;
      auto macro = std::find_if(mdt.begin(), mdt.end(), 
          [tokenLabel](Macro m) {
            return m.getLabel() == tokenLabel;
          });

      if(macro != mdt.end()) {
        preProcessedLines.insert(preProcessedLines.end(), macro->expand().begin(), macro->expand().end());
      }
    } else {
      std::vector<Equ> v = vals;
      // find all tokens in raw line that have an equ label matchin in vals
      auto valToken = std::find_if(lineRawTokens.begin(), lineRawTokens.end(), [v](RawToken t) { 
        return std::count(v.begin(), v.end(), [t](Equ equ) { return equ.getLabel() == t.text; });
      });

      if(valToken != lineRawTokens.end()) {
        auto equ = std::find_if(v.begin(), v.end(), [valToken](Equ equ) { return equ.getLabel() == valToken->text; })
        (*valToken) = ;
      }
    }

    for(auto rToken = lineRawTokens.begin(); rToken != lineRawTokens.end(); ++rToken) {
    }
  }
}

