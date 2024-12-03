/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP RPC protocol stack
 *
 * Description   : The definition and implementation for the RPC protocol stack in SUP
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "test_functor.h"

#include <sup/protocol/protocol_rpc_client.h>

#include <sup/protocol/protocol_rpc.h>
#include <sup/protocol/exceptions.h>

#include <gtest/gtest.h>

using namespace sup::protocol;

using ::testing::_;
using ::testing::AtLeast;

const sup::dto::AnyValue kReplyPayload{ sup::dto::Float64Type, 3.14 };

class ProtocolRPCClientAsyncTest : public ::testing::Test
{
protected:
  ProtocolRPCClientAsyncTest();
  virtual ~ProtocolRPCClientAsyncTest();
};

TEST_F(ProtocolRPCClientAsyncTest, AsyncSuccess)
{
  // Create function that imitates asynchronous communication:
  auto func = [](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kNone);
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kNone,
                                        AsyncCommand::kGetReply);
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output;
  // Check successful asynchronous invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(AtLeast(3));
  EXPECT_EQ(rpc_client.Invoke(input, output), Success);
  EXPECT_EQ(output, kReplyPayload);
}

ProtocolRPCClientAsyncTest::ProtocolRPCClientAsyncTest() = default;

ProtocolRPCClientAsyncTest::~ProtocolRPCClientAsyncTest() = default;
