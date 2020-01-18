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


// Interpreter.cpp

#include <crypto/ripemd160.h>
#include <crypto/sha1.h>
#include <crypto/sha256.h>
#include <other/hash.h>
#include "Interpreter.hpp"
#include "ScriptError.hpp"
#include "../types/Flags.hpp"
#include "ScriptVerifyFlags.hpp"
#include "../blockchain/TxIn.hpp"
#include "ScriptException.hpp"
#include <cassert>

bool Interpreter::EvalScript(
	const Script& script,
	Flags<ScriptVerifyFlags> flags,
	const BaseSignatureChecker& checker,
	SigVersion sigversion,
	ScriptError* serror
)
{
	Interpreter interpreter(
		flags,
		checker,
		sigversion,
		serror
	);

	return interpreter.resetAndExecute(script);
}

void Interpreter::dump(std::ostream& os)
{
	os << "STEP #" << _step << "\n";

	os << "Script: ";
	Script::dump(os, _curScriptPosition, _endScriptPosition);
	os << '\n';

	if (!_conditionalExecutionFlags.empty())
	{
		os << "Execution flags:  ";
		for (auto flag : _conditionalExecutionFlags)
		{
			os << (flag ? "Y" : "N");
		}
		os << '\n';
	}

	os << "Stack:  " << _stack << '\n';

	if (!_altStack.empty())
	{
		os << "AltStack:  " << _altStack << '\n';
	}

	os << '\n';
}

void Interpreter::reset()
{
	_step = 0;
	_opCount = 0;
	_conditionalExecutionFlags.clear();
	while (!_stack.empty())
	{
		_stack.pop();
	}
	while (!_altStack.empty())
	{
		_altStack.pop();
	}
	if (error)
	{
		*error = ScriptError::UNKNOWN_ERROR;
	}
	_endScriptPosition = Script::const_iterator{};
	_beginScriptPosition = _endScriptPosition;
	_curScriptPosition = _endScriptPosition;
	_separatorPosition = _endScriptPosition;
}

