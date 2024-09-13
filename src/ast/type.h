#pragma once

#include "common/types.h"

#include <memory>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace soul
{
	/**
	 * @brief Represents a 'Primitive' type in the language.
	 */
	enum class ScalarType : u8
	{
		type_boolean,
		type_character,
		type_float,
		type_integer,
		type_string,
		type_void, // only for functions.
	};
	class ArrayType;
	class StructType;
	class Type;

	std::strong_ordering operator<=>(const Type& lhs, const Type& rhs) noexcept;

	template <typename T>
	concept is_type_t = std::same_as<T, ScalarType> //
	                 || std::same_as<T, ArrayType>   //
	                 || std::same_as<T, StructType>  //
	                 || std::same_as<T, std::monostate> //
	;

	/**
	 * @brief Represents an 'Array' type in the language.
	 * Holds an underlying type making up the Array.
	 */
	class ArrayType
	{
		private:
			std::unique_ptr<Type> _contained_type = std::make_unique<Type>();

		public:
			ArrayType() noexcept = default;
			ArrayType(const ArrayType&) noexcept;
			ArrayType(ArrayType&&) noexcept;
			explicit ArrayType(Type contained_type);

			ArrayType& operator=(const ArrayType&) noexcept;
			ArrayType& operator=(ArrayType&&) noexcept;
			bool operator==(const ArrayType& other) const noexcept;
			auto operator<=>(const ArrayType& other) const noexcept;
			explicit operator std::string() const;

			/** @brief Returns the underlying type. */
			[[nodiscard]] const Type& type() const noexcept;

			/** @brief Returns the underlying type. */
			[[nodiscard]] Type& type() noexcept;
	};

	/**
	 * @brief Represents a 'Struct' type in the language.
	 * Holds multiple of underlying types making up the Struct.
	 */
	class StructType
	{
		private:
			std::vector<Type> _types;

		public:
			StructType() noexcept = default;
			StructType(const StructType&) noexcept = default;
			StructType(StructType&&) noexcept = default;
			explicit StructType(std::vector<Type> types);

			StructType& operator=(const StructType&) noexcept = default;
			StructType& operator=(StructType&&) noexcept = default;
			bool operator==(const StructType& other) const noexcept = default;
			auto operator<=>(const StructType& other) const noexcept;
			explicit operator std::string() const;

			/** @brief Returns the underlying types. */
			[[nodiscard]] const std::vector<Type>& types() const noexcept;

			/** @brief Returns the underlying types. */
			[[nodiscard]] std::vector<Type>& types() noexcept;
	};

	/**
	 * @brief Represents a 'Type' of a variable, function, etc. in the language.
	 */
	class Type
	{
		public:
			using unknown_t = std::monostate;
			using variant_t = std::variant<unknown_t, ScalarType, ArrayType, StructType>;

		private:
			variant_t _type = unknown_t{};

		public:
			Type() noexcept = default;
			Type(const Type&) noexcept = default;
			Type(Type&&) noexcept = default;
			explicit Type(is_type_t auto type)
			 : _type(std::move(type)) {}

			Type& operator=(const Type&) noexcept = default;
			Type& operator=(Type&&) noexcept = default;
			bool operator==(const Type&) const noexcept = default;
			explicit operator std::string() const;

			/**
			 * @brief Verifies that std::variant holds the specified type.
			 * @tparam T Type to verify.
			 * @return Returns true if this is a given type, false otherwise.
			 */
			template <is_type_t T>
			[[nodiscard]] constexpr bool is() const
			{
				return std::holds_alternative<T>(_type);
			}

			/**
			 * @brief Returns given type held by the std::variant.
			 * @important Verify the type with is() beforehand.
			 * @tparam Type to return.
			 */
			template <is_type_t T>
			[[nodiscard]] constexpr T& get()
			{
				return std::get<T>(_type);
			}

			/**
			 * @brief Returns given type held by the std::variant.
			 * @important Verify the type with is() beforehand.
			 * @tparam Type to return.
			 */
			template <is_type_t T>
			[[nodiscard]] constexpr const T& get() const
			{
				return std::get<T>(_type);
			}

			friend std::strong_ordering operator<=>(const Type& lhs, const Type& rhs) noexcept;
	};

	template <typename T>
	T& operator<<(T& stream, const Type& type)
	{
		stream << std::string(type);
		return stream;
	}
}  // namespace soul
