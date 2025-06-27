#ifndef OGSE_REFLECTIONS_H
#define OGSE_REFLECTIONS_H
#ifndef REFLECTIONS_QUALITY
	#define	REFLECTIONS_QUALITY 1
#endif
#define EPS 0.001

//#ifdef USE_HQ_REFLECTIONS
	static const float2 resolution = ogse_c_resolution.xy;
	static const float2 inv_resolution = ogse_c_resolution.zw;
/*#else
	static const float2 resolution = ogse_c_resolution.xy*0.5;
	static const float2 inv_resolution = ogse_c_resolution.zw*2;
#endif*/


#include "reflections.h" //OGSR

#define REFL_WATER 0
#define REFL_GROUND 1
#define REFL_BOTH 2
#include "common.h"

#define SKY_WITH_DEPTH				// sky renders with depth to avoid some problems with reflections
#define SKY_DEPTH float(10000.f)
#define SKY_EPS float(0.001)
#define FARPLANE float(180.0)

#ifndef SKY_WITH_DEPTH
//half is_sky(float depth)		{return step(depth, SKY_EPS);} //Diesel cut OGSR
half is_not_sky(float depth)	{return step(SKY_EPS, depth);}
#else
//half is_sky(float depth)		{return step(abs(depth - SKY_DEPTH), SKY_EPS);} //Diesel cut OGSR
half is_not_sky(float depth)	{return step(SKY_EPS, abs(depth - SKY_DEPTH));}
#endif

//TextureCube s_env0; //Diesel cut OGSR
//TextureCube s_env1; //Diesel cut OGSR
Texture2D s_img;

Texture2D s_img2;

//float4	af			: COLOR1;		// alpha&factor

/* //Diesel cut OGSR
float4 proj_to_screen(float4 proj)
{
	float4 screen = proj;
	screen.x = (proj.x + proj.w);
	screen.y = (proj.w - proj.y);
	screen.xy *= 0.5;
	
	
	return screen;
}
*/ //Diesel cut OGSR

float4 screen_to_proj(float2 screen, float z)
{
	float4 proj;
	proj.w = 1.0;
	proj.z = z;
	proj.x = screen.x*2 - proj.w;
	proj.y = -screen.y*2 + proj.w;
	return proj;
}

//float4 screen_to_proj(float4 screen, float z)
//{
//	float4 proj;
//	proj.x = (screen.x);
//	proj.y = (screen.y);
//	proj.xy *= 0.5;
//	return proj;
//}

float	get_depth_fast			(float2 tc)
{
#ifndef USE_MSAA
	float _P = s_position.SampleLevel( smp_nofilter, tc, 0 ).z;
#else
	float _P = s_position.Load(int3( tc * pos_decompression_params2.xy, 0 ), 0 ).z;
#endif
	return _P;
	//s_position.SampleLevel(smp_nofilter, tc, 0).z;
}


float	get_depth_proj			(float4 tc)
{
//	tc.xy /= tc.w;
	float2	tcProj			= tc.xy / tc.w;
#ifndef USE_MSAA
	float _P = s_position.Sample( smp_nofilter, tcProj ).z;
#else
	float _P = s_position.Load(int3( tcProj * pos_decompression_params2.xy, 0 ), 0 ).z;
#endif
	return _P;
	//s_position.Sample(smp_nofilter, tc.xy / tc.w).z;
}

