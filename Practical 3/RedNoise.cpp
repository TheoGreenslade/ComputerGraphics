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

void update();
void handleEvent(SDL_Event event);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
float distanceOfImagePlaneFromCamera = 5;
float depthBuffer[WIDTH][HEIGHT];

int main(int argc, char* argv[])
{
  SDL_Event event;
  while(true)
  {
    if(window.pollForInputEvents(&event)) handleEvent(event);

    update();

    for (int i = 0; i < WIDTH; i++){
      for (int j = 0; j < HEIGHT; j++){
        depthBuffer[i][j] = std::numeric_limits<float>::infinity();
      }
    }

    vector<Colour> materials = readMaterials("cornell-box/cornell-box.mtl");
    vector<ModelTriangle> triangles = readGeometry("cornell-box/cornell-box.obj", materials, 160.0);

    vec3 cameraPosition = vec3(0,2,5);
    mat4x4 cameraTransform = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,20,0);

    int trianglessize = triangles.size();
    for(int i = 0; i < trianglessize; i++){
      CanvasTriangle temp =  projectTriangleOnImagePlane(triangles[i], cameraPosition, cameraTransform, distanceOfImagePlaneFromCamera);
      //drawStrokedTri(temp);
      drawFilledTri(window, temp);
    }

    // Pixel temp1;
    // temp1.x = 50;
    // temp1.y = 200;
    // temp1.zinv = 49;

    // Pixel temp2;
    // temp2.x = 100;
    // temp2.y = 10;
    // temp2.zinv = 23;

    // vector<Pixel> pixels= interpolatePixels(temp1,temp2);
    // for(int i = 0; i < pixels.size(); i++){
    //  cout << pixels[i].x << "," << pixels[i].y << "," << pixels[i].zinv << endl;
    // }

    window.renderFrame();
  }
}

void update()
{
  // Function for performing animation (shifting artifacts or moving the camera)
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
    if(event.key.keysym.sym == SDLK_LEFT) cout << "LEFT" << endl;
    else if(event.key.keysym.sym == SDLK_RIGHT) cout << "RIGHT" << endl;
    else if(event.key.keysym.sym == SDLK_UP) cout << "UP" << endl;
    else if(event.key.keysym.sym == SDLK_DOWN) cout << "DOWN" << endl;
    // else if(event.key.keysym.sym == SDLK_u) randomStrokedTri();
    // else if(event.key.keysym.sym == SDLK_f) randomFilledTri();
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN){
   cout << "MOUSE CLICKED: x:" << event.button.x << " y:" << event.button.y << endl;
   CanvasPoint point = CanvasPoint(event.button.x,event.button.y);
   cout << point << endl;
  }
}
