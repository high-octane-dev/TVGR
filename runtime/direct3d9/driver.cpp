// HEAVY HEAVY WIP NOTHING HERE WORKS IT IS THE ROUGHEST OF ROUGH DRAFTS
#include <optional>
#include <array>
#include <d3d11.h>
#include <d3d11_4.h>
#include <xxhash.h>
#include "runtime/direct3d9/d3d9_x.hpp"
#include "runtime/kernel/heap.hpp"
#include "runtime/hook.hpp"
#include "melody/shader_pass_descriptor.hpp"
#include "melody/x360_video_card.hpp"
#include "recompiled/ppc/ppc_recomp_shared.h"
#include <runtime/logger.hpp>

inline DXGI_FORMAT to_dxgi(XD3DFormat _fmt) {
	if (_fmt == XD3DFormat::XD3DFMT_DXT1) {
		return DXGI_FORMAT_BC1_UNORM;
	}
	return DXGI_FORMAT_UNKNOWN;
}

inline D3D11_COMPARISON_FUNC to_comparison_func(uint32_t value) {
	switch (value) {
	case XD3DCMP_NEVER: return D3D11_COMPARISON_NEVER;
	case XD3DCMP_LESS: return D3D11_COMPARISON_LESS;
	case XD3DCMP_EQUAL: return D3D11_COMPARISON_EQUAL;
	case XD3DCMP_LESSEQUAL: return D3D11_COMPARISON_LESS_EQUAL;
	case XD3DCMP_GREATER: return D3D11_COMPARISON_GREATER;
	case XD3DCMP_NOTEQUAL: return D3D11_COMPARISON_NOT_EQUAL;
	case XD3DCMP_GREATEREQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
	case XD3DCMP_ALWAYS: return D3D11_COMPARISON_ALWAYS;
	default: return D3D11_COMPARISON_ALWAYS;
	}
}

inline D3D11_DEPTH_WRITE_MASK to_depth_write_mask(uint32_t value) {
	return value ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
}

inline D3D11_FILL_MODE to_fill_mode(uint32_t value) {
	switch (value) {
	case XD3DFILL_POINT: return D3D11_FILL_WIREFRAME;
	case XD3DFILL_WIREFRAME: return D3D11_FILL_WIREFRAME;
	case XD3DFILL_SOLID: return D3D11_FILL_SOLID;
	default: return D3D11_FILL_SOLID;
	}
}

inline D3D11_CULL_MODE to_cull_mode(uint32_t value) {
	switch (value) {
	case XD3DCULL_NONE: return D3D11_CULL_NONE;
	case XD3DCULL_CW: return D3D11_CULL_BACK;
	case XD3DCULL_CCW: return D3D11_CULL_FRONT;
	default: return D3D11_CULL_NONE;
	}
}

inline D3D11_BLEND to_blend(uint32_t value) {
	switch (value) {
	case XD3DBLEND_ZERO: return D3D11_BLEND_ZERO;
	case XD3DBLEND_ONE: return D3D11_BLEND_ONE;
	case XD3DBLEND_SRCCOLOR: return D3D11_BLEND_SRC_COLOR;
	case XD3DBLEND_INVSRCCOLOR: return D3D11_BLEND_INV_SRC_COLOR;
	case XD3DBLEND_SRCALPHA: return D3D11_BLEND_SRC_ALPHA;
	case XD3DBLEND_INVSRCALPHA: return D3D11_BLEND_INV_SRC_ALPHA;
	case XD3DBLEND_DESTALPHA: return D3D11_BLEND_DEST_ALPHA;
	case XD3DBLEND_INVDESTALPHA: return D3D11_BLEND_INV_DEST_ALPHA;
	case XD3DBLEND_DESTCOLOR: return D3D11_BLEND_DEST_COLOR;
	case XD3DBLEND_INVDESTCOLOR: return D3D11_BLEND_INV_DEST_COLOR;
	case XD3DBLEND_SRCALPHASAT: return D3D11_BLEND_SRC_ALPHA_SAT;
	default: return D3D11_BLEND_ONE;
	}
}

