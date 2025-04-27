#pragma once
#define RRE_GAME_TARGET_CARS1

#include <d3d9.h>
#include <cstdint>
#include "utils/xbox.h"


#if defined(RRE_GAME_TARGET_CARS1)
#define _XDK_VER_ 3529
#elif defined(RRE_GAME_TARGET_CARS2)
#define _XDK_VER_ 6274
#elif defined(RRE_GAME_TARGET_CARS3)
#define _XDK_VER_ 8276
#endif

struct XDirect3D;
struct XD3DDevice;
struct XD3DStateBlock;
struct XD3DVertexDeclaration;
struct XD3DVertexShader;
struct XD3DPixelShader;
struct XD3DResource;
struct XD3DBaseTexture;
struct XD3DTexture;
struct XD3DVolumeTexture;
struct XD3DCubeTexture;
struct XD3DArrayTexture;
struct XD3DLineTexture;
struct XD3DVertexBuffer;
struct XD3DIndexBuffer;
struct XD3DSurface;
struct XD3DVolume;
struct XD3DQuery;
struct XD3DPerfCounters;
struct XD3DConstantBuffer;
struct XD3DCommandBuffer;

enum XD3DRenderStateType : uint32_t {
    XD3DRS_ZENABLE = 40,
    XD3DRS_ZFUNC = 44,
    XD3DRS_ZWRITEENABLE = 48,
    XD3DRS_FILLMODE = 52,
    XD3DRS_CULLMODE = 56,
    XD3DRS_ALPHABLENDENABLE = 60,
    XD3DRS_SEPARATEALPHABLENDENABLE = 64,
    XD3DRS_BLENDFACTOR = 68,
    XD3DRS_SRCBLEND = 72,
    XD3DRS_DESTBLEND = 76,
    XD3DRS_BLENDOP = 80,
    XD3DRS_SRCBLENDALPHA = 84,
    XD3DRS_DESTBLENDALPHA = 88,
    XD3DRS_BLENDOPALPHA = 92,
    XD3DRS_ALPHATESTENABLE = 96,
    XD3DRS_ALPHAREF = 100,
    XD3DRS_ALPHAFUNC = 104,
    XD3DRS_STENCILENABLE = 108,
    XD3DRS_TWOSIDEDSTENCILMODE = 112,
    XD3DRS_STENCILFAIL = 116,
    XD3DRS_STENCILZFAIL = 120,
    XD3DRS_STENCILPASS = 124,
    XD3DRS_STENCILFUNC = 128,
    XD3DRS_STENCILREF = 132,
    XD3DRS_STENCILMASK = 136,
    XD3DRS_STENCILWRITEMASK = 140,
    XD3DRS_CCW_STENCILFAIL = 144,
    XD3DRS_CCW_STENCILZFAIL = 148,
    XD3DRS_CCW_STENCILPASS = 152,
    XD3DRS_CCW_STENCILFUNC = 156,
    XD3DRS_CCW_STENCILREF = 160,
    XD3DRS_CCW_STENCILMASK = 164,
    XD3DRS_CCW_STENCILWRITEMASK = 168,
    XD3DRS_CLIPPLANEENABLE = 172,
    XD3DRS_POINTSIZE = 176,
    XD3DRS_POINTSIZE_MIN = 180,
    XD3DRS_POINTSPRITEENABLE = 184,
    XD3DRS_POINTSIZE_MAX = 188,
    XD3DRS_MULTISAMPLEANTIALIAS = 192,
    XD3DRS_MULTISAMPLEMASK = 196,
    XD3DRS_SCISSORTESTENABLE = 200,
    XD3DRS_SLOPESCALEDEPTHBIAS = 204,
    XD3DRS_DEPTHBIAS = 208,
    XD3DRS_COLORWRITEENABLE = 212,
    XD3DRS_COLORWRITEENABLE1 = 216,
    XD3DRS_COLORWRITEENABLE2 = 220,
    XD3DRS_COLORWRITEENABLE3 = 224,
    XD3DRS_TESSELLATIONMODE = 228,
    XD3DRS_MINTESSELLATIONLEVEL = 232,
    XD3DRS_MAXTESSELLATIONLEVEL = 236,
    XD3DRS_WRAP0 = 240,
    XD3DRS_WRAP1 = 244,
    XD3DRS_WRAP2 = 248,
    XD3DRS_WRAP3 = 252,
    XD3DRS_WRAP4 = 256,
    XD3DRS_WRAP5 = 260,
    XD3DRS_WRAP6 = 264,
    XD3DRS_WRAP7 = 268,
    XD3DRS_WRAP8 = 272,
    XD3DRS_WRAP9 = 276,
    XD3DRS_WRAP10 = 280,
    XD3DRS_WRAP11 = 284,
    XD3DRS_WRAP12 = 288,
    XD3DRS_WRAP13 = 292,
    XD3DRS_WRAP14 = 296,
    XD3DRS_WRAP15 = 300,
    XD3DRS_VIEWPORTENABLE = 304,
    XD3DRS_HIGHPRECISIONBLENDENABLE = 308,
    XD3DRS_HIGHPRECISIONBLENDENABLE1 = 312,
    XD3DRS_HIGHPRECISIONBLENDENABLE2 = 316,
    XD3DRS_HIGHPRECISIONBLENDENABLE3 = 320,
    XD3DRS_HALFPIXELOFFSET = 324,
    XD3DRS_PRIMITIVERESETENABLE = 328,
    XD3DRS_PRIMITIVERESETINDEX = 332,
    XD3DRS_ALPHATOMASKENABLE = 336,
    XD3DRS_ALPHATOMASKOFFSETS = 340,
    XD3DRS_GUARDBAND_X = 344,
    XD3DRS_GUARDBAND_Y = 348,
    XD3DRS_DISCARDBAND_X = 352,
    XD3DRS_DISCARDBAND_Y = 356,
    XD3DRS_HISTENCILENABLE = 360,
    XD3DRS_HISTENCILWRITEENABLE = 364,
    XD3DRS_HISTENCILFUNC = 368,
    XD3DRS_HISTENCILREF = 372,
    XD3DRS_PRESENTINTERVAL = 376,
    XD3DRS_PRESENTIMMEDIATETHRESHOLD = 380,
    XD3DRS_HIZENABLE = 384,
    XD3DRS_HIZWRITEENABLE = 388,
    XD3DRS_LASTPIXEL = 392,
    XD3DRS_LINEWIDTH = 396,
    XD3DRS_BUFFER2FRAMES = 400,
    XD3DRS_MAX = 404,
    XD3DRS_FORCE_DWORD = 0x7fffffff,
};

