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

#include <sup/dto/anytype_helper.h>
#include <sup/protocol/exceptions.h>
#include <sup/protocol/function_protocol_pack.h>

using namespace sup::protocol;

class FunctionProtocolPackTest : public ::testing::Test
{
protected:
  FunctionProtocolPackTest();
  virtual ~FunctionProtocolPackTest();

  sup::dto::AnyValue m_empty;
  sup::dto::AnyValue m_array;
  sup::dto::AnyValue m_scalar;
  sup::dto::AnyValue m_struct;
};

TEST_F(FunctionProtocolPackTest, Boolean)
{
  EXPECT_THROW(FunctionProtocolPack(m_array, "whatever", true), InvalidOperationException);
  EXPECT_THROW(FunctionProtocolPack(m_scalar, "whatever", true), InvalidOperationException);
  EXPECT_THROW(FunctionProtocolPack(m_struct, "field_exists", true), InvalidOperationException);

  EXPECT_NO_THROW(FunctionProtocolPack(m_struct, "new_field", true));
  ASSERT_TRUE(m_struct.HasField("new_field"));
  EXPECT_EQ(m_struct["new_field"], true);

  EXPECT_NO_THROW(FunctionProtocolPack(m_empty, "new_field", true));
  ASSERT_TRUE(m_empty.HasField("new_field"));
  EXPECT_EQ(m_empty["new_field"], true);
}

TEST_F(FunctionProtocolPackTest, String)
{
  const std::string str = "string_value";
  EXPECT_THROW(FunctionProtocolPack(m_array, "whatever", str), InvalidOperationException);
  EXPECT_THROW(FunctionProtocolPack(m_scalar, "whatever", str), InvalidOperationException);
  EXPECT_THROW(FunctionProtocolPack(m_struct, "field_exists", str), InvalidOperationException);

  EXPECT_NO_THROW(FunctionProtocolPack(m_struct, "new_field", str));
  ASSERT_TRUE(m_struct.HasField("new_field"));
  EXPECT_EQ(m_struct["new_field"], str);

  EXPECT_NO_THROW(FunctionProtocolPack(m_empty, "new_field", str));
  ASSERT_TRUE(m_empty.HasField("new_field"));
  EXPECT_EQ(m_empty["new_field"], str);

  // Raw c strings should use the same underlying implementation (not the bool overload!)
  EXPECT_NO_THROW(FunctionProtocolPack(m_empty, "raw", "c string"));
  ASSERT_TRUE(m_empty.HasField("raw"));
  EXPECT_EQ(m_empty["raw"].GetType(), sup::dto::StringType);
  EXPECT_EQ(m_empty["raw"], std::string("c string"));
}

TEST_F(FunctionProtocolPackTest, StringList)
{
  const std::vector<std::string> string_list{ "one", "two", "three" };
  EXPECT_THROW(FunctionProtocolPack(m_array, "whatever", string_list), InvalidOperationException);
  EXPECT_THROW(FunctionProtocolPack(m_scalar, "whatever", string_list), InvalidOperationException);
  EXPECT_THROW(FunctionProtocolPack(m_struct, "field_exists", string_list),
               InvalidOperationException);

  EXPECT_NO_THROW(FunctionProtocolPack(m_struct, "new_field", string_list));
  ASSERT_TRUE(m_struct.HasField("new_field"));
  const auto& field_1 = m_struct["new_field"];
  EXPECT_TRUE(sup::dto::IsArrayValue(field_1));
  EXPECT_EQ(field_1.NumberOfElements(), 3);
  EXPECT_EQ(field_1[0], "one");
  EXPECT_EQ(field_1[1], "two");
  EXPECT_EQ(field_1[2], "three");

  EXPECT_NO_THROW(FunctionProtocolPack(m_empty, "new_field", string_list));
  ASSERT_TRUE(m_empty.HasField("new_field"));
  const auto& field_2 = m_empty["new_field"];
  EXPECT_TRUE(sup::dto::IsArrayValue(field_2));
  EXPECT_EQ(field_2.NumberOfElements(), 3);
  EXPECT_EQ(field_2[0], "one");
  EXPECT_EQ(field_2[1], "two");
  EXPECT_EQ(field_2[2], "three");
}

TEST_F(FunctionProtocolPackTest, AnyValue)
{
  sup::dto::AnyValue anyvalue = {
    { "flag", true },
    { "count", {21, sup::dto::UnsignedInteger32Type }}
  };
  EXPECT_THROW(FunctionProtocolPack(m_array, "whatever", anyvalue), InvalidOperationException);
  EXPECT_THROW(FunctionProtocolPack(m_scalar, "whatever", anyvalue), InvalidOperationException);
  EXPECT_THROW(FunctionProtocolPack(m_struct, "field_exists", anyvalue), InvalidOperationException);
  EXPECT_THROW(FunctionProtocolPack(m_struct, "does_not_matter", m_empty), InvalidOperationException);

  EXPECT_NO_THROW(FunctionProtocolPack(m_struct, "new_field", anyvalue));
  ASSERT_TRUE(m_struct.HasField("new_field"));
  EXPECT_EQ(m_struct["new_field"], anyvalue);

  EXPECT_NO_THROW(FunctionProtocolPack(m_empty, "new_field", anyvalue));
  ASSERT_TRUE(m_empty.HasField("new_field"));
  EXPECT_EQ(m_empty["new_field"], anyvalue);
}

TEST_F(FunctionProtocolPackTest, AnyType)
{
  sup::dto::AnyType anytype = {{
    { "enabled", sup::dto::BooleanType },
    { "counter", sup::dto::UnsignedInteger32Type }
  }, "counter_t"};
  auto json_type = sup::dto::AnyTypeToJSONString(anytype);
  EXPECT_THROW(FunctionProtocolPack(m_array, "whatever", anytype), InvalidOperationException);
  EXPECT_THROW(FunctionProtocolPack(m_scalar, "whatever", anytype), InvalidOperationException);
  EXPECT_THROW(FunctionProtocolPack(m_struct, "field_exists", anytype), InvalidOperationException);

  EXPECT_NO_THROW(FunctionProtocolPack(m_struct, "new_field", anytype));
  ASSERT_TRUE(m_struct.HasField("new_field"));
  EXPECT_EQ(m_struct["new_field"], json_type);

  EXPECT_NO_THROW(FunctionProtocolPack(m_empty, "new_field", anytype));
  ASSERT_TRUE(m_empty.HasField("new_field"));
  EXPECT_EQ(m_empty["new_field"], json_type);
}

FunctionProtocolPackTest::FunctionProtocolPackTest()
  : m_empty{}
  , m_array{sup::dto::AnyType{0, sup::dto::StringType}}
  , m_scalar{sup::dto::UnsignedInteger8Type}
  , m_struct{ {"field_exists", "doesntmatter"}}
{}

FunctionProtocolPackTest::~FunctionProtocolPackTest() = default;
