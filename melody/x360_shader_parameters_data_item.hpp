#pragma once
#include <cstddef>
#include <cstdint>
#include "graphics_data_item.hpp"

struct ShaderParameter {
	be<std::uint32_t> is_vertex;
	be<std::uint32_t> register_index;
	be<float> value[4];
};

class X360ShaderParametersDataItem : public GraphicsDataItem {
	be<std::uint32_t> parameter_count;
	xpointer<ShaderParameter> parameters;
};