bool Interpreter::execute(const Script& script)
{
	_beginScriptPosition = script.begin();
	_endScriptPosition = script.end();
	_curScriptPosition = _beginScriptPosition;
	_separatorPosition = _beginScriptPosition;

	if (error)
	{
		*error = ScriptError::UNKNOWN_ERROR;
	}

	_opCount = 0;

	if (isStacktraceEnabled)
	{
		_stacktrace << std::setw(40) << std::setfill('=') << '\n';
	}

	try
	{
		if (script.size() > Script::MAX_SCRIPT_SIZE)
		{
			throw ScriptException(ScriptError::SCRIPT_SIZE);
		}

		while (_curScriptPosition < _endScriptPosition)
		{
			if (isStacktraceEnabled)
			{
				dump(_stacktrace);
			}

			++_step;

			bool mustExecuted = std::count(_conditionalExecutionFlags.begin(), _conditionalExecutionFlags.end(), false) == 0;

			//
			// Read instruction
			//
			OpCode opCode;
			std::vector<uint8_t> data;
			if (!script.GetOp(_curScriptPosition, opCode, data))
			{
				throw ScriptException(ScriptError::BAD_OPCODE);
			}

			if (data.size() > Script::MAX_SCRIPT_ELEMENT_SIZE)
			{
				throw ScriptException(ScriptError::PUSH_SIZE);
			}

			// Note how OP_RESERVED does not count towards the opcode limit.
			if (opCode > OpCode::OP_16)
			{
				++_opCount;
				if (_opCount > Script::MAX_OPS_PER_SCRIPT)
				{
					throw ScriptException(ScriptError::OP_COUNT);
				}
			}

//			if (
//				opCode == OpCode::OP_CAT ||
//				opCode == OpCode::OP_SUBSTR ||
//				opCode == OpCode::OP_LEFT ||
//				opCode == OpCode::OP_RIGHT ||
//				opCode == OpCode::OP_INVERT ||
//				opCode == OpCode::OP_AND ||
//				opCode == OpCode::OP_OR ||
//				opCode == OpCode::OP_XOR ||
//				opCode == OpCode::OP_2MUL ||
//				opCode == OpCode::OP_2DIV ||
//				opCode == OpCode::OP_MUL ||
//				opCode == OpCode::OP_DIV ||
//				opCode == OpCode::OP_MOD ||
//				opCode == OpCode::OP_LSHIFT ||
//				opCode == OpCode::OP_RSHIFT
//			)
//			{
//				throw ScriptEvaluationException(ScriptError::DISABLED_OPCODE); // Disabled opcodes.
//			}

			if (OpCode::OP_IF <= opCode && opCode <= OpCode::OP_ENDIF)
			{
				switch (opCode)
				{
					case OpCode::OP_IF:
					case OpCode::OP_NOTIF:
					{	// <expression> if [statements] [else [statements]] endif
						if (!mustExecuted)
						{
							_conditionalExecutionFlags.push_back(false);
							break;
						}

						if (_stack.empty())
						{
							throw ScriptException(ScriptError::UNBALANCED_CONDITIONAL);
						}

						StackFrame stackFrame = _stack.take();
						if (sigversion == SigVersion::WITNESS_V0 && flags.isSet(ScriptVerifyFlags::MINIMALIF))
						{
							if (stackFrame.isData())
							{
								if (
									stackFrame.asData().size() > 1 ||
									(stackFrame.asData().size() == 1 && stackFrame.asData()[0] != 1)
								)
								{
									throw ScriptException(ScriptError::MINIMALIF);
								}
							}
							if (stackFrame.isNumber() && stackFrame.asNumber().getint() > 0xff)
							{
								throw ScriptException(ScriptError::MINIMALIF);
							}
						}

						bool executionFlag = CastToBool(stackFrame);
						if (opCode == OpCode::OP_NOTIF)
						{
							executionFlag = !executionFlag;
						}

						_conditionalExecutionFlags.push_back(executionFlag);
					}   break;

					case OpCode::OP_ELSE:
						if (_conditionalExecutionFlags.empty())
						{
							throw ScriptException(ScriptError::UNBALANCED_CONDITIONAL);
						}
						_conditionalExecutionFlags.back() = !_conditionalExecutionFlags.back();
						break;

					case OpCode::OP_ENDIF:
						if (_conditionalExecutionFlags.empty())
						{
							throw ScriptException(ScriptError::UNBALANCED_CONDITIONAL);
						}
						_conditionalExecutionFlags.pop_back();
						break;

					default: assert(OpCode::OP_IF <= opCode && opCode <= OpCode::OP_ENDIF);
				}
			}
			else if (mustExecuted)
			{
				//
				// Push data as is
				//
				if (opCode < OpCode::OP_PUSHDATA1)
				{
					op_PushData(opCode, std::move(data));
					continue;
				}

				switch (opCode)
				{
					//
					// Push data
					//
					case OpCode::OP_PUSHDATA1:
						op_PushData1(std::move(data));
						break;

					case OpCode::OP_PUSHDATA2:
						op_PushData2(std::move(data));
						break;

					case OpCode::OP_PUSHDATA4:
						op_PushData4(std::move(data));
						break;

					case OpCode::OP_1NEGATE:
					case OpCode::OP_1:
					case OpCode::OP_2:
					case OpCode::OP_3:
					case OpCode::OP_4:
					case OpCode::OP_5:
					case OpCode::OP_6:
					case OpCode::OP_7:
					case OpCode::OP_8:
					case OpCode::OP_9:
					case OpCode::OP_10:
					case OpCode::OP_11:
					case OpCode::OP_12:
					case OpCode::OP_13:
					case OpCode::OP_14:
					case OpCode::OP_15:
					case OpCode::OP_16:
						op_N(opCode);
						break;

					case OpCode::OP_NOP:
						op_Nop();
						break;

					case OpCode::OP_CHECKLOCKTIMEVERIFY:
						op_CheckLockTimeVerify();
						break;

					case OpCode::OP_CHECKSEQUENCEVERIFY:
						op_CheckSequenceVerify();
						break;

					case OpCode::OP_NOP1:
					case OpCode::OP_NOP4:
					case OpCode::OP_NOP5:
					case OpCode::OP_NOP6:
					case OpCode::OP_NOP7:
					case OpCode::OP_NOP8:
					case OpCode::OP_NOP9:
					case OpCode::OP_NOP10:
						op_Noop();
						break;

					case OpCode::OP_VERIFY:
						op_Verify();
						break;

					case OpCode::OP_RETURN:
						op_Return();
						break;

					case OpCode::OP_TOALTSTACK:
						op_ToAltStack();
						break;

					case OpCode::OP_FROMALTSTACK:
						op_FromAltStack();
						break;

					case OpCode::OP_2DROP:
						op_2Drop();
						break;

					case OpCode::OP_2DUP:
						op_2Dup();
						break;

					case OpCode::OP_3DUP:
						op_3Dup();
						break;

					case OpCode::OP_2OVER:
						op_2Over();
						break;

					case OpCode::OP_2ROT:
						op_2Rot();
						break;

					case OpCode::OP_2SWAP:
						op_2Swap();
						break;

					case OpCode::OP_IFDUP:
						op_IfDup();
						break;

					case OpCode::OP_DEPTH:
						op_Depth();
						break;

					case OpCode::OP_DROP:
						op_Drop();
						break;

					case OpCode::OP_DUP:
						op_Dup();
						break;

					case OpCode::OP_NIP:
						op_Nip();
						break;

					case OpCode::OP_OVER:
						op_Over();
						break;

					case OpCode::OP_PICK:
						op_Pick();
						break;

					case OpCode::OP_ROLL:
						op_Roll();
						break;

					case OpCode::OP_ROT:
						op_Rot();
						break;

					case OpCode::OP_SWAP:
						op_Swap();
						break;

					case OpCode::OP_TUCK:
						op_Tuck();
						break;

					case OpCode::OP_SIZE:
						op_Size();
						break;

					case OpCode::OP_EQUAL:
						op_Equal();
						break;

					case OpCode::OP_EQUALVERIFY:
						op_EqualVerify();
						break;

					case OpCode::OP_1ADD:
						op_1Add();
						break;

					case OpCode::OP_1SUB:
						op_1Sub();
						break;

					case OpCode::OP_NEGATE:
						op_Negate();
						break;

					case OpCode::OP_ABS:
						op_Abs();
						break;

					case OpCode::OP_NOT:
						op_Not();
						break;

					case OpCode::OP_0NOTEQUAL:
						op_0NotEqual();
						break;

					case OpCode::OP_ADD:
						op_Add();
						break;

					case OpCode::OP_SUB:
						op_Sub();
						break;

					case OpCode::OP_BOOLAND:
						op_BoolAnd();
						break;

					case OpCode::OP_BOOLOR:
						op_BoolOr();
						break;

					case OpCode::OP_NUMEQUAL:
						op_NumEqual();
						break;

					case OpCode::OP_NUMEQUALVERIFY:
						op_NumEqualVerify();
						break;

					case OpCode::OP_NUMNOTEQUAL:
						op_NumNotEqual();
						break;

					case OpCode::OP_LESSTHAN:
						op_LessThan();
						break;

					case OpCode::OP_GREATERTHAN:
						op_GreaterThan();
						break;

					case OpCode::OP_LESSTHANOREQUAL:
						opLessThanOrEqual();
						break;

					case OpCode::OP_GREATERTHANOREQUAL:
						op_GreaterThanOrEqual();
						break;

					case OpCode::OP_MIN:
						op_Min();
						break;

					case OpCode::OP_MAX:
						op_Max();
						break;

					case OpCode::OP_WITHIN:
						op_Within();
						break;

					case OpCode::OP_RIPEMD160:
						op_RIPEMD();
						break;

					case OpCode::OP_SHA1:
						op_SHA1();
						break;

					case OpCode::OP_SHA256:
						op_SHA256();
						break;

					case OpCode::OP_HASH160:
						op_Hash160();
						break;

					case OpCode::OP_HASH256:
						op_Hash256();
						break;

					case OpCode::OP_CODESEPARATOR:
						op_CodeSeparator();
						break;

					case OpCode::OP_CHECKSIG:
						op_CheckSig();
						break;

					case OpCode::OP_CHECKSIGVERIFY:
						op_CheckSigVerify();
						break;

					case OpCode::OP_CHECKMULTISIG:
						op_CheckMultiSig();
						break;

					case OpCode::OP_CHECKMULTISIGVERIFY:
						op_CheckMultiSigVerify();
						break;


					default: throw ScriptException(ScriptError::BAD_OPCODE);
				}
			}

			// Size limits
			if (_stack.size() + _altStack.size() > Script::MAX_STACK_SIZE)
			{
				throw ScriptException(ScriptError::STACK_SIZE);
			}
		}

		if (!_conditionalExecutionFlags.empty())
		{
			throw ScriptException(ScriptError::UNBALANCED_CONDITIONAL);
		}
	}
	catch (const ScriptException& exception)
	{
		if (error)
		{
			*error = exception.errorCode();
		}
		if (isStacktraceEnabled)
		{
			dump(_stacktrace);

			_stacktrace << "Error: " << std::to_string(exception.errorCode()) << '\n';
			_stacktrace << std::setw(40) << std::setfill('=') << '\n';
		}
		return false;
	}
	catch (const std::exception& exception)
	{
		if (error)
		{
			*error = ScriptError::UNKNOWN_ERROR;
		}
		if (isStacktraceEnabled)
		{
			dump(_stacktrace);

			_stacktrace << "Error: " << "OTHER(" << exception.what() << ')' << '\n';
			_stacktrace << std::setw(40) << std::setfill('=') << '\n';
		}
		return false;
	}

	if (error)
	{
		*error = ScriptError::OK;
	}
	if (isStacktraceEnabled)
	{
		dump(_stacktrace);

		_stacktrace << "No error" << '\n';
		_stacktrace << std::setw(40) << std::setfill('=') << '\n';
	}
	return true;
}

