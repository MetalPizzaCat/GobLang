#pragma once

#include "../execution/Machine.hpp"
#include <iostream>

namespace MachineFunctions
{

    /**
     * @brief Bind all standard functions to the instance of the machine
     * 
     * @param machine Instance of the interpreter to bind to
     */
    void bind(GobLang::Machine *machine);
    
    void printLine(GobLang::Machine *machine);

    void print(GobLang::Machine *machine);

    void createArrayOfSize(GobLang::Machine *machine);

    void getSizeof(GobLang::Machine *machine);

    void toString(GobLang::Machine *machine);

    /**
     * @brief Read a full line of user input and put it onto the stack
     *
     * @param machine
     */
    void input(GobLang::Machine *machine);

    void inputChar(GobLang::Machine *machine);

    namespace Math
    {
        void toInt(GobLang::Machine *machine);

        void randomIntInRange(GobLang::Machine *machine);

        void randomInt(GobLang::Machine *machine);

        void toFloat(GobLang::Machine *machine);

    }
}