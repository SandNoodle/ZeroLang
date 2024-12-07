#include "ast/visitors/stringify.h"

#include "ast/nodes/assign.h"
#include "ast/nodes/binary.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"

namespace soul
{
	StringifyVisitor::StringifyVisitor(bool print_types) : _print_types(print_types) {}

	std::string StringifyVisitor::string() const { return _ss.str(); }

	void StringifyVisitor::visit(const AssignNode& node)
	{
		_ss << '(';
		accept(node.lhs.get());
		_ss << '=';
		accept(node.rhs.get());
		_ss << ')';
	}

	void StringifyVisitor::visit(const BinaryNode& node)
	{
		_ss << '(';
		accept(node.lhs.get());
		_ss << to_string(node.op);
		accept(node.rhs.get());
		_ss << ')';
	}

	void StringifyVisitor::visit(const BlockNode& node) {}

	void StringifyVisitor::visit(const FunctionDeclarationNode& node)
	{
		//
	}

	void StringifyVisitor::visit(const LiteralNode& node) {}

	void StringifyVisitor::visit(const StructDeclarationNode& node) {}

	void StringifyVisitor::visit(const UnaryNode& node) {}

	void StringifyVisitor::visit(const VariableDeclarationNode& node) {}

#undef PRINT_TYPE

}  // namespace soul
