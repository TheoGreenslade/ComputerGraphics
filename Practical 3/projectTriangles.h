#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

CanvasPoint projectPointOnImagePlane(vec3 point, vec3 cameraPosition, mat3x3 cameraRotation, int distanceOfImagePlaneFromCamera);

CanvasTriangle projectTriangleOnImagePlane(ModelTriangle modelTriangle, vec3 cameraPosition, mat3x3 cameraRotation, int distanceOfImagePlaneFromCamera){
  CanvasPoint x = projectPointOnImagePlane(modelTriangle.vertices[0], cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera);
  CanvasPoint y = projectPointOnImagePlane(modelTriangle.vertices[1], cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera);
  CanvasPoint z = projectPointOnImagePlane(modelTriangle.vertices[2], cameraPosition, cameraRotation, distanceOfImagePlaneFromCamera);

  CanvasTriangle result = CanvasTriangle(x,y,z,modelTriangle.colour);
  return result;
}

CanvasPoint projectPointOnImagePlane(vec3 point, vec3 cameraPosition, mat3x3 cameraRotation, int distanceOfImagePlaneFromCamera){
  float bigZ = point.z - cameraPosition.z;
  float smallZ = distanceOfImagePlaneFromCamera;
  float bigX = point.x - cameraPosition.x;
  float bigY = point.y - cameraPosition.y;
  float proportion = smallZ/bigZ;
  float littleX = bigX * proportion;
  float littleY = bigY * proportion;
  CanvasPoint result = CanvasPoint(-littleX + WIDTH/2,littleY + HEIGHT/2, double(bigZ));
  return result;
}
