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


// ScriptVerifyFlags.hpp

#pragma once

/** Script verification flags.
 *
 *  All flags are intended to be soft forks: the set of acceptable scripts under
 *  flags (A | B) is a subset of the acceptable scripts under flag (A).
 */
enum class ScriptVerifyFlags
{
	NONE      = 0,

	// Evaluate P2SH subscripts (BIP16).
	P2SH      = (1U << 0),

	// Passing a non-strict-DER signature or one with undefined hashtype to a checksig operation causes script failure.
	// Evaluating a pubkey that is not (0x04 + 64 bytes) or (0x02 or 0x03 + 32 bytes) by checksig causes script failure.
	// (not used or intended as a consensus rule).
	STRICTENC = (1U << 1),

	// Passing a non-strict-DER signature to a checksig operation causes script failure (BIP62 rule 1)
	VERIFY_DERSIG    = (1U << 2),

	// Passing a non-strict-DER signature or one with S > order/2 to a checksig operation causes script failure
	// (BIP62 rule 5).
	LOW_S     = (1U << 3),

	// verify dummy stack item consumed by CHECKMULTISIG is of zero-length (BIP62 rule 7).
	NULLDUMMY = (1U << 4),

	// Using a non-push operator in the scriptSig causes script failure (BIP62 rule 2).
	SIGPUSHONLY = (1U << 5),

	// Require minimal encodings for all push operations (OP_0... OP_16, OP_1NEGATE where possible, direct
	// pushes up to 75 bytes, OP_PUSHDATA up to 255 bytes, OP_PUSHDATA2 for anything larger). Evaluating
	// any other push causes the script to fail (BIP62 rule 3).
	// In addition, whenever a stack element is interpreted as a number, it must be of minimal length (BIP62 rule 4).
	MINIMALDATA = (1U << 6),

	// Discourage use of NOPs reserved for upgrades (NOP1-10)
	//
	// Provided so that nodes can avoid accepting or mining transactions
	// containing executed NOP's whose meaning may change after a soft-fork,
	// thus rendering the script invalid; with this flag set executing
	// discouraged NOPs fails the script. This verification flag will never be
	// a mandatory flag applied to scripts in a block. NOPs that are not
	// executed, e.g.  within an unexecuted IF ENDIF block, are *not* rejected.
	// NOPs that have associated forks to give them new meaning (CLTV, CSV)
	// are not subject to this rule.
	DISCOURAGE_UPGRADABLE_NOPS  = (1U << 7),

	// Require that only a single stack element remains after evaluation. This changes the success criterion from
	// "At least one stack element must remain, and when interpreted as a boolean, it must be true" to
	// "Exactly one stack element must remain, and when interpreted as a boolean, it must be true".
	// (BIP62 rule 6)
	// Note: CLEANSTACK should never be used without P2SH or WITNESS.
	CLEANSTACK = (1U << 8),

	// Verify CHECKLOCKTIMEVERIFY
	//
	// See BIP65 for details.
	CHECKLOCKTIMEVERIFY = (1U << 9),

	// support CHECKSEQUENCEVERIFY opcode
	//
	// See BIP112 for details
	CHECKSEQUENCEVERIFY = (1U << 10),

	// Support segregated witness
	//
	WITNESS = (1U << 11),

	// Making v1-v16 witness program non-standard
	//
	DISCOURAGE_UPGRADABLE_WITNESS_PROGRAM = (1U << 12),

	// Segwit script only: Require the argument of OP_IF/NOTIF to be exactly 0x01 or empty vector
	//
	MINIMALIF = (1U << 13),

	// Signature(s) must be empty vector if a CHECK(MULTI)SIG operation failed
	//
	NULLFAIL = (1U << 14),

	// Public keys in segregated witness scripts must be compressed
	//
	WITNESS_PUBKEYTYPE = (1U << 15),

	// Making OP_CODESEPARATOR and FindAndDelete fail any non-segwit scripts
	//
	CONST_SCRIPTCODE = (1U << 16),
};
