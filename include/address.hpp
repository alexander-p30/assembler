#ifndef ADDRESS_H
#define ADDRESS_H

#include <cstdint>
#include <string>

#define NEXT(address) address.number + 1

enum class AddressType { Absolute, Relative, Undefined };

/*
 * An address composed of a number and a type, which
 * can be one of the following: Absoltue, Relative or
 * Undefined.
 */
struct Address {
  AddressType type;
  int32_t number;
};

Address nextAddress(Address a);

struct Location {
  std::string fileName;
  int32_t lineNumber;
  int32_t positionInLine;
};

#endif
