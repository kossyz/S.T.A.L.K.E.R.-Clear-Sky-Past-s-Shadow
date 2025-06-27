#include "common.h"

struct vf
{
	float2 tc0		: TEXCOORD0;
	float3 M1		: TEXCOORD1;
	float3 M2		: TEXCOORD2;
	float3 M3		: TEXCOORD3;
	float4 position	: TEXCOORD4;
	float4 hpos		: SV_Position;
};

vf main (v_static v)
{
	vf 		o;

	o.hpos 			= mul			(m_VP, v.P);			// xform, input in world coords
	o.tc0			= unpack_tc_base(v.tc,v.T.w,v.B.w);		// copy tc

	v.Nh			= unpack_D3DCOLOR(v.Nh);
	v.T				= unpack_D3DCOLOR(v.T);
	v.B				= unpack_D3DCOLOR(v.B);
	float3 	N 		= unpack_bx4(v.Nh);	// just scale (assume normal in the -.5f, .5f)
	float3 	T 		= unpack_bx4(v.T);	// 
	float3 	B 		= unpack_bx4(v.B);	// 
	float3x3 xform	= mul	((float3x3)m_V, float3x3(
						T.x,B.x,N.x,
						T.y,B.y,N.y,
						T.z,B.z,N.z
				));
	o.M1 			= xform[0];
	o.M2 			= xform[1];
	o.M3 			= xform[2];
	
	float3 Pe 		= mul(m_V, v.P);
	o.position 		= float4(Pe, v.Nh.w);

	return o;
}
