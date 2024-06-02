#pragma once

#include <utility>

namespace soul
{
	class assign_t;
	class binary_t;
	class block_t;
	class function_declaration_t;
	class literal_t;
	class struct_declaration_t;
	class unary_t;
	class variable_declaration_t;

	/**
	 * @brief Base class for visiting nodes in the Abstract Syntax Tree.
	 */
	class visitor_t
	{
		public:
			virtual ~visitor_t() = default;

			#define DEFINE_VISIT(node)                                                    \
				virtual void visit(node& param) { visit(std::as_const(param)); } \
				virtual void visit(const node& param) {}

			DEFINE_VISIT(assign_t);
			DEFINE_VISIT(binary_t);
			DEFINE_VISIT(block_t);
			DEFINE_VISIT(function_declaration_t);
			DEFINE_VISIT(literal_t);
			DEFINE_VISIT(struct_declaration_t);
			DEFINE_VISIT(unary_t);
			DEFINE_VISIT(variable_declaration_t);

			#undef DEFINE_VISIT
	};
}  // namespace soul
