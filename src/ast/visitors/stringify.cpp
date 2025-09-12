#include "ast/visitors/stringify.h"

#include "ast/nodes/binary.h"
#include "ast/nodes/block.h"
#include "ast/nodes/cast.h"
#include "ast/nodes/error.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/if.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"

#include <format>

namespace soul::ast::visitors
{
	using namespace soul::ast::nodes;

	// TODO: Prettify the JSON?

	static constexpr auto k_unnamed = "__unnamed__";

	StringifyVisitor::StringifyVisitor() {}

	std::string StringifyVisitor::string() const { return _ss.str(); }

	void StringifyVisitor::accept(const ASTNode::Reference node)
	{
		if (!node) {
			_ss << "\"__nullptr__\"";
			return;
		}
		_ss << '{';
		node->accept(*this);
		_ss << '}';
	}

	void StringifyVisitor::visit(const BinaryNode& node)
	{
		_ss << "\"type\":\"binary\",";
		_ss << "\"lhs\":";
		accept(node.lhs.get());
		_ss << ',';
		_ss << "\"operator\":\"" << ASTNode::name(node.op) << "\",";
		_ss << "\"rhs\":";
		accept(node.rhs.get());
	}

	void StringifyVisitor::visit(const nodes::BlockNode& node)
	{
		_ss << "\"type\":\"scope_block\",";
		_ss << "\"statements\":[";
		for (size_t index = 0; index < node.statements.size(); ++index) {
			accept(node.statements[index].get());
			if (index != node.statements.size() - 1) {
				_ss << ',';
			}
		}
		_ss << "]";
	}

	void StringifyVisitor::visit(const CastNode& node)
	{
		_ss << "\"type\":\"cast\"";
		_ss << ",\"type_identifier\":\"" << (!node.type_identifier.empty() ? node.type_identifier : k_unnamed) << "\"";
		_ss << ",\"expression\":";
		accept(node.expression.get());
	}

	void StringifyVisitor::visit(const ErrorNode& node)
	{
		_ss << "\"type\":\"error\",";
		_ss << "\"message\":\"" << node.message << "\"";
	}

	void StringifyVisitor::visit(const ForLoopNode& node)
	{
		_ss << "\"type\":\"for_loop\",";
		_ss << "\"initialization\":";
		accept(node.initialization.get());
		_ss << ",\"condition\":";
		accept(node.condition.get());
		_ss << ",\"update\":";
		accept(node.update.get());
		_ss << ",\"statements\":";
		accept(node.statements.get());
	}

	void StringifyVisitor::visit(const ForeachLoopNode& node)
	{
		_ss << "\"type\":\"foreach_loop\",";
		_ss << "\"variable\":";
		accept(node.variable.get());
		_ss << ",\"in_expression\":";
		accept(node.in_expression.get());
		_ss << ",\"statements\":";
		accept(node.statements.get());
	}

	void StringifyVisitor::visit(const FunctionDeclarationNode& node)
	{
		_ss << "\"type\":\"function_declaration\",";
		_ss << "\"name\":\"" << (!node.name.empty() ? node.name : k_unnamed) << "\",";
		_ss << "\"return_type\":\"" << (!node.return_type.empty() ? node.return_type : k_unnamed) << "\",";
		_ss << "\"parameters\":[";
		for (size_t index = 0; index < node.parameters.size(); ++index) {
			accept(node.parameters[index].get());
			if (index != node.parameters.size() - 1) {
				_ss << ',';
			}
		}
		_ss << "],";
		_ss << "\"statements\":";
		accept(node.statements.get());
	}

	void StringifyVisitor::visit(const IfNode& node)
	{
		_ss << "\"type\":\"if\",";
		_ss << "\"expression\":";
		accept(node.condition.get());
		_ss << ",\"if_statements\":";
		accept(node.if_statements.get());
		_ss << ",\"else_statements\":";
		accept(node.else_statements.get());
	}

	void StringifyVisitor::visit(const LiteralNode& node) { _ss << std::format("\"value\":\"{}\"", std::string(node)); }

	void StringifyVisitor::visit(const ModuleNode& node)
	{
		_ss << "\"type\":\"module_declaration\",";
		_ss << "\"name\":\"" << (!node.name.empty() ? node.name : k_unnamed) << "\",";
		_ss << "\"statements\":[";
		for (size_t index = 0; index < node.statements.size(); ++index) {
			accept(node.statements[index].get());
			if (index != node.statements.size() - 1) {
				_ss << ",";
			}
		}
		_ss << "]";
	}

	void StringifyVisitor::visit(const StructDeclarationNode& node)
	{
		_ss << "\"type\":\"struct_declaration\",";
		_ss << "\"name\":\"" << (!node.name.empty() ? node.name : k_unnamed) << "\",";
		_ss << "\"parameters\":[";
		for (size_t index = 0; index < node.parameters.size(); ++index) {
			accept(node.parameters[index].get());
			if (index != node.parameters.size() - 1) {
				_ss << ",";
			}
		}
		_ss << "]";
	}

	void StringifyVisitor::visit(const UnaryNode& node)
	{
		_ss << "\"type\":\"unary\",";
		_ss << "\"operator\":\"" << ASTNode::name(node.op) << "\",";
		_ss << "\"expression\":";
		accept(node.expr.get());
	}

	void StringifyVisitor::visit(const VariableDeclarationNode& node)
	{
		_ss << "\"type\":\"variable_declaration\",";
		_ss << "\"name\":\"" << (!node.name.empty() ? node.name : k_unnamed) << "\",";
		_ss << "\"type_identifier\":\"" << (!node.type_identifier.empty() ? node.type_identifier : k_unnamed) << "\",";
		_ss << "\"is_mutable\":" << std::boolalpha << node.is_mutable << ",";
		_ss << "\"expression\":";
		accept(node.expr.get());
	}

#undef PRINT_TYPE

}  // namespace soul::ast::visitors
