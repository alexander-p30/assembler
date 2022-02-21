#include "../include/address.hpp"

Address nextAddress(Address a) { return Address{a.type, NEXT(a)}; }

