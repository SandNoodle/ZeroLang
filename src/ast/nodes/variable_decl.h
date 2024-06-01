#pragma once

#include "ast/ast.h"

#include <string>

namespace soul
{
	class variable_decl_t : public ast_node_t
	{
		private:
			std::string _identifier;
	};
}
