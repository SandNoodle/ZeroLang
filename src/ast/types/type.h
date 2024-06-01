#pragma once

#include <cstdint>
#include <type_traits>
#include <variant>
#include <vector>

namespace soul
{
	enum class builtin_type_t : uint8_t
	{
		integer,
		real,
		boolean,
		character,
		string,
	};
	class type_t;
	class array_type_t;
	class struct_type_t;

	template <typename T>
	concept is_type = std::same_as<T, builtin_type_t> //
	               || std::same_as<T, array_type_t>   //
	               || std::same_as<T, struct_type_t>  //
	;

	class array_type_t
	{
		private:
			std::unique_ptr<type_t> _contained_type;

		public:
	};

	class struct_type_t
	{
		private:
			std::vector<type_t> _types;

		public:
	};

	/**
	 * @brief
	 */
	class type_t
	{
		public:
			using unknown_t = std::monostate;
			using variant_t = std::variant<builtin_type_t, array_type_t, struct_type_t, unknown_t>;

		private:
			variant_t _type = unknown_t;

		public:
			type_t(is_type auto type);

		public:
			template <is_type T>
			[[nodiscard]] bool is() const
			{
				return std::holds_alternative<T>(_type);
			}

			template <is_type T>
			[[nodiscard]] T& get()
			{
				return std::get<T>(_type);
			}

			template <is_type T>
			[[nodiscard]] const T& get() const
			{
				return std::get<T>(_type);
			}
	};
}
