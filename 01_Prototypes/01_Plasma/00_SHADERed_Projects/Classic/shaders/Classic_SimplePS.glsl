/*Classic_SimplePS.glsl
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

uniform sampler2D txBase;
uniform sampler2D txMid;
uniform sampler2D txCrs;
uniform sampler2D txFin;
uniform sampler2D txGrad;

out vec4 outColor;

void main()
{
    vec2 uv = gl_FragCoord.xy/uResolution;
    
	float baseTx = texture(txBase, vec2(uv.x+cos(uTime*0.1),uv.y-sin(uTime*0.0445))).r;
	float midTx = texture(txMid, vec2(uv.x-sin(uTime*0.2),uv.y+cos(uTime*0.056))).r;
	float crsTx = texture(txCrs, vec2(uv.x-cos(uTime*0.07),uv.y+sin(uTime*0.11))).r;
	float finTx = texture(txFin, vec2(uv.x+sin(uTime*0.033),uv.y-cos(uTime*0.05))).r;
    
    float t1 = clamp((midTx-baseTx),0,1);
    float t2 = clamp((finTx-crsTx),0,1);
    float mask = clamp(t1+t2,0,1);
    
    float muvx = mask;//-cos(gl_FragCoord.y*0.03-uTime*0.024);
  
    float muvy = mask+(sin(gl_FragCoord.x*0.012-cos(uTime*1.8))-cos(gl_FragCoord.y*0.02+sin(uTime*1.2)))*uTime*0.00005;
    outColor = texture(txGrad, vec2(muvx,muvy));
}