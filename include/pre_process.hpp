#ifndef PRE_PROCESS_H
#define PRE_PROCESS_H

#include "text_object.hpp"
#include <map>
#include <cstdint>

#define LINE_DEFINES_MACRO(line) line->getRawTokens().size() == 2 && line->getRawTokens()[1].text == "MACRO"
#define LINE_ENDS_MACRO(line) line.getRawTokens().size() == 1 && line.getRawTokens()[0].text == "ENDMACRO"
#define LINE_DEFINES_CONDITIONAL(line) line->getRawTokens().size() == 2 && line->getRawTokens()[0].text == "IF"
#define LINE_DEFINES_EQU(line) line->getRawTokens().size() == 3 && line->getRawTokens()[1].text == "EQU"

class Macro {
  private:
    std::string label;
    std::vector<RawLine> innerCode;
    bool end;
  public:
    Macro(std::string lab, std::vector<RawLine> code, bool hasEnd);
    std::string getLabel();
    std::vector<RawLine> expand();
    bool hasEnd();
};

class If {
  private:
    std::string label;
    bool cond;
    RawLine innerCode;
  public:
    If(std::string lab, RawLine code);
    bool setCond(int32_t val);
    bool shouldExpand();
    std::string getLabel();
    RawLine expand();
};

class Equ {
  private:
    RawToken token;
    std::string label;
    int32_t value;
  public:
    Equ(std::string label, int32_t v);
    int32_t getValue();
    std::string getLabel();
    RawToken expand();
};

class PreProcessor {
  private:
    std::vector<RawLine> lines;
    std::vector<Macro> mdt;
    std::vector<If> conditionals;
    std::vector<Equ> vals;
    std::vector<Macro>::iterator findMacro(std::string label);
    std::vector<If>::iterator findConditional(std::string label);
    std::vector<Equ>::iterator findVal(std::string label);
  public:
    PreProcessor(std::vector<RawLine> l);
    std::vector<RawLine> getPreProcessedLines();
};


#endif
