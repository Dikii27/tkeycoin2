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


// Interpreter_test.cpp

#include "Interpreter.hpp"

#include <gtest/gtest.h>
#include <util/Hex.hpp>
#include <crypto/keys/CKey.hpp>
#include <support/Random.hpp>

class OpCodeExecution : public ::testing::Test
{
protected:
	void SetUp() override
	{
		itpr = std::make_shared<Interpreter>(
			Flags<ScriptVerifyFlags>{},
			checker,
			version,
			&err
		);
		itpr_trace = std::make_shared<Interpreter>(
			Flags<ScriptVerifyFlags>{},
			checker,
			version,
			&err,
			std::cout
		);
	}
//	void TearDown() override
//	{
//	}

	BaseSignatureChecker checker;
	SigVersion version = SigVersion::BASE;
	ScriptError err;

	std::shared_ptr<Interpreter> itpr;
	std::shared_ptr<Interpreter> itpr_trace;
};


TEST_F(OpCodeExecution, EmptyScript)
{
	Script script;
	EXPECT_TRUE(itpr->resetAndExecute(script))
		<< "Empty script must be evaluated positively";
}

TEST_F(OpCodeExecution, OP_NOP)
{
	Script script;
	script << OpCode::OP_NOP;

	EXPECT_TRUE(itpr->resetAndExecute(script))
		<< "OP_NOP do nothing and must be evaluated positively";
	if (err != ScriptError::OK)
	{
		itpr_trace->resetAndExecute(script);
	}
	EXPECT_TRUE(itpr->stackData().empty())
		<< "OP_NOP mustn't change stack";
}

TEST_F(OpCodeExecution, OP_VERIFY)
{
	{
		Script script;
		script << 12 << 1 << OpCode::OP_VERIFY << 21;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "OP_VERIFY(true) mustn't stop evaluation";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 2 && itpr->stackData()[0] == ScriptNum(21))
			<< "OP_VERIFY(true) must be push true out of stack and does not touch other frames";
	}
	{
		Script script;
		script << 12 << 0 << OpCode::OP_VERIFY << 21;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "OP_VERIFY(false) must stop evaluation";
		if (err != ScriptError::VERIFY)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_EQ(err, ScriptError::VERIFY)
			<< "OP_VERIFY(false) must stop evaluation with error VERIFY";
		EXPECT_TRUE(itpr->stackData().size() == 2 && itpr->stackData()[0] == ScriptNum(0))
			<< "OP_VERIFY(false) must not change stack";
	}
}

TEST_F(OpCodeExecution, OP_RETURN)
{
	Script script;
	script << OpCode::OP_RETURN;

	EXPECT_FALSE(itpr->resetAndExecute(script))
		<< "OP_RETURN do nothing and must be executed negativelty";
	if (err != ScriptError::OP_RETURN)
	{
		itpr_trace->resetAndExecute(script);
	}
	EXPECT_EQ(err, ScriptError::OP_RETURN)
		<< "OP_RETURN must interrupt execurion with error OP_RETURN";
	EXPECT_TRUE(itpr->stackData().empty())
		<< "OP_RETURN mustn't change stack";
}

TEST_F(OpCodeExecution, OP_TOALTSTACK)
{
	{
		Script script;
		script << 12 << 21 << OpCode::OP_TOALTSTACK;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "OP_TOALTSTACK must executet positively if main stack not empty";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::OK)
			<< "OP_TOALTSTACK must executet positively if main stack not empty";
		EXPECT_TRUE(itpr->stackData().size() == 1 && itpr->stackData()[0] == ScriptNum(12))
			<< "OP_TOALTSTACK must move only one frame from main stack";
		EXPECT_TRUE(itpr->stackData(true).size() == 1 && itpr->stackData(true)[0] == ScriptNum(21))
			<< "OP_TOALTSTACK must move only one frame from main stack";
	}
	{
		Script script;
		script << OpCode::OP_TOALTSTACK;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "OP_TOALTSTACK must executet negatively if main stack is empty";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "OP_TOALTSTACK must interrupt execurion with error INVALID_STACK_OPERATION if main stack is empty";
	}
}

TEST_F(OpCodeExecution, OP_FROMALTSTACK)
{
	{
		Script script;
		script << 12 << 21 << OpCode::OP_TOALTSTACK << OpCode::OP_TOALTSTACK << OpCode::OP_FROMALTSTACK;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "OP_FROMALTSTACK must executed positively if alternative stack is not empty";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData(true).size() == 1 && itpr->stackData(true)[0] == ScriptNum(21))
			<< "OP_FROMALTSTACK must move only one frame from alternative stack";
		EXPECT_TRUE(itpr->stackData().size() == 1 && itpr->stackData()[0] == ScriptNum(12))
			<< "OP_FROMALTSTACK must move only one frame from to main stack";
	}
	{
		Script script;
		script << OpCode::OP_FROMALTSTACK;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "OP_FROMALTSTACK must executed negatively if alternative stack is empty";
		if (err != ScriptError::INVALID_ALTSTACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_ALTSTACK_OPERATION)
			<< "OP_FROMALTSTACK must interrupt execution with error INVALID_ALTSTACK_OPERATION if alternative stack is empty";
	}
}

