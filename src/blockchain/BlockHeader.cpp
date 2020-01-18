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
//


// BlockHeader.cpp

#include <serialization/SObj.hpp>
#include <other/HashStreams.hpp>
#include "BlockHeader.hpp"
#include <iomanip>

void BlockHeader::Serialize(std::ostream& os) const
{
	::SerializeList(os,
		_version,
		_prev,
		_merkle,
		_timestamp,
		_bits,
		_nonce,
		_chain
	);
}

void BlockHeader::Unserialize(std::istream& is)
{
	::UnserializeList(is,
		_version,
		_prev,
		_merkle,
		_timestamp,
		_bits,
		_nonce,
		_chain
	);
}

SVal BlockHeader::toSVal() const
{
	SObj obj;
	obj.emplace("version", _version);
	obj.emplace("prev", _prev.toSVal());
	obj.emplace("merkle", _merkle.toSVal());

	time_t ts = _timestamp;
	std::ostringstream oss; oss << std::put_time(std::localtime(&ts), "%y-%m-%d %H:%M:%S");
	obj.emplace("timestamp", oss.str());

//	obj.emplace("timestamp", _timestamp);
	obj.emplace("bits", _bits);
	obj.emplace("nonce", _nonce);
	obj.emplace("chain", _chain);
	if (inChain())
	{
		obj.emplace("height", _height);
	}
	return obj;
}

const uint256& BlockHeader::hash() const
{
	if (!_validHash)
	{
		Hash256Stream hs;
		BlockHeader::Serialize(hs);
		_hash = hs.hash();
		_validHash = true;
	}
	return _hash;
}