enum XD3DSamplerStateType : uint32_t {
    XD3DSAMP_ADDRESSU = 0,
    XD3DSAMP_ADDRESSV = 4,
    XD3DSAMP_ADDRESSW = 8,
    XD3DSAMP_BORDERCOLOR = 12,
    XD3DSAMP_MAGFILTER = 16,
    XD3DSAMP_MINFILTER = 20,
    XD3DSAMP_MIPFILTER = 24,
    XD3DSAMP_MIPMAPLODBIAS = 28,
    XD3DSAMP_MAXMIPLEVEL = 32,
    XD3DSAMP_MAXANISOTROPY = 36,
    XD3DSAMP_MAGFILTERZ = 40,
    XD3DSAMP_MINFILTERZ = 44,
    XD3DSAMP_SEPARATEZFILTERENABLE = 48,
    XD3DSAMP_MINMIPLEVEL = 52,
    XD3DSAMP_TRILINEARTHRESHOLD = 56,
    XD3DSAMP_ANISOTROPYBIAS = 60,
    XD3DSAMP_HGRADIENTEXPBIAS = 64,
    XD3DSAMP_VGRADIENTEXPBIAS = 68,
    XD3DSAMP_WHITEBORDERCOLORW = 72,
    XD3DSAMP_POINTBORDERENABLE = 76,
    XD3DSAMP_MAX = 80,
    XD3DSAMP_FORCE_DWORD = 0x7fffffff,
};

