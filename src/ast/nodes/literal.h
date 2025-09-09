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
		enum class LiteralType : u8
		{
			Unknown,

			Bool,
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
		LiteralType literal_type = {};

		public:
		LiteralNode(Value value, LiteralType literal_type);
		~LiteralNode() override = default;
		operator std::string() const noexcept;

		/**
		 * @brief Constructs new Literal node.
		 * @param type
		 * @param value
		 * @return
		 */
		static Dependency create(Value value, LiteralType literal_type = LiteralType::Unknown);
	};
}  // namespace soul::ast::nodes
