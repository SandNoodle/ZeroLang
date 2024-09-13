#pragma once

#include <cstdint>

namespace soul
{
	using u8  = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
	using u64 = std::uint64_t;

	using i8  = std::int8_t;
	using i16 = std::int16_t;
	using i32 = std::int32_t;
	using i64 = std::int64_t;

	using f32 = float;
	using f64 = double;
	static_assert(sizeof(f32) == sizeof(u32), "expected f32 to be 4 bytes.");
	static_assert(sizeof(f64) == sizeof(u64), "expected f64 to be 4 bytes.");
}  // namespace soul
