#pragma once

#include <utility>

namespace soul
{
	class AssignNode;
	class BinaryNode;
	class BlockNode;
	class FunctionDeclarationNode;
	class LiteralNode;
	class StructDeclarationNode;
	class UnaryNode;
	class VariableDeclarationNode;

	/**
	 * @brief Base class for visiting nodes in the Abstract Syntax Tree.
	 */
	class Visitor
	{
		public:
			virtual ~Visitor() = default;

			#define DEFINE_VISIT(node)                                                    \
				virtual void visit(node& param) { visit(std::as_const(param)); } \
				virtual void visit(const node& param) {}

			DEFINE_VISIT(AssignNode);
			DEFINE_VISIT(BinaryNode);
			DEFINE_VISIT(BlockNode);
			DEFINE_VISIT(FunctionDeclarationNode);
			DEFINE_VISIT(LiteralNode);
			DEFINE_VISIT(StructDeclarationNode);
			DEFINE_VISIT(UnaryNode);
			DEFINE_VISIT(VariableDeclarationNode);

			#undef DEFINE_VISIT
	};
}  // namespace soul
