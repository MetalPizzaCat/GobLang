#pragma once

#include "../execution/Machine.hpp"
#include <iostream>

namespace MachineFunctions
{
    void printLine(GobLang::Machine *machine);

    void createArrayOfSize(GobLang::Machine *machine);

    void getSizeof(GobLang::Machine * machine);
}