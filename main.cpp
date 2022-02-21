#include "./include/parse.hpp"
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

int main() {
  std::string rawLineText = "\tSOME_LABEL:\tCOPY A, B";
  Address addr = Address{AddressType::Absolute, 0};
  RawLine myRawLine = RawLine(rawLineText, addr);

  std::cout << rawLineText << std::endl;

  while (!myRawLine.isAtLastToken()) {
    RawToken t = myRawLine.getCurrentRawToken();
    std::string type =
        t.address.type == AddressType::Absolute ? "Absolute" : "Error";
    std::cout << "{Text: " << t.text << ", Address: {Type: " << type
              << ", Number: " << t.address.number << "}}" << std::endl;
    myRawLine.nextRawToken();
  }

  std::cout << std::endl;

  return 0;
}
