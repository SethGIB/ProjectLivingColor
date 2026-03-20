/*plasma_classic.frag
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
    //vec2 uv = gl_FragCoord.xy/uResolution;
    vec2 uv = vTexCoord0 * vec2(0.5625,1.0);

    vec2 uv_0 = vec2(uv.x+cos(ciElapsedSeconds*0.1),uv.y-sin(ciElapsedSeconds*0.0445));
	float baseTx = texture(uTxNoise, uv_0).r;
	
    vec2 uv_1 = vec2(uv.x-sin(ciElapsedSeconds*0.2),uv.y+cos(ciElapsedSeconds*0.056));
    float midTx = texture(uTxNoise, uv_1).g;
	
    vec2 uv_2 = vec2(uv.x-cos(ciElapsedSeconds*0.07),uv.y+sin(ciElapsedSeconds*0.11));
    float crsTx = texture(uTxNoise, uv_2).b;
	
    vec2 uv_3 = vec2(uv.x+sin(ciElapsedSeconds*0.033),uv.y-cos(ciElapsedSeconds*0.05));
    float finTx = texture(uTxNoise, uv_3).a;
    
    float t1 = clamp((midTx-baseTx),0,1);
    float t2 = clamp((finTx-crsTx),0,1);
    float mask = clamp(t1+t2,0,1);
    
    float muvx = mask;//-cos(gl_FragCoord.y*0.03-uTime*0.024);
  
    float muvy = mask+(sin(gl_FragCoord.x*0.012-cos(ciElapsedSeconds*1.8))-cos(gl_FragCoord.y*0.02+sin(ciElapsedSeconds*1.2)))*ciElapsedSeconds*0.00005;
    outColor = texture(uTxGrad, vec2(muvx,muvy));
}