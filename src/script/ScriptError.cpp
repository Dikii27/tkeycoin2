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


// ScriptError.cpp

#include <string>
#include "ScriptError.hpp"

const std::string& std::to_string(ScriptError scriptError)
{
	switch (scriptError)
	{
		// Success
		case ScriptError::OK                            : { static const std::string name("OK"); return name; }

		// Normal unsuccess
		case ScriptError::EVAL_FALSE                    : { static const std::string name("EVAL_FALSE"); return name; }
		case ScriptError::OP_RETURN                     : { static const std::string name("OP_RETURN"); return name; }

		// Limits of sizes/counts
		case ScriptError::SCRIPT_SIZE                   : { static const std::string name("SCRIPT_SIZE"); return name; }
		case ScriptError::PUSH_SIZE                     : { static const std::string name("PUSH_SIZE"); return name; }
		case ScriptError::OP_COUNT                      : { static const std::string name("OP_COUNT"); return name; }
		case ScriptError::STACK_SIZE                    : { static const std::string name("STACK_SIZE"); return name; }
		case ScriptError::SIG_COUNT                     : { static const std::string name("SIG_COUNT"); return name; }
		case ScriptError::PUBKEY_COUNT                  : { static const std::string name("PUBKEY_COUNT"); return name; }

		// Failed convertions
		case ScriptError::CONVERTION_TO_NUMBER          : { static const std::string name("CONVERTION_TO_NUMBER"); return name; }
		case ScriptError::CONVERTION_TO_DATA            : { static const std::string name("CONVERTION_TO_DATA"); return name; }

		// Failed verify operations
		case ScriptError::VERIFY                        : { static const std::string name("VERIFY"); return name; }
		case ScriptError::EQUALVERIFY                   : { static const std::string name("EQUALVERIFY"); return name; }
		case ScriptError::CHECKMULTISIGVERIFY           : { static const std::string name("CHECKMULTISIGVERIFY"); return name; }
		case ScriptError::CHECKSIGVERIFY                : { static const std::string name("CHECKSIGVERIFY"); return name; }
		case ScriptError::NUMEQUALVERIFY                : { static const std::string name("NUMEQUALVERIFY"); return name; }

		// Logical/Format/Canonical errors
		case ScriptError::BAD_OPCODE                    : { static const std::string name("BAD_OPCODE"); return name; }
		case ScriptError::DISABLED_OPCODE               : { static const std::string name("DISABLED_OPCODE"); return name; }
		case ScriptError::INVALID_STACK_OPERATION       : { static const std::string name("INVALID_STACK_OPERATION"); return name; }
		case ScriptError::INVALID_ALTSTACK_OPERATION    : { static const std::string name("INVALID_ALTSTACK_OPERATION"); return name; }
		case ScriptError::UNBALANCED_CONDITIONAL        : { static const std::string name("UNBALANCED_CONDITIONAL"); return name; }

		// CHECKLOCKTIMEVERIFY and CHECKSEQUENCEVERIFY
		case ScriptError::NEGATIVE_LOCKTIME             : { static const std::string name("NEGATIVE_LOCKTIME"); return name; }
		case ScriptError::UNSATISFIED_LOCKTIME          : { static const std::string name("UNSATISFIED_LOCKTIME"); return name; }

		// Malleability
		case ScriptError::SIG_HASHTYPE                  : { static const std::string name("SIG_HASHTYPE"); return name; }
		case ScriptError::SIG_DER                       : { static const std::string name("SIG_DER"); return name; }
		case ScriptError::MINIMALDATA                   : { static const std::string name("MINIMALDATA"); return name; }
		case ScriptError::SIG_PUSHONLY                  : { static const std::string name("SIG_PUSHONLY"); return name; }
		case ScriptError::SIG_HIGH_S                    : { static const std::string name("SIG_HIGH_S"); return name; }
		case ScriptError::SIG_NULLDUMMY                 : { static const std::string name("SIG_NULLDUMMY"); return name; }
		case ScriptError::PUBKEYTYPE                    : { static const std::string name("PUBKEYTYPE"); return name; }
		case ScriptError::CLEANSTACK                    : { static const std::string name("CLEANSTACK"); return name; }
		case ScriptError::MINIMALIF                     : { static const std::string name("MINIMALIF"); return name; }
		case ScriptError::SIG_NULLFAIL                  : { static const std::string name("SIG_NULLFAIL"); return name; }
		case ScriptError::NUMERIC_OPERATION             : { static const std::string name("NUMERIC_OPERATION"); return name; }

		// softfork safeness
		case ScriptError::DISCOURAGE_UPGRADABLE_NOPS    : { static const std::string name("DISCOURAGE_UPGRADABLE_NOPS"); return name; }
		case ScriptError::DISCOURAGE_UPGRADABLE_WITNESS_PROGRAM
														: { static const std::string name("DISCOURAGE_UPGRADABLE_WITNESS_PROGRAM"); return name; }

		// segregated witness
		case ScriptError::WITNESS_PROGRAM_WRONG_LENGTH  : { static const std::string name("WITNESS_PROGRAM_WRONG_LENGTH"); return name; }
		case ScriptError::WITNESS_PROGRAM_WITNESS_EMPTY : { static const std::string name("WITNESS_PROGRAM_WITNESS_EMPTY"); return name; }
		case ScriptError::WITNESS_PROGRAM_MISMATCH      : { static const std::string name("WITNESS_PROGRAM_MISMATCH"); return name; }
		case ScriptError::WITNESS_MALLEATED             : { static const std::string name("WITNESS_MALLEATED"); return name; }
		case ScriptError::WITNESS_MALLEATED_P2SH        : { static const std::string name("WITNESS_MALLEATED_P2SH"); return name; }
		case ScriptError::WITNESS_UNEXPECTED            : { static const std::string name("WITNESS_UNEXPECTED"); return name; }
		case ScriptError::WITNESS_PUBKEYTYPE            : { static const std::string name("WITNESS_PUBKEYTYPE"); return name; }

		// Constant scriptCode
		case ScriptError::OP_CODESEPARATOR              : { static const std::string name("OP_CODESEPARATOR"); return name; }
		case ScriptError::SIG_FINDANDDELETE             : { static const std::string name("SIG_FINDANDDELETE"); return name; }

		case ScriptError::ERROR_COUNT                   : { static const std::string name("ERROR_COUNT"); return name; }

		// Execution
		case ScriptError::NOT_EXECUTED                  : { static const std::string name("NOT_EXECUTED"); return name; }
		case ScriptError::UNKNOWN_ERROR                 : { static const std::string name("UNKNOWN_ERROR"); return name; }

		default: { static const std::string name("ERROR_invalid"); return name; }
	}
}
