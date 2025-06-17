#pragma once

#include "common/types/type.h"
#include "ast/visitors/visitor.h"

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace soul::ast
{
	class IVisitable
	{
		public:
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

		public:
		types::Type type = {};

		public:
		virtual ~ASTNode() = default;
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
}  // namespace soul::ast