inline D3D11_BLEND_OP to_blend_op(uint32_t value) {
	switch (value) {
	case XD3DBLENDOP_ADD: return D3D11_BLEND_OP_ADD;
	case XD3DBLENDOP_SUBTRACT: return D3D11_BLEND_OP_SUBTRACT;
	case XD3DBLENDOP_REVSUBTRACT: return D3D11_BLEND_OP_REV_SUBTRACT;
	case XD3DBLENDOP_MIN: return D3D11_BLEND_OP_MIN;
	case XD3DBLENDOP_MAX: return D3D11_BLEND_OP_MAX;
	default: return D3D11_BLEND_OP_ADD;
	}
}

inline D3D11_STENCIL_OP to_stencil_op(uint32_t value) {
	switch (value) {
	case XD3DSTENCILOP_KEEP: return D3D11_STENCIL_OP_KEEP;
	case XD3DSTENCILOP_ZERO: return D3D11_STENCIL_OP_ZERO;
	case XD3DSTENCILOP_REPLACE: return D3D11_STENCIL_OP_REPLACE;
	case XD3DSTENCILOP_INCRSAT: return D3D11_STENCIL_OP_INCR_SAT;
	case XD3DSTENCILOP_DECRSAT: return D3D11_STENCIL_OP_DECR_SAT;
	case XD3DSTENCILOP_INVERT: return D3D11_STENCIL_OP_INVERT;
	case XD3DSTENCILOP_INCR: return D3D11_STENCIL_OP_INCR;
	case XD3DSTENCILOP_DECR: return D3D11_STENCIL_OP_DECR;
	default: return D3D11_STENCIL_OP_KEEP;
	}
}

inline D3D11_TEXTURE_ADDRESS_MODE to_texture_addr(uint32_t addr)  {
	switch (addr) {
	case XD3DTADDRESS_WRAP:
		return D3D11_TEXTURE_ADDRESS_WRAP;
	case XD3DTADDRESS_MIRROR:
		return D3D11_TEXTURE_ADDRESS_MIRROR;
	case XD3DTADDRESS_CLAMP:
		return D3D11_TEXTURE_ADDRESS_CLAMP;
	case XD3DTADDRESS_BORDER:
		return D3D11_TEXTURE_ADDRESS_BORDER;
	case XD3DTADDRESS_MIRRORONCE:
		return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	}
}


inline D3D11_FILTER_TYPE to_filter_type(uint32_t filter) {
	switch (filter) {
	case XD3DTEXF_NONE:
	case XD3DTEXF_POINT:
		return D3D11_FILTER_TYPE_POINT;
	default:
	case XD3DTEXF_LINEAR:
		return D3D11_FILTER_TYPE_LINEAR;
	case XD3DTEXF_ANISOTROPIC:
		return D3D11_FILTER_TYPE_LINEAR;
	}
}

inline std::array<float, 4> to_vector(uint32_t color) {
	float a = ((color >> 24) & 0xFF) / 255.0f;
	float r = ((color >> 16) & 0xFF) / 255.0f;
	float g = ((color >> 8) & 0xFF) / 255.0f;
	float b = (color & 0xFF) / 255.0f;
	return { r, g, b, a };
}

/*
static D3D11_BLEND_DESC1 DEFAULT_BLEND{};
static ID3D11BlendState1* DEFAULT_BLEND_COMPILED = nullptr;
static XXH64_hash_t DEFAULT_BLEND_HASH{};
*/

struct CompiledPipeline {
	ID3D11BlendState1* blend;
	std::array<float, 4> blend_factor;
	std::array<float, 4> blend_factor_ccw;
	ID3D11RasterizerState2* rasterizer;
	ID3D11DepthStencilState* depth_stencil;
	std::uint32_t stencil_ref;
	std::uint32_t stencil_ref_ccw;
	std::array<ID3D11SamplerState*, 16> samplers;

	auto bind(ID3D11DeviceContext4* dev) {
		dev->OMSetBlendState(blend, blend_factor.data(), 0);
		dev->RSSetState(rasterizer);
		dev->OMSetDepthStencilState(depth_stencil, stencil_ref);
		dev->PSSetSamplers(0, samplers.size(), samplers.data());
	}

