#include "../include/assemble.hpp"
#include "../include/analyze.hpp"
#include "../include/inspect.hpp"
#include "../include/error.hpp"
#include "../include/parse.hpp"
#include "../include/pre_process.hpp"
#include "../include/text_object.hpp"
#include <cstring>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <memory>

using namespace std;

#define RUN_MODE 1
#define INPUT_FILENAME 2
#define OUTPUT_FILENAME 3
#define EQUAL_STRINGS(a, b) (strcmp(a, b) == 0)

void assemble(char * argv[]);

int main(int argc, char * argv[]) {
  if(argc != 4) {
    cout << "Erro! O comando é:\n";
    cout << "\t./montador {modo} {arquivo_de_entrada} {arquivo_de_saida}\n";
    return 0;
  }

  assemble(argv);

  return 0;
}

void assemble(char * argv[]) {
  ifstream t(argv[INPUT_FILENAME]);
  stringstream buffer;
  buffer << t.rdbuf();

  string inputText = buffer.str();
  string runMode = argv[RUN_MODE];

  Parser parser = Parser(FileData{argv[INPUT_FILENAME], inputText});
  Address baseAddress = Address{AddressType::Absolute, 0};


  if(runMode == "-p") {
    
  } else if(runMode == "-m") {
  } else if(runMode == "-o") {
  }
  PreProcessor * preProcessor;
  preProcessor = new PreProcessor(parser.getRawLines(), true);
  TwoPassAssembler assembler = TwoPassAssembler(preProcessor, baseAddress);

  MacroAnalyzer macroAnlz = MacroAnalyzer(make_shared<PreProcessor>(*preProcessor));
  macroAnlz.analyze();

  DirectiveAnalyzer directiveAnlz = DirectiveAnalyzer(make_shared<PreProcessor>(*preProcessor));
  directiveAnlz.analyze();

  LexicalAnalyzer lexicalAnlz = LexicalAnalyzer(make_shared<TwoPassAssembler>(assembler));
  lexicalAnlz.analyze();

  SyntacticalAnalyzer syntacticalAnlz = SyntacticalAnalyzer(make_shared<TwoPassAssembler>(assembler));
  syntacticalAnlz.analyze();

  SemanticAnalyzer semanticAnlz = SemanticAnalyzer(make_shared<TwoPassAssembler>(assembler));
  semanticAnlz.analyze();
}