bool Interpreter::CastToBool(const StackFrame& stackFrame)
{
	if (stackFrame.isNumber())
	{
		return stackFrame.asNumber() != 0;
	}
	else if (stackFrame.isData())
	{
		for (auto byte : stackFrame.asData())
		{
			if (byte)
			{
				return true;
			}
		}
	}
	return false;
}

int Interpreter::FindAndDelete(Script& haystack, const Script& needle)
{
    if (needle.empty())
    {
    	return 0;
    }

    auto pc = haystack.cbegin();
    auto pc2 = haystack.cbegin();
    auto end = haystack.cend();

	Script result;
    OpCode opcode;

	int found = 0;

    do
    {
        result.insert(result.end(), pc2, pc);
        while (pc + needle.size() < end && std::equal(needle.begin(), needle.end(), pc))
        {
            pc += needle.size();
            ++found;
        }
        pc2 = pc;
    }
    while (haystack.GetOp(pc, opcode));

    if (found > 0)
    {
        result.insert(result.end(), pc2, end);
	    haystack = std::move(result);
    }

    return found;
}

bool static IsCompressedOrUncompressedPubKey(const ScriptData& pubKey)
{
	if (pubKey.size() < CPubKey::COMPRESSED_PUBLIC_KEY_SIZE)
	{
		//  Non-canonical public key: too short
		return false;
	}
	if (pubKey[0] == 0x04)
	{
		if (pubKey.size() != CPubKey::PUBLIC_KEY_SIZE)
		{
			//  Non-canonical public key: invalid length for uncompressed key
			return false;
		}
	}
	else if (pubKey[0] == 0x02 || pubKey[0] == 0x03)
	{
		if (pubKey.size() != CPubKey::COMPRESSED_PUBLIC_KEY_SIZE)
		{
			//  Non-canonical public key: invalid length for compressed key
			return false;
		}
	}
	else
	{
		//  Non-canonical public key: neither compressed nor uncompressed
		return false;
	}
	return true;
}

bool static IsCompressedPubKey(const ScriptData& pubKey)
{
	if (pubKey.size() != CPubKey::COMPRESSED_PUBLIC_KEY_SIZE)
	{
		//  Non-canonical public key: invalid length for compressed key
		return false;
	}
	if (pubKey[0] != 0x02 && pubKey[0] != 0x03)
	{
		//  Non-canonical public key: invalid prefix for compressed key
		return false;
	}
	return true;
}

/**
 * A canonical signature exists of: <30> <total len> <02> <len R> <R> <02> <len S> <S> <hashtype>
 * Where R and S are not negative (their first byte has its highest bit not set), and not
 * excessively padded (do not start with a 0 byte, unless an otherwise negative number follows,
 * in which case a single 0 byte is necessary and even required).
 *
 * See https://bitcointalk.org/index.php?topic=8392.msg127623#msg127623
 *
 * This function is consensus-critical since BIP66.
 */