/*
inline D3DSAMPLERSTATETYPE convert(XD3DSamplerStateType type) {
    switch (type) {
        case XD3DSAMP_ADDRESSU:
            return D3DSAMP_ADDRESSU;
        case XD3DSAMP_ADDRESSV:
            return D3DSAMP_ADDRESSV;
        case XD3DSAMP_ADDRESSW:
            return D3DSAMP_ADDRESSW;
        case XD3DSAMP_BORDERCOLOR:
            return D3DSAMP_BORDERCOLOR;
        case XD3DSAMP_MAGFILTER:
            return D3DSAMP_MAGFILTER;
        case XD3DSAMP_MINFILTER:
            return D3DSAMP_MINFILTER;
        case XD3DSAMP_MIPFILTER:
            return D3DSAMP_MIPFILTER;
        case XD3DSAMP_MIPMAPLODBIAS:
            return D3DSAMP_MIPMAPLODBIAS;
        case XD3DSAMP_MAXMIPLEVEL:
            return D3DSAMP_MAXMIPLEVEL;
        case XD3DSAMP_MAXANISOTROPY:
            return D3DSAMP_MAXANISOTROPY;
        case XD3DSAMP_MAGFILTERZ:
            return D3DSAMP_FORCE_DWORD;
        case XD3DSAMP_MINFILTERZ:
            return D3DSAMP_FORCE_DWORD;
        case XD3DSAMP_SEPARATEZFILTERENABLE:
            return D3DSAMP_FORCE_DWORD;
        case XD3DSAMP_MINMIPLEVEL:
            return D3DSAMP_FORCE_DWORD;
        case XD3DSAMP_TRILINEARTHRESHOLD:
            return D3DSAMP_FORCE_DWORD;
        case XD3DSAMP_ANISOTROPYBIAS:
            return D3DSAMP_FORCE_DWORD;
        case XD3DSAMP_HGRADIENTEXPBIAS:
            return D3DSAMP_FORCE_DWORD;
        case XD3DSAMP_VGRADIENTEXPBIAS:
            return D3DSAMP_FORCE_DWORD;
        case XD3DSAMP_WHITEBORDERCOLORW:
            return D3DSAMP_FORCE_DWORD;
        case XD3DSAMP_POINTBORDERENABLE:
            return D3DSAMP_FORCE_DWORD;
        case XD3DSAMP_MAX:
            return D3DSAMP_FORCE_DWORD;
        case XD3DSAMP_FORCE_DWORD:
            return D3DSAMP_FORCE_DWORD;
    }
}
*/

