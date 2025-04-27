#pragma once
#include <cstdint>
#include <span>

struct FormatData;

extern "C" std::uint32_t untile_impl(
	std::uint8_t* output_buffer,
	std::size_t output_buffer_len,
	const std::uint8_t* input_buffer,
	std::size_t input_buffer_len,
	const FormatData* format,
	std::uint32_t blocks_x,
	std::uint32_t blocks_y,
	std::uint32_t offset_x,
	std::uint32_t offset_y
);

namespace x_flipper_360 {
	inline std::uint32_t untile(
		std::span<std::uint8_t>& output_buffer,
		const std::span<std::uint8_t>& input_buffer,
		const FormatData& format,
		std::uint32_t blocks_x,
		std::uint32_t blocks_y,
		std::uint32_t offset_x,
		std::uint32_t offset_y) {
		untile_impl(output_buffer.data(), output_buffer.size_bytes(), input_buffer.data(), input_buffer.size(), &format, blocks_x, blocks_y, offset_x, offset_y);
	}
};