	auto release() {
		blend->Release();
		rasterizer->Release();
		depth_stencil->Release();
		for (std::size_t i = 0; i < samplers.size(); i++) {
			samplers[i]->Release();
		}
	}
};

struct PipelineDesc {
	D3D11_BLEND_DESC1 blend;
	std::array<float, 4> blend_factor;
	std::array<float, 4> blend_factor_ccw;
	D3D11_RASTERIZER_DESC2 rasterizer;
	D3D11_DEPTH_STENCIL_DESC depth_stencil;
	std::uint32_t stencil_ref;
	std::uint32_t stencil_ref_ccw;
	std::array<D3D11_SAMPLER_DESC, 16> samplers;

	// If a sampler is like empty use a static global and AddRef before returning.
	auto compile(ID3D11Device5* dev) -> CompiledPipeline {
		ID3D11BlendState1* blend_state = nullptr;
		ID3D11RasterizerState2* rasterizer_state = nullptr;
		ID3D11DepthStencilState* depth_stencil_state = nullptr;
		std::array<ID3D11SamplerState*, 16> sampler_states{};
		
		dev->CreateBlendState1(&blend, &blend_state);
		dev->CreateRasterizerState2(&rasterizer, &rasterizer_state);
		dev->CreateDepthStencilState(&depth_stencil, &depth_stencil_state);

		for (std::size_t i = 0; i < samplers.size(); i++) {
			dev->CreateSamplerState(&samplers[i], &sampler_states[i]);
		}

		return { blend_state, blend_factor, blend_factor_ccw, rasterizer_state, depth_stencil_state, stencil_ref, stencil_ref_ccw, sampler_states };

		/*
		if (XXH3_64bits(&blend, sizeof(D3D11_BLEND_DESC1)) == DEFAULT_BLEND_HASH) {
			DEFAULT_BLEND_COMPILED->AddRef();
			blend_state = DEFAULT_BLEND_COMPILED;
		}
		else {
			dev->CreateBlendState1(&blend, &blend_state);
		}
		*/
	}
};

