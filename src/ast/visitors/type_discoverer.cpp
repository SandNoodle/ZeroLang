#include "ast/visitors/type_discoverer.h"

#include "ast/nodes/error.h"
#include "ast/nodes/struct_declaration.h"
#include "ast/nodes/variable_declaration.h"
#include "common/types/type.h"

namespace soul::ast::visitors
{
	using namespace types;
	using namespace ast::nodes;

	TypeDiscovererVisitor::TypeMap TypeDiscovererVisitor::get() noexcept { return _registered_types; }

	void TypeDiscovererVisitor::visit(nodes::StructDeclarationNode& node)
	{
		if (_registered_types.contains(node.name)) {
			_cloned_root = ErrorNode::create(std::format("redefinition of type '{}'", node.name));
			return;
		}

		StructType::ContainedTypes contained_types{};
		contained_types.reserve(node.parameters.size());
		for (std::size_t index = 0; index < node.parameters.size(); ++index) {
			const auto* param = dynamic_cast<VariableDeclarationNode*>(node.parameters[index].get());
			if (!param) {
				node.parameters[index] = ErrorNode::create(std::format(
					"cannot resolve type for '{}', because parameter is not of valid (node) type", node.name));
				continue;
			}
			if (!_registered_types.contains(param->type_identifier)) {
				node.parameters[index] = ErrorNode::create(
					std::format("cannot resolve type '{}', because it was not registered", param->type_identifier));
				continue;
			}
			contained_types.push_back(_registered_types.at(param->type_identifier));
		}
		_registered_types[node.name] = Type{ StructType{ std::move(contained_types) } };
		_cloned_root = clone(node);
	}
	TypeDiscovererVisitor::TypeMap TypeDiscovererVisitor::basic_types() noexcept
	{
		// IMPORTANT: Must match keywords defined in Lexer::scan_token.
		using namespace std::string_view_literals;
		static const TypeDiscovererVisitor::TypeMap k_basic_types = {
			{ "bool"sv, types::PrimitiveType::Kind::Boolean },
            { "chr"sv,  types::PrimitiveType::Kind::Char    },
			{ "f32"sv,  types::PrimitiveType::Kind::Float32 },
            { "f64"sv,  types::PrimitiveType::Kind::Float64 },
			{ "i32"sv,  types::PrimitiveType::Kind::Int32   },
            { "i64"sv,  types::PrimitiveType::Kind::Int64   },
			{ "str"sv,  types::PrimitiveType::Kind::String  },
            { "void"sv, types::PrimitiveType::Kind::Void    },
		};
		return k_basic_types;
	}
}  // namespace soul::ast::visitors