TEST_F(OpCodeExecution, OP_2DROP)
{
	{
		Script script;
		script << 1 << 2 << 3 << 4 << OpCode::OP_2DROP;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 2
			&& itpr->stackData()[0] == ScriptNum(2)
			&& itpr->stackData()[1] == ScriptNum(1)
		) << "Exactly two frames must be removed from the stack";
	}
	{
		Script script;
		script << 1 << OpCode::OP_2DROP;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_2DUP)
{
	{
		Script script;
		script << 1 << 2 << OpCode::OP_2DUP;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 4
			&& itpr->stackData()[0] == ScriptNum(2)
			&& itpr->stackData()[1] == ScriptNum(1)
			&& itpr->stackData()[2] == ScriptNum(2)
			&& itpr->stackData()[3] == ScriptNum(1)
		) << "Exactly two frames must be duplicated onto the stack";
	}
	{
		Script script;
		script << 1 << OpCode::OP_2DUP;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_3DUP)
{
	{
		Script script;
		script << 1 << 2 << 3 << OpCode::OP_3DUP;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 6
			&& itpr->stackData()[0] == ScriptNum(3)
			&& itpr->stackData()[1] == ScriptNum(2)
			&& itpr->stackData()[2] == ScriptNum(1)
			&& itpr->stackData()[3] == ScriptNum(3)
			&& itpr->stackData()[4] == ScriptNum(2)
			&& itpr->stackData()[5] == ScriptNum(1)
		) << "Exactly three frames must be removed from the stack";
	}
	{
		Script script;
		script << 1 << 2 << OpCode::OP_3DUP;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_2OVER)
{
	{
		Script script;
		script << 1 << 2 << 3 << 4 << OpCode::OP_2OVER;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 6
			&& itpr->stackData()[0] == ScriptNum(2)
			&& itpr->stackData()[1] == ScriptNum(1)
			&& itpr->stackData()[2] == ScriptNum(4)
			&& itpr->stackData()[3] == ScriptNum(3)
			&& itpr->stackData()[4] == ScriptNum(2)
			&& itpr->stackData()[5] == ScriptNum(1)
		) << "Exactly two frames of second pair must be copied onto the stack";
	}
	{
		Script script;
		script << 1 << 2 << 3 << OpCode::OP_2OVER;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_2ROT)
{
	{
		Script script;
		script << 1 << 2 << 3 << 4 << 5 << 6 << OpCode::OP_2ROT;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 6
			&& itpr->stackData()[0] == ScriptNum(2)
			&& itpr->stackData()[1] == ScriptNum(1)
			&& itpr->stackData()[2] == ScriptNum(6)
			&& itpr->stackData()[3] == ScriptNum(5)
			&& itpr->stackData()[4] == ScriptNum(4)
			&& itpr->stackData()[5] == ScriptNum(3)
		) << "Exactly two frames of third pair must be move onto the stack";
	}
	{
		Script script;
		script << 1 << 2 << 3 << 4 << 5 << OpCode::OP_2ROT;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_2SWAP)
{
	{
		Script script;
		script << 1 << 2 << 3 << 4 << OpCode::OP_2SWAP;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 4
			&& itpr->stackData()[0] == ScriptNum(2)
			&& itpr->stackData()[1] == ScriptNum(1)
			&& itpr->stackData()[2] == ScriptNum(4)
			&& itpr->stackData()[3] == ScriptNum(3)
		) << "Exactly two frames of second pair must be move onto the stack";
	}
	{
		Script script;
		script << 1 << 2 << 3 << OpCode::OP_2SWAP;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_IFDUP)
{
	{
		Script script;
		script << 1 << 2 << 3 << OpCode::OP_IFDUP;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 4
			&& itpr->stackData()[0] == ScriptNum(3)
			&& itpr->stackData()[1] == ScriptNum(3)
			&& itpr->stackData()[2] == ScriptNum(2)
			&& itpr->stackData()[3] == ScriptNum(1)
		) << "Exactly one frames must be duplicated on the stack if it casts to boolean as true";
	}
	{
		Script script;
		script << 2 << 1 << 0 << OpCode::OP_IFDUP;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 3
			&& itpr->stackData()[0] == ScriptNum(0)
			&& itpr->stackData()[1] == ScriptNum(1)
			&& itpr->stackData()[2] == ScriptNum(2)
		) << "Stack must not be changed if upper frame casts to boolean as false";
	}
	{
		Script script;
		script << OpCode::OP_IFDUP;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_DEPTH)
{
	{
		Script script;
		script << 1 << 2 << 3 << OpCode::OP_DEPTH;

		EXPECT_TRUE(itpr->resetAndExecute(script)) << "Mustn't be interrupted";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 4
			&& itpr->stackData()[0] == ScriptNum(3)
			&& itpr->stackData()[1] == ScriptNum(3)
			&& itpr->stackData()[2] == ScriptNum(2)
			&& itpr->stackData()[3] == ScriptNum(1)
		) << "Exactly one frames must be push on the stack, with value of stack size";
	}
	{
		Script script;
		script << OpCode::OP_DEPTH;

		EXPECT_TRUE(itpr->resetAndExecute(script)) << "Mustn't be interrupted";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(0)
		) << "Exactly one frames must be push on the stack, with value of stack size";
	}
}

