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


// Opcode.cpp

#include "OpCode.hpp"

const std::string& std::to_string(OpCode opCode)
{
	switch (opCode)
	{
			// push value
		case OpCode::OP_0                      : { static const std::string name("0"); return name; }
		case OpCode::OP_PUSHDATA1              : { static const std::string name("OP_PUSHDATA1"); return name; }
		case OpCode::OP_PUSHDATA2              : { static const std::string name("OP_PUSHDATA2"); return name; }
		case OpCode::OP_PUSHDATA4              : { static const std::string name("OP_PUSHDATA4"); return name; }
		case OpCode::OP_1NEGATE                : { static const std::string name("-1"); return name; }
		case OpCode::OP_RESERVED               : { static const std::string name("OP_RESERVED"); return name; }
		case OpCode::OP_1                      : { static const std::string name("1"); return name; }
		case OpCode::OP_2                      : { static const std::string name("2"); return name; }
		case OpCode::OP_3                      : { static const std::string name("3"); return name; }
		case OpCode::OP_4                      : { static const std::string name("4"); return name; }
		case OpCode::OP_5                      : { static const std::string name("5"); return name; }
		case OpCode::OP_6                      : { static const std::string name("6"); return name; }
		case OpCode::OP_7                      : { static const std::string name("7"); return name; }
		case OpCode::OP_8                      : { static const std::string name("8"); return name; }
		case OpCode::OP_9                      : { static const std::string name("9"); return name; }
		case OpCode::OP_10                     : { static const std::string name("10"); return name; }
		case OpCode::OP_11                     : { static const std::string name("11"); return name; }
		case OpCode::OP_12                     : { static const std::string name("12"); return name; }
		case OpCode::OP_13                     : { static const std::string name("13"); return name; }
		case OpCode::OP_14                     : { static const std::string name("14"); return name; }
		case OpCode::OP_15                     : { static const std::string name("15"); return name; }
		case OpCode::OP_16                     : { static const std::string name("16"); return name; }

			// control
		case OpCode::OP_NOP                    : { static const std::string name("OP_NOP"); return name; }
		case OpCode::OP_VER                    : { static const std::string name("OP_VER"); return name; }
		case OpCode::OP_IF                     : { static const std::string name("OP_IF"); return name; }
		case OpCode::OP_NOTIF                  : { static const std::string name("OP_NOTIF"); return name; }
		case OpCode::OP_VERIF                  : { static const std::string name("OP_VERIF"); return name; }
		case OpCode::OP_VERNOTIF               : { static const std::string name("OP_VERNOTIF"); return name; }
		case OpCode::OP_ELSE                   : { static const std::string name("OP_ELSE"); return name; }
		case OpCode::OP_ENDIF                  : { static const std::string name("OP_ENDIF"); return name; }
		case OpCode::OP_VERIFY                 : { static const std::string name("OP_VERIFY"); return name; }
		case OpCode::OP_RETURN                 : { static const std::string name("OP_RETURN"); return name; }

			// stack ops
		case OpCode::OP_TOALTSTACK             : { static const std::string name("OP_TOALTSTACK"); return name; }
		case OpCode::OP_FROMALTSTACK           : { static const std::string name("OP_FROMALTSTACK"); return name; }
		case OpCode::OP_2DROP                  : { static const std::string name("OP_2DROP"); return name; }
		case OpCode::OP_2DUP                   : { static const std::string name("OP_2DUP"); return name; }
		case OpCode::OP_3DUP                   : { static const std::string name("OP_3DUP"); return name; }
		case OpCode::OP_2OVER                  : { static const std::string name("OP_2OVER"); return name; }
		case OpCode::OP_2ROT                   : { static const std::string name("OP_2ROT"); return name; }
		case OpCode::OP_2SWAP                  : { static const std::string name("OP_2SWAP"); return name; }
		case OpCode::OP_IFDUP                  : { static const std::string name("OP_IFDUP"); return name; }
		case OpCode::OP_DEPTH                  : { static const std::string name("OP_DEPTH"); return name; }
		case OpCode::OP_DROP                   : { static const std::string name("OP_DROP"); return name; }
		case OpCode::OP_DUP                    : { static const std::string name("OP_DUP"); return name; }
		case OpCode::OP_NIP                    : { static const std::string name("OP_NIP"); return name; }
		case OpCode::OP_OVER                   : { static const std::string name("OP_OVER"); return name; }
		case OpCode::OP_PICK                   : { static const std::string name("OP_PICK"); return name; }
		case OpCode::OP_ROLL                   : { static const std::string name("OP_ROLL"); return name; }
		case OpCode::OP_ROT                    : { static const std::string name("OP_ROT"); return name; }
		case OpCode::OP_SWAP                   : { static const std::string name("OP_SWAP"); return name; }
		case OpCode::OP_TUCK                   : { static const std::string name("OP_TUCK"); return name; }

			// splice ops
//		case OpCode::OP_CAT                    : { static const std::string name("OP_CAT"); return name; }
//		case OpCode::OP_SUBSTR                 : { static const std::string name("OP_SUBSTR"); return name; }
//		case OpCode::OP_LEFT                   : { static const std::string name("OP_LEFT"); return name; }
//		case OpCode::OP_RIGHT                  : { static const std::string name("OP_RIGHT"); return name; }
		case OpCode::OP_SIZE                   : { static const std::string name("OP_SIZE"); return name; }

			// bit logic
//		case OpCode::OP_INVERT                 : { static const std::string name("OP_INVERT"); return name; }
//		case OpCode::OP_AND                    : { static const std::string name("OP_AND"); return name; }
//		case OpCode::OP_OR                     : { static const std::string name("OP_OR"); return name; }
//		case OpCode::OP_XOR                    : { static const std::string name("OP_XOR"); return name; }
		case OpCode::OP_EQUAL                  : { static const std::string name("OP_EQUAL"); return name; }
		case OpCode::OP_EQUALVERIFY            : { static const std::string name("OP_EQUALVERIFY"); return name; }
		case OpCode::OP_RESERVED1              : { static const std::string name("OP_RESERVED1"); return name; }
		case OpCode::OP_RESERVED2              : { static const std::string name("OP_RESERVED2"); return name; }

			// numeric
		case OpCode::OP_1ADD                   : { static const std::string name("OP_1ADD"); return name; }
		case OpCode::OP_1SUB                   : { static const std::string name("OP_1SUB"); return name; }
//		case OpCode::OP_2MUL                   : { static const std::string name("OP_2MUL"); return name; }
//		case OpCode::OP_2DIV                   : { static const std::string name("OP_2DIV"); return name; }
		case OpCode::OP_NEGATE                 : { static const std::string name("OP_NEGATE"); return name; }
		case OpCode::OP_ABS                    : { static const std::string name("OP_ABS"); return name; }
		case OpCode::OP_NOT                    : { static const std::string name("OP_NOT"); return name; }
		case OpCode::OP_0NOTEQUAL              : { static const std::string name("OP_0NOTEQUAL"); return name; }
		case OpCode::OP_ADD                    : { static const std::string name("OP_ADD"); return name; }
		case OpCode::OP_SUB                    : { static const std::string name("OP_SUB"); return name; }
//		case OpCode::OP_MUL                    : { static const std::string name("OP_MUL"); return name; }
//		case OpCode::OP_DIV                    : { static const std::string name("OP_DIV"); return name; }
//		case OpCode::OP_MOD                    : { static const std::string name("OP_MOD"); return name; }
//		case OpCode::OP_LSHIFT                 : { static const std::string name("OP_LSHIFT"); return name; }
//		case OpCode::OP_RSHIFT                 : { static const std::string name("OP_RSHIFT"); return name; }
		case OpCode::OP_BOOLAND                : { static const std::string name("OP_BOOLAND"); return name; }
		case OpCode::OP_BOOLOR                 : { static const std::string name("OP_BOOLOR"); return name; }
		case OpCode::OP_NUMEQUAL               : { static const std::string name("OP_NUMEQUAL"); return name; }
		case OpCode::OP_NUMEQUALVERIFY         : { static const std::string name("OP_NUMEQUALVERIFY"); return name; }
		case OpCode::OP_NUMNOTEQUAL            : { static const std::string name("OP_NUMNOTEQUAL"); return name; }
		case OpCode::OP_LESSTHAN               : { static const std::string name("OP_LESSTHAN"); return name; }
		case OpCode::OP_GREATERTHAN            : { static const std::string name("OP_GREATERTHAN"); return name; }
		case OpCode::OP_LESSTHANOREQUAL        : { static const std::string name("OP_LESSTHANOREQUAL"); return name; }
		case OpCode::OP_GREATERTHANOREQUAL     : { static const std::string name("OP_GREATERTHANOREQUAL"); return name; }
		case OpCode::OP_MIN                    : { static const std::string name("OP_MIN"); return name; }
		case OpCode::OP_MAX                    : { static const std::string name("OP_MAX"); return name; }
		case OpCode::OP_WITHIN                 : { static const std::string name("OP_WITHIN"); return name; }

			// crypto
		case OpCode::OP_RIPEMD160              : { static const std::string name("OP_RIPEMD160"); return name; }
		case OpCode::OP_SHA1                   : { static const std::string name("OP_SHA1"); return name; }
		case OpCode::OP_SHA256                 : { static const std::string name("OP_SHA256"); return name; }
		case OpCode::OP_HASH160                : { static const std::string name("OP_HASH160"); return name; }
		case OpCode::OP_HASH256                : { static const std::string name("OP_HASH256"); return name; }
		case OpCode::OP_CODESEPARATOR          : { static const std::string name("OP_CODESEPARATOR"); return name; }
		case OpCode::OP_CHECKSIG               : { static const std::string name("OP_CHECKSIG"); return name; }
		case OpCode::OP_CHECKSIGVERIFY         : { static const std::string name("OP_CHECKSIGVERIFY"); return name; }
		case OpCode::OP_CHECKMULTISIG          : { static const std::string name("OP_CHECKMULTISIG"); return name; }
		case OpCode::OP_CHECKMULTISIGVERIFY    : { static const std::string name("OP_CHECKMULTISIGVERIFY"); return name; }

			// expansion
		case OpCode::OP_NOP1                   : { static const std::string name("OP_NOP1"); return name; }
		case OpCode::OP_CHECKLOCKTIMEVERIFY    : { static const std::string name("OP_CHECKLOCKTIMEVERIFY"); return name; }
		case OpCode::OP_CHECKSEQUENCEVERIFY    : { static const std::string name("OP_CHECKSEQUENCEVERIFY"); return name; }
		case OpCode::OP_NOP4                   : { static const std::string name("OP_NOP4"); return name; }
		case OpCode::OP_NOP5                   : { static const std::string name("OP_NOP5"); return name; }
		case OpCode::OP_NOP6                   : { static const std::string name("OP_NOP6"); return name; }
		case OpCode::OP_NOP7                   : { static const std::string name("OP_NOP7"); return name; }
		case OpCode::OP_NOP8                   : { static const std::string name("OP_NOP8"); return name; }
		case OpCode::OP_NOP9                   : { static const std::string name("OP_NOP9"); return name; }
		case OpCode::OP_NOP10                  : { static const std::string name("OP_NOP10"); return name; }

		case OpCode::OP_INVALIDOPCODE          : { static const std::string name("OP_INVALIDOPCODE"); return name; }

		default                                :
		{
			if (OpCode::OP_0 < opCode && opCode < OpCode::OP_PUSHDATA1)
			{
				static const std::string name("OP__short_data"); return name;
			}
			else if (opCode <= OpCode::OP_NOP10)
			{
				static const std::string name("OP__deprecated"); return name;
			}
			else
			{
				static const std::string name("OP__invalid"); return name;
			}
		}
	}
}
