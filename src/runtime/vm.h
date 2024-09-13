#pragma once

#include "common/diagnostic.h"
#include "common/types.h"

#include <array>
#include <type_traits>

namespace soul
{
	enum value_t
	{
		i64 value_int;
		f64 value_float;
	};

	/**
	 * @brief
	 */
	struct chunk_t
	{
		std::vector<std::byte> code;
	};

	/**
	 * @brief Virtual Machine class capable of interpreting language's code.
	 */
	class vm_t
	{
		public:

		private:
			std::vector<diagnostic_t> _diagnostics;

		public:
			vm_t() = default;
			vm_t(const vm_t&) noexcept = delete;
			vm_t(vm_t&&) noexcept = default;
			~vm_t() = default;



		private:
	};

}  // namespace soul
