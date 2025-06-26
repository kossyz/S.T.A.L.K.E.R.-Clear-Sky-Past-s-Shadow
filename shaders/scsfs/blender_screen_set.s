function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("blender_screen_set","blender_screen_set")

--	Decouple sampler and texture
--	shader:sampler	("s_base")	: texture(t_base)	: clamp() : f_linear ()
--	TODO: DX10: move blender_screen_set to smp_rtlinear
	shader:dx10texture	("s_base", t_base)
	shader:dx10sampler	("smp_base")
end