bool static IsValidSignatureEncoding(const std::vector<unsigned char> &sig) {
	// Format: 0x30 [total-length] 0x02 [R-length] [R] 0x02 [S-length] [S] [sighash]
	// * total-length: 1-byte length descriptor of everything that follows,
	//   excluding the sighash byte.
	// * R-length: 1-byte length descriptor of the R value that follows.
	// * R: arbitrary-length big-endian encoded R value. It must use the shortest
	//   possible encoding for a positive integer (which means no null bytes at
	//   the start, except a single one when the next byte has its highest bit set).
	// * S-length: 1-byte length descriptor of the S value that follows.
	// * S: arbitrary-length big-endian encoded S value. The same rules apply.
	// * sighash: 1-byte value indicating what data is hashed (not part of the DER
	//   signature)

	// Minimum and maximum size constraints.
	if (sig.size() < 9) return false;
	if (sig.size() > 73) return false;

	// A signature is of type 0x30 (compound).
	if (sig[0] != 0x30) return false;

	// Make sure the length covers the entire signature.
	if (sig[1] != sig.size() - 3) return false;

	// Extract the length of the R element.
	unsigned int lenR = sig[3];

	// Make sure the length of the S element is still inside the signature.
	if (5 + lenR >= sig.size()) return false;

	// Extract the length of the S element.
	unsigned int lenS = sig[5 + lenR];

	// Verify that the length of the signature matches the sum of the length
	// of the elements.
	if ((size_t)(lenR + lenS + 7) != sig.size()) return false;

	// Check whether the R element is an integer.
	if (sig[2] != 0x02) return false;

	// Zero-length integers are not allowed for R.
	if (lenR == 0) return false;

	// Negative numbers are not allowed for R.
	if (sig[4] & 0x80) return false;

	// Null bytes at the start of R are not allowed, unless R would
	// otherwise be interpreted as a negative number.
	if (lenR > 1 && (sig[4] == 0x00) && !(sig[5] & 0x80)) return false;

	// Check whether the S element is an integer.
	if (sig[lenR + 4] != 0x02) return false;

	// Zero-length integers are not allowed for S.
	if (lenS == 0) return false;

	// Negative numbers are not allowed for S.
	if (sig[lenR + 6] & 0x80) return false;

	// Null bytes at the start of S are not allowed, unless S would otherwise be
	// interpreted as a negative number.
	if (lenS > 1 && (sig[lenR + 6] == 0x00) && !(sig[lenR + 7] & 0x80)) return false;

	return true;
}

bool static IsLowDERSignature(const StackFrame &vchSig, ScriptError* serror)
{
	assert(false); // TODO need to implement
//	if (!IsValidSignatureEncoding(vchSig))
//	{
//		throw ScriptEvaluationException(ScriptError::SIG_DER);
//	}
//	// https://bitcoin.stackexchange.com/a/12556:
//	//     Also note that inside transaction signatures, an extra hashtype byte
//	//     follows the actual signature data.
//	std::vector<unsigned char> vchSigCopy(vchSig.begin(), vchSig.begin() + vchSig.size() - 1);
//	// If the S value is above the order of the curve divided by two, its
//	// complement modulo the order could have been used instead, which is
//	// one byte shorter when encoded correctly.
//	if (!CPubKey::CheckLowS(vchSigCopy)) {
//		throw ScriptEvaluationException(ScriptError::SIG_HIGH_S);
//	}
//	return true;
}

/** Signature hash types/flags */
enum
{
	SIGHASH_ALL = 1,
	SIGHASH_NONE = 2,
	SIGHASH_SINGLE = 3,
	SIGHASH_ANYONECANPAY = 0x80,
};

bool static IsDefinedHashtypeSignature(const StackFrame &vchSig)
{
	if (!vchSig.isData())
	{
		return false;
	}
	auto& sigData = vchSig.asData();
	if (sigData.empty())
	{
		return false;
	}
	unsigned char nHashType = sigData.back() & (~(SIGHASH_ANYONECANPAY));
	if (nHashType < SIGHASH_ALL || nHashType > SIGHASH_SINGLE)
	{
		return false;
	}
	return true;
}

bool Interpreter::CheckSignatureEncoding(const StackFrame& vchSig, Flags<ScriptVerifyFlags> flags, ScriptError* serror)
{
	// Empty signature. Not strictly DER encoded, but allowed to provide a
	// compact way to provide an invalid signature for use with CHECK(MULTI)SIG
	if (!vchSig.isData())
	{
		return false;
	}
	auto& sigData = vchSig.asData();
	if (sigData.empty())
	{
		return true;
	}
	if (
		flags.isSetAny(ScriptVerifyFlags::VERIFY_DERSIG, ScriptVerifyFlags::LOW_S, ScriptVerifyFlags::STRICTENC) &&
		!IsValidSignatureEncoding(sigData)
	)
	{
		throw ScriptException(ScriptError::SIG_DER);
	}
	else if (
		flags.isSet(ScriptVerifyFlags::LOW_S) &&
		!IsLowDERSignature(vchSig, serror)
	)
	{
		// error is set
		return false;
	}
	else if (
		flags.isSet(ScriptVerifyFlags::STRICTENC) &&
		!IsDefinedHashtypeSignature(vchSig)
	)
	{
		throw ScriptException(ScriptError::SIG_HASHTYPE);
	}
	return true;
}

bool Interpreter::CheckPubKeyEncoding(
	const ScriptData& pubKey,
	Flags<ScriptVerifyFlags> flags,
	const SigVersion& sigversion,
	ScriptError* serror
)
{
	if (
		flags.isSet(ScriptVerifyFlags::STRICTENC) &&
		!IsCompressedOrUncompressedPubKey(pubKey)
	)
	{
		throw ScriptException(ScriptError::PUBKEYTYPE);
	}

	// Only compressed keys are accepted in segwit
	if (
		flags.isSet(ScriptVerifyFlags::WITNESS_PUBKEYTYPE) &&
		sigversion == SigVersion::WITNESS_V0 &&
		!IsCompressedPubKey(pubKey)
	)
	{
		throw ScriptException(ScriptError::WITNESS_PUBKEYTYPE);
	}
	return true;
}

void Interpreter::ensureStackHasEnoughFrames(size_t need)
{
	if (_stack.size() < need)
	{
		throw ScriptException(ScriptError::INVALID_STACK_OPERATION);
	}
}

void Interpreter::op_PushData(OpCode opCode, ScriptData&& data)
{
	if (flags.isSet(ScriptVerifyFlags::MINIMALDATA))
	{
		if (data.empty())
		{
			// Should have used OP_0.
			if (opCode != OpCode::OP_0)
			{
				throw ScriptException(ScriptError::MINIMALDATA);
			}
		}
		else if (data.size() == 1)
		{
			if (data[0] >= 1 && data[0] <= 16)
			{
				// Should have used OP_1 .. OP_16.
				throw ScriptException(ScriptError::MINIMALDATA);
			}
			if (data[0] == 0x81)
			{
				// Should have used OP_1NEGATE.
				throw ScriptException(ScriptError::MINIMALDATA);
			}
		}
	}
	if (data.size() <= sizeof(int32_t))
	{
		_stack.push(std::forward<StackFrame>(ScriptNum(data, flags.isSet(ScriptVerifyFlags::MINIMALDATA))));
	}
	else
	{
		_stack.push(std::forward<StackFrame>(data));
	}
}

