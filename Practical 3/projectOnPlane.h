#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 240

void draw();
void update();
void handleEvent(SDL_Event event);
void drawline(CanvasPoint start, CanvasPoint end, Colour colour);
void drawStrokedTri(CanvasTriangle triangle);
void randomStrokedTri();
void drawFilledTri(CanvasTriangle triangle);
void drawFilledTriFlat(CanvasTriangle triangle, int tri);
void randomFilledTri();
void displayPPMImage(string filename);
void drawTextureTriangle(CanvasTriangle triangle, string filename);
void drawTextureTriangleFlatBottom(CanvasTriangle triangle, string filename);
char*** readPPMImagePayload(ifstream ifs,int width,int height);
char ***malloc3dArray(int dim1, int dim2, int dim3);
float* interpolate(float start, float end, int steps);
void drawTextureTriangleFlatTop(CanvasTriangle triangle, string filename);
vector<Colour> readMaterials(string filename);
vector<ModelTriangle> readGeometry(string filename, vector<Colour> materials, float scalingFactor);
CanvasTriangle projectTriangleOnImagePlane(ModelTriangle modelTriangle, vec3 cameraPosition, mat4x4 cameraTransform);
CanvasPoint projectPointOnImagePlane(vec3 point, vec3 cameraPosition, mat4x4 cameraTransform);


// Function Declarations
////////////////////////////////////////////////////////////////////////////////

CanvasTriangle projectTriangleOnImagePlane(ModelTriangle modelTriangle, vec3 cameraPosition, mat4x4 cameraTransform){
  CanvasPoint x = projectPointOnImagePlane(modelTriangle.vertices[0], cameraPosition, cameraTransform);
  CanvasPoint y = projectPointOnImagePlane(modelTriangle.vertices[1], cameraPosition, cameraTransform);
  CanvasPoint z = projectPointOnImagePlane(modelTriangle.vertices[2], cameraPosition, cameraTransform);

  CanvasTriangle result = CanvasTriangle(x,y,z,modelTriangle.colour);
  return result;
}

CanvasPoint projectPointOnImagePlane(vec3 point, vec3 cameraPosition, mat4x4 cameraTransform){
  // vec3 imagePlane = cameraPosition - vec3(0,0,5);
  float bigZ = point.z - cameraPosition.z;
  float smallZ = distanceOfImagePlaneFromCamera;
  float bigX = point.x - cameraPosition.x;
  float bigY = point.y - cameraPosition.y;
  float proportion = smallZ/bigZ;
  float littleX = bigX * proportion * 40.0;
  float littleY = bigY * proportion * 40.0;
  CanvasPoint result = CanvasPoint(-littleX + WIDTH/2,littleY + HEIGHT/2, double(bigZ));
  return result;
}