float4 get_reflection (float3 screen_pixel_pos, float3 next_screen_pixel_pos, float3 reflect)
{
	float4 final_color = {0.01,0.01,0.01,1.0};
	
	
	//float4 H 	= s_hemi.Sample( smp_base,  0 );	// expand

	//float4 final_color = {1.0,1.0,1.0,1.0};
	//float2 factors = {1.f,1.f};
	
	float3 eye_direction = next_screen_pixel_pos - screen_pixel_pos;
	
	float3 main_vec = next_screen_pixel_pos - screen_pixel_pos;
	//float3 grad_vec = main_vec / (max(abs(main_vec.x), abs(main_vec.y)) * 256);
	//float3 grad_vec = eye_direction / (max(abs(eye_direction.x), abs(eye_direction.y)) * 720);
	float3 grad_vec = eye_direction / (max(abs(eye_direction.x), abs(eye_direction.y)) * 256);
	
	// handle case when reflect vector faces the camera
	// factors.x = dot(eye_direction, reflect);
	

	
	//[branch]
	//if (factors.x < -0.5)
	//	return final_color;
		
	float3 curr_pixel = screen_pixel_pos;
	curr_pixel.xy += float2(0.5,0.5)*ogse_c_resolution.zw;
	//float max_it = 140;
	float max_it = 14;
	//float max_it = 520;
	

	#if (REFLECTIONS_QUALITY == 1)
		grad_vec *= 2;
		max_it *= 0.5;
	#endif
	
	// antifreeze for camera down
	float cam_dir = eye_direction.y;
	if (cam_dir < -0.25f)
	{
		float k = cam_dir * (16.f * cam_dir);
		clamp(k, 1.f, 8.f);
		grad_vec *= k;
		max_it /= k;
	}
	
	float i = 0;

	while (i < max_it)
	{
		curr_pixel.xyz += grad_vec.xyz;

		float depth = get_depth_fast(curr_pixel.xy);
		depth = lerp(depth, 0.f, is_sky(depth));

		float delta = step(depth, curr_pixel.z)*step(screen_pixel_pos.z, depth);
		if (delta > 0.5)
		{
			
				

			
			final_color.xyz = s_img.SampleLevel(smp_base, curr_pixel.xy, 0).xyz;
						
			//final_color.xyz = s_img.SampleLevel(smp_base, curr_pixel.xy, 0).xyz;
			
			
			//float3 color0 = s_img.SampleLevel( smp_rtlinear, curr_pixel.xy, 0).xyz;
			//float3 color1 = s_img2.SampleLevel( smp_rtlinear, curr_pixel.xy, 0).xyz;
			//final_color.xyz = lerp(color0, color1, 0.5);
			//final_color.xyz	*= final_color.xyz*2;
			
			float2 temp = curr_pixel.xy;
			// make sure that there is no fade down the screen
			temp.y = lerp(temp.y, 0.5, step(0.5, temp.y));
			float screendedgefact = saturate(distance(temp , float2(0.5, 0.5)) * 2.0);
			final_color.w = pow(screendedgefact,6);// * screendedgefact;
			

			break;
		}
		i += 1.0;
	}

#if (REFLECTIONS_QUALITY == 2)
	if (i >= max_it) return final_color;
	curr_pixel.xyz -= grad_vec.xyz;
	grad_vec *= 0.125;
	for (int i = 0; i < 8; ++i)
	{
		curr_pixel.xyz += grad_vec.xyz;
/*		if ((curr_pixel.x > 1.0) || (curr_pixel.y > 1.0) || (curr_pixel.x < 0.0) || (curr_pixel.y < 0.0)) 
		{
			final_color.xyz = float3(1.0,0.0,0.0);
			break;
		}*/
		float depth = get_depth_fast(curr_pixel.xy);
		depth = lerp(depth, 0.f, is_sky(depth));

//		depth += 1000*step(depth, 1.0);
		float delta = step(depth, curr_pixel.z)*step(screen_pixel_pos.z, depth);
		if (delta > 0.5)
		{
			// edge detect
			//final_color.xyz = tex2Dlod(s_image, float4(curr_pixel.xy,0,0)).xyz;
			final_color.xyz = s_img.SampleLevel( smp_rtlinear, curr_pixel.xy, 0).xyz;
			//float3 color0 = s_img.SampleLevel( smp_rtlinear, curr_pixel.xy, 0).xyz;
			//float3 color1 = s_img2.SampleLevel( smp_rtlinear, curr_pixel.xy, 0).xyz;
			//final_color.xyz = lerp(color0, color1, 0.5);
			float2 temp = curr_pixel.xy;
			// make sure that there is no fade down the screen
			temp.y = lerp(temp.y, 0.5, step(0.5, temp.y));
			float screendedgefact = saturate(distance(temp , float2(0.5, 0.5)) * 2.0);
			final_color.w = pow(screendedgefact,6);// * screendedgefact;
			break;
		}
		++i;
	}
#endif
	return final_color;
}

#include "sload.h"

/* //Diesel cut OGSR
float3 calc_envmap(float3 vreflect)
{	


	float3	env0	= s_env0.Sample( smp_rtlinear, vreflect).xyz;
	float3	env1	= s_env1.Sample( smp_rtlinear, vreflect).xyz;
	float3	env	= lerp (env0,env1,L_ambient.w);
	//env	*= env*2;
	return env;
}
*/ //Diesel cut OGSR


float4 calc_reflections(float4 pos, float3 vreflect, float sw)
{
	float4 refl = {1.0,1.0,1.0,1.0};
	
	
#ifdef USE_REFLECTIONS


	float3 v_pixel_pos = mul((float3x4)m_V, pos);
	float4 p_pixel_pos = mul(m_VP, pos);
	float4 s_pixel_pos = proj_to_screen(p_pixel_pos);
	s_pixel_pos.xy /= s_pixel_pos.w;
	s_pixel_pos.z = v_pixel_pos.z;
		
	float3 reflect_vec = normalize(vreflect);
	float3 W_m_point = pos.xyz + reflect_vec;

	float3 V_m_point = mul((float3x4)m_V, float4(W_m_point, 1.0));
	float4 P_m_point = mul(m_VP, float4(W_m_point, 1.0));
	float4 S_m_point = proj_to_screen(P_m_point);
	S_m_point.xy /= S_m_point.w;
	S_m_point.z = V_m_point.z;
		
		
	if (sw > 0.01)
		refl = get_reflection(s_pixel_pos.xyz, S_m_point.xyz, reflect_vec);
#endif
	return refl;
}
#endif