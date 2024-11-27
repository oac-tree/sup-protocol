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

#include <sup/protocol/protocol_rpc.h>

#include <stdexcept>

namespace sup
{
namespace protocol
{
namespace test
{
TestFunctor::TestFunctor()
  : m_last_request{}
{}

TestFunctor::TestFunctor(TestFunctor*& handle)
  : TestFunctor{}
{
  handle = this;
}

TestFunctor::~TestFunctor() = default;

sup::dto::AnyValue TestFunctor::operator()(const sup::dto::AnyValue& input)
{
  m_last_request.reset(new sup::dto::AnyValue(input));
  auto encoding = utils::TryGetPacketEncoding(input).second;
  bool normal_request = input.HasField(constants::REQUEST_PAYLOAD);
  auto query_result =
    normal_request ? utils::TryExtractRPCPayload(input, constants::REQUEST_PAYLOAD, encoding)
                   : utils::TryExtractRPCPayload(input, constants::SERVICE_REQUEST_PAYLOAD,
                                                 encoding);
  if (!query_result.first)
  {
    throw std::runtime_error("Could not extract payload");
  }
  auto query = query_result.second;
  if (query.HasField(BAD_REPLY_FIELD) && query[BAD_REPLY_FIELD].As<bool>())
  {
    return sup::dto::AnyValue{{{"BadReplyFormat", {sup::dto::BooleanType, true}}}};
  }
  if (query.HasField(FUNCTOR_THROW_FIELD) && query[FUNCTOR_THROW_FIELD].As<bool>())
  {
    throw std::runtime_error("Throwing on demand");
  }
  return normal_request ? utils::CreateRPCReply(Success, query, encoding)
                        : utils::CreateServiceReply(Success, query, encoding);
}

sup::dto::AnyValue TestFunctor::GetLastRequest() const
{
  if (!m_last_request)
  {
    return {};
  }
  sup::dto::AnyValue result{*m_last_request};
  return result;
}


}  // namespace test

}  // namespace protocol

}  // namespace sup