TEST_F(OpCodeExecution, OP_DROP)
{
	{
		Script script;
		script << 11 << 22 << OpCode::OP_DROP;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(11)
		) << "Exactly one frames must be removed from the stack";
	}
	{
		Script script;
		script << 50 << OpCode::OP_DROP;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().empty())
			<< "Exactly one frames must be removed from the stack";
	}
	{
		Script script;
		script << OpCode::OP_DROP;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
						<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_DUP)
{
	{
		Script script;
		script << 100 << OpCode::OP_DUP;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 2
			&& itpr->stackData()[0] == ScriptNum(100)
			&& itpr->stackData()[1] == ScriptNum(100)
		) << "Exactly one frames must be removed from the stack";
	}
	{
		Script script;
		script << OpCode::OP_DUP;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_NIP)
{
	{
		Script script;
		script << 1 << 2 << OpCode::OP_NIP;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(2)
		) << "Only second frame must be removed from the stack";
	}
	{
		Script script;
		script << 1 << OpCode::OP_NIP;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_OVER)
{
	{
		Script script;
		script << 1 << 2 << OpCode::OP_OVER;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 3
			&& itpr->stackData()[0] == ScriptNum(1)
			&& itpr->stackData()[1] == ScriptNum(2)
			&& itpr->stackData()[2] == ScriptNum(1)
		) << "Only second frame must be copied onto the stack";
	}
	{
		Script script;
		script << 1 << OpCode::OP_OVER;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_PICK)
{
	{
		Script script;
		script << 104 << 103 << 102 << 101 << 100 << 2 << OpCode::OP_PICK;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 6
			&& itpr->stackData()[0] == ScriptNum(102)
			&& itpr->stackData()[1] == ScriptNum(100)
			&& itpr->stackData()[2] == ScriptNum(101)
			&& itpr->stackData()[3] == ScriptNum(102)
			&& itpr->stackData()[4] == ScriptNum(103)
			&& itpr->stackData()[5] == ScriptNum(104)
		) << "Only specified frame must be copied onto the stack";
	}
	{
		Script script;
		script << 104 << 103 << 102 << 101 << 100 << 5 << OpCode::OP_PICK;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_ROLL)
{
	{
		Script script;
		script << 104 << 103 << 102 << 101 << 100 << 2 << OpCode::OP_ROLL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 5
			&& itpr->stackData()[0] == ScriptNum(102)
			&& itpr->stackData()[1] == ScriptNum(100)
			&& itpr->stackData()[2] == ScriptNum(101)
			&& itpr->stackData()[3] == ScriptNum(103)
			&& itpr->stackData()[4] == ScriptNum(104)
		) << "Only specified frame must be moved onto the stack";
	}
	{
		Script script;
		script << 104 << 103 << 102 << 101 << 100 << 5 << OpCode::OP_ROLL;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_ROT)
{
	{
		Script script;
		script << 1 << 2 << 3 << OpCode::OP_ROT;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 3
			&& itpr->stackData()[0] == ScriptNum(1)
			&& itpr->stackData()[1] == ScriptNum(3)
			&& itpr->stackData()[2] == ScriptNum(2)
		) << "Only third frame must be moved onto the stack";
	}
	{
		Script script;
		script << 1 << 2 << OpCode::OP_ROT;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_SWAP)
{
	{
		Script script;
		script << 11 << 22 << OpCode::OP_SWAP;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 2
			&& itpr->stackData()[0] == ScriptNum(11)
			&& itpr->stackData()[1] == ScriptNum(22)
		) << "Only second frame must be moved onto the stack";
	}
	{
		Script script;
		script << 1 << OpCode::OP_SWAP;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_TUCK)
{
	{
		Script script;
		script << 1 << 2 << 3 << OpCode::OP_TUCK;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 4
			&& itpr->stackData()[0] == ScriptNum(3)
			&& itpr->stackData()[1] == ScriptNum(2)
			&& itpr->stackData()[2] == ScriptNum(3)
			&& itpr->stackData()[3] == ScriptNum(1)
		) << "Copy of first frame must be inserted under second frame of the stack";
	}
	{
		Script script;
		script << 1 << OpCode::OP_TUCK;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_SIZE)
{
	{
		Script script;
		script << ScriptData() << OpCode::OP_SIZE;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 2
			&& itpr->stackData()[0] == ScriptNum(0)
			&& itpr->stackData()[1] == ScriptData()
		) << "Size of first frame must be inserted on the stack";
	}
	{
		Script script;
		script << ScriptData({1,2,3,4,5,6,7,8,9}) << OpCode::OP_SIZE;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 2
			&& itpr->stackData()[0] == ScriptNum(9)
			&& itpr->stackData()[1] == ScriptData({1,2,3,4,5,6,7,8,9})
		) << "Size of first frame must be inserted on the stack";
	}
	{
		Script script;
		script << 1 << OpCode::OP_SIZE;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 2
			&& itpr->stackData()[0] == ScriptNum(1)
			&& itpr->stackData()[1].asNumber() == ScriptNum(1)
		) << "Size of first frame must be inserted on the stack";
	}
	{
		Script script;
		script << OpCode::OP_SIZE;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_EQUAL)
{
	{
		Script script;
		script << 123 << 123 << OpCode::OP_EQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(1)
		) << "Replace pair of frames of stack by '1' if them are bitwise equal, and by '0' else";
	}
	{
		Script script;
		script << 321 << 123 << OpCode::OP_EQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
            && itpr->stackData()[0] == ScriptNum(0)
		) << "Replace pair of frames of stack by '1' if them are bitwise equal, and by '0' else";
	}
	{
		Script script;
		script << ScriptData({1,2,3,4,5,6,7,8,9}) << ScriptData({1,2,3,4,5,6,7,8,9}) << OpCode::OP_EQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			 && itpr->stackData()[0] == ScriptNum(1)
		) << "Replace pair of frames of stack by '1' if them are bitwise equal, and by '0' else";
	}
	{
		Script script;
		script << ScriptData({1,2,3,4,99,6,7,8,9}) << ScriptData({1,2,3,4,5,6,7,8,9}) << OpCode::OP_EQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(0)
		) << "Replace pair of frames of stack by '1' if them are bitwise equal, and by '0' else";
	}
	{
		Script script;
		script << 1 << OpCode::OP_EQUAL;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_EQUALVERIFY)
{
	{
		Script script;
		script << 123 << 123 << OpCode::OP_EQUALVERIFY;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is bitwise equal upper two frames";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().empty())
			<< "Replace pair of frames from if them are bitwise equal, and interrupt otherwise";
	}
	{
		Script script;
		script << 123 << 321 << OpCode::OP_EQUALVERIFY;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not bitwise equal upper two frames";
		if (err != ScriptError::EQUALVERIFY)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::EQUALVERIFY)
			<< "Execution should be interrupted with the EQUALVERIFY error if there is not bitwise equal upper two frames";
	}
	{
		Script script;
		script << ScriptData({1,2,3,4,5,6,7,8,9}) << ScriptData({1,2,3,4,5,6,7,8,9}) << OpCode::OP_EQUALVERIFY;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is bitwise equal upper two frames";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().empty())
			<< "Replace pair of frames from if them are bitwise equal, and interrupt otherwise";
	}
	{
		Script script;
		script << ScriptData({1,2,3,4,99,6,7,8,9}) << ScriptData({1,2,3,4,5,6,7,8,9}) << OpCode::OP_EQUALVERIFY;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not bitwise equal upper two frames";
		if (err != ScriptError::EQUALVERIFY)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::EQUALVERIFY)
			<< "Execution should be interrupted with the EQUALVERIFY error if there is not bitwise equal upper two frames";
	}
	{
		Script script;
		script << 1 << OpCode::OP_EQUALVERIFY;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_1ADD)
{
	{
		Script script;
		script << 10000 << OpCode::OP_1ADD;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(10001)
		) << "Exactly higher frame must be increased";
	}
	{
		Script script;
		script << OpCode::OP_1ADD;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_1SUB)
{
	{
		Script script;
		script << 10000 << OpCode::OP_1SUB;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(9999)
		) << "Exactly higher frame must be decreased";
	}
	{
		Script script;
		script << OpCode::OP_1SUB;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_NEGATE)
{
	{
		Script script;
		script << 12345 << OpCode::OP_NEGATE;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(-12345)
		) << "Exactly higher frame must to change his sign";
	}
	{
		Script script;
		script << OpCode::OP_NEGATE;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_ABS)
{
	{
		Script script;
		script << 777 << -123 << OpCode::OP_ABS << 321 << OpCode::OP_ABS;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 3
			&& itpr->stackData()[0] == ScriptNum(321)
			&& itpr->stackData()[1] == ScriptNum(123)
			&& itpr->stackData()[2] == ScriptNum(777)
		) << "Exactly one frames must to drop his sign";
	}
	{
		Script script;
		script << OpCode::OP_NEGATE;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_NOT)
{
	{
		Script script;
		script << -1 << OpCode::OP_NOT << 0 << OpCode::OP_NOT << 1 << OpCode::OP_NOT << 9999 << OpCode::OP_NOT;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 4
			&& itpr->stackData()[0] == ScriptNum(0)
			&& itpr->stackData()[1] == ScriptNum(0)
			&& itpr->stackData()[2] == ScriptNum(1)
			&& itpr->stackData()[3] == ScriptNum(0)
		) << "Exactly one frames must be changed";
	}
	{
		Script script;
		script << OpCode::OP_NOT;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_0NOTEQUAL)
{
	{
		Script script;
		script << -1 << OpCode::OP_0NOTEQUAL << 0 << OpCode::OP_0NOTEQUAL << 1 << OpCode::OP_0NOTEQUAL << 9999 << OpCode::OP_0NOTEQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 4
			&& itpr->stackData()[0] == ScriptNum(1)
			&& itpr->stackData()[1] == ScriptNum(1)
			&& itpr->stackData()[2] == ScriptNum(0)
			&& itpr->stackData()[3] == ScriptNum(1)
		) << "Exactly one frames must be changed";
	}
	{
		Script script;
		script << OpCode::OP_0NOTEQUAL;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_ADD)
{
	{
		Script script;
		script << 1 << 2 << 3 << OpCode::OP_ADD;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 2
			&& itpr->stackData()[0] == ScriptNum(5)
			&& itpr->stackData()[1] == ScriptNum(1)
		) << "First pair of frames must be removed by their sum";
	}
	{
		Script script;
		script << 1 << OpCode::OP_ADD;

		EXPECT_FALSE(itpr->resetAndExecute(script))
						<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_SUB)
{
	{
		Script script;
		script << 1 << 2 << 3 << OpCode::OP_SUB;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 2
			&& itpr->stackData()[0] == ScriptNum(-1)
			&& itpr->stackData()[1] == ScriptNum(1)
		) << "First pair of frames must be removed by their sum";
	}
	{
		Script script;
		script << 1 << OpCode::OP_SUB;

		EXPECT_FALSE(itpr->resetAndExecute(script))
						<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_BOOLAND)
{
	{
		Script script;
		script
		<< 0 << 0 << OpCode::OP_BOOLAND
		<< 0 << 11 << OpCode::OP_BOOLAND
		<< 22 << 0 << OpCode::OP_BOOLAND
		<< 33 << 44 << OpCode::OP_BOOLAND;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 4
			&& itpr->stackData()[0] == ScriptNum(1)
			&& itpr->stackData()[1] == ScriptNum(0)
			&& itpr->stackData()[2] == ScriptNum(0)
			&& itpr->stackData()[3] == ScriptNum(0)
		) << "Pair of frames must be removed by their conjunction (boolean 'AND')";
	}
	{
		Script script;
		script << 1 << OpCode::OP_BOOLAND;

		EXPECT_FALSE(itpr->resetAndExecute(script))
						<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_BOOLOR)
{
	{
		Script script;
		script
			<< 0 << 0 << OpCode::OP_BOOLOR
			<< 0 << 11 << OpCode::OP_BOOLOR
			<< 22 << 0 << OpCode::OP_BOOLOR
			<< 33 << 44 << OpCode::OP_BOOLOR;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 4
			&& itpr->stackData()[0] == ScriptNum(1)
			&& itpr->stackData()[1] == ScriptNum(1)
			&& itpr->stackData()[2] == ScriptNum(1)
			&& itpr->stackData()[3] == ScriptNum(0)
		) << "Pair of frames must be removed by their disjunction (boolean 'OR')";
	}
	{
		Script script;
		script << 1 << OpCode::OP_BOOLOR;

		EXPECT_FALSE(itpr->resetAndExecute(script))
						<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_NUMEQUAL)
{
	{
		Script script;
		script << 123 << 123 << OpCode::OP_NUMEQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(1)
		) << "Must replace first pair of frames by '1' if this frames is equal as numbers";
	}
	{
		Script script;
		script << 321 << 123 << OpCode::OP_NUMEQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(0)
		) << "Must replace first pair of frames by '0' if this frames is not equal as numbers";
	}
	{
		Script script;
		script << 1 << OpCode::OP_NUMEQUAL;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_NUMEQUALVERIFY)
{
	{
		Script script;
		script << 777 << 123 << 123 << OpCode::OP_NUMEQUALVERIFY;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is bitwise equal upper two frames";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(777)
		) << "Must remove first pair of frames if they is equal as numbers";
	}
	{
		Script script;
		script << 123 << 321 << OpCode::OP_NUMEQUALVERIFY;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not equal upper two frames as numbers";
		if (err != ScriptError::NUMEQUALVERIFY)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::NUMEQUALVERIFY)
			<< "Execution should be interrupted with the NUMEQUALVERIFY error if there is not equal upper two frames as numbers";
	}
	{
		Script script;
		script << 777 << ScriptData({1,2,3,4}) << ScriptData({1,2,3,4}) << OpCode::OP_NUMEQUALVERIFY;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is bitwise equal upper two frames";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(777)
		) << "Must remove first pair of frames if they is equal as numbers";
	}
	{
		Script script;
		script << ScriptData({1,2,3,4}) << ScriptData({1,2,3}) << OpCode::OP_NUMEQUALVERIFY;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not equal upper two frames as numbers";
		if (err != ScriptError::NUMEQUALVERIFY)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::NUMEQUALVERIFY)
			<< "Execution should be interrupted with the NUMEQUALVERIFY error if there is not equal upper two frames as numbers";
	}
	{
		Script script;
		script << 1 << OpCode::OP_NUMEQUALVERIFY;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_NUMNOTEQUAL)
{
	{
		Script script;
		script << 123 << 123 << OpCode::OP_NUMNOTEQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(0)
		) << "Must replace first pair of frames by '0' if this frames is equal as numbers";
	}
	{
		Script script;
		script << 321 << 123 << OpCode::OP_NUMNOTEQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(1)
		) << "Must replace first pair of frames by '1' if this frames is not equal as numbers";
	}
	{
		Script script;
		script << 1 << OpCode::OP_NUMNOTEQUAL;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_LESSTHAN)
{
	{
		Script script;
		script << 123 << 321 << OpCode::OP_LESSTHAN;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(1)
		) << "Must be replaced pair of frames by '1' if first frame less than second one, and by '0' else";
	}
	{
		Script script;
		script << 111 << 111 << OpCode::OP_LESSTHAN;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(0)
		) << "Must be replaced pair of frames by '1' if first frame less than second one, and by '0' else";
	}
	{
		Script script;
		script << 321 << 123 << OpCode::OP_LESSTHAN;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(0)
		) << "Must be replaced pair of frames by '1' if first frame less than second one, and by '0' else";
	}
	{
		Script script;
		script << 1 << OpCode::OP_LESSTHAN;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_GREATERTHAN)
{
	{
		Script script;
		script << 123 << 321 << OpCode::OP_GREATERTHAN;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(0)
		) << "Must be replaced pair of frames by '1' if first frame greater than second one, and by '0' else";
	}
	{
		Script script;
		script << 111 << 111 << OpCode::OP_GREATERTHAN;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(0)
		) << "Must be replaced pair of frames by '1' if first frame greater than second one, and by '0' else";
	}
	{
		Script script;
		script << 321 << 123 << OpCode::OP_GREATERTHAN;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(1)
		) << "Must be replaced pair of frames by '1' if first frame greater than second one, and by '0' else";
	}
	{
		Script script;
		script << 1 << OpCode::OP_GREATERTHAN;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_LESSTHANOREQUAL)
{
	{
		Script script;
		script << 123 << 321 << OpCode::OP_LESSTHANOREQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(1)
		) << "Must be replaced pair of frames by '1' if first frame less than or equal second one, and by '0' else";
	}
	{
		Script script;
		script << 111 << 111 << OpCode::OP_LESSTHANOREQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(1)
		) << "Must be replaced pair of frames by '1' if first frame less than or equal second one, and by '0' else";
	}
	{
		Script script;
		script << 321 << 123 << OpCode::OP_LESSTHANOREQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(0)
		) << "Must be replaced pair of frames by '1' if first frame less than or equal second one, and by '0' else";
	}
	{
		Script script;
		script << 1 << OpCode::OP_LESSTHANOREQUAL;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_GREATERTHANOREQUAL)
{
	{
		Script script;
		script << 123 << 321 << OpCode::OP_GREATERTHANOREQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(0)
		) << "Must be replaced pair of frames by '1' if first frame greater than second or equal one, and by '0' else";
	}
	{
		Script script;
		script << 111 << 111 << OpCode::OP_GREATERTHANOREQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(1)
		) << "Must be replaced pair of frames by '1' if first frame greater than or equal second one, and by '0' else";
	}
	{
		Script script;
		script << 321 << 123 << OpCode::OP_GREATERTHANOREQUAL;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(1)
		) << "Must be replaced pair of frames by '1' if first frame greater than or equal second one, and by '0' else";
	}
	{
		Script script;
		script << 1 << OpCode::OP_GREATERTHANOREQUAL;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_MIN)
{
	{
		Script script;
		script << 111 << 222 << OpCode::OP_MIN;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(111)
		) << "Must be replaced pair of frames by the minimum of them";
	}
	{
		Script script;
		script << 222 << 111 << OpCode::OP_MIN;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(111)
		) << "Must be replaced pair of frames by the minimum of them";
	}
	{
		Script script;
		script << 1 << OpCode::OP_MIN;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_MAX)
{
	{
		Script script;
		script << 111 << 222 << OpCode::OP_MAX;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(222)
		) << "Must be replaced pair of frames by the maximum of them";
	}
	{
		Script script;
		script << 222 << 111 << OpCode::OP_MAX;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(222)
		) << "Must be replaced pair of frames by the maximum of them";
	}
	{
		Script script;
		script << 1 << OpCode::OP_MAX;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
}

