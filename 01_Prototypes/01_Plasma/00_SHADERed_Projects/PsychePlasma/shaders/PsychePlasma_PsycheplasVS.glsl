/*PsychePlasma_PsychplasVS.glsl
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

layout (location = 0) in vec2 pos;

void main() {
   gl_Position = vec4(pos, 0.0f, 1);
}
