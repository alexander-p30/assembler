#include "include/assemble.hpp"
#include "include/analyze.hpp"
#include "include/inspect.hpp"
#include "include/error.hpp"
#include "include/parse.hpp"
#include "include/pre_process.hpp"
#include "include/text_object.hpp"
#include <cstring>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <memory>

using namespace std;

#define RUN_MODE 1
#define INPUT_FILENAME 2
#define OUTPUT_FILENAME 3
#define DIRECTIVE_MODE "-p"
#define MACRO_MODE "-m"
#define ASSEMBLE_MODE "-o"

#define EQUAL_STRINGS(a, b) (strcmp(a, b) == 0)

void assemble(char * argv[]);

int main(int argc, char * argv[]) {
  if(argc != 4) {
    cout << "Erro! O comando e:\n";
    cout << "\t./montador {modo} {arquivo_de_entrada} {arquivo_de_saida}\n";
    return 0;
  }

  const vector<string> validModes{DIRECTIVE_MODE, MACRO_MODE, ASSEMBLE_MODE};

  if(!count(validModes.begin(), validModes.end(), argv[RUN_MODE])) {
    cout << "Erro! Modo invalido! Os modos disponiveis sao: ";
    cout << "'" << DIRECTIVE_MODE << "', '" 
      << MACRO_MODE << "' e '" 
      << ASSEMBLE_MODE << "'\n";
    return 0;
  }

  assemble(argv);

  return 0;
}

void writeFile(FileData * fileData) {
  ofstream file(fileData->fileName);
  file << fileData->fileContent << endl;
}

void analyze(Analyzer * analyzer, vector<shared_ptr<Error>> * errors) {
  vector<shared_ptr<Error>> analysisErrors = analyzer->analyze();

  errors->insert(errors->end(), analysisErrors.begin(), analysisErrors.end());
}

shared_ptr<PreProcessor> preProcess(Parser * parser, string runMode, 
    vector<shared_ptr<Error>> * errors) {
  bool shouldProcessMacros = runMode == MACRO_MODE || runMode == ASSEMBLE_MODE;

  shared_ptr<PreProcessor> preProcessor = shared_ptr<PreProcessor>{
    new PreProcessor(parser->getRawLines(), shouldProcessMacros)
  };

  DirectiveAnalyzer directiveAnalyzer = DirectiveAnalyzer(preProcessor);
  analyze(&directiveAnalyzer, errors);

  if(shouldProcessMacros) {
    MacroAnalyzer macroAnalyzer = MacroAnalyzer(preProcessor);
    analyze(&macroAnalyzer, errors);
  }

  return preProcessor;
}

void assemble(char * argv[]) {
  FileData * inputFileData = new FileData{ .fileName = argv[INPUT_FILENAME], .fileContent = ""};
  FileData * outputFileData = new FileData{ .fileName = argv[OUTPUT_FILENAME], .fileContent = ""};

  ifstream file(argv[INPUT_FILENAME]);
  stringstream buffer;
  buffer << file.rdbuf();
  inputFileData->fileContent = buffer.str();

  string runMode = argv[RUN_MODE];

  vector<shared_ptr<Error>> errors;

  Parser parser = Parser(*inputFileData);
  Address baseAddress = Address{ .type = AddressType::Absolute, .number = 0};
  shared_ptr<PreProcessor> preProcessor = preProcess(&parser, runMode, &errors);

  if(runMode == DIRECTIVE_MODE || runMode == MACRO_MODE) {
    outputFileData->fileContent = preProcessor->getPreProcessedText();
    writeFile(outputFileData);
    return;
  }

  shared_ptr<TwoPassAssembler> assembler{ new TwoPassAssembler(preProcessor, baseAddress) };

  LexicalAnalyzer lexicalAnalyzer = LexicalAnalyzer(assembler);
  analyze(&lexicalAnalyzer, &errors);

  SyntacticalAnalyzer syntacticalAnalyzer = SyntacticalAnalyzer(assembler);
  analyze(&syntacticalAnalyzer, &errors);

  SemanticAnalyzer semanticAnalyzer = SemanticAnalyzer(assembler);
  analyze(&semanticAnalyzer, &errors);

  sort(errors.begin(), errors.end(), [](shared_ptr<Error> errA, shared_ptr<Error> errB) { 
      Location locA = errA->getLocation();
      Location locB = errB->getLocation();
      return tie(locA.lineNumber, locA.positionInLine) < tie(locB.lineNumber, locB.positionInLine);
    });

  for_each(errors.begin(), errors.end(), [](shared_ptr<Error> err) { 
      cout << err->message() << endl; 
    });

  outputFileData->fileContent = assembler->getAsmText();
  writeFile(outputFileData);
}
