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

#include <sup/protocol/base/async_invoke.h>

#include <gtest/gtest.h>

#include <future>

using namespace sup::protocol;

class AsyncRequestTestProtocol : public Protocol
{
public:
  AsyncRequestTestProtocol(std::future<void> go_future);
  ~AsyncRequestTestProtocol() = default;

  ProtocolResult Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;
  ProtocolResult Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;
private:
  std::future<void> m_go_future;
};

class AsyncRequestTest : public ::testing::Test
{
protected:
  AsyncRequestTest();
  virtual ~AsyncRequestTest();
};

TEST_F(AsyncRequestTest, Construction)
{
  // Check status of AsyncInvoke after construction.
  sup::dto::AnyValue input{ sup::dto::UnsignedInteger32Type, 42u };
  std::promise<void> go;
  AsyncRequestTestProtocol protocol{go.get_future()};
  AsyncInvoke req{protocol, input};
  EXPECT_FALSE(req.IsReady());
  EXPECT_FALSE(req.IsReadyForRemoval());
  auto reply = req.GetReply();
  EXPECT_EQ(reply.first, InvalidAsynchronousOperationError);
  go.set_value();
}

TEST_F(AsyncRequestTest, GetReply)
{
  // Check successfull reply. After GetReply(), the request is no longer ready and has become
  // expired.
  sup::dto::AnyValue input{ sup::dto::UnsignedInteger32Type, 42u };
  std::promise<void> go;
  AsyncRequestTestProtocol protocol{go.get_future()};
  AsyncInvoke req{protocol, input};
  EXPECT_FALSE(req.IsReady());
  EXPECT_FALSE(req.IsReadyForRemoval());
  go.set_value();
  ASSERT_TRUE(req.WaitForReady(1.0));
  EXPECT_TRUE(req.IsReady());
  EXPECT_FALSE(req.IsReadyForRemoval());
  auto reply = req.GetReply();
  EXPECT_EQ(reply.first, Success);
  EXPECT_EQ(reply.second, input);
  EXPECT_FALSE(req.IsReady());
  EXPECT_TRUE(req.IsReadyForRemoval());
}

TEST_F(AsyncRequestTest, Invalidate)
{
  // Check status when request is invalidated.
  sup::dto::AnyValue input{ sup::dto::UnsignedInteger32Type, 42u };
  std::promise<void> go;
  AsyncRequestTestProtocol protocol{go.get_future()};
  AsyncInvoke req{protocol, input};
  EXPECT_FALSE(req.IsReady());
  EXPECT_FALSE(req.IsReadyForRemoval());
  go.set_value();
  ASSERT_TRUE(req.WaitForReady(1.0));
  req.Invalidate();
  EXPECT_FALSE(req.IsReady());
  EXPECT_TRUE(req.IsReadyForRemoval());
  auto reply = req.GetReply();
  EXPECT_EQ(reply.first, InvalidAsynchronousOperationError);
  EXPECT_NE(reply.second, input);
}

AsyncRequestTest::AsyncRequestTest() = default;

AsyncRequestTest::~AsyncRequestTest() = default;

AsyncRequestTestProtocol::AsyncRequestTestProtocol(std::future<void> go_future)
  : m_go_future{std::move(go_future)}
{}

ProtocolResult AsyncRequestTestProtocol::Invoke(const sup::dto::AnyValue& input,
                                                sup::dto::AnyValue& output)
{
  output = input;
  m_go_future.get();
  return Success;
}

ProtocolResult AsyncRequestTestProtocol::Service(const sup::dto::AnyValue& input,
                                                 sup::dto::AnyValue& output)
{
  output = input;
  return Success;
}
