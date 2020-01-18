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


// ScriptEvaluationException.hpp

#pragma once


#include <stdexcept>
#include "ScriptError.hpp"

class ScriptException : public std::runtime_error
{
private:
	ScriptError _scriptError;

public:
	ScriptException() = delete; // Default-constructor
	ScriptException(ScriptException&&) noexcept = delete; // Move-constructor
	ScriptException(const ScriptException&) = delete; // Copy-constructor
	~ScriptException() override = default; // Destructor
	ScriptException& operator=(ScriptException&&) noexcept = delete; // Move-assignment
	ScriptException& operator=(ScriptException const&) = delete; // Copy-assignment

	explicit ScriptException(ScriptError scriptError)
	: std::runtime_error("ScriptError[" + std::to_string(scriptError) + "]")
	, _scriptError(scriptError)
	{}

	ScriptException(ScriptError scriptError, std::string description)
	: std::runtime_error("ScriptError[" + std::to_string(scriptError) + "]: " + std::move(description))
	, _scriptError(scriptError)
	{}

	[[nodiscard]]
	const ScriptError& errorCode() const
	{
		return _scriptError;
	}
};


