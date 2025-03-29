#include "ast/visitors/type_resolver.h"

#include "ast/nodes/assign.h"
#include "ast/nodes/binary.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/function_declaration.h"
#include "ast/nodes/if.h"
#include "ast/nodes/literal.h"
#include "ast/nodes/module.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/unary.h"
#include "ast/nodes/variable_declaration.h"
#include "ast/types/type.h"

namespace soul::ast::visitors
{
	using namespace soul::ast::nodes;
	using namespace soul::ast::types;

	TypeResolverVisitor::TypeResolverVisitor() { register_basic_types(); }

	void TypeResolverVisitor::visit(AssignNode& node)
	{
		resolve(node.lhs.get());
		resolve(node.rhs.get());
	}

	void TypeResolverVisitor::visit(BinaryNode& node)
	{
		resolve(node.lhs.get());
		resolve(node.rhs.get());
	}

	void TypeResolverVisitor::visit(ForLoopNode& node)
	{
		resolve(node.initialization.get());
		resolve(node.condition.get());
		resolve(node.update.get());
		for (auto& statement : node.statements) {
			resolve(statement.get());
		}
	}

	void TypeResolverVisitor::visit(ForeachLoopNode& node)
	{
		resolve(node.variable.get());
		resolve(node.in_expression.get());
		for (auto& statement : node.statements) {
			resolve(statement.get());
		}
	}

	void TypeResolverVisitor::visit(FunctionDeclarationNode& node)
	{
		// TODO: Verify that the function's return type is registered.

		for (auto& statement : node.parameters) {
			resolve(statement.get());
			// TODO: Verify that the names are unique (different from each other).
		}

		// TODO: Verify that the function types are unique.

		for (auto& statement : node.statements) {
			resolve(statement.get());
		}

		// TODO: Add the function to the list of function for this module.
	}

	void TypeResolverVisitor::visit(IfNode& node)
	{
		resolve(node.condition.get());
		for (auto& statement : node.if_statements) {
			resolve(statement.get());
		}
		for (auto& statement : node.else_statements) {
			resolve(statement.get());
		}
	}

	void TypeResolverVisitor::visit(LiteralNode& node)
	{
		// TODO: Infer value to Type.
	}

	void TypeResolverVisitor::visit(ModuleNode& node) { /* NOTE: Modules are typeless by default. */ }

	void TypeResolverVisitor::visit(StructDeclarationNode& node)
	{
		for (auto& parameter : node.parameters) {
			accept(parameter.get());
		}

		if (_registered_types.contains(node.name)) {
			// ERROR: CALLBACK.
			return;
		}

		StructType::ContainedTypes sub_types;
		for (const auto& subtype : node.parameters) {
			sub_types.push_back(subtype->type);
		}
		register_type(node.name, Type(StructType(std::move(sub_types))));
	}

	void TypeResolverVisitor::visit(UnaryNode& node) {}

	void TypeResolverVisitor::visit(VariableDeclarationNode& node)
	{
		if (_registered_types.contains(node.type_identifier)) {
			//			node.type = _registered_types.at(node.type_identifier);
		}

		accept(node.expr.get());

		// TODO: Verify that the expression's type matches the variable declared type.
	}

	void TypeResolverVisitor::resolve(ASTNode::Reference node)
	{
		if (!node) {
			return;
		}
	}

	bool TypeResolverVisitor::register_type(const std::string& name, Type&& type)
	{
		if (_registered_types.contains(name)) {
			return false;
		}
		_registered_types[name] = std::move(type);
		return true;
	}

	void TypeResolverVisitor::register_basic_types()
	{
		std::ignore = register_type("void", Type{ PrimitiveType::Kind::Void });
		std::ignore = register_type("i32", Type{ PrimitiveType::Kind::Int32 });
		std::ignore = register_type("i64", Type{ PrimitiveType::Kind::Int64 });
		std::ignore = register_type("f32", Type{ PrimitiveType::Kind::Float32 });
		std::ignore = register_type("f64", Type{ PrimitiveType::Kind::Float64 });
		std::ignore = register_type("str", Type{ PrimitiveType::Kind::String });
		std::ignore = register_type("chr", Type{ PrimitiveType::Kind::Char });
	}
}  // namespace soul::ast::visitors
