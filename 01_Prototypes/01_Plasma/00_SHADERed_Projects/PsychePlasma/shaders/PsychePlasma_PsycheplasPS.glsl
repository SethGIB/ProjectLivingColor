/*PsychePlasma_PsychplasPS.glsl
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
uniform sampler2D uTxMid;
uniform sampler2D uTxDetail;
uniform sampler2D uTxOvl;
uniform sampler2D uTxGradient;

out vec4 outColor;

void main()
{
    vec2 uv = gl_FragCoord.xy/uResolution;
    
    vec2 uvbase = vec2(uv.x+uTime*0.1, uv.y);
    float base = texture(uTxBase, uvbase).r;
    
    vec2 uvmid = vec2(uv.x-uTime*0.0667, uv.y);
    float mid = texture(uTxMid, uvmid).g;
    
    vec2 uvdet = vec2(sin(uv.x-uTime*0.01), uv.y+uTime*0.1);
    float detail = texture(uTxDetail, uvdet).b;
    
    vec2 uvovl = vec2(uv.x+cos(uTime*0.1), sin(uv.y-uTime*0.04));
    float ovl = texture(uTxOvl, uvovl).b;
    
    float comp = base+mid;
    comp = comp-detail;
    comp *= ovl;
    comp = clamp(comp, 0.0, 1.0);
    vec3 grad = texture(uTxGradient, vec2(cos(comp-uTime),comp+sin(uTime*0.5))).rgb;
    outColor = vec4(grad,1.0);
}