void Interpreter::op_PushData1(ScriptData&& data)
{
	if (flags.isSet(ScriptVerifyFlags::MINIMALDATA))
	{
		if (data.empty())
		{
			// Should have used OP_0.
			throw ScriptException(ScriptError::MINIMALDATA);
		}
		else if (data.size() == 1)
		{
			if (data[0] >= 1 && data[0] <= 16)
			{
				// Should have used OP_1 .. OP_16.
				throw ScriptException(ScriptError::MINIMALDATA);
			}
			if (data[0] == 0x81)
			{
				// Should have used OP_1NEGATE.
				throw ScriptException(ScriptError::MINIMALDATA);
			}
		}
		else if (data.size() < static_cast<size_t>(OpCode::OP_PUSHDATA1))
		{
			// Must have used a direct push (opcode indicating number of bytes pushed + those bytes).
			throw ScriptException(ScriptError::MINIMALDATA);
		}
	}
	if (data.size() <= sizeof(int32_t))
	{
		_stack.push(std::forward<StackFrame>(ScriptNum(data, flags.isSet(ScriptVerifyFlags::MINIMALDATA))));
	}
	else
	{
		_stack.push(std::forward<StackFrame>(data));
	}
}

void Interpreter::op_PushData2(ScriptData&& data)
{
	if (flags.isSet(ScriptVerifyFlags::MINIMALDATA))
	{
		if (data.size() <= 0xff)
		{
			throw ScriptException(ScriptError::MINIMALDATA);
		}
	}
	_stack.push(std::forward<StackFrame>(data));
}

void Interpreter::op_PushData4(ScriptData&& data)
{
	if (flags.isSet(ScriptVerifyFlags::MINIMALDATA))
	{
		if (data.size() <= 0xffff)
		{
			throw ScriptException(ScriptError::MINIMALDATA);
		}
	}
	_stack.push(std::forward<StackFrame>(data));
}

void Interpreter::op_N(OpCode opCode)
{
	// ( -- value)
	ScriptNum x(static_cast<int64_t>(opCode) - static_cast<int64_t>(OpCode::OP_1) + 1);
	_stack.push(std::move(x));
	// The result of these opcodes should always be the minimal way to push the data
	// they push, so no need for a CheckMinimalPush here.
}

void Interpreter::op_Nop()
{
	// Do nothing
}

void Interpreter::op_CheckLockTimeVerify()
{
	if (!flags.isSet(ScriptVerifyFlags::CHECKLOCKTIMEVERIFY))
	{
		// not enabled; treat as a NOP2
		return;
	}

	ensureStackHasEnoughFrames(1);

	// Note that elsewhere numeric opcodes are limited to
	// operands in the range -2**31+1 to 2**31-1, however it is
	// legal for opcodes to produce results exceeding that
	// range. This limitation is implemented by ScriptNum's
	// default 4-byte limit.
	//
	// If we kept to that limit we'd have a year 2038 problem,
	// even though the nLockTime field in transactions
	// themselves is uint32 which only becomes meaningless
	// after the year 2106.
	//
	// Thus as a special case we tell ScriptNum to accept up
	// to 5-byte bignums, which are good until 2**39-1, well
	// beyond the 2**32-1 limit of the nLockTime field itself.
	const ScriptNum nLockTime(_stack.top().asData(), flags.isSet(ScriptVerifyFlags::MINIMALDATA), 5);

	// In the rare event that the argument may be < 0 due to
	// some arithmetic being done first, you can always use
	// 0 MAX CHECKLOCKTIMEVERIFY.
	if (nLockTime < 0)
	{
		throw ScriptException(ScriptError::NEGATIVE_LOCKTIME);
	}

	// Actually compare the specified lock time with the transaction.
	if (!checker.CheckLockTime(nLockTime))
	{
		throw ScriptException(ScriptError::UNSATISFIED_LOCKTIME);
	}
}

void Interpreter::op_CheckSequenceVerify()
{
	if (!flags.isSet(ScriptVerifyFlags::CHECKSEQUENCEVERIFY))
	{
		// not enabled; treat as a NOP3
		return;
	}

	ensureStackHasEnoughFrames(1);

	// nSequence, like nLockTime, is a 32-bit unsigned integer
	// field. See the comment in CHECKLOCKTIMEVERIFY regarding
	// 5-byte numeric operands.
	const ScriptNum nSequence(_stack.top().asData(), flags.isSet(ScriptVerifyFlags::MINIMALDATA), 5);

	// In the rare event that the argument may be < 0 due to
	// some arithmetic being done first, you can always use
	// 0 MAX CHECKSEQUENCEVERIFY.
	if (nSequence < 0)
	{
		throw ScriptException(ScriptError::NEGATIVE_LOCKTIME);
	}

	// To provide for future soft-fork extensibility, if the
	// operand has the disabled lock-time flag set,
	// CHECKSEQUENCEVERIFY behaves as a NOP.
	if ((nSequence & TxIn::SEQUENCE_LOCKTIME_DISABLE_FLAG) != 0)
	{
		return;
	}

	// Compare the specified sequence number with the input.
	if (!checker.CheckSequence(nSequence))
	{
		throw ScriptException(ScriptError::UNSATISFIED_LOCKTIME);
	}
}

void Interpreter::op_Noop()
{
	if (flags.isSet(ScriptVerifyFlags::DISCOURAGE_UPGRADABLE_NOPS))
	{
		throw ScriptException(ScriptError::DISCOURAGE_UPGRADABLE_NOPS);
	}
}

void Interpreter::op_Verify()
{
	// (true -- ) or
	// (false -- false) and return
	ensureStackHasEnoughFrames(1);
	if (!CastToBool(_stack.top()))
	{
		throw ScriptException(ScriptError::VERIFY);
	}

	_stack.pop();
}

