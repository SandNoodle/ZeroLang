#include <gtest/gtest.h>

#include "ast/type.h"

namespace soul::ut
{
	TEST(TypeSystemTest, DefaultConstructor)
	{
		type_t type;
		ASSERT_TRUE(type.is<type_t::unknown_t>());
	}

	TEST(TypeSystemTest, Copy)
	{
		type_t expected_type((array_type_t(type_t(scalar_type_t::type_float))));
		ASSERT_TRUE(expected_type.is<array_type_t>());
		ASSERT_TRUE(expected_type.get<array_type_t>().type().is<scalar_type_t>());
		ASSERT_EQ(expected_type.get<array_type_t>().type().get<scalar_type_t>(), scalar_type_t::type_float);

		type_t result_type = expected_type;
		ASSERT_TRUE(result_type.is<array_type_t>());
		ASSERT_TRUE(result_type.get<array_type_t>().type().is<scalar_type_t>());
		ASSERT_EQ(result_type.get<array_type_t>().type().get<scalar_type_t>(), scalar_type_t::type_float);

		ASSERT_EQ(expected_type, result_type);
	}

	TEST(TypeSystemTest, Move)
	{
		type_t expected_type(scalar_type_t::type_boolean);
		ASSERT_TRUE(expected_type.is<scalar_type_t>());
		ASSERT_EQ(expected_type.get<scalar_type_t>(), scalar_type_t::type_boolean);

		type_t result_type(struct_type_t({type_t(scalar_type_t::type_integer), type_t(scalar_type_t::type_string)}));
		ASSERT_TRUE(result_type.is<struct_type_t>());
		const auto& types = result_type.get<struct_type_t>().types();
		ASSERT_EQ(types.size(), 2);
		ASSERT_TRUE(types[0].is<scalar_type_t>());
		EXPECT_EQ(types[0].get<scalar_type_t>(), scalar_type_t::type_integer);
		ASSERT_TRUE(types[1].is<scalar_type_t>());
		EXPECT_EQ(types[1].get<scalar_type_t>(), scalar_type_t::type_string);

		result_type = std::move(expected_type);
		ASSERT_TRUE(result_type.is<scalar_type_t>());
		ASSERT_EQ(result_type.get<scalar_type_t>(), scalar_type_t::type_boolean);
	}
}  // namespace soul::ut
