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


// ScriptError.hpp

#pragma once

enum class ScriptError
{
	// Success
	OK = 0,

	// Normal unsuccess
	EVAL_FALSE,
	OP_RETURN,

	// Limits of sizes/counts
	SCRIPT_SIZE,
	PUSH_SIZE,
	OP_COUNT,
	STACK_SIZE,
	SIG_COUNT,
	PUBKEY_COUNT,

	// Failed convertions
	CONVERTION_TO_NUMBER,
	CONVERTION_TO_DATA,

	// Failed verify operations
	VERIFY,
	EQUALVERIFY,
	CHECKMULTISIGVERIFY,
	CHECKSIGVERIFY,
	NUMEQUALVERIFY,

	// Logical/Format/Canonical errors
	BAD_OPCODE,
	DISABLED_OPCODE,
	INVALID_STACK_OPERATION,
	INVALID_ALTSTACK_OPERATION,
	UNBALANCED_CONDITIONAL,

	// CHECKLOCKTIMEVERIFY and CHECKSEQUENCEVERIFY
	NEGATIVE_LOCKTIME,
	UNSATISFIED_LOCKTIME,

	// Malleability
	SIG_HASHTYPE,
	SIG_DER,
	MINIMALDATA,
	SIG_PUSHONLY,
	SIG_HIGH_S,
	SIG_NULLDUMMY,
	PUBKEYTYPE,
	CLEANSTACK,
	MINIMALIF,
	SIG_NULLFAIL,
	NUMERIC_OPERATION,

	// softfork safeness
	DISCOURAGE_UPGRADABLE_NOPS,
	DISCOURAGE_UPGRADABLE_WITNESS_PROGRAM,

	// segregated witness
	WITNESS_PROGRAM_WRONG_LENGTH,
	WITNESS_PROGRAM_WITNESS_EMPTY,
	WITNESS_PROGRAM_MISMATCH,
	WITNESS_MALLEATED,
	WITNESS_MALLEATED_P2SH,
	WITNESS_UNEXPECTED,
	WITNESS_PUBKEYTYPE,

	// Constant scriptCode
	OP_CODESEPARATOR,
	SIG_FINDANDDELETE,

	ERROR_COUNT,

	// Execution
	NOT_EXECUTED,
	UNKNOWN_ERROR,

};

namespace std
{
const std::string& to_string(ScriptError scriptError);
}
