//  Copyright (c) 2017-2019 Tkeycoin Dao. All rights reserved.
//  Copyright (c) 2019-2020 TKEY DMCC LLC & Tkeycoin Dao. All rights reserved.
//  Website: www.tkeycoin.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//


// RollbackStackAndRestoreContext.hpp


#pragma once

#include <exception>
#include "ThreadPool.hpp"

class RollbackStackAndRestoreContext final: public std::exception
{
private:
	ucontext_t* _context;

public:
	RollbackStackAndRestoreContext() = delete;
	RollbackStackAndRestoreContext(ucontext_t* context) noexcept
	: _context(context)
	{
	};
	~RollbackStackAndRestoreContext() noexcept override
	{
		ThreadPool::continueContext(_context);
	}

	// Non-copyable
	RollbackStackAndRestoreContext(const RollbackStackAndRestoreContext&) = delete; // Copy-constructor
	RollbackStackAndRestoreContext& operator=(const RollbackStackAndRestoreContext&) = delete; // Copy-assignment

	// Non-moveable
	RollbackStackAndRestoreContext(RollbackStackAndRestoreContext&&) noexcept = default; // Move-constructor
	RollbackStackAndRestoreContext& operator=(RollbackStackAndRestoreContext&&) noexcept = delete; // Move-assignment

	const char* what() const noexcept override
	{
		return "Rollback current stack and restore parent";
	}
};