void Interpreter::op_Return()
{
	throw ScriptException(ScriptError::OP_RETURN);
}

void Interpreter::op_ToAltStack()
{
	ensureStackHasEnoughFrames(1);
	_altStack.push(_stack.take());
}

void Interpreter::op_FromAltStack()
{
	if (_altStack.empty())
	{
		throw ScriptException(ScriptError::INVALID_ALTSTACK_OPERATION);
	}
	_stack.push(_altStack.take());
}

void Interpreter::op_2Drop()
{
	// (x1 x2 -- )
	ensureStackHasEnoughFrames(2);
	_stack.pop();
	_stack.pop();
}

void Interpreter::op_2Dup()
{
	// (x1 x2 -- x1 x2 x1 x2)
	ensureStackHasEnoughFrames(2);
	auto x2 = _stack.take();
	auto x1 = _stack.top();
	_stack.push(x2);
	_stack.push(std::move(x1));
	_stack.push(std::move(x2));
}

void Interpreter::op_3Dup()
{
	// (x1 x2 x3 -- x1 x2 x3 x1 x2 x3)
	ensureStackHasEnoughFrames(3);
	auto x3 = _stack.take();
	auto x2 = _stack.take();
	auto x1 = _stack.top();
	_stack.push(x2);
	_stack.push(x3);
	_stack.push(std::move(x1));
	_stack.push(std::move(x2));
	_stack.push(std::move(x3));
}

void Interpreter::op_2Over()
{
	// (x1 x2 x3 x4 -- x1 x2 x3 x4 x1 x2)
	ensureStackHasEnoughFrames(4);

	auto x4 = _stack.take();
	auto x3 = _stack.take();
	auto x2 = _stack.take();
	auto x1 = _stack.top();
	_stack.push(x2);
	_stack.push(std::move(x3));
	_stack.push(std::move(x4));
	_stack.push(std::move(x1));
	_stack.push(std::move(x2));
}

void Interpreter::op_2Rot()
{
	// (x1 x2 x3 x4 x5 x6 -- x3 x4 x5 x6 x1 x2)
	ensureStackHasEnoughFrames(6);
	auto x6 = _stack.take();
	auto x5 = _stack.take();
	auto x4 = _stack.take();
	auto x3 = _stack.take();
	auto x2 = _stack.take();
	auto x1 = _stack.take();
	_stack.push(std::move(x3));
	_stack.push(std::move(x4));
	_stack.push(std::move(x5));
	_stack.push(std::move(x6));
	_stack.push(std::move(x1));
	_stack.push(std::move(x2));
}

void Interpreter::op_2Swap()
{
	// (x1 x2 x3 x4 -- x3 x4 x1 x2)
	ensureStackHasEnoughFrames(4);
	auto x4 = _stack.take();
	auto x3 = _stack.take();
	auto x2 = _stack.take();
	auto x1 = _stack.take();
	_stack.push(std::move(x3));
	_stack.push(std::move(x4));
	_stack.push(std::move(x1));
	_stack.push(std::move(x2));
}

void Interpreter::op_IfDup()
{
	// (x | x)    if x==0
	// (x | x x)  if x!=0
	ensureStackHasEnoughFrames(1);
	StackFrame& x = _stack.top();
	if (CastToBool(x))
	{
		_stack.push(x);
	}
}

void Interpreter::op_Depth()
{
	// -- stacksize
	_stack.push(ScriptNum(_stack.size()));
}

void Interpreter::op_Drop()
{
	// (x -- )
	ensureStackHasEnoughFrames(1);
	_stack.pop();
}

void Interpreter::op_Dup()
{
	// (x -- x x)
	ensureStackHasEnoughFrames(1);
	_stack.push(_stack.top());
}

void Interpreter::op_Nip()
{
	// (x1 x2 -- x2)
	ensureStackHasEnoughFrames(2);
	auto x2 = _stack.take();
	_stack.pop();
	_stack.push(std::move(x2));
}

void Interpreter::op_Over()
{
	// (x1 x2 -- x1 x2 x1)
	ensureStackHasEnoughFrames(2);
	auto x2 = _stack.take();
	auto x1 = _stack.top();
	_stack.push(std::move(x2));
	_stack.push(std::move(x1));
}

void Interpreter::op_Pick()
{
	// (xn ... x2 x1 x0 n -- xn ... x2 x1 x0 xn)
	ensureStackHasEnoughFrames(2);
	auto n = 1 + _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA)).getint();
	ensureStackHasEnoughFrames(n);
	Stack tmp;
	while (--n)
	{
		tmp.push(_stack.take());
	}
	StackFrame& xN = _stack.top();
	while (!tmp.empty())
	{
		_stack.push(tmp.take());
	}
	_stack.push(xN);
}

void Interpreter::op_Roll()
{
	// (xn ... x2 x1 x0 n - ... x2 x1 x0 xn)
	ensureStackHasEnoughFrames(2);
	auto n = 1 + _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA)).getint();
	ensureStackHasEnoughFrames(n);
	Stack tmp;
	while (--n)
	{
		tmp.push(_stack.take());
	}
	StackFrame xN = _stack.take();
	while (!tmp.empty())
	{
		_stack.push(tmp.take());
	}
	_stack.push(std::move(xN));
}

void Interpreter::op_Rot()
{
	// (x1 x2 x3 -- x2 x3 x1)
	ensureStackHasEnoughFrames(3);
	auto x3 = _stack.take();
	auto x2 = _stack.take();
	auto x1 = _stack.take();
	_stack.push(std::move(x2));
	_stack.push(std::move(x3));
	_stack.push(std::move(x1));
}

void Interpreter::op_Swap()
{
	// (x1 x2 -- x2 x1)
	ensureStackHasEnoughFrames(2);
	auto x2 = _stack.take();
	auto x1 = _stack.take();
	_stack.push(std::move(x2));
	_stack.push(std::move(x1));
}

