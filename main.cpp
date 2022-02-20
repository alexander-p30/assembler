#include "./include/parse.hpp"
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

int main() {
  std::string lineText = "\tSOME_LABEL:\tCOPY A, B";
  Address addr = Address{AddressType::Absolute, 0};
  Line myLine = Line(lineText, addr);

  std::cout << lineText << std::endl;

  while (!myLine.isAtLastToken()) {
    RawToken t = myLine.getCurrentRawToken();
    std::string type =
        t.address.type == AddressType::Absolute ? "Absolute" : "Error";
    std::cout << "{Text: " << t.text << ", Address: {Type: " << type
              << ", Number: " << t.address.number << "}}" << std::endl;
    myLine.nextRawToken();
  }

  std::cout << std::endl;

  return 0;
}
