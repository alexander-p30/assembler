#ifndef INSPECT_H
#define INSPECT_H

#include "../include/assemble.hpp"
#include "../include/parse.hpp"
#include "../include/pre_process.hpp"
#include "../include/text_object.hpp"
#include <algorithm>
#include <iostream>
#include <memory>

using namespace std;

void inspect(Parser p, int x);
void inspect(RawLine l, int x);
void inspect(RawToken t, int x);
void inspect(Address a, int x);
void inspect(AddressType t, int x);
void inspect(Location a, int x);
void inspect(ProgramLine l, int x);
void inspect(std::shared_ptr<Token> t, int x);

#endif
