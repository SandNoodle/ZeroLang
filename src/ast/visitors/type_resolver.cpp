#include "ast/visitors/type_resolver.h"

#include "ast/nodes/binary.h"
#include "ast/nodes/block.h"
#include "ast/nodes/cast.h"
#include "ast/nodes/error.h"
#include "ast/nodes/for_loop.h"
#include "ast/nodes/foreach_loop.h"
#include "ast/nodes/function_call.h"
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

	TypeResolverVisitor::TypeResolverVisitor(TypeMap type_map) : _registered_types(std::move(type_map)) {}

	void TypeResolverVisitor::visit(const BinaryNode& node)
	{
		CopyVisitor::visit(node);

		auto& binary_node = as<BinaryNode>();
		if (!node.lhs) {
			binary_node.lhs = ErrorNode::create("[INTERNAL] BinaryNode does not contain LHS expression (nullptr)");
		}
		if (!node.rhs) {
			binary_node.rhs = ErrorNode::create("[INTERNAL] BinaryNode does not contain RHS expression (nullptr)");
		}
		if (dynamic_cast<ErrorNode*>(binary_node.lhs.get()) && dynamic_cast<ErrorNode*>(binary_node.rhs.get())) {
			return;
		}

		const auto result_type
			= get_type_for_operator(binary_node.op, std::array{ binary_node.lhs->type, binary_node.rhs->type });
		if (result_type == Type{}) {
			_current_clone = ErrorNode::create(std::format("operator ('{}') does not exist for types '{}' and '{}'",
			                                               ASTNode::name(binary_node.op),
			                                               std::string(binary_node.lhs->type),
			                                               std::string(binary_node.rhs->type)));
			return;
		}
		_current_clone->type = result_type;
	}

	void TypeResolverVisitor::visit(const BlockNode& node)
	{
		// ENTER SCOPE: Mark a restorepoint for variables declared in the current scope.
		const std::size_t variables_until_this_point = _variables_in_scope.size();

		CopyVisitor::visit(node);
		_current_clone->type = PrimitiveType::Kind::Void;

		// EXIT SCOPE: Remove variables defined in that scope.
		const std::size_t variables_declared_in_scope = _variables_in_scope.size() - variables_until_this_point;
		_variables_in_scope.erase(_variables_in_scope.end() - static_cast<std::ptrdiff_t>(variables_declared_in_scope),
		                          _variables_in_scope.end());
	}

	void TypeResolverVisitor::visit(const nodes::CastNode& node)
	{
		CopyVisitor::visit(node);

		if (!node.expression) {
			_current_clone = ErrorNode::create("[INTERNAL] CastNode does not contain an expression (nullptr)");
		}

		const auto& cast_node = as<CastNode>();
		const auto  from_type = cast_node.expression->type;
		const auto  to_type   = get_type_or_default(cast_node.type_identifier);
		if (get_cast_type(from_type, to_type) == CastNode::Type::Impossible) {
			_current_clone = ErrorNode::create(
				std::format("cannot cast from type '{}' to '{}'", std::string(from_type), std::string(to_type)));
			return;
		}

		_current_clone->type = to_type;
	}

	void TypeResolverVisitor::visit(const ForLoopNode& node)
	{
		CopyVisitor::visit(node);

		const auto& for_loop = as<ForLoopNode>();
		if (for_loop.condition) {
			const bool is_condition_bool_coercible
				= get_cast_type(for_loop.condition->type, PrimitiveType::Kind::Boolean) != CastNode::Type::Impossible;
			if (!is_condition_bool_coercible) {
				_current_clone = ErrorNode::create(
					std::format("condition in for loop statement must be convertible to a '{}' type",
				                std::string(Type{ PrimitiveType::Kind::Boolean })));
				return;
			}
		}

		_current_clone->type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(const ForeachLoopNode& node)
	{
		CopyVisitor::visit(node);

		auto& foreach_node = as<ForeachLoopNode>();
		if (!node.variable) {
			foreach_node.variable
				= ErrorNode::create("[INTERNAL] ForeachLoopNode does not contain variable expression (nullptr)");
		}
		if (!node.in_expression) {
			foreach_node.in_expression
				= ErrorNode::create("[INTERNAL] ForeachLoopNode does not contain in_expression expression (nullptr)");
		}
		if (dynamic_cast<ErrorNode*>(foreach_node.variable.get())
		    && dynamic_cast<ErrorNode*>(foreach_node.in_expression.get())) {
			return;
		}

		if (!foreach_node.in_expression->type.is<ArrayType>()) {
			_current_clone = ErrorNode::create(
				std::format("expression iterated in for each loop statement must be of an array type"));
			return;
		}

		const auto relation_type
			= get_cast_type(foreach_node.in_expression->type.as<ArrayType>().data_type(), foreach_node.variable->type);
		if (relation_type == CastNode::Type::Impossible) {
			_current_clone = ErrorNode::create(std::format(
				"type missmatch in for each loop statement between variable ('{}') and iterated expression ('{}')",
				std::string(foreach_node.variable->type),
				std::string(foreach_node.in_expression->type)));
			return;
		}

		_current_clone->type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(const nodes::FunctionCallNode& node)
	{
		CopyVisitor::visit(node);

		auto& function_call = as<FunctionCallNode>();
		auto  want_types    = function_call.parameters
		                | std::views::transform([](const auto& parameter) -> types::Type { return parameter->type; });
		const auto function_declaration = get_function_declaration(node.name, want_types);
		if (!function_declaration.has_value()) {
			_current_clone = ErrorNode::create(std::format("cannot call non-existing function '{}'", node.name));
			return;
		}
		_current_clone->type = function_declaration->return_type;
	}

	void TypeResolverVisitor::visit(const FunctionDeclarationNode& node)
	{
		// NOTE: Soul does not support global variables; we can assume that a function declaration is an entirely new
		// scope without any previous declarations.
		_variables_in_scope.clear();

		CopyVisitor::visit(node);

		auto& function_declaration = as<FunctionDeclarationNode>();
		auto  want_types           = function_declaration.parameters
		                | std::views::transform([](const auto& parameter) -> types::Type { return parameter->type; });
		if (get_function_declaration(node.name, want_types)) {
			_current_clone
				= ErrorNode::create(std::format("function declaration '{}' shadows previous one", node.name));
			return;
		}

		for (std::size_t index = 0; index < function_declaration.parameters.size(); ++index) {
			const auto* parameter = function_declaration.parameters[index].get();
			if (dynamic_cast<const ErrorNode*>(parameter)) {
				return;
			}
			if (!dynamic_cast<const VariableDeclarationNode*>(parameter)) {
				function_declaration.parameters[index]
					= ErrorNode::create(std::format("[INTERNAL] FunctionDeclarationNode contains "
				                                    "non-VariableDeclarationNode in the parameter list (at {})",
				                                    index));
				return;
			}
		}

		_current_clone->type = get_type_or_default(node.type_identifier);
		_functions_in_module.emplace_back(
			node.name,
			FunctionDeclaration{ .input_types = want_types | std::ranges::to<std::vector<types::Type>>(),
		                         .return_type = function_declaration.type

		    });
	}

	void TypeResolverVisitor::visit(const IfNode& node)
	{
		CopyVisitor::visit(node);

		const auto& if_node = as<IfNode>();
		const bool  is_condition_bool_coercible
			= get_cast_type(if_node.condition->type, PrimitiveType::Kind::Boolean) != CastNode::Type::Impossible;
		if (!is_condition_bool_coercible) {
			_current_clone = ErrorNode::create(
				std::format("condition in if statement statement must be convertible to a '{}' type",
			                std::string(Type{ PrimitiveType::Kind::Boolean })));
			return;
		}

		_current_clone->type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(const LiteralNode& node)
	{
		CopyVisitor::visit(node);

		if (node.literal_type == LiteralNode::Type::Identifier) {
			const auto& type_identifier = get_variable_type(node.value.get<std::string>());
			if (!type_identifier) {
				_current_clone = ErrorNode::create(
					std::format("use of undeclared identifier '{}'", node.value.get<std::string>()));
				return;
			}
			_current_clone->type = *type_identifier;
			return;
		}

		static constexpr std::array k_literal_to_type{
			std::make_pair(LiteralNode::Type::Unknown, PrimitiveType::Kind::Unknown),
			std::make_pair(LiteralNode::Type::Boolean, PrimitiveType::Kind::Boolean),
			std::make_pair(LiteralNode::Type::Char, PrimitiveType::Kind::Char),
			std::make_pair(LiteralNode::Type::Float32, PrimitiveType::Kind::Float32),
			std::make_pair(LiteralNode::Type::Float64, PrimitiveType::Kind::Float64),
			std::make_pair(LiteralNode::Type::Int32, PrimitiveType::Kind::Int32),
			std::make_pair(LiteralNode::Type::Int64, PrimitiveType::Kind::Int64),
			std::make_pair(LiteralNode::Type::String, PrimitiveType::Kind::String),
		};
		const auto it{ std::ranges::find(
			k_literal_to_type, node.literal_type, &decltype(k_literal_to_type)::value_type::first) };
		if (it == std::end(k_literal_to_type)) [[unlikely]] {
			_current_clone->type = PrimitiveType::Kind::Unknown;
			return;
		}
		_current_clone->type = it->second;
	}

	void TypeResolverVisitor::visit(const ModuleNode& node)
	{
		CopyVisitor::visit(node);

		// NOTE: Modules are a collection of type declarations and functions, thus don't have their own type.
		_current_clone->type = PrimitiveType::Kind::Void;
	}

	void TypeResolverVisitor::visit(const StructDeclarationNode& node)
	{
		CopyVisitor::visit(node);
		_current_clone->type = get_type_or_default(node.name);
	}

	void TypeResolverVisitor::visit(const UnaryNode& node)
	{
		CopyVisitor::visit(node);

		const auto& unary_node = as<UnaryNode>();
		if (!unary_node.expression) {
			_current_clone = ErrorNode::create("[INTERNAL] UnaryNode does not contain expression (nullptr)");
			return;
		}

		const auto result_type = get_type_for_operator(unary_node.op, std::array{ unary_node.expression->type });
		if (result_type == Type{}) {
			_current_clone = ErrorNode::create(std::format("operator ('{}') does not exist for type '{}'",
			                                               ASTNode::name(unary_node.op),
			                                               std::string(unary_node.expression->type)));
			return;
		}
		_current_clone->type = result_type;
	}

	void TypeResolverVisitor::visit(const VariableDeclarationNode& node)
	{
		CopyVisitor::visit(node);

		if (get_variable_type(node.name)) {
			_current_clone
				= ErrorNode::create(std::format("variable declaration '{}' shadows previous one", node.name));
			return;
		}

		_current_clone->type = get_type_or_default(node.type_identifier);
		_variables_in_scope.emplace_back(std::make_pair(node.name, _current_clone->type));
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

	std::optional<Type> TypeResolverVisitor::get_variable_type(std::string_view name) const noexcept
	{
		const auto it{ std::ranges::find(
			_variables_in_scope, name, &decltype(_variables_in_scope)::value_type::first) };
		if (it == std::end(_variables_in_scope)) [[unlikely]] {
			return std::nullopt;
		}
		return it->second;
	}
}  // namespace soul::ast::visitors
