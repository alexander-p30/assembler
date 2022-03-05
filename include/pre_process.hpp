#ifndef PRE_PROCESS_H
#define PRE_PROCESS_H

#include "text_object.hpp"
#include <map>
#include <cstdint>

class Macro {
  private:
    std::string label;
    std::vector<RawLine> innerCode;
    bool end;
  public:
    std::shared_ptr<RawToken> definitionRawToken;
    Macro(RawToken def, std::vector<RawLine> code, bool hasEnd);
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
    std::shared_ptr<RawToken> rawToken;
    If(RawToken rToken, RawLine code);
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
    std::shared_ptr<RawToken> rawToken;
    Equ(RawToken rToken, int32_t v);
    int32_t getValue();
    std::string getLabel();
    RawToken expand();
};

class PreProcessor {
  private:
    std::vector<RawLine> lines;
    std::vector<RawLine> preProcessedLines;
    std::vector<Macro> mdt;
    std::vector<If> conditionals;
    std::vector<Equ> vals;
    std::vector<Macro>::iterator findMacro(std::string label);
    std::vector<If>::iterator findConditional(std::string label);
    std::vector<Equ>::iterator findVal(std::string label);
  public:
    PreProcessor(std::vector<RawLine> lines, bool shouldProcessMacros);
    std::vector<RawLine> getLines();
    std::vector<RawLine> getPreProcessedLines();
    std::string getPreProcessedText();
    std::vector<Macro> getMdt();
    std::vector<If> getConditionals();
    std::vector<Equ> getVals();
};


#endif
