dxc.exe -T vs_6_1 -E vs_main -Fo 2d_vs_color.cso  2d_vs_color.hlsl
dxc.exe -T ps_6_1 -E ps_main -Fo 2d_ps_color.cso  2d_ps_color.hlsl
dxc.exe -T vs_6_1 -E vs_main -Fo vs_depth_pass.cso  vs_depth_pass.hlsl