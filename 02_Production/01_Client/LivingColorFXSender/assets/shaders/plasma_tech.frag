/*plasma_tech.frag
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

float blendDarken(float base, float blend, float opacity) {
	return (min(blend,base) * opacity + base * (1.0 - opacity));
}

float blendDifference(float base, float blend, float opacity) {
	return (abs(base-blend) * opacity + base * (1.0 - opacity));
}

void main()
{
    vec2 uv = vTexCoord0 * vec2(0.5625,1.0) * 0.25;
    
    vec2 uv_0 = vec2(uv.x+sin(ciElapsedSeconds*0.03), uv.y-cos(ciElapsedSeconds*0.05));
    float base = texture(uTxNoise, uv_0).r; //layer 0
    
    vec2 uv_1 = vec2(uv.x+cos(ciElapsedSeconds*0.06), uv.y-sin(ciElapsedSeconds*0.03));
    float grid = texture(uTxNoise, uv_1).g; //layer 1
    
    vec2 uv_2 = vec2(uv.x-cos(ciElapsedSeconds*0.04), uv.y+sin(ciElapsedSeconds*0.02));
    float det0 = texture(uTxNoise, uv_2).b; //layer 2
    
    vec2 uv_3 = vec2(uv.x-sin(ciElapsedSeconds*0.02667), uv.y+cos(ciElapsedSeconds*0.0334));
    float det1 = texture(uTxNoise, uv_3).a; //layer 3
    
    float layer0 = blendDifference(base, grid, 1.0f);                  
    float layer2 = blendDarken(layer0, det0, 1.0f);
    float mask = blendDifference(layer2, det1, 1.0f);
    
    outColor = texture(uTxGrad, vec2(sin(mask+ciElapsedSeconds*0.2),cos(mask-ciElapsedSeconds*0.32)));
}