enum XD3DFormat : uint32_t {
    XD3DFMT_DXT1 = 0x1a200152,
    XD3DFMT_LIN_DXT1 = 0x1a200052,
    XD3DFMT_DXT2 = 0x1a200153,
    XD3DFMT_LIN_DXT2 = 0x1a200053,
    XD3DFMT_DXT3 = 0x1a200153,
    XD3DFMT_LIN_DXT3 = 0x1a200053,
    XD3DFMT_DXT3A = 0x1a20017a,
    XD3DFMT_LIN_DXT3A = 0x1a20007a,
    XD3DFMT_DXT3A_1111 = 0x1a20017d,
    XD3DFMT_LIN_DXT3A_1111 = 0x1a20007d,
    XD3DFMT_DXT4 = 0x1a200154,
    XD3DFMT_LIN_DXT4 = 0x1a200054,
    XD3DFMT_DXT5 = 0x1a200154,
    XD3DFMT_LIN_DXT5 = 0x1a200054,
    XD3DFMT_DXT5A = 0x1a20017b,
    XD3DFMT_LIN_DXT5A = 0x1a20007b,
    XD3DFMT_DXN = 0x1a200171,
    XD3DFMT_LIN_DXN = 0x1a200071,
    XD3DFMT_CTX1 = 0x1a20017c,
    XD3DFMT_LIN_CTX1 = 0x1a20007c,
    XD3DFMT_A8 = 0x4900102,
    XD3DFMT_LIN_A8 = 0x4900002,
    XD3DFMT_L8 = 0x28000102,
    XD3DFMT_LIN_L8 = 0x28000002,
    XD3DFMT_R5G6B5 = 0x28280144,
    XD3DFMT_LIN_R5G6B5 = 0x28280044,
    XD3DFMT_R6G5B5 = 0x28280145,
    XD3DFMT_LIN_R6G5B5 = 0x28280045,
    XD3DFMT_L6V5U5 = 0x2a200b45,
    XD3DFMT_LIN_L6V5U5 = 0x2a200a45,
    XD3DFMT_X1R5G5B5 = 0x28280143,
    XD3DFMT_LIN_X1R5G5B5 = 0x28280043,
    XD3DFMT_A1R5G5B5 = 0x18280143,
    XD3DFMT_LIN_A1R5G5B5 = 0x18280043,
    XD3DFMT_A4R4G4B4 = 0x1828014f,
    XD3DFMT_LIN_A4R4G4B4 = 0x1828004f,
    XD3DFMT_X4R4G4B4 = 0x2828014f,
    XD3DFMT_LIN_X4R4G4B4 = 0x2828004f,
    XD3DFMT_Q4W4V4U4 = 0x1a20ab4f,
    XD3DFMT_LIN_Q4W4V4U4 = 0x1a20aa4f,
    XD3DFMT_A8L8 = 0x800014a,
    XD3DFMT_LIN_A8L8 = 0x800004a,
    XD3DFMT_G8R8 = 0x2d20014a,
    XD3DFMT_LIN_G8R8 = 0x2d20004a,
    XD3DFMT_V8U8 = 0x2d20ab4a,
    XD3DFMT_LIN_V8U8 = 0x2d20aa4a,
    XD3DFMT_D16 = 0x1a220158,
    XD3DFMT_LIN_D16 = 0x1a220058,
    XD3DFMT_L16 = 0x28000158,
    XD3DFMT_LIN_L16 = 0x28000058,
    XD3DFMT_R16F = 0x2da2ab5e,
    XD3DFMT_LIN_R16F = 0x2da2aa5e,
    XD3DFMT_R16F_EXPAND = 0x2da2ab5b,
    XD3DFMT_LIN_R16F_EXPAND = 0x2da2aa5b,
    XD3DFMT_UYVY = 0x1a20014c,
    XD3DFMT_LIN_UYVY = 0x1a20004c,
    XD3DFMT_LE_UYVY = 0x1a20010c,
    XD3DFMT_LE_LIN_UYVY = 0x1a20000c,
    XD3DFMT_G8R8_G8B8 = 0x1828014c,
    XD3DFMT_LIN_G8R8_G8B8 = 0x1828004c,
    XD3DFMT_R8G8_B8G8 = 0x1828014b,
    XD3DFMT_LIN_R8G8_B8G8 = 0x1828004b,
    XD3DFMT_YUY2 = 0x1a20014b,
    XD3DFMT_LIN_YUY2 = 0x1a20004b,
    XD3DFMT_LE_YUY2 = 0x1a20010b,
    XD3DFMT_LE_LIN_YUY2 = 0x1a20000b,
    XD3DFMT_A8R8G8B8 = 0x18280186,
    XD3DFMT_LIN_A8R8G8B8 = 0x18280086,
    XD3DFMT_X8R8G8B8 = 0x28280186,
    XD3DFMT_LIN_X8R8G8B8 = 0x28280086,
    XD3DFMT_A8B8G8R8 = 0x1a200186,
    XD3DFMT_LIN_A8B8G8R8 = 0x1a200086,
    XD3DFMT_X8B8G8R8 = 0x2a200186,
    XD3DFMT_LIN_X8B8G8R8 = 0x2a200086,
    XD3DFMT_X8L8V8U8 = 0x2a200b86,
    XD3DFMT_LIN_X8L8V8U8 = 0x2a200a86,
    XD3DFMT_Q8W8V8U8 = 0x1a20ab86,
    XD3DFMT_LIN_Q8W8V8U8 = 0x1a20aa86,
    XD3DFMT_A2R10G10B10 = 0x182801b6,
    XD3DFMT_LIN_A2R10G10B10 = 0x182800b6,
    XD3DFMT_X2R10G10B10 = 0x282801b6,
    XD3DFMT_LIN_X2R10G10B10 = 0x282800b6,
    XD3DFMT_A2B10G10R10 = 0x1a2001b6,
    XD3DFMT_LIN_A2B10G10R10 = 0x1a2000b6,
    XD3DFMT_A2W10V10U10 = 0x1a202bb6,
    XD3DFMT_LIN_A2W10V10U10 = 0x1a202ab6,
    XD3DFMT_A16L16 = 0x8000199,
    XD3DFMT_LIN_A16L16 = 0x8000099,
    XD3DFMT_G16R16 = 0x2d200199,
    XD3DFMT_LIN_G16R16 = 0x2d200099,
    XD3DFMT_V16U16 = 0x2d20ab99,
    XD3DFMT_LIN_V16U16 = 0x2d20aa99,
    XD3DFMT_R10G11B11 = 0x282801b7,
    XD3DFMT_LIN_R10G11B11 = 0x282800b7,
    XD3DFMT_R11G11B10 = 0x282801b8,
    XD3DFMT_LIN_R11G11B10 = 0x282800b8,
    XD3DFMT_W10V11U11 = 0x2a20abb7,
    XD3DFMT_LIN_W10V11U11 = 0x2a20aab7,
    XD3DFMT_W11V11U10 = 0x2a20abb8,
    XD3DFMT_LIN_W11V11U10 = 0x2a20aab8,
    XD3DFMT_G16R16F = 0x2d22ab9f,
    XD3DFMT_LIN_G16R16F = 0x2d22aa9f,
    XD3DFMT_G16R16F_EXPAND = 0x2d22ab9c,
    XD3DFMT_LIN_G16R16F_EXPAND = 0x2d22aa9c,
    XD3DFMT_L32 = 0x280001a1,
    XD3DFMT_LIN_L32 = 0x280000a1,
    XD3DFMT_R32F = 0x2da2aba4,
    XD3DFMT_LIN_R32F = 0x2da2aaa4,
    XD3DFMT_D24S8 = 0x2d200196,
    XD3DFMT_LIN_D24S8 = 0x2d200096,
    XD3DFMT_D24X8 = 0x2da00196,
    XD3DFMT_LIN_D24X8 = 0x2da00096,
    XD3DFMT_D24FS8 = 0x1a220197,
    XD3DFMT_LIN_D24FS8 = 0x1a220097,
    XD3DFMT_D32 = 0x1a2201a1,
    XD3DFMT_LIN_D32 = 0x1a2200a1,
    XD3DFMT_A16B16G16R16 = 0x1a20015a,
    XD3DFMT_LIN_A16B16G16R16 = 0x1a20005a,
    XD3DFMT_Q16W16V16U16 = 0x1a20ab5a,
    XD3DFMT_LIN_Q16W16V16U16 = 0x1a20aa5a,
    XD3DFMT_A16B16G16R16F = 0x1a22ab60,
    XD3DFMT_LIN_A16B16G16R16F = 0x1a22aa60,
    XD3DFMT_A16B16G16R16F_EXPAND = 0x1a22ab5d,
    XD3DFMT_LIN_A16B16G16R16F_EXPAND = 0x1a22aa5d,
    XD3DFMT_A32L32 = 0x80001a2,
    XD3DFMT_LIN_A32L32 = 0x80000a2,
    XD3DFMT_G32R32 = 0x2d2001a2,
    XD3DFMT_LIN_G32R32 = 0x2d2000a2,
    XD3DFMT_V32U32 = 0x2d20aba2,
    XD3DFMT_LIN_V32U32 = 0x2d20aaa2,
    XD3DFMT_G32R32F = 0x2d22aba5,
    XD3DFMT_LIN_G32R32F = 0x2d22aaa5,
    XD3DFMT_A32B32G32R32 = 0x1a2001a3,
    XD3DFMT_LIN_A32B32G32R32 = 0x1a2000a3,
    XD3DFMT_Q32W32V32U32 = 0x1a20aba3,
    XD3DFMT_LIN_Q32W32V32U32 = 0x1a20aaa3,
    XD3DFMT_A32B32G32R32F = 0x1a22aba6,
    XD3DFMT_LIN_A32B32G32R32F = 0x1a22aaa6,
    XD3DFMT_A2B10G10R10F_EDRAM = 0x1a2201bf,
    XD3DFMT_G16R16_EDRAM = 0x2d20ab8d,
    XD3DFMT_A16B16G16R16_EDRAM = 0x1a20ab55,
    XD3DFMT_LE_X8R8G8B8 = 0x28280106,
    XD3DFMT_LE_A8R8G8B8 = 0x18280106,
    XD3DFMT_LE_X2R10G10B10 = 0x28280136,
    XD3DFMT_LE_A2R10G10B10 = 0x18280136,
    XD3DFMT_INDEX16 = 0x1,
    XD3DFMT_INDEX32 = 0x6,
    XD3DFMT_LE_INDEX16 = 0x0,
    XD3DFMT_LE_INDEX32 = 0x4,
    XD3DFMT_VERTEXDATA = 0x8,
    XD3DFMT_UNKNOWN = 0xFFFFFFFF
};

