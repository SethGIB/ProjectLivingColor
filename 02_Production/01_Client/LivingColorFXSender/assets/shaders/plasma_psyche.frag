/*plasma_psyche.frag
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

void main()
{
    vec2 uv = vTexCoord0 * vec2(0.5625,1.0) * 0.5;
    
    vec2 uv_0 = vec2(uv.x+ciElapsedSeconds*0.1, uv.y);
    float base = texture(uTxNoise, uv_0).r;
    
    vec2 uv_1 = vec2(uv.x-ciElapsedSeconds*0.0667, uv.y);
    float mid = texture(uTxNoise, uv_1).g;
    
    vec2 uv_2 = vec2(sin(uv.x-ciElapsedSeconds*0.01), uv.y+ciElapsedSeconds*0.1);
    float detail = texture(uTxNoise, uv_2).b;
    
    vec2 uv_3 = vec2(uv.x+cos(ciElapsedSeconds*0.1), sin(uv.y-ciElapsedSeconds*0.04));
    float ovl = texture(uTxNoise, uv_3).a;
    
    float comp = base+mid;
    comp = comp-detail;
    comp *= ovl;
    comp = clamp(comp, 0.0, 1.0);
    vec3 grad = texture(uTxGrad, vec2(cos(comp-ciElapsedSeconds),comp+sin(ciElapsedSeconds*0.5))).rgb;
    outColor = vec4(grad,1.0);
}