inline PipelineDesc from_shader_pass(X360ShaderPassDescriptor* desc) {
	PipelineDesc pipeline{};
	for (int i = 0; i < desc->render_state_parameter_count; i++) {
		auto value = desc->render_state_descriptor[i].value.get();
		switch (desc->render_state_descriptor[i].type.get()) {
		case XD3DRS_ZENABLE:
			pipeline.depth_stencil.DepthEnable = value;
			break;
		case XD3DRS_ZFUNC:
			pipeline.depth_stencil.DepthFunc = to_comparison_func(value);
			break;
		case XD3DRS_ZWRITEENABLE:
			pipeline.depth_stencil.DepthWriteMask = to_depth_write_mask(value);
			break;
		case XD3DRS_FILLMODE:
			pipeline.rasterizer.FillMode = to_fill_mode(value);
			break;
		case XD3DRS_CULLMODE:
			pipeline.rasterizer.CullMode = to_cull_mode(value);
			break;
		case XD3DRS_ALPHABLENDENABLE:
			pipeline.blend.AlphaToCoverageEnable = value;
			break;
		case XD3DRS_SEPARATEALPHABLENDENABLE:
			pipeline.blend.IndependentBlendEnable = value;
			break;
		case XD3DRS_BLENDFACTOR:
			pipeline.blend_factor = to_vector(value);
			break;
		case XD3DRS_SRCBLEND:
			pipeline.blend.RenderTarget[0].SrcBlend = to_blend(value);
			break;
		case XD3DRS_DESTBLEND:
			pipeline.blend.RenderTarget[0].DestBlend = to_blend(value);
			break;
		case XD3DRS_BLENDOP:
			pipeline.blend.RenderTarget[0].BlendOp = to_blend_op(value);
			break;
		case XD3DRS_SRCBLENDALPHA:
			pipeline.blend.RenderTarget[0].SrcBlendAlpha = to_blend(value);
			break;
		case XD3DRS_DESTBLENDALPHA:
			pipeline.blend.RenderTarget[0].DestBlendAlpha = to_blend(value);
			break;
		case XD3DRS_BLENDOPALPHA:
			pipeline.blend.RenderTarget[0].BlendOpAlpha = to_blend_op(value);
			break;
		case XD3DRS_ALPHATESTENABLE:
			// Fixed-Function
			break;
		case XD3DRS_ALPHAREF:
			// Fixed-Function
			break;
		case XD3DRS_ALPHAFUNC:
			// Fixed-Function
			break;
		case XD3DRS_STENCILENABLE:
			pipeline.depth_stencil.StencilEnable = value != FALSE;
			break;
		case XD3DRS_TWOSIDEDSTENCILMODE:
			pipeline.depth_stencil.StencilEnable = value != FALSE;
			break;
		case XD3DRS_STENCILFAIL:
			pipeline.depth_stencil.FrontFace.StencilFailOp = to_stencil_op(value);
			break;
		case XD3DRS_STENCILZFAIL:
			pipeline.depth_stencil.FrontFace.StencilDepthFailOp = to_stencil_op(value);
			break;
		case XD3DRS_STENCILPASS:
			pipeline.depth_stencil.FrontFace.StencilPassOp = to_stencil_op(value);
			break;
		case XD3DRS_STENCILFUNC:
			pipeline.depth_stencil.FrontFace.StencilFunc = to_comparison_func(value);
			break;
		case XD3DRS_STENCILREF:
			pipeline.stencil_ref = value;
			break;
		case XD3DRS_STENCILMASK:
			pipeline.depth_stencil.StencilReadMask = static_cast<unsigned char>(value);
			break;
		case XD3DRS_STENCILWRITEMASK:
			pipeline.depth_stencil.StencilWriteMask = static_cast<unsigned char>(value);
			break;
		case XD3DRS_CCW_STENCILFAIL:
			pipeline.depth_stencil.BackFace.StencilFailOp = to_stencil_op(value);
			break;
		case XD3DRS_CCW_STENCILZFAIL:
			pipeline.depth_stencil.BackFace.StencilDepthFailOp = to_stencil_op(value);
			break;
		case XD3DRS_CCW_STENCILPASS:
			pipeline.depth_stencil.BackFace.StencilPassOp = to_stencil_op(value);
			break;
		case XD3DRS_CCW_STENCILFUNC:
			pipeline.depth_stencil.BackFace.StencilFunc = to_comparison_func(value);
			break;
		case XD3DRS_CCW_STENCILREF:
			pipeline.stencil_ref_ccw = value;
			break;
		case XD3DRS_CCW_STENCILMASK:
			break;
		case XD3DRS_CCW_STENCILWRITEMASK:
			break;
		case XD3DRS_CLIPPLANEENABLE:
			break;
		case XD3DRS_POINTSIZE:
			break;
		case XD3DRS_POINTSIZE_MIN:
			break;
		case XD3DRS_POINTSPRITEENABLE:
			break;
		case XD3DRS_POINTSIZE_MAX:
			break;
		case XD3DRS_MULTISAMPLEANTIALIAS:
			break;
		case XD3DRS_MULTISAMPLEMASK:
			break;
		case XD3DRS_SCISSORTESTENABLE:
			break;
		case XD3DRS_SLOPESCALEDEPTHBIAS:
			break;
		case XD3DRS_DEPTHBIAS:
			break;
		case XD3DRS_COLORWRITEENABLE:
			break;
		case XD3DRS_COLORWRITEENABLE1:
			break;
		case XD3DRS_COLORWRITEENABLE2:
			break;
		case XD3DRS_COLORWRITEENABLE3:
			break;
		case XD3DRS_TESSELLATIONMODE:
			break;
		case XD3DRS_MINTESSELLATIONLEVEL:
			break;
		case XD3DRS_MAXTESSELLATIONLEVEL:
			break;
		case XD3DRS_WRAP0:
			break;
		case XD3DRS_WRAP1:
			break;
		case XD3DRS_WRAP2:
			break;
		case XD3DRS_WRAP3:
			break;
		case XD3DRS_WRAP4:
			break;
		case XD3DRS_WRAP5:
			break;
		case XD3DRS_WRAP6:
			break;
		case XD3DRS_WRAP7:
			break;
		case XD3DRS_WRAP8:
			break;
		case XD3DRS_WRAP9:
			break;
		case XD3DRS_WRAP10:
			break;
		case XD3DRS_WRAP11:
			break;
		case XD3DRS_WRAP12:
			break;
		case XD3DRS_WRAP13:
			break;
		case XD3DRS_WRAP14:
			break;
		case XD3DRS_WRAP15:
			break;
		case XD3DRS_VIEWPORTENABLE:
			break;
		case XD3DRS_HIGHPRECISIONBLENDENABLE:
			break;
		case XD3DRS_HIGHPRECISIONBLENDENABLE1:
			break;
		case XD3DRS_HIGHPRECISIONBLENDENABLE2:
			break;
		case XD3DRS_HIGHPRECISIONBLENDENABLE3:
			break;
		case XD3DRS_HALFPIXELOFFSET:
			break;
		case XD3DRS_PRIMITIVERESETENABLE:
			break;
		case XD3DRS_PRIMITIVERESETINDEX:
			break;
		case XD3DRS_ALPHATOMASKENABLE:
			break;
		case XD3DRS_ALPHATOMASKOFFSETS:
			break;
		case XD3DRS_GUARDBAND_X:
			break;
		case XD3DRS_GUARDBAND_Y:
			break;
		case XD3DRS_DISCARDBAND_X:
			break;
		case XD3DRS_DISCARDBAND_Y:
			break;
		case XD3DRS_HISTENCILENABLE:
			break;
		case XD3DRS_HISTENCILWRITEENABLE:
			break;
		case XD3DRS_HISTENCILFUNC:
			break;
		case XD3DRS_HISTENCILREF:
			break;
		case XD3DRS_PRESENTINTERVAL:
			break;
		case XD3DRS_PRESENTIMMEDIATETHRESHOLD:
			break;
		case XD3DRS_HIZENABLE:
			break;
		case XD3DRS_HIZWRITEENABLE:
			break;
		case XD3DRS_LASTPIXEL:
			break;
		case XD3DRS_LINEWIDTH:
			break;
		case XD3DRS_BUFFER2FRAMES:
			break;
		case XD3DRS_MAX:
			break;
		case XD3DRS_FORCE_DWORD:
			break;
		}
	}
	
	for (int i = 0; i < desc->sampler_parameter_count; i++) {
		auto& sampler = pipeline.samplers[desc->sampler_state_descriptor[i].sampler];
		auto value = desc->sampler_state_descriptor[i].value.get();
		switch (desc->sampler_state_descriptor[i].type) {
		case XD3DSAMP_ADDRESSU:
			sampler.AddressU = to_texture_addr(value);
			break;
		case XD3DSAMP_ADDRESSV:
			sampler.AddressV = to_texture_addr(value);
			break;
		case XD3DSAMP_ADDRESSW:
			sampler.AddressW = to_texture_addr(value);
			break;
		case XD3DSAMP_BORDERCOLOR:
			sampler.BorderColor[0] = ((value >> 16) & 0xFF) / 255.0f;
			sampler.BorderColor[1] = ((value >> 8) & 0xFF) / 255.0f;
			sampler.BorderColor[2] = (value & 0xFF) / 255.0f;
			sampler.BorderColor[3] = ((value >> 24) & 0xFF) / 255.0f;
			break;
		case XD3DSAMP_MAGFILTER:
			{
				D3D11_FILTER_TYPE min_filter = D3D11_DECODE_MIN_FILTER(sampler.Filter);
				D3D11_FILTER_TYPE mip_filter = D3D11_DECODE_MIP_FILTER(sampler.Filter);
				sampler.Filter = D3D11_ENCODE_BASIC_FILTER(min_filter, to_filter_type(value), mip_filter, D3D11_FILTER_REDUCTION_TYPE_STANDARD);
			}
			break;
		case XD3DSAMP_MINFILTER:
			{
				D3D11_FILTER_TYPE mag_filter = D3D11_DECODE_MAG_FILTER(sampler.Filter);
				D3D11_FILTER_TYPE mip_filter = D3D11_DECODE_MIP_FILTER(sampler.Filter);
				sampler.Filter = D3D11_ENCODE_BASIC_FILTER(to_filter_type(value), mag_filter, mip_filter, D3D11_FILTER_REDUCTION_TYPE_STANDARD);
			}
			break;
		case XD3DSAMP_MIPFILTER:
			{
				D3D11_FILTER_TYPE min_filter = D3D11_DECODE_MIN_FILTER(sampler.Filter);
				D3D11_FILTER_TYPE mag_filter = D3D11_DECODE_MAG_FILTER(sampler.Filter);
				sampler.Filter = D3D11_ENCODE_BASIC_FILTER(min_filter, mag_filter, to_filter_type(value), D3D11_FILTER_REDUCTION_TYPE_STANDARD);
			}
			break;
		case XD3DSAMP_MIPMAPLODBIAS:
			sampler.MipLODBias = std::bit_cast<float>(value);
			break;
		case XD3DSAMP_MAXMIPLEVEL:
			sampler.MaxLOD = std::bit_cast<float>(value);
			break;
		case XD3DSAMP_MAXANISOTROPY:
			sampler.MaxAnisotropy = value;
			break;
		case XD3DSAMP_MAGFILTERZ:
			break;
		case XD3DSAMP_MINFILTERZ:
			break;
		case XD3DSAMP_SEPARATEZFILTERENABLE:
			break;
		case XD3DSAMP_MINMIPLEVEL:
			sampler.MinLOD = std::bit_cast<float>(value);
			break;
		case XD3DSAMP_TRILINEARTHRESHOLD:
			break;
		case XD3DSAMP_ANISOTROPYBIAS:
			break;
		case XD3DSAMP_HGRADIENTEXPBIAS:
			break;
		case XD3DSAMP_VGRADIENTEXPBIAS:
			break;
		case XD3DSAMP_WHITEBORDERCOLORW:
			break;
		case XD3DSAMP_POINTBORDERENABLE:
			break;
		case XD3DSAMP_MAX:
			break;
		case XD3DSAMP_FORCE_DWORD:
			break;
		}
	}
	return pipeline;
}

