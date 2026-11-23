#pragma once
#include <iostream>
#include <algorithm>
#include <fstream>
#include "codegen/Parser.hpp"
#include "codegen/CodeGenerator.hpp"

#include "codegen/Disassembly.hpp"
#include "execution/State.hpp"
namespace GobLang::Exec
{
	void print(State &state)
	{
		if (std::optional<Value> v = state.popFromStack(); v.has_value())
		{
			std::cout << ValueOperations::toString(v.value()) << std::endl;
		}
	}

	void len(State &state)
	{
		if (std::optional<Value> v = state.popFromStack(); v.has_value())
		{
			if (std::holds_alternative<GobLang::ArrayObject *>(v.value()))
			{
				ArrayObject const *obj = (ArrayObject *)std::get<ArrayObject *>(v.value());
				state.pushToStack(obj->getSize());
			}
			else if (std::holds_alternative<GobLang::StringObject *>(v.value()))
			{
				ArrayObject const *obj = (ArrayObject *)std::get<StringObject *>(v.value());
				state.pushToStack(obj->getSize());
			}
		}
	}

	void executeCode(std::string const &code)
	{
		State state;
		state.setGlobalVariable("print", state.createClosure(&print));
		state.setGlobalVariable("len", state.createClosure(&len));

		state.executeClosure(*state.loadString(code));
	}
}