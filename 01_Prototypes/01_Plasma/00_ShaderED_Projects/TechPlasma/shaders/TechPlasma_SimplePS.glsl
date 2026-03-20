/*TechPlasma_SimplePS.glsl
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

#version 330

uniform vec2 uResolution;
uniform float uTime;

uniform sampler2D uTxBase;
uniform sampler2D uTxGrid;
uniform sampler2D uTxDet0;
uniform sampler2D uTxDet1;
uniform sampler2D uTxGradient;

out vec4 outColor;

float blendDarken(float base, float blend, float opacity) {
	return (min(blend,base) * opacity + base * (1.0 - opacity));
}

float blendDifference(float base, float blend, float opacity) {
	return (abs(base-blend) * opacity + base * (1.0 - opacity));
}

void main()
{
    vec2 uv = gl_FragCoord.xy/uResolution;
    
    vec2 uvBase = vec2(uv.x+sin(uTime*0.03), uv.y-cos(uTime*0.05));
    float base = texture(uTxBase, uvBase).r; //layer 0
    
    vec2 uvGrid = vec2(uv.x+cos(uTime*0.06), uv.y-sin(uTime*0.03));
    float grid = texture(uTxGrid, uvGrid).r; //layer 1
    
    vec2 uvDet0 = vec2(uv.x-cos(uTime*0.04), uv.y+sin(uTime*0.02));
    float det0 = texture(uTxDet0, uvDet0).r; //layer 2
    
    vec2 uvDet1 = vec2(uv.x-sin(uTime*0.02667), uv.y+cos(uTime*0.0334));
    float det1 = texture(uTxDet1, uvDet1).r; //layer 3
    
    float layer0 = blendDifference(base, grid, 1.0f);                  
    float layer2 = blendDarken(layer0, det0, 1.0f);
    float mask = blendDifference(layer2, det1, 1.0f);
    
    float gU = sin(mask+uTime*0.2)*0.5+0.5;
    float gV = cos(mask-uTime*0.32)*0.5+0.5;
    
    outColor = texture(uTxGradient, vec2(gU, gV));
  
}