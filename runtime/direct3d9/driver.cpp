// HEAVY HEAVY WIP NOTHING HERE WORKS IT IS THE ROUGHEST OF ROUGH DRAFTS
#include <optional>
#include <array>
#include <d3d11.h>
#include <d3d11_4.h>
#include "runtime/direct3d9/d3d9_x.hpp"
#include "melody/shader_pass_descriptor.hpp"

inline DXGI_FORMAT to_dxgi(XD3DFormat _fmt) {
	if (_fmt == XD3DFormat::XD3DFMT_DXT1) {
		return DXGI_FORMAT_BC1_UNORM;
	}
	return DXGI_FORMAT_UNKNOWN;
}

inline D3D11_COMPARISON_FUNC to_comparison_func(uint32_t value) {
    switch (value) {
    case D3DCMP_NEVER: return D3D11_COMPARISON_NEVER;
    case D3DCMP_LESS: return D3D11_COMPARISON_LESS;
    case D3DCMP_EQUAL: return D3D11_COMPARISON_EQUAL;
    case D3DCMP_LESSEQUAL: return D3D11_COMPARISON_LESS_EQUAL;
    case D3DCMP_GREATER: return D3D11_COMPARISON_GREATER;
    case D3DCMP_NOTEQUAL: return D3D11_COMPARISON_NOT_EQUAL;
    case D3DCMP_GREATEREQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
    case D3DCMP_ALWAYS: return D3D11_COMPARISON_ALWAYS;
    default: return D3D11_COMPARISON_ALWAYS;
    }
}

inline D3D11_DEPTH_WRITE_MASK to_depth_write_mask(uint32_t value) {
    return value ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
}

inline D3D11_FILL_MODE to_fill_mode(uint32_t value) {
    switch (value) {
    case D3DFILL_POINT: return D3D11_FILL_WIREFRAME;
    case D3DFILL_WIREFRAME: return D3D11_FILL_WIREFRAME;
    case D3DFILL_SOLID: return D3D11_FILL_SOLID;
    default: return D3D11_FILL_SOLID;
    }
}

inline D3D11_CULL_MODE to_cull_mode(uint32_t value) {
    switch (value) {
    case D3DCULL_NONE: return D3D11_CULL_NONE;
    case D3DCULL_CW: return D3D11_CULL_BACK;
    case D3DCULL_CCW: return D3D11_CULL_FRONT;
    default: return D3D11_CULL_NONE;
    }
}

inline D3D11_BLEND to_blend(uint32_t value) {
    switch (value) {
    case D3DBLEND_ZERO: return D3D11_BLEND_ZERO;
    case D3DBLEND_ONE: return D3D11_BLEND_ONE;
    case D3DBLEND_SRCCOLOR: return D3D11_BLEND_SRC_COLOR;
    case D3DBLEND_INVSRCCOLOR: return D3D11_BLEND_INV_SRC_COLOR;
    case D3DBLEND_SRCALPHA: return D3D11_BLEND_SRC_ALPHA;
    case D3DBLEND_INVSRCALPHA: return D3D11_BLEND_INV_SRC_ALPHA;
    case D3DBLEND_DESTALPHA: return D3D11_BLEND_DEST_ALPHA;
    case D3DBLEND_INVDESTALPHA: return D3D11_BLEND_INV_DEST_ALPHA;
    case D3DBLEND_DESTCOLOR: return D3D11_BLEND_DEST_COLOR;
    case D3DBLEND_INVDESTCOLOR: return D3D11_BLEND_INV_DEST_COLOR;
    case D3DBLEND_SRCALPHASAT: return D3D11_BLEND_SRC_ALPHA_SAT;
    default: return D3D11_BLEND_ONE;
    }
}

inline D3D11_BLEND_OP to_blend_op(uint32_t value) {
    switch (value) {
    case D3DBLENDOP_ADD: return D3D11_BLEND_OP_ADD;
    case D3DBLENDOP_SUBTRACT: return D3D11_BLEND_OP_SUBTRACT;
    case D3DBLENDOP_REVSUBTRACT: return D3D11_BLEND_OP_REV_SUBTRACT;
    case D3DBLENDOP_MIN: return D3D11_BLEND_OP_MIN;
    case D3DBLENDOP_MAX: return D3D11_BLEND_OP_MAX;
    default: return D3D11_BLEND_OP_ADD;
    }
}

inline D3D11_STENCIL_OP to_stencil_op(uint32_t value) {
    switch (value) {
    case D3DSTENCILOP_KEEP: return D3D11_STENCIL_OP_KEEP;
    case D3DSTENCILOP_ZERO: return D3D11_STENCIL_OP_ZERO;
    case D3DSTENCILOP_REPLACE: return D3D11_STENCIL_OP_REPLACE;
    case D3DSTENCILOP_INCRSAT: return D3D11_STENCIL_OP_INCR_SAT;
    case D3DSTENCILOP_DECRSAT: return D3D11_STENCIL_OP_DECR_SAT;
    case D3DSTENCILOP_INVERT: return D3D11_STENCIL_OP_INVERT;
    case D3DSTENCILOP_INCR: return D3D11_STENCIL_OP_INCR;
    case D3DSTENCILOP_DECR: return D3D11_STENCIL_OP_DECR;
    default: return D3D11_STENCIL_OP_KEEP;
    }
}


D3D11_TEXTURE_ADDRESS_MODE to_texture_addr(uint32_t addr)  {
    switch (addr) {
    case D3DTADDRESS_WRAP:
        return D3D11_TEXTURE_ADDRESS_WRAP;
    case D3DTADDRESS_MIRROR:
        return D3D11_TEXTURE_ADDRESS_MIRROR;
    case D3DTADDRESS_CLAMP:
        return D3D11_TEXTURE_ADDRESS_CLAMP;
    case D3DTADDRESS_BORDER:
        return D3D11_TEXTURE_ADDRESS_BORDER;
    case D3DTADDRESS_MIRRORONCE:
        return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
    }
}


D3D11_FILTER_TYPE to_filter_type(uint32_t filter) {
    switch (filter) {
    case D3DTEXF_NONE:
    case D3DTEXF_POINT:
        return D3D11_FILTER_TYPE_POINT;
    default:
    case D3DTEXF_LINEAR:
        return D3D11_FILTER_TYPE_LINEAR;
    case D3DTEXF_ANISOTROPIC:
        return D3D11_FILTER_TYPE_LINEAR;
    }
}


struct PipelineDesc {
	D3D11_BLEND_DESC blend;
	D3D11_RASTERIZER_DESC rasterizer;
	D3D11_DEPTH_STENCIL_DESC depth_stencil;
	std::array<D3D11_SAMPLER_DESC, 16> samplers;

    // If a sampler is like empty use a static global and AddRef before returning.
    auto compile() -> std::tuple<ID3D11BlendState1*, ID3D11RasterizerState2*, ID3D11DepthStencilState*, std::array<ID3D11SamplerState*, 16>> {
        
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
            // OMSetBlendState
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
            // OMSetDepthStencilState
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
            // OMSetDepthStencilState
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