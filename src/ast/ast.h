#pragma once

#include "ast/type.h"
#include "ast/visitors/visitable.h"
#include "ast/visitors/visitor.h"

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace soul
{
	/**
	 * @brief Represents a single Node in the Abstract Syntax Tree (AST).
	 */
	class ASTNode : public IVisitable
	{
		public:
		using dependency_t   = std::unique_ptr<ASTNode>;
		using dependencies_t = std::vector<dependency_t>;
		using identifier_t   = std::string;

		protected:
		Type _type{};

		public:
		virtual ~ASTNode() = default;

		/**
		 * @brief Returns underlying type of the node,
		 * or type_t::empty_t otherwise.
		 */
		[[nodiscard]] virtual const Type& type() const noexcept { return _type; }

		/**
		 * @brief Returns underlying type of the node,
		 * or type_t::empty_t otherwise.
		 */
		[[nodiscard]] virtual Type& type() noexcept { return _type; }
	};

	/**
	 * @brief Class that automatically calls accept method on a node.
	 * @tparam DerivedT Type derived from ast_node_t.
	 * @important if DerivedT does not derive from ast_node_t,
	 * its an Undefined Behavior then.
	 */
	template <typename DerivedT>
	class ASTNodeAcceptor : public ASTNode
	{
		private:
		void accept(Visitor& visitor) override { visitor.visit(*static_cast<DerivedT*>(this)); }

		void accept(Visitor& visitor) const override { visitor.visit(*static_cast<const DerivedT*>(this)); }
	};
}  // namespace soul
