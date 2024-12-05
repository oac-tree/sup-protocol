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

#include <sup/protocol/protocol_factory_utils.h>

#include <sup/protocol/exceptions.h>

#include <sup/protocol/factory/rpc_client_stack.h>
#include <sup/protocol/factory/rpc_server_stack.h>

#include <map>

namespace sup
{
namespace protocol
{

std::unique_ptr<RPCServerInterface> CreateRPCServerStack(
  std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
  Protocol& protocol)
{
  return std::unique_ptr<RPCServerInterface>(new RPCServerStack(factory_func, protocol));
}

std::unique_ptr<Protocol> CreateRPCClientStack(
  std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func, PayloadEncoding encoding)
{
  return std::unique_ptr<Protocol>(new RPCClientStack(factory_func, encoding));
}

std::unique_ptr<Protocol> CreateRPCClientStack(
  std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func,
  ProtocolRPCClientConfig config)
{
  return std::unique_ptr<Protocol>(new RPCClientStack(factory_func, config));
}

PayloadEncoding ParsePayloadEncoding(const sup::dto::AnyValue& config)
{
  if (!config.HasField(kEncoding))
  {
    return PayloadEncoding::kBase64;
  }
  ValidateConfigurationField(config, kEncoding, sup::dto::StringType);
  auto encoding_str = config[kEncoding].As<std::string>();
  static const std::map<std::string, PayloadEncoding> encoding_map = {
    { kEncoding_None, PayloadEncoding::kNone },
    { kEncoding_Base64, PayloadEncoding::kBase64 }
  };
  auto iter = encoding_map.find(encoding_str);
  if (iter == encoding_map.end())
  {
    const std::string error = "Could not parse encoding field: unknow encoding";
    throw InvalidOperationException(error);
  }
  return iter->second;
}

void ValidateConfigurationField(const sup::dto::AnyValue& config, const std::string& field_name,
                                const sup::dto::AnyType& anytype)
{
  if (!config.HasField(field_name))
  {
    const std::string error = "Configuration is missing the required field [" + field_name + "]";
    throw InvalidOperationException(error);
  }
  if (config[field_name].GetType() != anytype)
  {
    const std::string error = "Configuration with field [" + field_name + "] has wrong type";
    throw InvalidOperationException(error);
  }
}

}  // namespace protocol

}  // namespace sup