TEST_F(OpCodeExecution, OP_RIPEMD160)
{
	{
		Script script;
		script << OpCode::OP_RIPEMD160;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
	{
		Script script;
		script << ScriptData{} << OpCode::OP_RIPEMD160;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0].asData() == ScriptData(Hex::Parse("9c1185a5 c5e9fc54 61280897 7ee8f548 b2258d31"))
		) << "Must be replaced upper frame by RIPEMD160-hash of his data";
	}
	{
		Script script;
		script << "The quick brown fox jumps over the lazy dog" << OpCode::OP_RIPEMD160;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0].asData() == ScriptData(Hex::Parse("37f332f6 8db77bd9 d7edd496 9571ad67 1cf9dd3b"))
		) << "Must be replaced upper frame by RIPEMD160-hash of his data";
	}
}

TEST_F(OpCodeExecution, OP_SHA1)
{
	{
		Script script;
		script << OpCode::OP_SHA1;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
	{
		Script script;
		script << ScriptData{} << OpCode::OP_SHA1;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0].asData() == ScriptData(Hex::Parse("da39a3ee 5e6b4b0d 3255bfef 95601890 afd80709"))
		) << "Must be replaced upper frame by SHA1-hash of his data";
	}
	{
		Script script;
		script << "The quick brown fox jumps over the lazy dog" << OpCode::OP_SHA1;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0].asData() == ScriptData(Hex::Parse("2fd4e1c6 7a2d28fc ed849ee1 bb76e739 1b93eb12"))
		) << "Must be replaced upper frame by SHA1-hash of his data";
	}
}

