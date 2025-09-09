#include "ast/visitors/type_resolver.h"

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
#include "common/types/type.h"

#include <format>

namespace soul::ast::visitors
{
	using namespace ast::nodes;
	using namespace ast;
	using namespace types;

	CastNode::Type get_cast_type(const Type& from_type, const Type& to_type);

	TypeResolverVisitor::TypeResolverVisitor(TypeMap type_map, Options options)
		: _registered_types(std::move(type_map)), _options(options)
	{
	}

	void TypeResolverVisitor::visit(BinaryNode& node)
	{
		accept(node.lhs.get());
		accept(node.rhs.get());

		_current_clone       = clone(node);
		_current_clone->type = PrimitiveType::Kind::Void;  // TODO
	}

	void TypeResolverVisitor::visit(BlockNode& node)
	{
		// ENTER SCOPE: Mark a restorepoint for variables declared in the current scope.
		const std::size_t variables_until_this_point = _variables_in_scope.size();

		for (auto& statement : node.statements) {
			accept(statement.get());
		}

		// EXIT SCOPE: Remove variables defined in that scope.
		const std::size_t variables_declared_in_scope = _variables_in_scope.size() - variables_until_this_point;
		_variables_in_scope.erase(_variables_in_scope.end() - static_cast<std::ptrdiff_t>(variables_declared_in_scope),
		                          _variables_in_scope.end());

		_current_clone       = clone(node);
		_current_clone->type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(nodes::CastNode& node)
	{
		accept(node.expression.get());

		_current_clone       = clone(node);
		_current_clone->type = get_type_or_default(node.type_identifier);
	}

	void TypeResolverVisitor::visit(ForLoopNode& node)
	{
		accept(node.initialization.get());
		accept(node.condition.get());
		accept(node.update.get());
		accept(node.statements.get());

		_current_clone       = clone(node);
		_current_clone->type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(ForeachLoopNode& node)
	{
		accept(node.variable.get());
		accept(node.in_expression.get());
		accept(node.statements.get());

		_current_clone       = clone(node);
		_current_clone->type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(FunctionDeclarationNode& node)
	{
		for (auto& parameter : node.parameters) {
			accept(parameter.get());
		}
		accept(node.statements.get());

		_current_clone       = clone(node);
		_current_clone->type = get_type_or_default(node.return_type);
	}

	void TypeResolverVisitor::visit(IfNode& node)
	{
		accept(node.condition.get());
		accept(node.if_statements.get());
		accept(node.else_statements.get());

		_current_clone       = clone(node);
		_current_clone->type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(LiteralNode& node)
	{
		_current_clone = clone(node);

		if (node.literal_type == LiteralNode::LiteralType::Unknown) {
			_current_clone->type = PrimitiveType::Kind::Unknown;
			return;
		}

		if (node.literal_type == LiteralNode::LiteralType::Bool) {
			_current_clone->type = PrimitiveType::Kind::Boolean;
			return;
		}

		if (node.literal_type == LiteralNode::LiteralType::Float32) {
			_current_clone->type = PrimitiveType::Kind::Float32;
			return;
		}

		if (node.literal_type == LiteralNode::LiteralType::Float64) {
			_current_clone->type = PrimitiveType::Kind::Float64;
			return;
		}

		if (node.literal_type == LiteralNode::LiteralType::Identifier) {
			_current_clone->type = PrimitiveType::Kind::Unknown;  // TODO
			return;
		}

		if (node.literal_type == LiteralNode::LiteralType::Int32) {
			_current_clone->type = PrimitiveType::Kind::Int32;
			return;
		}

		if (node.literal_type == LiteralNode::LiteralType::Int64) {
			_current_clone->type = PrimitiveType::Kind::Int64;
			return;
		}

		if (node.literal_type == LiteralNode::LiteralType::String) {
			_current_clone->type = PrimitiveType::Kind::String;
			return;
		}

		_current_clone->type = PrimitiveType::Kind::Unknown;
	}

	void TypeResolverVisitor::visit(const ModuleNode& node)
	{
		// NOTE: Modules are a collection of type declarations and functions, thus don't have their own type.
		_current_clone       = clone(node);
		_current_clone->type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(StructDeclarationNode& node)
	{
		_current_clone = clone(node);

		for (auto& parameter : node.parameters) {
			accept(parameter.get());
		}
		node.type = get_type_or_default(node.name);
	}

	void TypeResolverVisitor::visit(UnaryNode& node)
	{
		_current_clone = clone(node);

		// TODO
	}

	void TypeResolverVisitor::visit(VariableDeclarationNode& node)
	{
		if (is_variable_declared(node.name)) {
			_current_clone
				= ErrorNode::create(std::format("variable declaration '{}' is being shadowed by a new one", node.name));
			return;
		}

		_variables_in_scope.emplace_back(std::make_pair(node.name, &node));

		accept(node.expr.get());

		_current_clone       = clone(node);
		_current_clone->type = get_type_or_default(node.type_identifier);
	}

	CastNode::Type get_cast_type(const Type& from_type, const Type& to_type)
	{
		static const std::unordered_map<PrimitiveType::Kind, std::unordered_map<PrimitiveType::Kind, CastNode::Type>>
			k_cast_type = {
				{
                 PrimitiveType::Kind::Boolean,
                 {
						{ PrimitiveType::Kind::Boolean, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::Int32, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::Int64, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::String, CastNode::Type::Explicit },
					}  //
				},
				{
                 PrimitiveType::Kind::Char,
                 {
						{ PrimitiveType::Kind::Char, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::String, CastNode::Type::Implicit },
					}  //
				},
				{
                 PrimitiveType::Kind::Float32,
                 {
						{ PrimitiveType::Kind::Float32, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::Float64, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::Int32, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::Int64, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::String, CastNode::Type::Explicit },
					}  //
				},
				{
                 PrimitiveType::Kind::Float64,
                 {
						{ PrimitiveType::Kind::Float32, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::Float64, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::Int32, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::Int64, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::String, CastNode::Type::Explicit },
					}  //
				},
				{
                 PrimitiveType::Kind::Int32,
                 {
						{ PrimitiveType::Kind::Boolean, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::Float32, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::Float64, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::Int32, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::Int64, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::String, CastNode::Type::Explicit },
					}  //
				},
				{
                 PrimitiveType::Kind::Int64,
                 {
						{ PrimitiveType::Kind::Boolean, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::Float32, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::Float64, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::Int32, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::Int64, CastNode::Type::Implicit },
						{ PrimitiveType::Kind::String, CastNode::Type::Explicit },
					}  //
				},
				{
                 PrimitiveType::Kind::String,
                 {
						{ PrimitiveType::Kind::Float32, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::Float64, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::Int32, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::Int64, CastNode::Type::Explicit },
						{ PrimitiveType::Kind::String, CastNode::Type::Implicit },
					}  //
				},
        };

		// NOTE: If the types are equivalent, no casts should take place.
		if (from_type == to_type) {
			return CastNode::Type::Implicit;
		}

		if (from_type.is<PrimitiveType>() && to_type.is<PrimitiveType>()) {
			const auto from = from_type.as<PrimitiveType>().type;
			const auto to   = to_type.as<PrimitiveType>().type;
			// NOTE: We cant cast the types we haven't resolved yet.
			if (from == PrimitiveType::Kind::Unknown || to == PrimitiveType::Kind::Unknown) {
				return CastNode::Type::Impossible;
			}

			if (k_cast_type.contains(from) && k_cast_type.at(from).contains(to)) {
				return k_cast_type.at(from).at(to);
			}

			// NOTE: Assume that undefined casts are impossible.
			return CastNode::Type::Impossible;
		}

		if (from_type.is<ArrayType>() && to_type.is<ArrayType>()) {
			// Arrays can be cast only if their data types are castable.
			return get_cast_type(from_type.as<ArrayType>().data_type(), to_type.as<ArrayType>().data_type());
		}

		if (from_type.is<StructType>() || to_type.is<StructType>()) {
			return CastNode::Type::Impossible;
		}

		// NOTE: There was a missmatch between the types.
		return CastNode::Type::Impossible;
	}

	types::Type TypeResolverVisitor::get_type_or_default(std::string_view type_identifier) const noexcept
	{
		if (_registered_types.contains(type_identifier)) {
			return _registered_types.at(type_identifier);
		}
		return types::Type{};
	}

	bool TypeResolverVisitor::is_variable_declared(std::string_view name) const noexcept
	{
		return std::ranges::find(_variables_in_scope, name, &decltype(_variables_in_scope)::value_type::first)
		    != std::end(_variables_in_scope);
	}
}  // namespace soul::ast::visitors
