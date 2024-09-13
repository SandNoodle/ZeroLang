#pragma once

#include "ast/ast.h"
#include "common/types.h"

#include <string>
#include <variant>
#include <type_traits>

namespace soul
{
	struct boolean_literal_t { bool value; };
	struct float_literal_t { f64 value; };
	struct function_literal_t { std::string value; };
	struct integer_literal_t { i64 value; };
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
	class LiteralNode : public ASTNodeAcceptor<LiteralNode>
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
			explicit LiteralNode(const value_t& value = null_t{});
			~LiteralNode() override = default;

			/**
			 * @brief Constructs new Literal expression node.
			 * @param 
			 * @return New 'Literal' expression node.
			 */
			static dependency_t create(const value_t& value = null_t{});
	};
}  // namespace soul
