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
 * Copyright (c) : 2010-2023 ITER Organization,
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

using namespace sup::protocol;

class ExceptionsTest : public ::testing::Test
{
protected:
  ExceptionsTest();
  virtual ~ExceptionsTest();
};

TEST_F(ExceptionsTest, InvalidOperationException)
{
  const std::string error_message = "Somthing went wrong";
  InvalidOperationException e{error_message};
  EXPECT_EQ(e.what(), error_message);
}

ExceptionsTest::ExceptionsTest() = default;

ExceptionsTest::~ExceptionsTest() = default;
