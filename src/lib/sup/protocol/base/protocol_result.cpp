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

#include <sup/dto/basic_scalar_types.h>
#include <sup/protocol/protocol_result.h>

#include <map>

namespace sup
{
namespace protocol
{
namespace status
{
enum ProtocolStatus : sup::dto::uint32
{
  SUCCESS = 0,
  NOT_CONNECTED,
  CLIENT_NETWORK_ENCODING,
  SERVER_NETWORK_ENCODING,
  CLIENT_NETWORK_DECODING,
  SERVER_NETWORK_DECODING,
  CLIENT_TRANSPORT_ENCODING,
  SERVER_TRANSPORT_ENCODING,
  CLIENT_TRANSPORT_DECODING,
  SERVER_TRANSPORT_DECODING,
  CLIENT_PROTOCOL_ENCODING,
  SERVER_PROTOCOL_ENCODING,
  CLIENT_PROTOCOL_DECODING,
  SERVER_PROTOCOL_DECODING,
  SERVER_UNSUPPORTED_PAYLOAD_ENCODING,
  INVALID_REQUEST_IDENTIFIER,
  INVALID_ASYNCHROUNOUS_OPERATION,
  SERVER_PROTOCOL_EXCEPTION,
  CLIENT_TRANSPORT_EXCEPTION,
  ASYNCHRONOUS_PROTOCOL_TIMEOUT
};
}  // namespace status

ProtocolResult::ProtocolResult()
  : m_value{status::SUCCESS}
{}

ProtocolResult::~ProtocolResult() = default;

ProtocolResult::ProtocolResult(unsigned int value) noexcept
  : m_value{value}
{}

ProtocolResult::ProtocolResult(const ProtocolResult&) = default;

ProtocolResult& ProtocolResult::operator=(const ProtocolResult&) & = default;

ProtocolResult::ProtocolResult(ProtocolResult&&) noexcept = default;

ProtocolResult& ProtocolResult::operator=(
  ProtocolResult&&) & noexcept = default;

unsigned int ProtocolResult::GetValue() const
{
  return m_value;
}

bool ProtocolResult::Success() const
{
  return m_value == status::SUCCESS;
}

bool ProtocolResult::operator==(const ProtocolResult& other) const
{
  return m_value == other.m_value;
}

bool ProtocolResult::operator!=(const ProtocolResult& other) const
{
  return !this->operator==(other);
}

std::string ProtocolResultToString(const ProtocolResult& result)
{
  static const std::map<unsigned int, std::string> results = {
      {status::SUCCESS, "Success"},
      {status::NOT_CONNECTED, "NotConnected"},
      {status::CLIENT_NETWORK_ENCODING, "ClientNetworkEncodingError"},
      {status::SERVER_NETWORK_ENCODING, "ServerNetworkEncodingError"},
      {status::CLIENT_NETWORK_DECODING, "ClientNetworkDecodingError"},
      {status::SERVER_NETWORK_DECODING, "ServerNetworkDecodingError"},
      {status::CLIENT_TRANSPORT_ENCODING, "ClientTransportEncodingError"},
      {status::SERVER_TRANSPORT_ENCODING, "ServerTransportEncodingError"},
      {status::CLIENT_TRANSPORT_DECODING, "ClientTransportDecodingError"},
      {status::SERVER_TRANSPORT_DECODING, "ServerTransportDecodingError"},
      {status::CLIENT_PROTOCOL_ENCODING, "ClientProtocolEncodingError"},
      {status::SERVER_PROTOCOL_ENCODING, "ServerProtocolEncodingError"},
      {status::CLIENT_PROTOCOL_DECODING, "ClientProtocolDecodingError"},
      {status::SERVER_PROTOCOL_DECODING, "ServerProtocolDecodingError"},
      {status::SERVER_UNSUPPORTED_PAYLOAD_ENCODING, "ServerUnsupportedPayloadEncodingError"},
      {status::INVALID_REQUEST_IDENTIFIER, "InvalidRequestIdentifierError"},
      {status::INVALID_ASYNCHROUNOUS_OPERATION, "InvalidAsynchronousOperationError"},
      {status::SERVER_PROTOCOL_EXCEPTION, "ServerProtocolException"},
      {status::CLIENT_TRANSPORT_EXCEPTION, "ClientTransportException"},
      {status::ASYNCHRONOUS_PROTOCOL_TIMEOUT, "AsynchronousProtocolTimeout"}};
  auto it = results.find(result.GetValue());
  if (it != results.end())
  {
    return it->second;
  }
  return "Unknown ProtocolResult: " + std::to_string(result.GetValue());
}

const ProtocolResult Success{status::SUCCESS};
const ProtocolResult NotConnected{status::NOT_CONNECTED};
const ProtocolResult ClientNetworkEncodingError{status::CLIENT_NETWORK_ENCODING};
const ProtocolResult ServerNetworkEncodingError{status::SERVER_NETWORK_ENCODING};
const ProtocolResult ClientNetworkDecodingError{status::CLIENT_NETWORK_DECODING};
const ProtocolResult ServerNetworkDecodingError{status::SERVER_NETWORK_DECODING};
const ProtocolResult ClientTransportEncodingError{status::CLIENT_TRANSPORT_ENCODING};
const ProtocolResult ServerTransportEncodingError{status::SERVER_TRANSPORT_ENCODING};
const ProtocolResult ClientTransportDecodingError{status::CLIENT_TRANSPORT_DECODING};
const ProtocolResult ServerTransportDecodingError{status::SERVER_TRANSPORT_DECODING};
const ProtocolResult ClientProtocolEncodingError{status::CLIENT_PROTOCOL_ENCODING};
const ProtocolResult ServerProtocolEncodingError{status::SERVER_PROTOCOL_ENCODING};
const ProtocolResult ClientProtocolDecodingError{status::CLIENT_PROTOCOL_DECODING};
const ProtocolResult ServerProtocolDecodingError{status::SERVER_PROTOCOL_DECODING};
const ProtocolResult ServerUnsupportedPayloadEncodingError{
  status::SERVER_UNSUPPORTED_PAYLOAD_ENCODING};
const ProtocolResult InvalidRequestIdentifierError{status::INVALID_REQUEST_IDENTIFIER};
const ProtocolResult InvalidAsynchronousOperationError{status::INVALID_ASYNCHROUNOUS_OPERATION};
const ProtocolResult ServerProtocolException{status::SERVER_PROTOCOL_EXCEPTION};
const ProtocolResult ClientTransportException{status::CLIENT_TRANSPORT_EXCEPTION};
const ProtocolResult AsynchronousProtocolTimeout{status::ASYNCHRONOUS_PROTOCOL_TIMEOUT};

}  // namespace protocol

}  // namespace sup
