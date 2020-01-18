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


// SignatureChecker.hpp

#pragma once

#include <cstdint>
#include <vector>
#include <script/Script.hpp>

enum class SigVersion
{
	BASE = 0,
	WITNESS_V0 = 1,
};

class BaseSignatureChecker
{
//public:
//	SignatureChecker() = delete; // Default-constructor
//	SignatureChecker(SignatureChecker&&) noexcept = delete; // Move-constructor
//	SignatureChecker(const SignatureChecker&) = delete; // Copy-constructor
//	virtual ~SignatureChecker() override = default; // Destructor
//	SignatureChecker& operator=(SignatureChecker&&) noexcept = delete; // Move-assignment
//	SignatureChecker& operator=(SignatureChecker const&) = delete; // Copy-assignment
//
//	SignatureChecker()
//	{}
private:
	friend class OpCodeExecution_OP_CHECKSIG_Test;
	uint256 _hash;
	void setHash(uint256 hash)
	{
		_hash = hash;
	}

public:
	virtual ~BaseSignatureChecker() = default;

	[[nodiscard]]
	virtual bool CheckSig(
		const std::vector<uint8_t>& sigData,
		const std::vector<uint8_t>& pubKeyData,
		const Script& script,
		SigVersion sigversion
	) const;

	[[nodiscard]]
	virtual bool CheckLockTime(const ScriptNum& nLockTime) const
	{
		return false;
	}

	[[nodiscard]]
	virtual bool CheckSequence(const ScriptNum& nSequence) const
	{
		return false;
	}
};
