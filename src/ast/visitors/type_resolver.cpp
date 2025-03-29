#include "ast/visitors/type_resolver.h"

#include "ast/nodes/assign.h"
#include "ast/nodes/binary.h"
#include "ast/nodes/cast.h"
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

#include <unordered_set>

namespace soul::ast::visitors
{
	using namespace soul::ast::nodes;
	using namespace soul::ast::types;

	TypeResolverVisitor::TypeResolverVisitor() { register_basic_types(); }

	void TypeResolverVisitor::visit(AssignNode& node)
	{
		accept(node.lhs.get());
		accept(node.rhs.get());
		node.type = node.lhs->type;
	}

	void TypeResolverVisitor::visit(BinaryNode& node)
	{
		accept(node.lhs.get());
		accept(node.rhs.get());

		// TODO What should be the type of the binary node? Common type to both expressions?
		//      Does it need further resolving?
	}

	void TypeResolverVisitor::visit(nodes::CastNode& node)
	{
		accept(node.expression.get());
		if (!_registered_types.contains(node.type_identifier)) {
			diagnostic(DiagnosticType::Error, DiagnosticCode::TypeResolverTypeUnknown, node.type_identifier);
			return;
		}

		node.type = _registered_types[node.type_identifier];
	}

	void TypeResolverVisitor::visit(ForLoopNode& node)
	{
		accept(node.initialization.get());
		accept(node.condition.get());
		accept(node.update.get());
		for (auto& statement : node.statements) {
			accept(statement.get());
		}
		node.type = PrimitiveType::Kind::Unknown;
	}

	void TypeResolverVisitor::visit(ForeachLoopNode& node)
	{
		accept(node.variable.get());
		accept(node.in_expression.get());
		for (auto& statement : node.statements) {
			accept(statement.get());
		}
		node.type = PrimitiveType::Kind::Unknown;
	}

	void TypeResolverVisitor::visit(FunctionDeclarationNode& node)
	{
		if (_registered_types.contains(node.return_type)) {
			node.type = _registered_types[node.return_type];
		} else {
			// TODO: Should this be an Error diagnostic, but don't prevent further type resolving?
		}

		for (auto& parameter : node.parameters) {
			accept(parameter.get());
		}

		for (auto& statement : node.statements) {
			accept(statement.get());
		}
	}

	void TypeResolverVisitor::visit(IfNode& node)
	{
		accept(node.condition.get());
		for (auto& statement : node.if_statements) {
			accept(statement.get());
		}
		for (auto& statement : node.else_statements) {
			accept(statement.get());
		}
		node.type = PrimitiveType::Kind::Unknown;
	}

	void TypeResolverVisitor::visit(LiteralNode& node)
	{
		if (node.value.is<Value::UnknownValue>()) {
			using namespace std::string_view_literals;
			// TODO Error - we cannot infer the value type of Value::UnknownValue.
			diagnostic(DiagnosticType::Error, DiagnosticCode::TypeResolverCannotInferUnknownType);
			return;
		}

		if (node.value.is<bool>()) {
			node.type = PrimitiveType::Kind::Boolean;
			return;
		}

		if (node.value.is<i32>()) {
			node.type = PrimitiveType::Kind::Int32;
			return;
		}

		if (node.value.is<i64>()) {
			node.type = PrimitiveType::Kind::Int64;
			return;
		}

		if (node.value.is<f32>()) {
			node.type = PrimitiveType::Kind::Float32;
			return;
		}

		if (node.value.is<f64>()) {
			node.type = PrimitiveType::Kind::Float64;
			return;
		}

		if (node.value.is<std::string>()) {
			node.type = PrimitiveType::Kind::String;
			return;
		}

		if (node.value.is<char>()) {
			node.type = PrimitiveType::Kind::Char;
			return;
		}

		// TODO: Debug Error - could not infer value to type. Did you forget to update this here?
	}

	void TypeResolverVisitor::visit(ModuleNode& node)
	{
		// NOTE: Modules are typeless by default.
		node.type = PrimitiveType::Kind::Unknown;
	}

	void TypeResolverVisitor::visit(StructDeclarationNode& node)
	{
		for (auto& parameter : node.parameters) {
			accept(parameter.get());
		}

		if (_registered_types.contains(node.name)) {
			diagnostic(DiagnosticType::Error, DiagnosticCode::TypeResolverTypeRedefined, node.name);
			return;
		}

		StructType::ContainedTypes sub_types;
		for (const auto& subtype : node.parameters) {
			sub_types.push_back(subtype->type);
		}
		register_type(node.name, Type(StructType(std::move(sub_types))));
	}

	void TypeResolverVisitor::visit(UnaryNode& node) { node.type = node.expr->type; }

	void TypeResolverVisitor::visit(VariableDeclarationNode& node)
	{
		if (_registered_types.contains(node.type_identifier)) {
			node.type = _registered_types[node.type_identifier];
		}

		accept(node.expr.get());
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
		std::ignore = register_type("bool", Type{ PrimitiveType::Kind::Boolean });
		std::ignore = register_type("chr", Type{ PrimitiveType::Kind::Char });
		std::ignore = register_type("f32", Type{ PrimitiveType::Kind::Float32 });
		std::ignore = register_type("f64", Type{ PrimitiveType::Kind::Float64 });
		std::ignore = register_type("i32", Type{ PrimitiveType::Kind::Int32 });
		std::ignore = register_type("i64", Type{ PrimitiveType::Kind::Int64 });
		std::ignore = register_type("str", Type{ PrimitiveType::Kind::String });
		std::ignore = register_type("void", Type{ PrimitiveType::Kind::Void });
	}
}  // namespace soul::ast::visitors
