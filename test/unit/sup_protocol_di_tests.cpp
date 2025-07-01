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
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/protocol/registered_names.h>

#include "test_protocol.h"

#include <sup/protocol/protocol_rpc.h>

#include <sup/di/error_codes.h>
#include <sup/di/object_manager.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/any_functor.h>

#include <gtest/gtest.h>

using namespace sup::protocol;

class SupProtocolDiTests : public ::testing::Test
{
protected:
  SupProtocolDiTests() = default;
  ~SupProtocolDiTests() = default;
};

class TestFunctor : public sup::dto::AnyFunctor
{
public:
  TestFunctor() = default;
  ~TestFunctor() = default;
  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override { return input; }
};

const bool TestFunctor_Registered = sup::di::GlobalObjectManager().RegisterInstance(
  std::unique_ptr<sup::dto::AnyFunctor>{new TestFunctor{}}, "test_functor");

const bool TestProtocol_Registered = sup::di::GlobalObjectManager().RegisterInstance(
  std::unique_ptr<Protocol>{new test::TestProtocol{}}, "test_protocol");

TEST_F(SupProtocolDiTests, ProtocolRPCClient)
{
  // Create ProtocolRPCClient
  auto error_code = sup::di::GlobalObjectManager().CreateInstance(
    PROTOCOL_RPC_CLIENT, "protocol_client", {"test_functor"});
  EXPECT_EQ(error_code, sup::di::ErrorCode::kSuccess);
}

TEST_F(SupProtocolDiTests, ProtocolRPCServer)
{
  // Create ProtocolRPCServer
  auto error_code = sup::di::GlobalObjectManager().CreateInstance(
    PROTOCOL_RPC_SERVER, "protocol_server", {"test_protocol"});
  EXPECT_EQ(error_code, sup::di::ErrorCode::kSuccess);
}

TEST_F(SupProtocolDiTests, ConnectedProtocolClientServer)
{
  // Create ProtocolRPCServer
  auto error_code = sup::di::GlobalObjectManager().CreateInstance(
    PROTOCOL_RPC_SERVER, "protocol_server_2", {"test_protocol"});
  EXPECT_EQ(error_code, sup::di::ErrorCode::kSuccess);

  // Create ProtocolRPCClient and inject protocol server
  error_code = sup::di::GlobalObjectManager().CreateInstance(
    PROTOCOL_RPC_CLIENT, "protocol_client_2", {"protocol_server_2"});
  EXPECT_EQ(error_code, sup::di::ErrorCode::kSuccess);

  // Get protocol client reference and retrieve application info from it
  auto& protocol_client =
    sup::di::GlobalObjectManager().GetInstance<Protocol&>("protocol_client_2");
  auto protocol_info = utils::GetApplicationProtocolInfo(protocol_client);
  EXPECT_EQ(protocol_info.m_application_type, test::TEST_PROTOCOL_TYPE);
  EXPECT_EQ(protocol_info.m_application_version, test::TEST_PROTOCOL_VERSION);
}
