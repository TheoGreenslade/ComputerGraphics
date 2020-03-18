#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

// void raytrace(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera);
RayTriangleIntersection getClosestIntersection(vec3 cameraPosition, vec3 r, vector<ModelTriangle> triangles);

void raytrace(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera){
  for(int i = 0; i < WIDTH; i++){
    for(int j = 0; j < HEIGHT; j++){
      vec3 temp = vec3((WIDTH/2)-i, (HEIGHT/2)-j, distanceOfImagePlaneFromCamera);
      vec3 r = temp * cameraRotation;
      RayTriangleIntersection intersection = getClosestIntersection(cameraPosition,r,triangles);
      if(intersection.distanceFromCamera != std::numeric_limits<float>::infinity()){
        Colour colour = intersection.intersectedTriangle.colour;
        uint32_t pixel_colour = (255<<24) + (int(colour.red)<<16) + (int(colour.green)<<8) + int(colour.blue);
        window.setPixelColour(i, HEIGHT - j, pixel_colour);
      }
    }
  }
  cout << "done" << endl;
}

RayTriangleIntersection getClosestIntersection(vec3 cameraPosition, vec3 r, vector<ModelTriangle> triangles){
  vec3 bestSolution = vec3(std::numeric_limits<float>::infinity(),0,0);
  ModelTriangle bestTri;
  int trianglessize = triangles.size();
  for(int i = 0; i < trianglessize; i++){
    vec3 e0 = triangles[i].vertices[1] - triangles[i].vertices[0];
    vec3 e1 = triangles[i].vertices[2] - triangles[i].vertices[0];
    vec3 SPVector = cameraPosition-triangles[i].vertices[0];
    mat3 DEMatrix(-r, e0, e1);
    vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;
    //check if u and v are between 0 and 1
    if((possibleSolution[1] >= 0) && (possibleSolution[1] <= 1) && (possibleSolution[2] >= 0) && (possibleSolution[2] <= 1) && ((possibleSolution[1] + possibleSolution[2]) <= 1)){
      if(bestSolution[0] == std::numeric_limits<float>::infinity()){
        bestSolution = possibleSolution;
        bestTri = triangles[i];
      }
      else if(possibleSolution[0] > bestSolution[0]){
        bestSolution = possibleSolution;
        bestTri = triangles[i];
      }
    }
  }
  vec3 intersectionPoint = bestTri.vertices[0] + bestSolution[1]*(bestTri.vertices[1]-bestTri.vertices[0]) + bestSolution[2]*(bestTri.vertices[2]-bestTri.vertices[0]);
  RayTriangleIntersection result = RayTriangleIntersection(intersectionPoint, bestSolution[0], bestTri);
  //cout << intersectionPoint[0] << ", " << intersectionPoint[1] << ", " << intersectionPoint[2] << endl;
  return result;
}
