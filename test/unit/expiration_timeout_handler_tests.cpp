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

#include <sup/protocol/base/expiration_timeout_handler.h>

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

using namespace sup::protocol;

class ExpirationTimeoutHandlerTest : public ::testing::Test
{
protected:
  ExpirationTimeoutHandlerTest();
  virtual ~ExpirationTimeoutHandlerTest();
};

TEST_F(ExpirationTimeoutHandlerTest, Constructed)
{
  // First query will always return true
  ExpirationTimeoutHandler handler{5.0};
  EXPECT_TRUE(handler.IsCleanUpNeeded());
  EXPECT_FALSE(handler.IsCleanUpNeeded());
}

TEST_F(ExpirationTimeoutHandlerTest, UpdatedHandler)
{
  ExpirationTimeoutHandler handler{0.05};
  EXPECT_TRUE(handler.IsCleanUpNeeded());
  EXPECT_FALSE(handler.IsCleanUpNeeded());
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_TRUE(handler.IsCleanUpNeeded());
  EXPECT_FALSE(handler.IsCleanUpNeeded());
}

ExpirationTimeoutHandlerTest::ExpirationTimeoutHandlerTest() = default;

ExpirationTimeoutHandlerTest::~ExpirationTimeoutHandlerTest() = default;
