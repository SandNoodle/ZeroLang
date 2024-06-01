#pragma once

#include "ast/ast.h"
#include "ast/type_system/type.h"

namespace soul
{
	class unary_expr_t : public ast_node_t
	{
		private:
			operator_type_t         _operator;
			ast_node_t::reference_t _node;
			type_t                  _type{};
	};
}
