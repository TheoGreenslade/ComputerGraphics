#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

CanvasPoint projectPointOnImagePlane(vec3 point, vec3 cameraPosition, mat4x4 cameraTransform, int distanceOfImagePlaneFromCamera);

CanvasTriangle projectTriangleOnImagePlane(ModelTriangle modelTriangle, vec3 cameraPosition, mat4x4 cameraTransform, int distanceOfImagePlaneFromCamera){
  CanvasPoint x = projectPointOnImagePlane(modelTriangle.vertices[0], cameraPosition, cameraTransform, distanceOfImagePlaneFromCamera);
  CanvasPoint y = projectPointOnImagePlane(modelTriangle.vertices[1], cameraPosition, cameraTransform, distanceOfImagePlaneFromCamera);
  CanvasPoint z = projectPointOnImagePlane(modelTriangle.vertices[2], cameraPosition, cameraTransform, distanceOfImagePlaneFromCamera);

  CanvasTriangle result = CanvasTriangle(x,y,z,modelTriangle.colour);
  return result;
}

CanvasPoint projectPointOnImagePlane(vec3 point, vec3 cameraPosition, mat4x4 cameraTransform, int distanceOfImagePlaneFromCamera){
  float bigZ = point.z - cameraPosition.z;
  float smallZ = distanceOfImagePlaneFromCamera;
  float bigX = point.x - cameraPosition.x;
  float bigY = point.y - cameraPosition.y;
  float proportion = smallZ/bigZ;
  float littleX = bigX * proportion * 40.0;
  float littleY = bigY * proportion * 40.0;
  CanvasPoint result = CanvasPoint(-littleX + WIDTH/2,littleY + HEIGHT/2);
  return result;
}
