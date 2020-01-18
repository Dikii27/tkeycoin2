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


// SignatureChecker.cpp

#include <crypto/keys/CPubKey.hpp>
#include <types/Flags.hpp>
#include <other/HashWriter.hpp>
#include "SignatureChecker.hpp"
#include "SignatureHashType.hpp"

[[nodiscard]]
bool BaseSignatureChecker::CheckSig(
	const std::vector<uint8_t>& sigData,
	const std::vector<uint8_t>& pubKeyData,
	const Script& script,
	SigVersion sigversion
) const
{
	CPubKey pubkey(pubKeyData);
	if (!pubkey.IsValid())
	{
		return false;
	}

	if (sigData.empty())
	{
		return false;
	}

	return pubkey.Verify(_hash, sigData);
}
