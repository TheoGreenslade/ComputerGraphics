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
#include <BoundingBox.h>
#include <PPMImage.h>

#include "projectTriangles.h"
#include "readAndWrite.h"
#include "textureMap.h"
#include "draw.h"
#include "raytrace.h"
#include "generativeGeometry.h"
#include "culling.h"
#include "physics.h"
#include "clipping.h"
#include "initialise.h"

// ffmpeg -framerate 5 -i %03d.ppm -c:v mpeg4 -q 5 out.mp4

using namespace std;
using namespace glm;

#define WIDTH 600
#define HEIGHT 600
#define PI 3.14159265

void update(vec3 cameraTransform, mat3x3 cameraRotationTransform);
void wireframe(DrawingWindow window, vector<ModelTriangle> triangles);
void rasterise(DrawingWindow window, vector<ModelTriangle> triangles);
void handleEvent(SDL_Event event);
void lookat();
void cameraShot(int t, vec3 startTransform, vec3 endTransform);
void handleEvent(SDL_Event event);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
float distanceOfImagePlaneFromCamera = WIDTH/2;
vec3 cameraPosition = vec3(-10,-10,30);
mat3x3 cameraRotation = mat3(1,0,0,0,1,0,0,0,1);
vec3 startingPosition;
mat3x3 startingRotation;
vec3 lightSource = vec3(0, 0, 0);
int mode = 1;
int t = 0;
vector<ModelTriangle> initialTriangles;
vector<ModelTriangle> triangles;
vector<Colour> materials;
vector<ModelTriangle> sphere;
vector<ModelTriangle> logo;

int main(int argc, char* argv[])
{
  materials = readMaterials("cornell-box/cornell-box.mtl");
  initialTriangles = readGeometry("cornell-box/cornell-box.obj", materials, 2);
  initialTriangles = initialiseMirrors(initialTriangles);
  initialTriangles = initialiseGlass (initialTriangles);
  //initialTriangles = initialiseGravity(initialTriangles);
  triangles = initialTriangles;
  triangles = liftCubes(triangles);
  initialiseVelocities(materials);
  initialiseCORs(materials);

  sphere = readGeometrySphere("sphere.obj", 0.05);
  vector<vector<ModelTriangle>> planetsVector = initialisePlanets(sphere);
  vector<ModelTriangle> planets = updatePlanets(planetsVector);
  
  logo = readGeometryLogo("logo/logo.obj", 0.005);
  logo = initialiseScaleAndPosition(logo, 1, vec3(0.5,1.5,7));
  logo = initialiseGlass(logo);

  vector<PPMImage> textures;
  textures = readPlanetTextures();

  vector<ModelTriangle> stars = initialiseStars();
  
  lookat();
  
  SDL_Event event;
  while(true)
  {
    if(window.pollForInputEvents(&event)) handleEvent(event);
    initialiseDepth();
    initialiseWindow(window);
    vector<ModelTriangle> visiblePlanetTriangles = cullTriangles(planets, cameraPosition);
    visiblePlanetTriangles = clipTriangles(visiblePlanetTriangles,cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera);
    //visiblePlanetTriangles.insert(visiblePlanetTriangles.end(), stars.begin(), stars.end());
    visiblePlanetTriangles.insert(visiblePlanetTriangles.end(), logo.begin(), logo.end());

    if(mode == 1){
      t=0;
      startingPosition = cameraPosition;
      startingRotation = cameraRotation;
      wireframe(window, visiblePlanetTriangles);
      window.renderFrame();
    }else if(mode == 2){
      if(t < 240) cameraShot(t, startingPosition, vec3(5,6,6));
      else mode = 0;
      planetsVector = updatePlanetPositions(planetsVector);
      planets = updatePlanets(planetsVector);
      wireframe(window, visiblePlanetTriangles);
      window.renderFrame();
      //writePPMFile(window, t);
      t++;
    }else if(mode == 3){
      rasterise(window, planets);
      window.renderFrame();
    }else if(mode == 4){
      raytraceAntiAlias(window, planets, cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera, lightSource, visiblePlanetTriangles, textures);
      mode = 0;
      window.renderFrame();
    }else if(mode == 5){
      vector<ModelTriangle> plane = generatePlane(5, 5, -2, -5);
      raytrace(window, plane, cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera, lightSource, plane, textures);
      mode = 0;
      window.renderFrame();
    }else if(mode == 6){
      raytrace(window, planets, cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera, lightSource, visiblePlanetTriangles, textures);
      mode = 0;
      window.renderFrame();
    }else if(mode == 7){
      vector<PPMImage> logoTextures;
      PPMImage logoTexture = readPPMImage("logo/texture.ppm");
      logoTextures.push_back(logoTexture);
      raytrace(window, logo, cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera, lightSource, logo, logoTextures);
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
    CanvasTriangle temp =  projectTriangleOnImagePlane(triangles[i], cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera);
    drawFilledTri(window, temp);
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
}

void cameraShot(int t, vec3 startTransform, vec3 endTransform){
  vec3 tDiff   = startTransform - endTransform;
  vec3 tStep   = tDiff * vec3(0.004166, 0.004166, 0.004166);
  lookat();
  update(-tStep, mat3(1,0,0,0,1,0,0,0,1));
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
    else if(event.key.keysym.sym == SDLK_8) mode = 8;

    else if(event.key.keysym.sym == SDLK_p) writePPMFile(window);

  }
  else if(event.type == SDL_MOUSEBUTTONDOWN){
   cout << "MOUSE CLICKED: x:" << event.button.x << " y:" << event.button.y << endl;
   CanvasPoint point = CanvasPoint(event.button.x,event.button.y);
   cout << point << endl;
  }
}
