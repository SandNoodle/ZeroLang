#include "ast/visitors/type_resolver.h"

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
#include "common/types/type.h"

namespace soul::ast::visitors
{
	using namespace soul::ast::nodes;
	using namespace soul::types;

	TypeResolverVisitor::TypeResolverVisitor(TypeMap type_map, ResolveFlags flags)
		: _registered_types(std::move(type_map)), _flags(flags)
	{
	}

	void TypeResolverVisitor::visit(BinaryNode& node)
	{
		accept(node.lhs.get());
		accept(node.rhs.get());

		if (get_cast_type(node.lhs->type, node.rhs->type) == CastType::Impossible) {
			node.type = PrimitiveType::Kind::Unknown;
			return;
		}

		// TODO: Resolved type depends on the operand too!
		node.type = PrimitiveType::Kind::Unknown;  // TODO: FIX
	}

	void TypeResolverVisitor::visit(nodes::CastNode& node)
	{
		accept(node.expression.get());

		auto to_type = get_type_from_identifier(node.type_identifier);
		if (get_cast_type(node.expression->type, to_type) == CastType::Impossible) {
			node.type = PrimitiveType::Kind::Unknown;
			return;
		}

		node.type = std::move(to_type);
	}

	void TypeResolverVisitor::visit(ForLoopNode& node)
	{
		accept(node.initialization.get());
		accept(node.condition.get());
		accept(node.update.get());
		for (auto& statement : node.statements) {
			accept(statement.get());
		}
		node.type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(ForeachLoopNode& node)
	{
		accept(node.variable.get());
		accept(node.in_expression.get());
		for (auto& statement : node.statements) {
			accept(statement.get());
		}
		node.type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(FunctionDeclarationNode& node)
	{
		for (auto& parameters : node.parameters) {
			accept(parameters.get());
		}
		for (auto& statement : node.statements) {
			accept(statement.get());
		}
		node.type = get_type_from_identifier(node.return_type);
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
		node.type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(LiteralNode& node)
	{
		if (node.value.is<Value::UnknownValue>()) {
			node.type = PrimitiveType::Kind::Unknown;
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

		node.type = PrimitiveType::Kind::Unknown;
	}

	void TypeResolverVisitor::visit(ModuleNode& node)
	{
		for (auto& statement : node.statements) {
			accept(statement.get());
		}

		// NOTE: Modules are a collection of type declarations and functions, thus don't have their own type.
		node.type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(StructDeclarationNode& node)
	{
		for (auto& parameter : node.parameters) {
			accept(parameter.get());
		}

		node.type = get_type_from_identifier(node.name);
	}

	void TypeResolverVisitor::visit(UnaryNode& node)
	{
		accept(node.expr.get());

		// TODO: Resolved type depends on the operand too!
		node.type = PrimitiveType::Kind::Unknown;  // TODO: FIX
	}

	void TypeResolverVisitor::visit(VariableDeclarationNode& node)
	{
		accept(node.expr.get());
		node.type = get_type_from_identifier(node.type_identifier);
	}
	static const std::unordered_map<PrimitiveType::Kind, std::unordered_map<PrimitiveType::Kind, CastType>> k_cast_type
		= {
			  {
               PrimitiveType::Kind::Boolean,
               {
					  { PrimitiveType::Kind::Boolean, CastType::Implicit },
					  { PrimitiveType::Kind::Int32, CastType::Explicit },
					  { PrimitiveType::Kind::Int64, CastType::Explicit },
					  { PrimitiveType::Kind::String, CastType::Explicit },
				  }  //
			  },
			  {
               PrimitiveType::Kind::Char,
               {
					  { PrimitiveType::Kind::Char, CastType::Implicit },
					  { PrimitiveType::Kind::String, CastType::Implicit },
				  }  //
			  },
			  {
               PrimitiveType::Kind::Float32,
               {
					  { PrimitiveType::Kind::Float32, CastType::Implicit },
					  { PrimitiveType::Kind::Float64, CastType::Implicit },
					  { PrimitiveType::Kind::Int32, CastType::Explicit },
					  { PrimitiveType::Kind::Int64, CastType::Explicit },
					  { PrimitiveType::Kind::String, CastType::Explicit },
				  }  //
			  },
			  {
               PrimitiveType::Kind::Float64,
               {
					  { PrimitiveType::Kind::Float32, CastType::Explicit },
					  { PrimitiveType::Kind::Float64, CastType::Implicit },
					  { PrimitiveType::Kind::Int32, CastType::Explicit },
					  { PrimitiveType::Kind::Int64, CastType::Explicit },
					  { PrimitiveType::Kind::String, CastType::Explicit },
				  }  //
			  },
			  {
               PrimitiveType::Kind::Int32,
               {
					  { PrimitiveType::Kind::Boolean, CastType::Explicit },
					  { PrimitiveType::Kind::Float32, CastType::Implicit },
					  { PrimitiveType::Kind::Float64, CastType::Implicit },
					  { PrimitiveType::Kind::Int32, CastType::Implicit },
					  { PrimitiveType::Kind::Int64, CastType::Implicit },
					  { PrimitiveType::Kind::String, CastType::Explicit },
				  }  //
			  },
			  {
               PrimitiveType::Kind::Int64,
               {
					  { PrimitiveType::Kind::Boolean, CastType::Explicit },
					  { PrimitiveType::Kind::Float32, CastType::Implicit },
					  { PrimitiveType::Kind::Float64, CastType::Implicit },
					  { PrimitiveType::Kind::Int32, CastType::Explicit },
					  { PrimitiveType::Kind::Int64, CastType::Implicit },
					  { PrimitiveType::Kind::String, CastType::Explicit },
				  }  //
			  },
			  {
               PrimitiveType::Kind::String,
               {
					  { PrimitiveType::Kind::Float32, CastType::Explicit },
					  { PrimitiveType::Kind::Float64, CastType::Explicit },
					  { PrimitiveType::Kind::Int32, CastType::Explicit },
					  { PrimitiveType::Kind::Int64, CastType::Explicit },
					  { PrimitiveType::Kind::String, CastType::Implicit },
				  }  //
			  },
    };

	CastType TypeResolverVisitor::get_cast_type(const Type& from_type, const Type& to_type)
	{
		// NOTE: If the types are equivalent, no casts should take place.
		if (from_type == to_type) {
			return CastType::Implicit;
		}

		if (from_type.is<PrimitiveType>() && to_type.is<PrimitiveType>()) {
			const auto from = from_type.as<PrimitiveType>().type;
			const auto to   = to_type.as<PrimitiveType>().type;
			// NOTE: For unresolved types casts are impossible - to 'what' should we cast from/to?
			if (from == PrimitiveType::Kind::Unknown || to == PrimitiveType::Kind::Unknown) {
				return CastType::Impossible;
			}

			if (k_cast_type.contains(from) && k_cast_type.at(from).contains(to)) {
				return k_cast_type.at(from).at(to);
			}

			// NOTE: Assume that undefined casts are impossible.
			return CastType::Impossible;
		}

		if (from_type.is<ArrayType>() && to_type.is<ArrayType>()) {
			// Arrays can be cast only if their data types are castable.
			return get_cast_type(from_type.as<ArrayType>().data_type(), to_type.as<ArrayType>().data_type());
		}

		if (from_type.is<StructType>() || to_type.is<StructType>()) {
			// Casting from/to Struct types is not supported; maybe in the future.
			return CastType::Impossible;
		}

		// NOTE: There was a missmatch between the types.
		return CastType::Impossible;
	}

	types::Type TypeResolverVisitor::get_type_from_identifier(std::string_view type_identifier) const noexcept
	{
		if (_registered_types.contains(type_identifier)) {
			return _registered_types.at(type_identifier);
		}
		return types::Type{};
	}
}  // namespace soul::ast::visitors