using XD3DDeclUsage = D3DDECLUSAGE;

#pragma pack(push, 1)

struct XD3DViewport {
    be<uint32_t> X;
    be<uint32_t> Y;
    be<uint32_t> Width;
    be<uint32_t> Height;
    be<float> MinZ;
    be<float> MaxZ;
    be<uint32_t> Flags;
};

struct XD3DTextureFetchConstant {
    be<uint32_t> inner[6];
};

struct XD3DSurfaces {
    xpointer<XD3DSurface> pDepthStencilSurface;
    xpointer<XD3DSurface> pRenderTarget[4];
};

struct XD3DVertexElement {
    be<uint16_t> Stream;
    be<uint16_t> Offset;
    be<uint32_t> Type;
    uint8_t Method;
    uint8_t Usage;
    uint8_t UsageIndex;
    uint8_t Padding;
};

struct XD3DDeviceConstants {
    XD3DTextureFetchConstant SamplerStates[0x20];
    be<float> VertexShaderFloatConstants[0x400];
    be<float> PixelShaderFloatConstants[0x400];
    be<uint32_t> VertexShaderBoolConstants[0x4];
    be<uint32_t> PixelShaderBoolConstants[0x4];
    be<uint32_t> VertexShaderIntConstants[16];
    be<uint32_t> PixelShaderIntConstants[16];
};

