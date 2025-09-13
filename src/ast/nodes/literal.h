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
		Value       value        = {};
		Type        literal_type = {};

		public:
		LiteralNode(Value value, Type literal_type);
		~LiteralNode() override = default;
		operator std::string() const noexcept;

		/**
		 * @brief Constructs new Literal node.
		 * @param type
		 * @param value
		 * @return
		 */
		static Dependency create(Value value, Type literal_type = Type::Unknown);
	};
}  // namespace soul::ast::nodes
