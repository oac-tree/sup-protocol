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

#include <sup/protocol/function_protocol.h>

using namespace sup::protocol;

class FunctionProtocolTest : public ::testing::Test
{
protected:
  FunctionProtocolTest();
  virtual ~FunctionProtocolTest();
};

TEST_F(FunctionProtocolTest, FunctionProtocolInput)
{
  const std::string func_name = "MyFunction";
  auto input = FunctionProtocolInput(func_name);
  EXPECT_TRUE(sup::dto::IsStructValue(input));
  EXPECT_TRUE(input.HasField(sup::protocol::FUNCTION_FIELD_NAME));
  EXPECT_EQ(input[FUNCTION_FIELD_NAME].GetType(), sup::dto::StringType);
  EXPECT_EQ(input[FUNCTION_FIELD_NAME].As<std::string>(), func_name);
  EXPECT_EQ(input.NumberOfMembers(), 1);
}

FunctionProtocolTest::FunctionProtocolTest() = default;

FunctionProtocolTest::~FunctionProtocolTest() = default;
