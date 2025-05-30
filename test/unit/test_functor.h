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

#ifndef SUP_CONFIG_TEST_FUNCTOR_H_
#define SUP_CONFIG_TEST_FUNCTOR_H_

#include <sup/dto/any_functor.h>
#include <sup/dto/anyvalue.h>

#include <gmock/gmock.h>

#include <functional>
#include <memory>

namespace sup
{
namespace protocol
{
namespace test
{
const std::string BAD_REPLY_FIELD = "bad_reply";
const std::string FUNCTOR_THROW_FIELD = "throw";

class TestFunctor : public sup::dto::AnyFunctor
{
public:
  TestFunctor();

  explicit TestFunctor(TestFunctor*& handle);

  ~TestFunctor();

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override;

  sup::dto::AnyValue GetLastRequest() const;

private:
  std::unique_ptr<sup::dto::AnyValue> m_last_request;
};

class MockFunctor : public sup::dto::AnyFunctor
{
public:
  MOCK_METHOD(sup::dto::AnyValue, CallOperator, (const sup::dto::AnyValue&));
  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override
  {
    return CallOperator(input);
  }
  void DelegateTo(std::function<sup::dto::AnyValue(const sup::dto::AnyValue&)> func)
  {
    ON_CALL(*this, CallOperator).WillByDefault(func);
  }
};

}  // namespace test

}  // namespace protocol

}  // namespace sup

#endif  // SUP_CONFIG_TEST_FUNCTOR_H_
