#pragma once

#include "common/diagnostic.h"
#include "common/types.h"

#include <vector>
#include <type_traits>

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
		std::vector<Diagnostic> _diagnostics;

		public:
		VirtualMachine()                     = default;
		VirtualMachine(const VirtualMachine&) noexcept = delete;
		VirtualMachine(VirtualMachine&&) noexcept      = default;
		~VirtualMachine()                    = default;

		private:
	};

}  // namespace soul
