#pragma once

#include "ast/visitors/visitor.h"
#include "common/types/type.h"
#include "lexer/token.h"

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace soul::ast
{
	/**
	 * @brief IVisitable interface enables visitor traversal of a given class.
	 */
	class IVisitable
	{
		public:
		virtual ~IVisitable() = default;

		virtual void accept(visitors::IVisitor& visitor)       = 0;
		virtual void accept(visitors::IVisitor& visitor) const = 0;

		friend visitors::IVisitor;
	};

	/**
	 * @brief Represents a single Node in the Abstract Syntax Tree (AST).
	 */
	class ASTNode : public IVisitable
	{
		public:
		using Dependency   = std::unique_ptr<ASTNode>;
		using Dependencies = std::vector<Dependency>;
		using Reference    = ASTNode*;
		using References   = std::vector<Reference>;
		using Identifier   = std::string;
		using ScopeBlock   = std::unique_ptr<nodes::BlockNode>;
		enum class Operator : u8;

		public:
		types::Type type = {};

		public:
		virtual ~ASTNode() = default;

		static std::string_view name(const Operator op) noexcept;
		static std::string_view internal_name(const Operator op) noexcept;
		static Operator         as_operator(Token::Type) noexcept;
	};

	/**
	 * @brief VisitorAcceptor is a utility class that simplifies traversing the AST, by calling the correct
	 * `Visitor::visit` method for a given node.
	 * @tparam Node Type that satisfies NodeKind concept.
	 */
	template <nodes::NodeKind Node>
	class VisitorAcceptor : public ASTNode
	{
		private:
		void accept(visitors::IVisitor& visitor) override { visitor.visit(static_cast<Node&>(*this)); }
		void accept(visitors::IVisitor& visitor) const override { visitor.visit(static_cast<const Node&>(*this)); }
	};

	/**
	 * @brief Operator which represents the type of binding relation between nodes.
	 */
	enum class ASTNode::Operator : u8
	{
		Unknown,

		// Assignment
		Assign,
		AddAssign,
		SubAssign,
		MulAssign,
		DivAssign,
		ModAssign,

		// Arithmetic
		Add,
		Sub,
		Mul,
		Div,
		Mod,
		Increment,
		Decrement,

		// Comparison
		Equal,
		NotEqual,
		Greater,
		GreaterEqual,
		Less,
		LessEqual,

		// Logical
		LogicalNot,
		LogicalAnd,
		LogicalOr,
	};
}  // namespace soul::ast
