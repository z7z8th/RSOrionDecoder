#ifndef __RS_DATE_H__
#define __RS_DATE_H__

#include <iostream>
#include <iomanip>

void OutputDateTime() {
    auto time = std::time(nullptr);
    std::cout << "[" << std::put_time(std::localtime(&time), "%F %T%z") << "]"; // ISO 8601 format.
}

#endif // __RS_DATE_H__