static_assert(sizeof(XD3DDeviceConstants) == 0x23A0);

struct XD3DDeviceTagCollection {
    be<uint64_t> Mask[5];
};

struct XD3DDevicePublic {
    XD3DDeviceTagCollection Pending;
    be<uint64_t> PredicatedPendingMask2;
    xpointer<be<uint32_t>> Ring;
    xpointer<be<uint32_t>> RingLimit;
    xpointer<be<uint32_t>> RingGuarantee;
    be<uint32_t> ReferenceCount;
    be<uint32_t> SetRenderStateCalls[XD3DRenderStateType::XD3DRS_MAX / 4];
    be<uint32_t> SetSamplerStateCalls[XD3DSamplerStateType::XD3DSAMP_MAX / 4];
    be<uint32_t> GetRenderStateCalls[XD3DRenderStateType::XD3DRS_MAX / 4];
    be<uint32_t> GetSamplerStateCalls[XD3DSamplerStateType::XD3DSAMP_MAX / 4];  
    uint8_t Alignment[120];
    XD3DDeviceConstants Constants;
    be<float> ClipPlanes[6][4];
    be<uint32_t> DestinationPacket[0x48 / 4];
    be<uint32_t> WindowPacket[0x18 / 4];
    be<uint32_t> ValuesPacket[0x54 / 4];
    be<uint32_t> ProgramPacket[0x14 / 4];
    be<uint32_t> ControlPacket[0x30 / 4];
    be<uint32_t> TessellatorPacket[0x54 / 4];
    be<uint32_t> MiscPacket[0x98 / 4];
    be<uint32_t> PointPacket[0x20 / 4];
};

static_assert(sizeof(XD3DDevicePublic) == 0x2A84);

#if _XDK_VER_ == 3529

struct XD3DDevice : public XD3DDevicePublic {
    uint8_t UnkReserved[0x28C];
    be<uint32_t> VertexDeclaration;
    uint8_t UnkReserved2[0x344];
    struct {
        be<float> X;
        be<float> Y;
        be<float> Width;
        be<float> Height;
        be<float> MinZ;
        be<float> MaxZ;
        be<uint32_t> Flags;
    } Viewport;
    uint8_t UnkReserved3[0x1F8C];
};

static_assert(sizeof(XD3DDevice) == 0x5000);
#elif _XDK_VER_ == 6274
static_assert(sizeof(XD3DDevice) == 0x5800);
#elif _XDK_VER_ == 8276

struct XD3DDevice : public XD3DDevicePublic {
    uint8_t UnkReserved[0x3AC];
    be<uint32_t> VertexDeclaration;
    uint8_t UnkReserved2[0x33C];
    struct {
        be<float> X;
        be<float> Y;
        be<float> Width;
        be<float> Height;
        be<float> MinZ;
        be<float> MaxZ;
        be<uint32_t> Flags;
    } Viewport;
     uint8_t UnkReserved3[0x2C74];
};

static_assert(sizeof(XD3DDevice) == 0x5E00);

#endif

struct XD3DResource {
    be<uint32_t> Common;
    be<uint32_t> ReferenceCount;
    be<uint32_t> Fence;
    be<uint32_t> ReadFence;
    be<uint32_t> Identifier;
    be<uint32_t> BaseFlush;
};

struct XD3DBaseTexture : public XD3DResource {
    be<uint32_t> MipFlush;
    XD3DTextureFetchConstant Format;
};

#pragma pack(pop)
