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

#include <ranges>

namespace soul::ast::visitors
{
	using namespace soul::ast::nodes;

	StringifyVisitor::StringifyVisitor() {}

	std::string StringifyVisitor::string() const { return _ss.str(); }

	void StringifyVisitor::accept(const ASTNode::Reference node)
	{
		if (!node) {
			_ss << "null";
			return;
		}

		_indent_level += k_indent_amount;
		_ss << "{\n";

		node->accept(*this);

		_indent_level -= k_indent_amount;
		_ss << std::format("\n{}}}", std::string(_indent_level, ' '));
	}

	void StringifyVisitor::visit(const BinaryNode& node)
	{
		encode("type", "binary");
		encode("operator", ASTNode::internal_name(node.op));
		encode("lhs", node.lhs.get());
		encode("rhs", node.rhs.get(), false);
	}

	void StringifyVisitor::visit(const nodes::BlockNode& node)
	{
		encode("type", "scope_block");
		encode("statements",
		       node.statements | std::views::transform([](const auto& e) -> ASTNode::Reference { return e.get(); })
		           | std::ranges::to<ASTNode::References>(),
		       false);
	}

	void StringifyVisitor::visit(const CastNode& node)
	{
		encode("type", "cast");
		encode("type_identifier", node.type_identifier);
		encode("expression", node.expression.get(), false);
	}

	void StringifyVisitor::visit(const ErrorNode& node)
	{
		encode("type", "error");
		encode("message", node.message, false);
	}

	void StringifyVisitor::visit(const ForLoopNode& node)
	{
		encode("type", "for_loop");
		encode("initialization", node.initialization.get());
		encode("condition", node.condition.get());
		encode("update", node.update.get());
		encode("statements", node.statements.get(), false);
	}

	void StringifyVisitor::visit(const ForeachLoopNode& node)
	{
		encode("type", "foreach_loop");
		encode("variable", node.variable.get());
		encode("in_expression", node.in_expression.get());
		encode("statements", node.statements.get(), false);
	}

	void StringifyVisitor::visit(const FunctionDeclarationNode& node)
	{
		encode("type", "function_declaration");
		encode("name", node.name);
		encode("type_identifier", node.type_identifier);
		encode("parameters", node.parameters | std::views::transform([](const auto& e) -> ASTNode::Reference {
								 return e.get();
							 }) | std::ranges::to<ASTNode::References>());
		encode("statements", node.statements.get(), false);
	}

	void StringifyVisitor::visit(const IfNode& node)
	{
		encode("type", "if");
		encode("expression", node.condition.get());
		encode("if_statements", node.if_statements.get());
		encode("else_statements", node.else_statements.get(), false);
	}

	void StringifyVisitor::visit(const LiteralNode& node)
	{
		encode("type", "literal");
		encode("literal_type", LiteralNode::internal_name(node.literal_type));
		encode("value", std::string(node), false);
	}

	void StringifyVisitor::visit(const ModuleNode& node)
	{
		encode("type", "module_declaration");
		encode("name", node.name);
		encode("statements",
		       node.statements | std::views::transform([](const auto& e) -> ASTNode::Reference { return e.get(); })
		           | std::ranges::to<ASTNode::References>(),
		       false);
	}

	void StringifyVisitor::visit(const StructDeclarationNode& node)
	{
		encode("type", "struct_declaration");
		encode("name", node.name);
		encode("parameters",
		       node.parameters | std::views::transform([](const auto& e) -> ASTNode::Reference { return e.get(); })
		           | std::ranges::to<ASTNode::References>(),
		       false);
	}

	void StringifyVisitor::visit(const UnaryNode& node)
	{
		encode("type", "unary");
		encode("operator", ASTNode::internal_name(node.op));
		encode("expression", node.expression.get(), false);
	}

	void StringifyVisitor::visit(const VariableDeclarationNode& node)
	{
		encode("type", "variable_declaration");
		encode("name", node.name);
		encode("type_identifier", node.type_identifier);
		encode("is_mutable", node.is_mutable ? "true" : "false");
		encode("expression", node.expression.get(), false);
	}

	std::string StringifyVisitor::current_indent() const { return std::string(_indent_level, ' '); }

	void StringifyVisitor::encode(std::string_view key, std::string_view value, bool add_trailing_comma)
	{
		_ss << current_indent();
		_ss << std::format("\"{}\": \"{}\"", key, !value.empty() ? value : k_unnamed);
		if (add_trailing_comma) {
			_ss << ",\n";
		}
	}

	void StringifyVisitor::encode(std::string_view key, const ASTNode::Reference node, bool add_trailing_comma)
	{
		_ss << current_indent();
		_ss << std::format("\"{}\": ", key);
		accept(node);
		if (add_trailing_comma) {
			_ss << ",\n";
		}
	}

	void StringifyVisitor::encode(std::string_view key, const ASTNode::References& parameters, bool add_trailing_comma)
	{
		_ss << current_indent();

		if (parameters.empty()) {
			_ss << std::format("\"{}\": []", key);
		} else {
			_indent_level += k_indent_amount;

			_ss << std::format("\"{}\": [\n", key);
			_ss << current_indent();
			for (std::size_t index = 0; index < parameters.size(); ++index) {
				accept(parameters[index]);
				if (index != parameters.size() - 1) {
					_ss << std::format(",\n{}", current_indent());
				}
			}

			_indent_level -= k_indent_amount;
			_ss << std::format("\n{}]", current_indent());
		}

		if (add_trailing_comma) {
			_ss << ",\n";
		}
	}
}  // namespace soul::ast::visitors
