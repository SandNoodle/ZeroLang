#pragma once

#include "common/diagnostic.h"
#include "core/types.h"

#include <type_traits>
#include <vector>

namespace soul
{
	union Value
	{
		i64 value_int;
		f64 value_float;
	};

	/**
	 * @brief
	 */
	struct Chunk
	{
		std::vector<std::byte> code;
	};

	/**
	 * @brief Virtual Machine class capable of interpreting language's code.
	 */
	class VirtualMachine
	{
		public:
		private:
		Diagnostics _diagnostics;

		public:
		VirtualMachine()                               = default;
		VirtualMachine(const VirtualMachine&) noexcept = delete;
		VirtualMachine(VirtualMachine&&) noexcept      = default;
		~VirtualMachine()                              = default;

		private:
	};

}  // namespace soul
