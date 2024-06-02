#pragma once

#include "ast/type.h"
#include "ast/visitors/visitable.h"
#include "ast/visitors/visitor.h"

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace soul
{
	/**
	 * @brief Represents a single Node in the Abstract Syntax Tree (AST).
	 */
	class ast_node_t : public visitable_t
	{
		public:
			using dependency_t = std::unique_ptr<ast_node_t>;
			using dependencies_t = std::vector<dependency_t>;
			using identifier_t = std::string;

		protected:
			type_t _type{};

		public:
			virtual ~ast_node_t() = default;

			/**
			 * @brief Returns underlying type of the node,
			 * or type_t::empty_t otherwise.
			 */
			virtual const type_t& type() const noexcept
			{
				return _type;
			}

			/**
			 * @brief Returns underlying type of the node,
			 * or type_t::empty_t otherwise.
			 */
			virtual type_t& type() noexcept
			{
				return _type;
			}
	};

    /**
     * @brief Class that automatically calls accept method on a node.
     * @tparam DerivedT Type derived from ast_node_t.
	 * @important if DerivedT does not derive from ast_node_t,
	 * its an Undefined Behavior then.
     */
    template <typename DerivedT>
    class ast_node_acceptor_t : public ast_node_t
    {
    private:
        virtual void accept(visitor_t& visitor) override
        {
            visitor.visit(*static_cast<DerivedT*>(this));
        }

        virtual void accept(visitor_t& visitor) const override
        {
            visitor.visit(*static_cast<const DerivedT*>(this));
        }
    };
}  // namespace soul
