#pragma once

#include "ast/ast.h"
#include "common/value.h"
#include "core/types.h"

namespace soul::ast::nodes
{
	/**
	 * @brief Represents a 'Literal' expression in the Abstract Syntax Tree (AST).
	 */
	class LiteralNode : public VisitorAcceptor<LiteralNode>
	{
		public:
		enum class Type : u8
		{
			Unknown,

			Boolean,
			Char,
			Float32,
			Float64,
			Identifier,
			Int32,
			Int64,
			String,
		};

		public:
		Value value{};
		Type  literal_type{ Type::Unknown };

		public:
		LiteralNode(Value value, Type literal_type);
		~LiteralNode() override = default;
		operator std::string() const noexcept;

		/**
		 * @brief Constructs new Literal node.
		 * @param type Type of stored Value, i.e. identifier, string, int32, etc.
		 * @param value Value associated with the literal.
		 * @return New 'LiteralNode' node.
		 */
		static Dependency create(Value value, Type literal_type);

		static std::string_view internal_name(const Type type) noexcept;
	};
}  // namespace soul::ast::nodes
