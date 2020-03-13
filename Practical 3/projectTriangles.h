#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

CanvasPoint projectPointOnImagePlane(vec3 point, vec3 cameraPosition, mat3x3 cameraRotation, int distanceOfImagePlaneFromCamera);
vec3 rotatePoint(mat3x3 cameraRotation, vec3 p);

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
  vec3 p = vec3(bigX, bigY, bigZ);
  vec3 newP = rotatePoint(cameraRotation, p);
  float proportion = smallZ/newP.z;
  float littleX = newP.x * proportion;
  float littleY = newP.y * proportion;

  CanvasPoint result = CanvasPoint(-littleX + WIDTH/2,littleY + HEIGHT/2, double(newP.z));
  return result;
}

vec3 rotatePoint(mat3x3 cameraRotation, vec3 p){
  return cameraRotation * p;
}