/*
PPC_FUNC_IMPL(__imp__sub_82584270);
PPC_FUNC(sub_82584270) {
	// logger::log_format("[X360VideoCard::DisplayToScreen] Displaying frame");
	X360VideoCard* _this = reinterpret_cast<X360VideoCard*>(GLOBAL_MEMORY.translate(ctx.r3.u32));
	int unk = ctx.r4.u32;
	__imp__sub_82584270(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8256A130);
PPC_FUNC(sub_8256A130) {
	// logger::log_format("[CarsGame::PresentFrame] Presenting frame");
	__imp__sub_8256A130(ctx, base);
}


PPC_FUNC_IMPL(__imp__sub_82585278);
PPC_FUNC(sub_82585278) {
	// logger::log_format("[X360MediaControl::LoadMovie] Stealing your hearts!!!!");
	__imp__sub_82585278(ctx, base);
}
*/

GuestFunctionStub(sub_820D6E98);
GuestFunctionStub(sub_820D4288);

/*
// FUCK YOU RB for TURNING ON LTCG 🖕🖕🖕🖕🖕🖕🖕🖕🖕
X360VideoCard* X360VideoCard_Create(X360VideoCard* _this) {
	XD3DDevice* device = GLOBAL_USER_HEAP.alloc_physical<XD3DDevice>();
	std::memset(device, 0, sizeof(XD3DDevice));
	device->Viewport.Width = 1280.0f;
	device->Viewport.Height = 720.0f;
	device->Viewport.MaxZ = 1.0f;


	xpointer<XD3DDevice>* global_device_ptr = reinterpret_cast<xpointer<XD3DDevice>*>(GLOBAL_MEMORY.translate(0x82604988));
	global_device_ptr->ptr.set(GLOBAL_MEMORY.map_virtual(device));
}

GuestFunctionHook(sub_82584490, X360VideoCard_Create);
*/