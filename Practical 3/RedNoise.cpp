#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>

#include "projectTriangles.h"
#include "read.h"
#include "textureMap.h"
#include "draw.h"

using namespace std;
using namespace glm;

#define WIDTH 600
#define HEIGHT 600

void update(vec3 cameraTransform);
void handleEvent(SDL_Event event);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
float distanceOfImagePlaneFromCamera = 250;
vec3 cameraPosition = vec3(0,6,3);
mat3x3 cameraRotation = mat3(1,0,0,0,0,1,0,0,0);

int main(int argc, char* argv[])
{
  vector<Colour> materials = readMaterials("cornell-box/cornell-box.mtl");
  vector<ModelTriangle> triangles = readGeometry("cornell-box/cornell-box.obj", materials, 160.0);

  SDL_Event event;
  while(true)
  {
    if(window.pollForInputEvents(&event)) handleEvent(event);
    initialiseDepth();
    initialiseWindow(window);

    int trianglessize = triangles.size();
    for(int i = 0; i < trianglessize; i++){
      CanvasTriangle temp =  projectTriangleOnImagePlane(triangles[i], cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera);
      //drawStrokedTri(temp);
      drawFilledTri(window, temp);
    }
    window.renderFrame();
  }
}

void update(vec3 cameraTransform)
{
  cameraPosition = cameraPosition + cameraTransform;
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
         if(event.key.keysym.sym == SDLK_LEFT)  update(vec3(1, 0, 0));
    else if(event.key.keysym.sym == SDLK_RIGHT) update(vec3(-1, 0, 0));
    else if(event.key.keysym.sym == SDLK_UP)    update(vec3(0, -1, 0));
    else if(event.key.keysym.sym == SDLK_DOWN)  update(vec3(0, 1, 0));
    else if(event.key.keysym.sym == SDLK_f)     update(vec3(0, 0, -1));
    else if(event.key.keysym.sym == SDLK_b)     update(vec3(0, 0, 1));
    // else if(event.key.keysym.sym == SDLK_u) randomStrokedTri();
    // else if(event.key.keysym.sym == SDLK_f) randomFilledTri();
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN){
   cout << "MOUSE CLICKED: x:" << event.button.x << " y:" << event.button.y << endl;
   CanvasPoint point = CanvasPoint(event.button.x,event.button.y);
   cout << point << endl;
  }
}
