#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <RayTriangleIntersection.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>

#include "projectTriangles.h"
#include "readAndWrite.h"
#include "textureMap.h"
#include "draw.h"
#include "raytrace.h"
#include "generativeGeometry.h"
#include "culling.h"
#include "physics.h"
#include "gouraud.h"
#include "phong.h"

using namespace std;
using namespace glm;

#define WIDTH 600
#define HEIGHT 600
#define PI 3.14159265

void update(vec3 cameraTransform, mat3x3 cameraRotationTransform);
void wireframe(DrawingWindow window, vector<ModelTriangle> triangles);
void rasterise(DrawingWindow window, vector<ModelTriangle> triangles);
void handleEvent(SDL_Event event);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
float distanceOfImagePlaneFromCamera = WIDTH/2;
vec3 cameraPosition = vec3(0,3,3);
mat3x3 cameraRotation = mat3(1,0,0,0,1,0,0,0,1);
vec3 lightSource = vec3(0, 4, -1.8);
int mode = 2;
vector<ModelTriangle> initialTriangles;
vector<ModelTriangle> triangles;
vector<Colour> materials;
vector<ModelTriangle> sphere;
vector<ModelTriangle> logo;

int main(int argc, char* argv[])
{
  materials = readMaterials("cornell-box/cornell-box.mtl");
  initialTriangles = readGeometry("cornell-box/cornell-box.obj", materials, 160.0);
  triangles = initialTriangles;
  triangles = liftCubes(triangles);
  initialiseVelocities(materials);
  initialiseCORs(materials);

  sphere = readGeometrySphere("sphere.obj", 0.1);
  logo = readGeometryLogo("logo/logo.obj", 0.01);

  SDL_Event event;
  while(true)
  {
    if(window.pollForInputEvents(&event)) handleEvent(event);
    initialiseDepth();
    initialiseWindow(window);
    triangles = gravity(triangles, velocities,materials);
    vector<ModelTriangle> visibleTriangles = cullTriangles(triangles, cameraPosition);
    // cout << visibleTriangles.size() << endl;
    if(mode == 1){
      wireframe(window, triangles);
      window.renderFrame();
    }else if(mode == 2){
      // rasterise(window, triangles);
      rasterise(window, visibleTriangles);
      window.renderFrame();
    }else if(mode == 3){
      // cullTriangles(triangles, cameraPosition);
      raytrace(window, triangles, cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera, lightSource, visibleTriangles);
      mode = 0;
      window.renderFrame();
    }else if(mode == 4){
      raytraceAntiAlias(window, triangles, cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera, lightSource);
      mode = 0;
      window.renderFrame();
    }else if(mode == 5){
      vector<ModelTriangle> plane = generatePlane(5, 5, -2.5, -5);
      //vector<ModelTriangle> visibleTriangles = cullTriangles(plane, cameraPosition);
      raytrace(window, plane, cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera, lightSource, plane);
      mode = 0;
      window.renderFrame();
    }else if(mode == 6){
      //rasterise(window, sphere);
      raytraceGouraud(window, sphere, cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera, vec3(-2, 4, 3), sphere);
      mode = 0;
      window.renderFrame();
    }else if(mode == 7){
      //rasterise(window, sphere);
      char*** logoTexture = readPPMPayload("logo/texture.ppm");
      raytraceTextures(window, logo, cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera, vec3(0, 4, 3), logo, logoTexture);
      mode = 0;
      window.renderFrame();
    }
  }
}

void wireframe(DrawingWindow window, vector<ModelTriangle> triangles){
  int trianglessize = triangles.size();
  for(int i = 0; i < trianglessize; i++){
    CanvasTriangle temp =  projectTriangleOnImagePlane(triangles[i], cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera);
    drawStrokedTri(window, temp);
  }
}

void rasterise(DrawingWindow window, vector<ModelTriangle> triangles){
  int trianglessize = triangles.size();
  for(int i = 0; i < trianglessize; i++){
  // for(int i = 20; i < 21; i++){
    CanvasTriangle temp =  projectTriangleOnImagePlane(triangles[i], cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera);
    drawFilledTri(window, temp);
    // cout << triangles[i] << endl;
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
  float x = cameraPosition[0];
  float z = cameraPosition[2];
  float oldAngle = atan2(z, x);
  float newAngle = oldAngle + 0.1745;
  float r = sqrt((x*x) + (z*z));
  float newX = cos(newAngle)*r;
  float newZ = sin(newAngle)*r;
  cameraPosition = vec3(newX,cameraPosition[1],newZ);
  lookat();
  // update(vec3(1, 0, 0),  mat3(1,0,0,0,1,0,0,0,1));

  // vec3 vec = vec3(cameraPosition/40, 0, 0);
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
      cameraRotation = mat3(1,0,0,0,1,0,0,0,1);
      triangles = initialTriangles;
    }

    else if(event.key.keysym.sym == SDLK_g){
      triangles = liftCubes(triangles);
      initialiseVelocities(materials);
    }

    else if(event.key.keysym.sym == SDLK_l) lookat();
    else if(event.key.keysym.sym == SDLK_o) orbit();

    else if(event.key.keysym.sym == SDLK_1) mode = 1;
    else if(event.key.keysym.sym == SDLK_2) mode = 2;
    else if(event.key.keysym.sym == SDLK_3) mode = 3;
    else if(event.key.keysym.sym == SDLK_4) mode = 4;
    else if(event.key.keysym.sym == SDLK_5) mode = 5;
    else if(event.key.keysym.sym == SDLK_6) mode = 6;
    else if(event.key.keysym.sym == SDLK_7) mode = 7;

    else if(event.key.keysym.sym == SDLK_p) writePPMFile(window);

  }
  else if(event.type == SDL_MOUSEBUTTONDOWN){
   cout << "MOUSE CLICKED: x:" << event.button.x << " y:" << event.button.y << endl;
   CanvasPoint point = CanvasPoint(event.button.x,event.button.y);
   cout << point << endl;
  }
}