void Interpreter::op_Tuck()
{
	// (x1 x2 -- x2 x1 x2)
	ensureStackHasEnoughFrames(2);
	auto x2 = _stack.take();
	auto x1 = _stack.take();
	_stack.push(x2);
	_stack.push(std::move(x1));
	_stack.push(std::move(x2));
}

void Interpreter::op_Size()
{
	// (in -- in size)
	ensureStackHasEnoughFrames(1);
	_stack.push(ScriptNum(_stack.top().asData().size()));
}

void Interpreter::op_Equal()
{
	// (x1 x2 - bool)
	ensureStackHasEnoughFrames(2);
	auto x2 = _stack.take().asData();
	auto x1 = _stack.take().asData();
	_stack.push(ScriptNum(x1 == x2));
}

void Interpreter::op_EqualVerify()
{
	op_Equal();

	// (true -- )
	// (false -- false) and return
	if (!CastToBool(_stack.top()))
	{
		throw ScriptException(ScriptError::EQUALVERIFY);
	}

	_stack.pop();
}

void Interpreter::op_1Add()
{
	// (x -- x+1)
	ensureStackHasEnoughFrames(1);
	ScriptNum x = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(x + 1);
}

void Interpreter::op_1Sub()
{
	// (x -- x-1)
	ensureStackHasEnoughFrames(1);
	ScriptNum x = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(x - 1);
}

void Interpreter::op_Negate()
{
	// (x -- -x)
	ensureStackHasEnoughFrames(1);
	ScriptNum x = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(-x);
}

void Interpreter::op_Abs()
{
	// (±x -- x)
	ensureStackHasEnoughFrames(1);
	ScriptNum x = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(x < 0 ? -x : x);
}

void Interpreter::op_Not()
{
	// (x -- !x)
	ensureStackHasEnoughFrames(1);
	ScriptNum x = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(!x));
}

void Interpreter::op_0NotEqual()
{
	// (x -- !!x)
	ensureStackHasEnoughFrames(1);
	ScriptNum x = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(bool(x)));
}

void Interpreter::op_Add()
{
	// (x1 x2 -- x1+x2)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(x1 + x2);
}

void Interpreter::op_Sub()
{
	// (x1 x2 -- x1-x2)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(x1 - x2);
}

void Interpreter::op_BoolAnd()
{
	// (x1 x2 -- x1&&x2)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(bool(x1) && bool(x2)));
}

void Interpreter::op_BoolOr()
{
	// (x1 x2 -- x1||x2)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(bool(x1) || bool(x2)));
}

void Interpreter::op_NumEqual()
{
	// (x1 x2 -- x1==x2)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(x1 == x2));
}

void Interpreter::op_NumEqualVerify()
{
	op_NumEqual();

	// (true -- )
	// (false -- false) and return
	if (!CastToBool(_stack.top()))
	{
		throw ScriptException(ScriptError::NUMEQUALVERIFY);
	}

	_stack.pop();
}

void Interpreter::op_NumNotEqual()
{
	// (x1 x2 -- x1!=x2)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(x1 != x2));
}

void Interpreter::op_LessThan()
{
	// (x1 x2 -- x1<x2)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(x1 < x2));
}

void Interpreter::op_GreaterThan()
{
	// (x1 x2 -- x1>x2)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(x1 > x2));
}

void Interpreter::opLessThanOrEqual()
{
	// (x1 x2 -- x1<=x2)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(x1 <= x2));
}

void Interpreter::op_GreaterThanOrEqual()
{
	// (x1 x2 -- x1>=x2)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(x1 >= x2));
}

void Interpreter::op_Min()
{
	// (x1 x2 -- min)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(x1 < x2 ? x1 : x2));
}

void Interpreter::op_Max()
{
	// (x1 x2 -- max)
	ensureStackHasEnoughFrames(2);
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(x1 > x2 ? x1 : x2));
}

void Interpreter::op_Within()
{
	// (x min max -- out)
	ensureStackHasEnoughFrames(3);
	ScriptNum x3 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x2 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	ScriptNum x1 = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA));
	_stack.push(ScriptNum(x2 <= x1 && x1 < x3));
}

void Interpreter::op_RIPEMD()
{
	// (in -- hash)
	ensureStackHasEnoughFrames(1);
	ScriptData inData = _stack.take().asData();
	ScriptData outData(CRIPEMD160::OUTPUT_SIZE);
	CRIPEMD160().Write(inData.data(), inData.size()).Finalize(outData.data());
	_stack.push(std::move(outData));
}

void Interpreter::op_SHA1()
{
	// (in -- hash)
	ensureStackHasEnoughFrames(1);
	ScriptData inData = _stack.take().asData();
	ScriptData outData(CSHA1::OUTPUT_SIZE);
	CSHA1().Write(inData.data(), inData.size()).Finalize(outData.data());
	_stack.push(std::move(outData));
}

void Interpreter::op_SHA256()
{
	// (in -- hash)
	ensureStackHasEnoughFrames(1);
	ScriptData inData = _stack.take().asData();
	ScriptData outData(CSHA256::OUTPUT_SIZE);
	CSHA256().Write(inData.data(), inData.size()).Finalize(outData.data());
	_stack.push(std::move(outData));
}

void Interpreter::op_Hash160()
{
	// (in -- hash)
	ensureStackHasEnoughFrames(1);
	ScriptData inData = _stack.take().asData();
	ScriptData outData(CHash160::OUTPUT_SIZE);
	CHash160().Write(inData.data(), inData.size()).Finalize(outData.data());
	_stack.push(std::move(outData));
}

void Interpreter::op_Hash256()
{
	// (in -- hash)
	ensureStackHasEnoughFrames(1);
	ScriptData inData = _stack.take().asData();
	ScriptData outData(CHash256::OUTPUT_SIZE);
	CHash256().Write(inData.data(), inData.size()).Finalize(outData.data());
	_stack.push(std::move(outData));
}

