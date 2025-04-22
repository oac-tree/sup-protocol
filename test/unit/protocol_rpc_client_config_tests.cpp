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

#include <gtest/gtest.h>

#include <sup/protocol/protocol_rpc_client_config.h>

#include <sup/dto/anyvalue.h>

using namespace sup::protocol;

class ProtocolRPCClientConfigTest : public ::testing::Test
{
protected:
  ProtocolRPCClientConfigTest();
  virtual ~ProtocolRPCClientConfigTest();
};

TEST_F(ProtocolRPCClientConfigTest, DefaultConstructor)
{
  ProtocolRPCClientConfig config{};
  EXPECT_EQ(config.m_encoding, PayloadEncoding::kBase64);
  EXPECT_EQ(config.m_async, false);
  EXPECT_EQ(config.m_timeout_sec, 0.0);
  EXPECT_EQ(config.m_polling_interval_sec, 0.0);
  EXPECT_TRUE(ValidateProtocolRPCClientConfig(config));
}

TEST_F(ProtocolRPCClientConfigTest, EncodingConstructor)
{
  ProtocolRPCClientConfig config{PayloadEncoding::kNone};
  EXPECT_EQ(config.m_encoding, PayloadEncoding::kNone);
  EXPECT_EQ(config.m_async, false);
  EXPECT_EQ(config.m_timeout_sec, 0.0);
  EXPECT_EQ(config.m_polling_interval_sec, 0.0);
  EXPECT_TRUE(ValidateProtocolRPCClientConfig(config));
}

TEST_F(ProtocolRPCClientConfigTest, AsyncConstructor)
{
  ProtocolRPCClientConfig config{PayloadEncoding::kBase64, 5.0, 1.0};
  EXPECT_EQ(config.m_encoding, PayloadEncoding::kBase64);
  EXPECT_EQ(config.m_async, true);
  EXPECT_EQ(config.m_timeout_sec, 5.0);
  EXPECT_EQ(config.m_polling_interval_sec, 1.0);
  EXPECT_TRUE(ValidateProtocolRPCClientConfig(config));
}

TEST_F(ProtocolRPCClientConfigTest, InvalidConfig)
{
  {
    // Unsupported encoding:
    auto encoding = static_cast<PayloadEncoding>(42);
    ProtocolRPCClientConfig config{encoding};
    EXPECT_FALSE(ValidateProtocolRPCClientConfig(config));
  }
  {
    // zero timeout:
    double timeout = 0.0;
    double polling_interval = 1.0;
    ProtocolRPCClientConfig config{PayloadEncoding::kBase64, timeout, polling_interval};
    EXPECT_FALSE(ValidateProtocolRPCClientConfig(config));
  }
  {
    // negative timeout:
    double timeout = -5.0;
    double polling_interval = 1.0;
    ProtocolRPCClientConfig config{PayloadEncoding::kBase64, timeout, polling_interval};
    EXPECT_FALSE(ValidateProtocolRPCClientConfig(config));
  }
  {
    // zero polling interval:
    double timeout = 10.0;
    double polling_interval = 0.0;
    ProtocolRPCClientConfig config{PayloadEncoding::kBase64, timeout, polling_interval};
    EXPECT_FALSE(ValidateProtocolRPCClientConfig(config));
  }
  {
    // negative polling interval:
    double timeout = 10.0;
    double polling_interval = -1.0;
    ProtocolRPCClientConfig config{PayloadEncoding::kBase64, timeout, polling_interval};
    EXPECT_FALSE(ValidateProtocolRPCClientConfig(config));
  }
}

ProtocolRPCClientConfigTest::ProtocolRPCClientConfigTest() = default;

ProtocolRPCClientConfigTest::~ProtocolRPCClientConfigTest() = default;
