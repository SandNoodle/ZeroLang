#pragma once

#include "ast/ast.h"
#include "core/types.h"

#include <string>
#include <type_traits>
#include <variant>

namespace soul
{
	/**
	 * @brief Represents a 'Literal' expression in the Abstract Syntax Tree (AST).
	 */
	class LiteralNode : public VisitorAcceptor<LiteralNode>
	{
		public:
		using ValueType = std::variant<std::monostate, i64, f64, std::string>;

		public:
		ValueType value = std::monostate{};

		public:
		LiteralNode(ValueType value);
		~LiteralNode() override = default;

		/**
		 * @brief Constructs new Literal node.
		 * @param type
		 * @param value
		 * @return
		 */
		static Dependency create(ValueType value);
	};
}  // namespace soul
