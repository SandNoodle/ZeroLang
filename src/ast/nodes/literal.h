#pragma once

#include "ast/ast.h"

#include <cstdint>
#include <string>
#include <variant>
#include <type_traits>

namespace soul
{
	struct boolean_literal_t { bool value; };
	struct float_literal_t { double value; };
	struct function_literal_t { std::string value; };
	struct integer_literal_t { int64_t value; };
	struct string_literal_t { std::string value; };
	struct variable_literal_t { std::string value; };

	template <typename T>
	concept is_literal_t = std::same_as<T, boolean_literal_t> //
		              || std::same_as<T, float_literal_t>     //
		              || std::same_as<T, function_literal_t>  //
		              || std::same_as<T, integer_literal_t>   //
		              || std::same_as<T, variable_literal_t>  //
		              || std::same_as<T, string_literal_t>    //
		              || std::same_as<T, std::monostate>      //
	;

	/**
	 * @brief Represents a 'Literal' expression
	 * in the Abstract Syntax Tree (AST).
	 */
	class literal_t : public ast_node_acceptor_t<literal_t>
	{
		public:
			using null_t = std::monostate;
			using value_t = std::variant<
				null_t,
				boolean_literal_t,
				float_literal_t,
				function_literal_t,
				integer_literal_t,
				variable_literal_t,
				string_literal_t
			>;

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
