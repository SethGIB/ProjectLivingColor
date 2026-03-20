/*plasma_neo.frag
This file is part of the Living Color project (https://github.com/SethGIB/ProjectLivingColor)

Copyright (C) 2026 Seth Gibson

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

PORTIONS OF THIS CODE ARE BASED ON THE FOLLOWING:
- Cinder (https://libcinder.org/)
- ASIO (https://think-async.com/Asio/)
- OpenCV (https://opencv.org/)
- RealSense SDK (https://realsenseai.com/)

AND ARE USED IN COMPLIANCE WITH THEIR RESPECTIVE LICENSES. SEE THEIR WEBSITES FOR MORE INFORMATION.*/	

#version 150

uniform vec2 uResolution;
uniform float ciElapsedSeconds;

uniform sampler2D uTxNoise;
uniform sampler2D uTxGrad;

in vec2 vTexCoord0;
out vec4 outColor;

float blendOverlay(float base, float blend, float opacity)
{
	return (base<0.5?(2.0*base*blend):(1.0-2.0*(1.0-base)*(1.0-blend))) * opacity + base * (1.0f-opacity);
}

float blendColorBurn(float base, float blend, float opacity)
{
	return ((blend==0.0)?blend:max((1.0-((1.0-base)/blend)),0.0)) * opacity + base * (1.0f-opacity);
}

float blendScreen(float base, float blend, float opacity)
{
	return (1.0-((1.0-base)*(1.0-blend))) * opacity + base * (1.0f-opacity);
}
float kL2Alpha = 0.5f;
float kL1Alpha = 0.2f;
float kL0Alpha = 0.7f;
float kGrpAlpha = 0.9f;
float kBaseAlpha = 0.48f;

void main()
{
	vec2 uv = vTexCoord0 * vec2(0.5625,1.0) * 0.5;
    
	vec2 uv_0 = vec2(uv.x+sin((0.01-ciElapsedSeconds)*0.115f),uv.y-cos(0.002-ciElapsedSeconds*0.077));
	float t_l2 = texture(uTxNoise, uv_0).a;
    
	vec2 uv_1 = vec2(uv.x, uv.y+ciElapsedSeconds*0.06);
	float t_l1 = texture(uTxNoise, uv_1).b;
    
	vec2 uv_2 = vec2(uv.x, uv.y+ciElapsedSeconds*-0.04);
	float t_l0 = texture(uTxNoise, uv_2).g;
    
	vec2 uv_3 = vec2(uv.x-cos(ciElapsedSeconds*0.15f), uv.y);
	float t_b = texture(uTxNoise, uv_3).r;
    
	float t_brn = blendColorBurn(t_l0*kL0Alpha, t_l1, kL1Alpha);
	float t_ovl = blendOverlay(t_brn, t_l2, kL2Alpha);
	float t_scrn = blendScreen(t_b*kBaseAlpha, t_ovl, kGrpAlpha);
			
    outColor = texture(uTxGrad, vec2(t_scrn+sin(ciElapsedSeconds*0.5), t_scrn-cos(ciElapsedSeconds*0.26633)));
}