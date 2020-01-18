//  Copyright (c) 2017-2020 TKEY DMCC LLC & Tkeycoin Dao. All rights reserved.
//  Website: www.tkeycoin.com
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.


// Stack.hpp

#pragma once

#include <stack>
#include "StackFrame.hpp"

class Stack : private std::stack<StackFrame>
{
public:
	Stack() = default; // Default-constructor
	Stack(Stack&&) noexcept = delete; // Move-constructor
	Stack(const Stack&) = delete; // Copy-constructor
	~Stack() = default; // Destructor
	Stack& operator=(Stack&&) noexcept = delete; // Move-assignment
	Stack& operator=(Stack const&) = delete; // Copy-assignment

	using std::stack<StackFrame>::empty;
	using std::stack<StackFrame>::size;
	using std::stack<StackFrame>::pop;
	using std::stack<StackFrame>::top;
	using std::stack<StackFrame>::push;

	StackFrame take()
	{
		assert(!empty());
		StackFrame stackFrame = std::move(top());
		pop();
		return stackFrame;
	}

	friend std::ostream& operator<<(std::ostream& os, Stack& stack)
	{
		if (stack.empty())
		{
			os << "empty";
			return os;
		}
		Stack tmp;
		while (!stack.empty())
		{
			tmp.push(stack.take());
		}
		while (!tmp.empty())
		{
			auto stackFrame = tmp.take();
			os << stackFrame << ' ';
			stack.push(std::move(stackFrame));
		}
		return os;
	}

	void push(ScriptNum&& scriptNum)
	{
		std::stack<StackFrame>::push(std::forward<StackFrame>(scriptNum));
	}
	void push(const ScriptNum& scriptNum)
	{
		std::stack<StackFrame>::push(scriptNum);
	}
};
