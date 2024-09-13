#include <gtest/gtest.h>

#include "ast/type.h"

namespace soul::ut
{
	TEST(TypeSystemTest, DefaultConstructor)
	{
		Type type;
		ASSERT_TRUE(type.is<Type::unknown_t>());
	}

	TEST(TypeSystemTest, Copy)
	{
		Type expected_type((ArrayType(Type(ScalarType::type_float))));
		ASSERT_TRUE(expected_type.is<ArrayType>());
		ASSERT_TRUE(expected_type.get<ArrayType>().type().is<ScalarType>());
		ASSERT_EQ(expected_type.get<ArrayType>().type().get<ScalarType>(), ScalarType::type_float);

		Type result_type = expected_type;
		ASSERT_TRUE(result_type.is<ArrayType>());
		ASSERT_TRUE(result_type.get<ArrayType>().type().is<ScalarType>());
		ASSERT_EQ(result_type.get<ArrayType>().type().get<ScalarType>(), ScalarType::type_float);

		ASSERT_EQ(expected_type, result_type);
	}

	TEST(TypeSystemTest, Move)
	{
		Type expected_type(ScalarType::type_boolean);
		ASSERT_TRUE(expected_type.is<ScalarType>());
		ASSERT_EQ(expected_type.get<ScalarType>(), ScalarType::type_boolean);

		Type result_type(StructType({ Type(ScalarType::type_integer), Type(ScalarType::type_string) }));
		ASSERT_TRUE(result_type.is<StructType>());
		const auto& types = result_type.get<StructType>().types();
		ASSERT_EQ(types.size(), 2);
		ASSERT_TRUE(types[0].is<ScalarType>());
		EXPECT_EQ(types[0].get<ScalarType>(), ScalarType::type_integer);
		ASSERT_TRUE(types[1].is<ScalarType>());
		EXPECT_EQ(types[1].get<ScalarType>(), ScalarType::type_string);

		result_type = std::move(expected_type);
		ASSERT_TRUE(result_type.is<ScalarType>());
		ASSERT_EQ(result_type.get<ScalarType>(), ScalarType::type_boolean);
	}
}  // namespace soul::ut
