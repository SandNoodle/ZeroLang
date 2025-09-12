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
		static Operator         as_operator(Token::Type) noexcept;
	};

	/**
	 * @brief Class that automatically calls accept method on a given AST node.
	 * @tparam DerivedT Type to call visitor on.
	 */
	template <typename DerivedT>
	class VisitorAcceptor : public ASTNode
	{
		private:
		void accept(visitors::IVisitor& visitor) override { visitor.visit(static_cast<DerivedT&>(*this)); }
		void accept(visitors::IVisitor& visitor) const override { visitor.visit(static_cast<const DerivedT&>(*this)); }
	};

	/**
	 * @brief Represents binding relation between nodes.
	 */
	enum class ASTNode::Operator : u8
	{
		Unknown,

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
