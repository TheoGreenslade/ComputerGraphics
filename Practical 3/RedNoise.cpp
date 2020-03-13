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
#define PI 3.14159265

void update(vec3 cameraTransform, mat3x3 cameraRotationTransform);
void handleEvent(SDL_Event event);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
float distanceOfImagePlaneFromCamera = WIDTH/2;
vec3 cameraPosition = vec3(0,3,3);
mat3x3 cameraRotation = mat3(1,0,0,0,1,0,0,0,1);

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

void update(vec3 cameraTransform, mat3x3 cameraRotationTransform)
{
    cameraPosition = cameraPosition + (cameraTransform * cameraRotation);
    cameraRotation = cameraRotation * cameraRotationTransform;
    cout << cameraPosition.x << ", " << cameraPosition.y << ", " << cameraPosition.z << ", " << endl;
}

void lookat() {
   vec3 forward = normalize(cameraPosition);
   vec3 vertical = vec3(0,1,0);
   vec3 right = normalize(cross(vertical, cameraPosition));
   vec3 up = normalize(cross(forward, right));
   cameraRotation = mat3(right[0], up[0], forward[0],
                         right[1], up[1], forward[1],
                         right[2], up[2], forward[2]);
}

void orbit() {
  // vec3 vec = vec3(cameraPosition)/40, 0, 0);
  // vec3 vec2 = cameraRotation*vec;
  // update(vec3(length(vec2[0], vec2.y, vec2.z), mat3(1,0,0,0,1,0,0,0,1));
}

void handleEvent(SDL_Event event)
{
  int theta = 5;
  if(event.type == SDL_KEYDOWN) {
         if(event.key.keysym.sym == SDLK_RIGHT) update(vec3(1, 0, 0),  mat3(1,0,0,0,1,0,0,0,1));
    else if(event.key.keysym.sym == SDLK_LEFT)  update(vec3(-1, 0, 0), mat3(1,0,0,0,1,0,0,0,1));
    else if(event.key.keysym.sym == SDLK_DOWN)  update(vec3(0, -1, 0), mat3(1,0,0,0,1,0,0,0,1));
    else if(event.key.keysym.sym == SDLK_UP)    update(vec3(0, 1, 0),  mat3(1,0,0,0,1,0,0,0,1));
    else if(event.key.keysym.sym == SDLK_f)     update(vec3(0, 0, -1), mat3(1,0,0,0,1,0,0,0,1));
    else if(event.key.keysym.sym == SDLK_b)     update(vec3(0, 0, 1),  mat3(1,0,0,0,1,0,0,0,1));

    else if(event.key.keysym.sym == SDLK_w)     update(vec3(0, 0, 0),  mat3(1,0,0,0, cos ( theta * PI / 180.0 ), -(sin ( theta * PI / 180.0 )),0, sin ( theta * PI / 180.0 ), cos ( theta * PI / 180.0 )));
    else if(event.key.keysym.sym == SDLK_s)     update(vec3(0, 0, 0),  mat3(1,0,0,0, cos ( (-theta) * PI / 180.0 ), -(sin ( (-theta) * PI / 180.0 )),0, sin ( (-theta) * PI / 180.0 ), cos ( (-theta) * PI / 180.0 )));

    else if(event.key.keysym.sym == SDLK_d)     update(vec3(0, 0, 0),  mat3(cos ( theta * PI / 180.0 ),0, sin ( theta * PI / 180.0 ),0,1,0, -(sin ( theta * PI / 180.0 )),0, cos ( theta * PI / 180.0 )));
    else if(event.key.keysym.sym == SDLK_a)     update(vec3(0, 0, 0),  mat3(cos ( (-theta) * PI / 180.0 ),0, sin ( (-theta) * PI / 180.0 ),0,1,0, -(sin ( (-theta) * PI / 180.0 )),0, cos ( (-theta) * PI / 180.0 )));

    else if(event.key.keysym.sym == SDLK_r){
      cameraPosition = vec3(0,3,3);
      cameraRotation = mat3(1,0,0,0,1,0,0,0,1);}

    else if(event.key.keysym.sym == SDLK_l) lookat();
    else if(event.key.keysym.sym == SDLK_o) orbit();
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN){
   cout << "MOUSE CLICKED: x:" << event.button.x << " y:" << event.button.y << endl;
   CanvasPoint point = CanvasPoint(event.button.x,event.button.y);
   cout << point << endl;
  }
}
