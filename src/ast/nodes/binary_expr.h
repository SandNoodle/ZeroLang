#pragma once

#include "ast/ast.h"
#include "ast/type_system/type.h"

namespace soul
{
	class binary_expr_t : public ast_node_t
	{
		private:
			operator_type_t _type;
			ast_node_t::dependency_t _lhs;
			ast_node_t::dependency_t _rhs;
			type_t                  _type{};

		public:

	};
}
