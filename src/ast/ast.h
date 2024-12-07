#pragma once

#include "ast/visitors/visitor.h"

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace soul
{
	class IVisitable
	{
		public:
		virtual void accept(IVisitor& visitor)       = 0;
		virtual void accept(IVisitor& visitor) const = 0;

		friend IVisitor;
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
		void accept(IVisitor& visitor) override { visitor.visit(static_cast<DerivedT&>(*this)); }
		void accept(IVisitor& visitor) const override { visitor.visit(static_cast<const DerivedT&>(*this)); }
	};
}  // namespace soul