TEST_F(OpCodeExecution, OP_SHA256)
{
	{
		Script script;
		script << OpCode::OP_SHA256;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
	{
		Script script;
		script << ScriptData{} << OpCode::OP_SHA256;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0].asData() == ScriptData(Hex::Parse("e3b0c442 98fc1c14 9afbf4c8 996fb924 27ae41e4 649b934c a495991b 7852b855"))
		) << "Must be replaced upper frame by SHA256-hash of his data";
	}
	{
		Script script;
		script << "The quick brown fox jumps over the lazy dog" << OpCode::OP_SHA256;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}

		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0].asData() == ScriptData(Hex::Parse("d7a8fbb3 07d78094 69ca9abc b0082e4f 8d5651e4 6d3cdb76 2d02d0bf 37c9e592"))
		) << "Must be replaced upper frame by SHA256-hash of his data";
	}
}

TEST_F(OpCodeExecution, OP_HASH160)
{
	{
		Script script;
		script << OpCode::OP_HASH160;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
	{
		Script script;
		script << ScriptData{} << OpCode::OP_HASH160;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0].asData() == ScriptData(Hex::Parse("b472a266 d0bd89c1 3706a413 2ccfb16f 7c3b9fcb"))
		) << "Must be replaced upper frame by Hash160 of his data";
	}
	{
		Script script;
		script << "The quick brown fox jumps over the lazy dog" << OpCode::OP_HASH160;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0].asData() == ScriptData(Hex::Parse("0e3397b4 abc7a382 b3ea2365 883c3c7c a5f07600"))
		) << "Must be replaced upper frame by Hash160 of his data";
	}
}