void Interpreter::op_CodeSeparator()
{
	// With ScriptVerifyFlags::CONST_SCRIPTCODE, OP_CODESEPARATOR in non-segwit script is rejected even in an unexecuted branch
	if (sigversion == SigVersion::BASE && flags.isSet(ScriptVerifyFlags::CONST_SCRIPTCODE))
	{
		throw ScriptException(ScriptError::OP_CODESEPARATOR);
	}

	// If ScriptVerifyFlags::CONST_SCRIPTCODE flag is set, use of OP_CODESEPARATOR is rejected in pre-segwit
	// script, even in an unexecuted branch (this is checked above the opcode case statement).

	// Hash starts after the code separator
	_separatorPosition = _curScriptPosition;
}

void Interpreter::op_CheckSig()
{
	// (sig pubkey -- bool)
	ensureStackHasEnoughFrames(2);

	auto pubKey = _stack.take().asData();
	auto sig    = _stack.take().asData();

	// Subset of script starting at the most recent codeseparator
	Script scriptCode(_separatorPosition, _endScriptPosition);

	// Drop the signature in pre-segwit scripts but not segwit scripts
	if (sigversion == SigVersion::BASE)
	{
		int found = FindAndDelete(scriptCode, Script(sig));
		if (found > 0 && flags.isSet(ScriptVerifyFlags::CONST_SCRIPTCODE))
		{
			throw ScriptException(ScriptError::SIG_FINDANDDELETE);
		}
	}

	CheckSignatureEncoding(sig, flags, error);
	CheckPubKeyEncoding(pubKey, flags, sigversion, error);

	bool isValid = checker.CheckSig(sig, pubKey, scriptCode, sigversion);

	if (!isValid && flags.isSet(ScriptVerifyFlags::NULLFAIL) && !sig.empty())
	{
		throw ScriptException(ScriptError::SIG_NULLFAIL);
	}

	_stack.push(ScriptNum(isValid));
}

void Interpreter::op_CheckSigVerify()
{
	op_CheckSig();

	// (true -- )
	// (false -- false) and return
	if (!CastToBool(_stack.top()))
	{
		throw ScriptException(ScriptError::CHECKSIGVERIFY);
	}

	_stack.pop();
}

void Interpreter::op_CheckMultiSig()
{
	// ([sig ...] num_of_signatures [pubkey ...] num_of_pubkeys -- bool)
	ensureStackHasEnoughFrames(1);

	int keysCount = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA)).getint();
	if (keysCount < 0 || keysCount > Script::MAX_PUBKEYS_PER_MULTISIG)
	{
		throw ScriptException(ScriptError::PUBKEY_COUNT);
	}

	_opCount += keysCount;
	if (_opCount > Script::MAX_OPS_PER_SCRIPT)
	{
		throw ScriptException(ScriptError::OP_COUNT);
	}

	std::vector<ScriptData> keys;
	keys.reserve(keysCount);

	ensureStackHasEnoughFrames(keysCount);

	for (auto i = 0; i < keysCount; i++)
	{
		auto key = _stack.take().asData();
		// TODO Check key
		keys.emplace_back(std::move(key));
	}

	int sigsCount = _stack.take().asNumber(flags.isSet(ScriptVerifyFlags::MINIMALDATA)).getint();
	if (sigsCount < 0 || sigsCount > keysCount)
	{
		throw ScriptException(ScriptError::SIG_COUNT);
	}

	_opCount += sigsCount;
	if (_opCount > Script::MAX_OPS_PER_SCRIPT)
	{
		throw ScriptException(ScriptError::OP_COUNT);
	}

	std::vector<ScriptData> sigs;
	sigs.reserve(sigsCount);

	ensureStackHasEnoughFrames(sigsCount);

	for (auto i = 0; i < sigsCount; i++)
	{
		auto sig = _stack.take().asData();
		// TODO Check sig
		sigs.emplace_back(std::move(sig));
	}

	// Subset of script starting at the most recent codeseparator
	Script scriptCode(_separatorPosition, _endScriptPosition);

	// Drop the signature in pre-segwit scripts but not segwit scripts
	if (sigversion == SigVersion::BASE)
	{
		for (auto& sig : sigs)
		{
			int found = FindAndDelete(scriptCode, Script(sig));
			if (found > 0 && flags.isSet(ScriptVerifyFlags::CONST_SCRIPTCODE))
			{
				throw ScriptException(ScriptError::SIG_FINDANDDELETE);
			}
		}
	}

	bool fSuccess = true;
	int sigIndex = 0;
	int keyIndex = 0;
	while (fSuccess && sigsCount > 0)
	{
		auto& sig    = sigs[sigIndex];
		auto& pubKey = keys[keyIndex];

		// Note how this makes the exact order of pubkey/signature evaluation
		// distinguishable by CHECKMULTISIG NOT if the STRICTENC flag is set.
		// See the script_(in)valid tests for details.
		CheckSignatureEncoding(sig, flags, error);
		CheckPubKeyEncoding(pubKey, flags, sigversion, error);

		// Check signature
		bool fOk = checker.CheckSig(sig, pubKey, scriptCode, sigversion);

		if (fOk)
		{
			sigIndex++;
			sigsCount--;
		}
		keyIndex++;
		keysCount--;

		// If there are more signatures left than keys left,
		// then too many signatures have failed. Exit early,
		// without checking any further signatures.
		if (sigsCount > keysCount)
		{
			fSuccess = false;
		}
	}

	// A bug causes CHECKMULTISIG to consume one extra argument
	// whose contents were not checked in any way.
	//
	// Unfortunately this is a potential source of mutability,
	// so optionally verify it is exactly equal to zero prior
	// to removing it from the stack.
	ensureStackHasEnoughFrames(1);
	if (flags.isSet(ScriptVerifyFlags::NULLDUMMY) && !_stack.top().asData().empty())
	{
		throw ScriptException(ScriptError::SIG_NULLDUMMY);
	}
	_stack.pop();

	_stack.push(ScriptNum(fSuccess));
}

void Interpreter::op_CheckMultiSigVerify()
{
	op_CheckMultiSig();

	// (true -- )
	// (false -- false) and return
	if (!CastToBool(_stack.top()))
	{
		throw ScriptException(ScriptError::CHECKMULTISIGVERIFY);
	}

	_stack.pop();
}

