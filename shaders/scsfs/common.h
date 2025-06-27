#ifndef        COMMON_H
#define        COMMON_H

#include "shared\common.h"

#include "common_defines.h"
#include "common_policies.h"
#include "common_iostructs.h"
#include "common_samplers.h"
#include "common_cbuffers.h"
#include "common_functions.h"
#include "ogse_config.h"

#ifdef        USE_R2_STATIC_SUN
#  define xmaterial float(1.0h/4.h)
#else
#  define xmaterial float(L_material.w)
#endif

// new uniform variables - OGSE Team
// global constants
uniform float4 ogse_c_resolution;	// x - width, y - height, z - 1/width, w - 1/height
uniform float4 ogse_c_screen;		// x - fFOV, y - fAspect, z - Zf/(Zf-Zn), w - Zn*tan(fFov/2)
uniform float4 ogse_c_jitter;		// x - jitter u, y - jitter v. Test for smaa t2x, don't touch
uniform float4 ogse_c_various;		// x - moonroad flag from weather

float3x4 m_sunmask;   //теперь единая переменная для всех файлов, включая rain


//#define USE_SUNMASK    //тени облаков
#define USE_CINEMATIC  //можно сопоставить с консольной командой или включить навсегда

#define FXPS technique _render{pass _code{PixelShader=compile ps_3_0 main();}}
#define FXVS technique _render{pass _code{VertexShader=compile vs_3_0 main();}}

#include "ogse_gbuffer.h"
#endif