TEST_F(OpCodeExecution, OP_HASH256)
{
	{
		Script script;
		script << OpCode::OP_HASH256;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
	{
		Script script;
		script << ScriptData{} << OpCode::OP_HASH256;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0].asData() == ScriptData(Hex::Parse("5df6e0e2 761359d3 0a827505 8e299fcc 03815345 45f55cf4 3e41983f 5d4c9456"))
		) << "Must be replaced upper frame by Hash256 of his data";
	}
	{
		Script script;
		script << "The quick brown fox jumps over the lazy dog" << OpCode::OP_HASH256;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";
		if (err != ScriptError::OK)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_TRUE(itpr->stackData().size() == 1
			&& itpr->stackData()[0].asData() == ScriptData(Hex::Parse("6d377950 21e544d8 2b41850e df7aabab 9a0ebe27 4e54a519 840c4666 f35b3937"))
		) << "Must be replaced upper frame by Hash256 of his data";
	}
}

TEST_F(OpCodeExecution, OP__IF)
{
	{
		Script script;
		script << OpCode::OP_IF;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::UNBALANCED_CONDITIONAL)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::UNBALANCED_CONDITIONAL)
			<< "Execution should be interrupted with the UNBALANCED_CONDITIONAL error if there is not enough data on the stack";
	}
	{
		Script script;
		script
			<< -2
			<< -1
				<< OpCode::OP_IF
					<< 0
						<< OpCode::OP_IF
							<< 1
						<< OpCode::OP_ELSE
							<< 2
					<< OpCode::OP_ENDIF
				<< OpCode::OP_ELSE
					<< 3
						<< OpCode::OP_IF
							<< 4
						<< OpCode::OP_ELSE
							<< 5
					<< OpCode::OP_ENDIF
			<< OpCode::OP_ENDIF
			<< 0
				<< OpCode::OP_IF
					<< 6
				<< OpCode::OP_ELSE
					<< 7
			<< OpCode::OP_ENDIF
			<< 8;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack and conditional statements balanced";

		bool success = err == ScriptError::OK
			&& itpr->stackData().size() == 4
			&& itpr->stackData()[0] == ScriptNum(8)
			&& itpr->stackData()[1] == ScriptNum(7)
			&& itpr->stackData()[2] == ScriptNum(2)
			&& itpr->stackData()[3] == ScriptNum(-2);
		if (!success)
		{
			ADD_FAILURE() << "Unexpected result. See execution log:";
			itpr_trace->resetAndExecute(script);
		}
	}
}

