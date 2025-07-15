#include "ast/visitors/type_discoverer.h"

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
	using namespace types;
	using namespace ast::nodes;

	TypeDiscovererVisitor::TypeDiscovererVisitor() { register_basic_types(); }

	TypeDiscovererVisitor::TypeMap TypeDiscovererVisitor::get() noexcept { return _registered_types; }

	void TypeDiscovererVisitor::visit(const nodes::StructDeclarationNode& node)
	{
		if (_registered_types.contains(node.name)) {
			// TODO: Error!
			return;
		}

		StructType::ContainedTypes contained_types{};
		contained_types.reserve(node.parameters.size());
		for (std::size_t index = 0; index < node.parameters.size(); ++index) {
			const auto* param = dynamic_cast<VariableDeclarationNode*>(node.parameters[index].get());
			if (!param) {
				// TODO: Error!
				return;
			}
			if (!_registered_types.contains(param->type_identifier)) {
				// TODO: Error!
				return;
			}
			contained_types.push_back(_registered_types.at(param->type_identifier));
		}
		_registered_types[node.name] = Type{ StructType{ std::move(contained_types) } };
	}

	void TypeDiscovererVisitor::register_basic_types()
	{
		// IMPORTANT: Must match keywords defined in Lexer::create_identifier_token.
		using namespace std::string_view_literals;
		_registered_types["bool"sv] = PrimitiveType::Kind::Boolean;
		_registered_types["chr"sv]  = PrimitiveType::Kind::Char;
		_registered_types["f32"sv]  = PrimitiveType::Kind::Float32;
		_registered_types["f64"sv]  = PrimitiveType::Kind::Float64;
		_registered_types["i32"sv]  = PrimitiveType::Kind::Int32;
		_registered_types["i64"sv]  = PrimitiveType::Kind::Int64;
		_registered_types["str"sv]  = PrimitiveType::Kind::String;
		_registered_types["void"sv] = PrimitiveType::Kind::Void;
	}
}  // namespace soul::ast::visitors
