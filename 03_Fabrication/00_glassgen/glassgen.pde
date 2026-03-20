/*glassgen.pde
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

import toxi.geom.*;
import toxi.geom.mesh2d.*;
import toxi.processing.*;

VoronoiMgr vor = new VoronoiMgr();
ToxiclibsSupport gfx;

void setup()
{
  size(500,800);
  gfx = new ToxiclibsSupport(this);
  vor.setupGrid(8,15,500,800, true);
  //vor.setupRandom(20,50,450,80,720, false);
  //vor.setupRadials(8, new PVector(50,80), new PVector(8,16), new PVector(100,500), false);
  vor.setupRadials(12, new PVector(width-200,height-400), new PVector(8,16), new PVector(80,500), false);
  print(vor.regions().size());
}

boolean dShowPoints = true;
boolean dShowVoronoi = true;
boolean dShowTris = true;

void draw()
{
  background(0);
  noFill();
  if(dShowPoints)
  {
    stroke(255,192,0);
    vor.showPoints();
  }
  
  if(dShowVoronoi)
  {
    stroke(0,255,128);
    for( var p : vor.regions())
    {
      gfx.polygon2D(p);
    }
  }
  
  if(dShowTris)
  {
    stroke(0, 128, 255);
    beginShape(TRIANGLES);
    for (var t : vor.delaunay()) {
      gfx.triangle(t, false);
    }
    endShape();
  }
}

void mousePressed(MouseEvent e)
{
  vor.onMousePressed(e);
}

void mouseDragged(MouseEvent e)
{
  vor.onMouseDragged(e);
}

void mouseReleased()
{
  vor.onMouseReleased();
}

void keyPressed()
{
  if(key=='p')
  {
    dShowPoints = !dShowPoints;
  }
  if(key=='d')
  {
    dShowVoronoi = !dShowVoronoi;
  }
  if(key=='t')
  {
    dShowTris = !dShowTris;
  }
}
