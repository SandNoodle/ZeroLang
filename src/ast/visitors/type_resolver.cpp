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
#include "common/types/type.h"

#include <unordered_set>

namespace soul::ast::visitors
{
	using namespace soul::ast::nodes;
	using namespace soul::types;

	TypeResolverVisitor::TypeResolverVisitor(ResolveFlags flags) : _flags(flags) { register_basic_types(); }

	// TODO MOVE OUT.
	enum class CastType : u8
	{
		Implicit,
		Explicit,
		Impossible,
	};

	static const std::unordered_map<PrimitiveType::Kind, std::unordered_map<PrimitiveType::Kind, CastType>> k_cast_type
		= {
			  {
               PrimitiveType::Kind::Boolean,
               {
					  { PrimitiveType::Kind::Boolean, CastType::Implicit },
					  { PrimitiveType::Kind::Char, CastType::Impossible },
					  { PrimitiveType::Kind::Float32, CastType::Impossible },
					  { PrimitiveType::Kind::Float64, CastType::Impossible },
					  { PrimitiveType::Kind::Int32, CastType::Explicit },
					  { PrimitiveType::Kind::Int64, CastType::Explicit },
					  { PrimitiveType::Kind::String, CastType::Explicit },
				  }  //
			  },
			  {
               PrimitiveType::Kind::Char,
               {
					  { PrimitiveType::Kind::Boolean, CastType::Impossible },
					  { PrimitiveType::Kind::Char, CastType::Implicit },
					  { PrimitiveType::Kind::Float32, CastType::Impossible },
					  { PrimitiveType::Kind::Float64, CastType::Impossible },
					  { PrimitiveType::Kind::Int32, CastType::Impossible },
					  { PrimitiveType::Kind::Int64, CastType::Impossible },
					  { PrimitiveType::Kind::String, CastType::Implicit },
				  }  //
			  },
			  {
               PrimitiveType::Kind::Float32,
               {
					  { PrimitiveType::Kind::Boolean, CastType::Impossible },
					  { PrimitiveType::Kind::Char, CastType::Impossible },
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
					  { PrimitiveType::Kind::Boolean, CastType::Impossible },
					  { PrimitiveType::Kind::Char, CastType::Impossible },
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
					  { PrimitiveType::Kind::Char, CastType::Impossible },
					  { PrimitiveType::Kind::Float32, CastType::Explicit },
					  { PrimitiveType::Kind::Float64, CastType::Explicit },
					  { PrimitiveType::Kind::Int32, CastType::Implicit },
					  { PrimitiveType::Kind::Int64, CastType::Implicit },
					  { PrimitiveType::Kind::String, CastType::Explicit },
				  }  //
			  },
			  {
               PrimitiveType::Kind::Int64,
               {
					  { PrimitiveType::Kind::Boolean, CastType::Impossible },
					  { PrimitiveType::Kind::Char, CastType::Impossible },
					  { PrimitiveType::Kind::Float32, CastType::Explicit },
					  { PrimitiveType::Kind::Float64, CastType::Explicit },
					  { PrimitiveType::Kind::Int32, CastType::Explicit },
					  { PrimitiveType::Kind::Int64, CastType::Implicit },
					  { PrimitiveType::Kind::String, CastType::Explicit },
				  }  //
			  },
			  {
               PrimitiveType::Kind::String,
               {
					  { PrimitiveType::Kind::Boolean, CastType::Impossible },
					  { PrimitiveType::Kind::Char, CastType::Impossible },
					  { PrimitiveType::Kind::Float32, CastType::Explicit },
					  { PrimitiveType::Kind::Float64, CastType::Explicit },
					  { PrimitiveType::Kind::Int32, CastType::Explicit },
					  { PrimitiveType::Kind::Int64, CastType::Explicit },
					  { PrimitiveType::Kind::String, CastType::Implicit },
				  }  //
			  },
    };

	CastType cast_type(const Type& from_type, const Type& to_type)
	{
		if (from_type.is<PrimitiveType>() && to_type.is<PrimitiveType>()) {
			const auto from = from_type.as<PrimitiveType>().type;
			const auto to   = to_type.as<PrimitiveType>().type;
			// Generally we'd like to avoid casts we did not define...
			if (!k_cast_type.contains(from)) {
				return CastType::Impossible;
			}
			const auto& possible_casts = k_cast_type.at(from);
			// ...same here.
			if (!possible_casts.contains(to)) {
				return CastType::Impossible;
			}
			return possible_casts.at(to);
		}

		if (from_type.is<StructType>() || to_type.is<StructType>()) {
			// Casting from/to Struct types is not supported; maybe in the future.
			return CastType::Impossible;
		}
		if (from_type.is<ArrayType>() && to_type.is<ArrayType>()) {
			return cast_type(from_type.as<ArrayType>().data_type(), to_type.as<ArrayType>().data_type());
		}

		return CastType::Impossible;
	}
	// TODO: END MOVE OUT.

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

		// Both nodes have the same type - great!
		if (node.lhs->type == node.rhs->type) {
			node.type = node.lhs->type;
			return;
		}

		// If strict casting is enabled then all casts should be explicit.
		const bool should_force_strict_casts = _flags & ResolveFlags::ForceStrictCasts;
		if (should_force_strict_casts && node.lhs->type != node.rhs->type) {
//			diagnostic(DiagnosticType::Error,
//			           DiagnosticCode::TypeResolverForceStrictCasts,
//			           std::string(node.lhs->type),
//			           std::string(node.rhs->type));

			// We don't know what the destination type should be, nor we should care at this point.
			node.type = PrimitiveType::Kind::Unknown;
			return;
		}
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
			diagnostic(DiagnosticType::Error, DiagnosticCode::TypeResolverTypeUnknown, node.return_type);
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
