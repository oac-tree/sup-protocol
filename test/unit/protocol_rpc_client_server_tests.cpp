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

#include "test_protocol.h"

#include <sup/protocol/protocol_rpc_client.h>
#include <sup/protocol/protocol_rpc_server.h>

#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>

#include <gtest/gtest.h>

using namespace sup::protocol;

class AnyFunctorSpy : public sup::dto::AnyFunctor
{
public:
  explicit AnyFunctorSpy(sup::dto::AnyFunctor& functor)
    : m_functor{functor} {}
  ~AnyFunctorSpy() = default;

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override
  {
    m_inputs.push_back(input);
    const auto output = m_functor(input);
    m_outputs.push_back(output);
    return output;
  }

  const std::vector<sup::dto::AnyValue>& GetInputs() const { return m_inputs; }

  const std::vector<sup::dto::AnyValue>& GetOutputs() const { return m_outputs; }

private:
  sup::dto::AnyFunctor& m_functor;
  std::vector<sup::dto::AnyValue> m_inputs;
  std::vector<sup::dto::AnyValue> m_outputs;
};

void DumpAnyValues(const std::vector<sup::dto::AnyValue>& values)
{
  for (const auto& val : values)
  {
    test::DumpAnyValue(val);
  }
}

class ProtocolRPCClientServerTest : public ::testing::Test
{
protected:
  ProtocolRPCClientServerTest();
  virtual ~ProtocolRPCClientServerTest();

  test::TestProtocol m_test_protocol;
};

TEST_F(ProtocolRPCClientServerTest, AsyncInvoke)
{
  ProtocolRPCServer rpc_server{m_test_protocol};
  AnyFunctorSpy spy{rpc_server};
  ProtocolRPCClientConfig client_config{PayloadEncoding::kNone, 0.2, 0.02};
  ProtocolRPCClient rpc_client{spy, client_config};
  sup::dto::AnyValue input{sup::dto::SignedInteger32Type, 42};
  sup::dto::AnyValue output;
  EXPECT_EQ(rpc_client.Invoke(input, output), Success);
  EXPECT_GE(spy.GetInputs().size(), 3);
  EXPECT_GE(spy.GetOutputs().size(), 3);
  // DumpAnyValues(spy.GetInputs());
  // DumpAnyValues(spy.GetOutputs());
}

TEST_F(ProtocolRPCClientServerTest, AsyncInvokeEcho)
{
  ProtocolRPCServer rpc_server{m_test_protocol};
  AnyFunctorSpy spy{rpc_server};
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{spy, client_config};
  sup::dto::AnyValue input = {{
    { "value", {sup::dto::UnsignedInteger32Type, 42u }},
    { test::ECHO_FIELD, {sup::dto::BooleanType, true }}
  }};
  sup::dto::AnyValue output;
  EXPECT_EQ(rpc_client.Invoke(input, output), Success);
  EXPECT_GE(spy.GetInputs().size(), 3);
  EXPECT_GE(spy.GetOutputs().size(), 3);
  EXPECT_EQ(input, output);
}

TEST_F(ProtocolRPCClientServerTest, ServerException)
{
  ProtocolRPCServer rpc_server{m_test_protocol};
  AnyFunctorSpy spy{rpc_server};
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{spy, client_config};
  sup::dto::AnyValue input = {{
    { test::THROW_FIELD, {sup::dto::BooleanType, true }}
  }};
  sup::dto::AnyValue output;
  EXPECT_EQ(rpc_client.Invoke(input, output), ServerProtocolException);
  EXPECT_GE(spy.GetInputs().size(), 3);
  EXPECT_GE(spy.GetOutputs().size(), 3);
}

ProtocolRPCClientServerTest::ProtocolRPCClientServerTest()
  : m_test_protocol{}
{}

ProtocolRPCClientServerTest::~ProtocolRPCClientServerTest() = default;

