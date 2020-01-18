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


// Interpreter.hpp

#pragma once

#include <fstream>
#include <crypto/keys/CPubKey.hpp>
#include <script/checkers/SignatureChecker.hpp>
#include <blockchain/TxWitness.hpp>
#include "Script.hpp"
#include "Stack.hpp"
#include "ScriptError.hpp"
#include "../types/Flags.hpp"
#include "ScriptVerifyFlags.hpp"

//using valtype = ::StackFrame;
//using opcodetype = ::OpCode;
//using CScript = ::Script;
//using CScriptWitness = ::ScriptWitness;
using CPubKey = ::CPubKey;

class Interpreter final
{
private:
	const Flags<ScriptVerifyFlags> flags;
	const BaseSignatureChecker& checker;
	const SigVersion sigversion;
	ScriptError* const error;
	const bool isStacktraceEnabled;
	std::ostream& _stacktrace;

	size_t _step;
	size_t _opCount;
	Stack _stack;
	Stack _altStack;
	std::vector<bool> _conditionalExecutionFlags;

	Script::const_iterator _beginScriptPosition;
	Script::const_iterator _endScriptPosition;
	Script::const_iterator _curScriptPosition;
	Script::const_iterator _separatorPosition;

public:
	Interpreter() = delete; // Default-constructor
//	Interpreter(Interpreter&&) noexcept = delete; // Move-constructor
//	Interpreter(const Interpreter&) = delete; // Copy-constructor
	~Interpreter() = default; // Destructor
//	Interpreter& operator=(Interpreter&&) noexcept = default; // Move-assignment
//	Interpreter& operator=(Interpreter const&) = default; // Copy-assignment

	Interpreter(
		Flags<ScriptVerifyFlags> flags,
		const BaseSignatureChecker& checker,
		SigVersion version,
		ScriptError* pError,
		std::ostream& stacktrace
	)
	: flags(flags)
	, checker(checker)
	, sigversion(version)
	, error(pError)
	, isStacktraceEnabled(true)
	, _stacktrace(stacktrace)
	, _step(0)
	, _opCount(0)
	{
	}

	Interpreter(
		Flags<ScriptVerifyFlags> flags,
		const BaseSignatureChecker& checker,
		SigVersion version,
		ScriptError* pError
	)
	: flags(flags)
	, checker(checker)
	, sigversion(version)
	, error(pError)
	, isStacktraceEnabled(false)
	, _stacktrace([]()->std::ostream&{ static std::ofstream nullstream; nullstream.setstate(std::iostream::badbit); return nullstream;}())
	, _step(0)
	, _opCount(0)
	{
	}

	static bool EvalScript(
		const Script& script,
		Flags<ScriptVerifyFlags> flags,
		const BaseSignatureChecker& checker,
		SigVersion sigversion,
		ScriptError* serror
	);

	bool execute(const Script& script);

	inline bool resetAndExecute(const Script& script)
	{
		reset();
		return execute(script);
	}

	bool VerifyScript(
		const Script& scriptSig,
		const Script& scriptPubKey,
		const TxWitness* witness,
		unsigned int flags,
		const BaseSignatureChecker& checker,
		ScriptError* serror = nullptr
	);

	void reset();

	void dump(std::ostream& os);

	[[nodiscard]]
	std::vector<StackFrame> stackData(bool forAltStack = false) const
	{
		std::vector<StackFrame> ret;

		auto& stack = const_cast<Stack&>(forAltStack ? _altStack : _stack);
		while (!stack.empty())
		{
			ret.emplace_back(stack.take());
		}
		for (auto i = ret.rbegin(); i != ret.rend(); ++i)
		{
			stack.push(*i);
		}

		return ret;
	}

private:

	bool static CastToBool(const StackFrame& stackFrame);

	int static FindAndDelete(Script& haystack, const Script& needle);

	bool static CheckSignatureEncoding(const StackFrame &vchSig, Flags<ScriptVerifyFlags> flags, ScriptError* serror);

	bool static CheckPubKeyEncoding(const ScriptData& pubKey, Flags<ScriptVerifyFlags> flags, const SigVersion &sigversion, ScriptError* serror);


private:
	void ensureStackHasEnoughFrames(size_t need);

	void op_PushData(OpCode opCode, ScriptData&& data);
	void op_PushData1(ScriptData&& data);
	void op_PushData2(ScriptData&& data);
	void op_PushData4(ScriptData&& data);
	void op_N(OpCode opCode);

	void op_CheckLockTimeVerify();

	void op_CheckSequenceVerify();

	void op_Noop();

	void op_Verify();

	void op_Return();

	void op_ToAltStack();

	void op_FromAltStack();

	void op_2Drop();

	void op_2Dup();

	void op_3Dup();

	void op_2Over();

	void op_2Rot();

	void op_2Swap();

	void op_IfDup();

	void op_Depth();

	void op_Drop();

	void op_Dup();

	void op_Nip();

	void op_Over();

	void op_Pick();

	void op_Roll();

	void op_Rot();

	void op_Swap();

	void op_Tuck();

	void op_Size();

	void op_Equal();

	void op_EqualVerify();

	void op_1Add();

	void op_1Sub();

	void op_Negate();

	void op_Abs();

	void op_Not();

	void op_0NotEqual();

	void op_Add();

	void op_Sub();

	void op_BoolAnd();

	void op_BoolOr();

	void op_NumEqual();

	void op_NumEqualVerify();

	void op_NumNotEqual();

	void op_LessThan();

	void op_GreaterThan();

	void opLessThanOrEqual();

	void op_GreaterThanOrEqual();

	void op_Min();

	void op_Max();

	void op_Within();

	void op_RIPEMD();

	void op_SHA1();

	void op_SHA256();

	void op_Hash160();

	void op_Hash256();

	void op_CheckSig();

	void op_CheckSigVerify();

	void op_CheckMultiSig();

	void op_CheckMultiSigVerify();

	void op_CodeSeparator();

	void op_Nop();
};