TEST_F(OpCodeExecution, OP_CHECKSIG)
{
	auto keyData = Hex::Parse("dd53097b213c0d7062410fa361e448c851e6946b75b1c0528361346671d2adad");

	{
		CKey key;
		key.Set(keyData.begin(), keyData.end(), true);

		CPubKey pubKey = key.GetPubKey();
//		std::cout << "PubKey: '" << Hex::toString(pubKey) << "'\n";

		std::string msg = "The quick brown fox jumps over the lazy dog";
//		std::cout << "Message: '" << msg << "'\n";

		uint256 hash = Hash(msg.cbegin(), msg.cend());
//		std::cout << "Hash: '" << Hex::toString(hash) << "'\n";

		std::vector<uint8_t> sig;
		key.Sign(hash, sig);
//		std::cout << "Sig: '" << Hex::toString(sig) << "'\n";

		auto valid = pubKey.Verify(hash, sig);
//		std::cout << "Valid: '" << std::boolalpha << valid << "'\n";
	}
	{
		Script script;
		script << ScriptData{} << OpCode::OP_CHECKSIG;

		EXPECT_FALSE(itpr->resetAndExecute(script))
			<< "Execution should be interrupted if there is not enough data on the stack";
		if (err != ScriptError::INVALID_STACK_OPERATION)
		{
			itpr_trace->resetAndExecute(script);
		}
		EXPECT_EQ(err, ScriptError::INVALID_STACK_OPERATION)
			<< "Execution should be interrupted with the INVALID_STACK_OPERATION error if there is not enough data on the stack";
	}
	{
		CKey key;
		key.MakeNewKey(true);
		key.Set(keyData.begin(), keyData.end(), true);

		std::string msg = "The quick brown fox jumps over the lazy dog";
		uint256 hash = Hash(msg.cbegin(), msg.cend());
		checker.setHash(hash);

		std::vector<uint8_t> sig;
        auto ok = key.Sign(hash, sig);

        auto pubKey = key.GetPubKey();

		Script script;
		script
			<< ScriptData{sig.data(), sig.size()}
			<< ScriptData{pubKey.data(), pubKey.size()}
			<< OpCode::OP_CHECKSIG;

		EXPECT_TRUE(itpr->resetAndExecute(script))
			<< "Mustn't be interrupted if there is enough data on the stack";

		bool success = err == ScriptError::OK
			&& itpr->stackData().size() == 1
			&& itpr->stackData()[0] == ScriptNum(1);
		if (!success)
		{
			ADD_FAILURE() << "Unexpected result. See execution log:";
			itpr_trace->resetAndExecute(script);
		}
	}
}
