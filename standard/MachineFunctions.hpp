#pragma once

#include "../execution/Machine.hpp"
#include <iostream>

namespace MachineFunctions
{
    void printLine(SimpleLang::Machine *machine);

    void createArrayOfSize(SimpleLang::Machine *machine);
}