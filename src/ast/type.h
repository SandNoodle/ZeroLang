#pragma once

#include <cstdint>
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
	enum class scalar_type_t : uint8_t
	{
		type_boolean,
		type_character,
		type_float,
		type_integer,
		type_string,
		type_void, // only for functions.
	};
	class array_type_t;
	class struct_type_t;
	class type_t;

	std::strong_ordering operator<=>(const type_t& lhs, const type_t& rhs) noexcept;

	template <typename T>
	concept is_type_t = std::same_as<T, scalar_type_t> //
	                 || std::same_as<T, array_type_t>   //
	                 || std::same_as<T, struct_type_t>  //
	                 || std::same_as<T, std::monostate> //
	;

	/**
	 * @brief Represents an 'Array' type in the language.
	 * Holds an underlying type making up the Array.
	 */
	class array_type_t
	{
		private:
			std::unique_ptr<type_t> _contained_type = std::make_unique<type_t>();

		public:
			array_type_t() noexcept = default;
			array_type_t(const array_type_t&) noexcept;
			array_type_t(array_type_t&&) noexcept;
			explicit array_type_t(type_t contained_type);

			array_type_t& operator=(const array_type_t&) noexcept;
			array_type_t& operator=(array_type_t&&) noexcept;
			bool operator==(const array_type_t& other) const noexcept;
			auto operator<=>(const array_type_t& other) const noexcept;
			explicit operator std::string() const;

			/** @brief Returns the underlying type. */
			[[nodiscard]] const type_t& type() const noexcept;

			/** @brief Returns the underlying type. */
			[[nodiscard]] type_t& type() noexcept;
	};

	/**
	 * @brief Represents a 'Struct' type in the language.
	 * Holds multiple of underlying types making up the Struct.
	 */
	class struct_type_t
	{
		private:
			std::vector<type_t> _types;

		public:
			struct_type_t() noexcept = default;
			struct_type_t(const struct_type_t&) noexcept = default;
			struct_type_t(struct_type_t&&) noexcept = default;
			explicit struct_type_t(std::vector<type_t> types);

			struct_type_t& operator=(const struct_type_t&) noexcept = default;
			struct_type_t& operator=(struct_type_t&&) noexcept = default;
			bool operator==(const struct_type_t& other) const noexcept = default;
			auto operator<=>(const struct_type_t& other) const noexcept;
			explicit operator std::string() const;

			/** @brief Returns the underlying types. */
			[[nodiscard]] const std::vector<type_t>& types() const noexcept;

			/** @brief Returns the underlying types. */
			[[nodiscard]] std::vector<type_t>& types() noexcept;
	};

	/**
	 * @brief Represents a 'Type' of a variable, function, etc. in the language.
	 */
	class type_t
	{
		public:
			using unknown_t = std::monostate;
			using variant_t = std::variant<unknown_t, scalar_type_t, array_type_t, struct_type_t>;

		private:
			variant_t _type = unknown_t{};

		public:
			type_t() noexcept = default;
			type_t(const type_t&) noexcept = default;
			type_t(type_t&&) noexcept = default;
			explicit type_t(is_type_t auto type)
			 : _type(std::move(type)) {}

			type_t& operator=(const type_t&) noexcept = default;
			type_t& operator=(type_t&&) noexcept = default;
			bool operator==(const type_t&) const noexcept = default;
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

			friend std::strong_ordering operator<=>(const type_t& lhs, const type_t& rhs) noexcept;
	};

	template <typename T>
	T& operator<<(T& stream, const type_t& type)
	{
		stream << std::string(type);
		return stream;
	}
}  // namespace soul
