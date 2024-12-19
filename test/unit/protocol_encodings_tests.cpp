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

#include <gtest/gtest.h>

#include <sup/protocol/exceptions.h>
#include <sup/protocol/base/protocol_encodings.h>

using namespace sup::protocol;

class ProtocolEncodingsTest : public ::testing::Test
{
protected:
  ProtocolEncodingsTest();
  virtual ~ProtocolEncodingsTest();
};

TEST_F(ProtocolEncodingsTest, Encode)
{
  {
    // No encoding used
    sup::dto::AnyValue val = {{
      {"flag", {sup::dto::BooleanType, true}},
      {"setpoint", {sup::dto::Float64Type, 220.0}}
    }};
    auto encoded = encoding::Encode(val, PayloadEncoding::kNone);
    EXPECT_EQ(encoded, val);
  }
  {
    // Base64 encoding used
    sup::dto::AnyValue val = {{
      {"flag", {sup::dto::BooleanType, true}},
      {"setpoint", {sup::dto::Float64Type, 220.0}}
    }};
    auto encoded_1 = encoding::Encode(val, PayloadEncoding::kBase64);
    auto encoded_2 = encoding::Base64Encode(val);
    EXPECT_EQ(encoded_1, encoded_2);
  }
}

TEST_F(ProtocolEncodingsTest, Decode)
{
  {
    // No encoding used
    sup::dto::AnyValue val = {{
      {"flag", {sup::dto::BooleanType, true}},
      {"setpoint", {sup::dto::Float64Type, 220.0}}
    }};
    auto encoded = encoding::Encode(val, PayloadEncoding::kNone);
    auto decoded = encoding::Decode(encoded, PayloadEncoding::kNone);
    EXPECT_EQ(decoded, val);
  }
  {
    // Base64 encoding used
    sup::dto::AnyValue val = {{
      {"flag", {sup::dto::BooleanType, true}},
      {"setpoint", {sup::dto::Float64Type, 220.0}}
    }};
    auto encoded_1 = encoding::Encode(val, PayloadEncoding::kBase64);
    auto decoded_1 = encoding::Decode(encoded_1, PayloadEncoding::kBase64);
    auto encoded_2 = encoding::Base64Encode(val);
    auto decoded_2 = encoding::Base64Decode(encoded_2);
    EXPECT_EQ(decoded_1, decoded_2);
  }
}

TEST_F(ProtocolEncodingsTest, Exceptions)
{
  {
    // Unknown encoding during encode:
    sup::dto::AnyValue val = {{
      {"flag", {sup::dto::BooleanType, true}},
      {"setpoint", {sup::dto::Float64Type, 220.0}}
    }};
    auto encoding = static_cast<PayloadEncoding>(42);
    EXPECT_THROW(encoding::Encode(val, encoding), InvalidOperationException);
  }
  {
    // Unknown encoding during decode:
    sup::dto::AnyValue val = {{
      {"flag", {sup::dto::BooleanType, true}},
      {"setpoint", {sup::dto::Float64Type, 220.0}}
    }};
    auto encoded = encoding::Encode(val, PayloadEncoding::kBase64);
    auto encoding = static_cast<PayloadEncoding>(42);
    EXPECT_THROW(encoding::Decode(encoded, encoding), InvalidOperationException);
  }
  {
    // Base64 decode with invalid type of input:
    sup::dto::AnyValue val = {{
      {"flag", {sup::dto::BooleanType, true}},
      {"setpoint", {sup::dto::Float64Type, 220.0}}
    }};
    EXPECT_THROW(encoding::Decode(val, PayloadEncoding::kBase64), InvalidOperationException);
    EXPECT_THROW(encoding::Base64Decode(val), InvalidOperationException);
  }
  {
    // Base64 decode with input that is not a base64 encoded string:
    sup::dto::AnyValue val{ sup::dto::StringType, "ABC" };
    EXPECT_THROW(encoding::Decode(val, PayloadEncoding::kBase64), std::runtime_error);
    EXPECT_THROW(encoding::Base64Decode(val), std::runtime_error);
  }
}

ProtocolEncodingsTest::ProtocolEncodingsTest() = default;

ProtocolEncodingsTest::~ProtocolEncodingsTest() = default;
