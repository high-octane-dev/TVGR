#pragma once
#include "base_object.hpp"
#include "x360_shader_parameters_data_item.hpp"
#include "runtime/direct3d9/d3d9_x.hpp"

struct ShaderPassAnimationData;

struct ShaderPassDescriptor : BaseObject {
	unsigned char texture_map_count;
	be<unsigned short> texture_map_string_table_indices[16];
	be<unsigned int> shader_parameter_count;
	xpointer<ShaderPassAnimationData> animation_data;
	xpointer<xpointer<ShaderPassAnimationData>> animation_data_list;
	be<unsigned int> is_2d;
	be<unsigned int> dependent_material_string_table_indices[2];
	be<float> material_pass_start;
	be<float> material_pass_end;
	be<unsigned int> force_unique;
	char texture_coordinate_indices[16];
};

struct ShaderPassSamplerDescEntry {
	be<unsigned int> sampler;
	be<XD3DSamplerStateType> type;
	be<unsigned int> value;
};

struct ShaderPassRenderStateDescEntry {
	be<XD3DRenderStateType> type;
	be<unsigned int> value;
};

struct X360ShaderPassDescriptor : public ShaderPassDescriptor {
	unsigned char flags;
	be<short> vertex_shader_index;
	be<short> pixel_shader_index;
	be<unsigned int> sampler_parameter_count;
	be<unsigned int> render_state_parameter_count;
	be<unsigned int> unused0;
	be<unsigned int> unused1;
	be<int> camera_string_table_index;
	be<int> material_index;
	be<float> gloss_power;
	xpointer<ShaderPassSamplerDescEntry> sampler_state_descriptor;
	xpointer<ShaderPassRenderStateDescEntry> render_state_descriptor;
	xpointer<X360ShaderParametersDataItem> shader_parameters_data_item;
};