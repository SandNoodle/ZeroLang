#pragma once

#include <concepts>
#include <type_traits>

namespace soul::ast::nodes
{
	class BinaryNode;
	class BlockNode;
	class CastNode;
	class ErrorNode;
	class ForLoopNode;
	class ForeachLoopNode;
	class FunctionCallNode;
	class FunctionDeclarationNode;
	class IfNode;
	class LiteralNode;
	class ModuleNode;
	class StructDeclarationNode;
	class UnaryNode;
	class VariableDeclarationNode;

	/**
	 * @brief NodeKind is a concept that specifies (and is satisfied by) nodes that can be represented in the
	 * Abstract Syntax Tree (AST).
	 */
	template <typename T>
	concept NodeKind = std::same_as<T, BinaryNode>               //
	                || std::same_as<T, BlockNode>                //
	                || std::same_as<T, CastNode>                 //
	                || std::same_as<T, ErrorNode>                //
	                || std::same_as<T, ForLoopNode>              //
	                || std::same_as<T, ForeachLoopNode>          //
	                || std::same_as<T, FunctionCallNode>         //
	                || std::same_as<T, FunctionDeclarationNode>  //
	                || std::same_as<T, IfNode>                   //
	                || std::same_as<T, LiteralNode>              //
	                || std::same_as<T, ModuleNode>               //
	                || std::same_as<T, StructDeclarationNode>    //
	                || std::same_as<T, UnaryNode>                //
	                || std::same_as<T, VariableDeclarationNode>  //
		;
}  // namespace soul::ast::nodes
