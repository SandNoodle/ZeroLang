#pragma once

#include "ast/ast.h"

#include <cstdint>
#include <string>
#include <variant>
#include <type_traits>

namespace soul
{
	struct variable_value_t { std::string identifier; };
	struct function_value_t { std::string identifier; };
	struct integer_value_t { int64_t value; };
	struct float_value_t { double value; };

	template <typename T>
	concept is_value_t = std::same_as<T, variable_value_t>  //
		              || std::same_as<T, function_value_t>  //
		              || std::same_as<T, integer_value_t>  //
		              || std::same_as<T, float_value_t>  //
		              || std::same_as<T, std::monostate>  //
	;

	/**
	 * @brief Represents a 'Literal' expression
	 * in the Abstract Syntax Tree (AST).
	 */
	class literal_t : public ast_node_acceptor_t<literal_t>
	{
		public:
			using null_t = std::monostate;
			using value_t = std::variant<null_t, variable_value_t, function_value_t, integer_value_t, float_value_t>;

		private:
			value_t _value = null_t{};

		public:
			explicit literal_t(const value_t& value = null_t{});
			~literal_t() override = default;

			/**
			 * @brief Constructs new Literal expression node.
			 * @param 
			 * @return New 'Literal' expression node.
			 */
			static dependency_t create(const value_t& value = null_t{});
	};
}  // namespace soul
