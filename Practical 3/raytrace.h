#define WIDTH 600
#define HEIGHT 600
#define PI 3.14159265

using namespace std;
using namespace glm;

// void raytrace(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera);
RayTriangleIntersection getClosestIntersection(vec3 cameraPosition, vec3 r, vector<ModelTriangle> triangles);
float proximityLighting(RayTriangleIntersection RayTriangleIntersection, vec3 lightSource);
float angleOfIncidence(RayTriangleIntersection rTI, vec3 lightSource);
bool inHardShaddow(RayTriangleIntersection rTI, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j);
vector<ModelTriangle> removeTriangle(ModelTriangle triangle, vector<ModelTriangle> triangles);

void raytrace(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera, vec3 lightSource){
  float ambientLight = 0.2;
  cout << "Raytracing..." << endl;

  for(int i = 0; i < WIDTH - 1; i++){
    for(int j = 0; j < HEIGHT - 1; j++){
      vec3 temp = vec3(i-(WIDTH/2),(HEIGHT/2)-j, -distanceOfImagePlaneFromCamera);
      vec3 r = normalize(temp * cameraRotation);
      RayTriangleIntersection intersection = getClosestIntersection(cameraPosition,r,triangles);

      // if ( (i==1) && (j==1) ){
      //   cout << "camera position: " << cameraPosition.x << "," << cameraPosition.y << "," << cameraPosition.z << endl << endl;
      //   cout << "temp: " << temp.x << "," << temp.y << "," << temp.z << endl << endl;
      //   cout << "r: " << r.x << "," << r.y << "," << r.z << endl << endl;
      //   cout << "intersected triangle" << intersection.intersectedTriangle << endl;
      // }

      if(intersection.distanceFromCamera != std::numeric_limits<float>::infinity()){
        float b = proximityLighting(intersection, lightSource);
        float a = angleOfIncidence(intersection, lightSource);
        float brightness = std::max(b * a, ambientLight);
        if(inHardShaddow(intersection, lightSource, triangles, i, j)){
          // if( i == 387 && (HEIGHT - 1 - j) == 395){
          //   cout << i << ", " << HEIGHT - 1 - j << ", " << intersection.distanceFromCamera << endl;
          // }
          brightness = ambientLight;
        }
        Colour colour = intersection.intersectedTriangle.colour;
        uint32_t pixel_colour = (255<<24) + (int(colour.red * brightness)<<16) + (int(colour.green * brightness)<<8) + int(colour.blue * brightness);
        window.setPixelColour(i, j, pixel_colour);
      }
    }
  }
  cout << "done." << endl;
}

RayTriangleIntersection getClosestIntersection(vec3 cameraPosition, vec3 r, vector<ModelTriangle> triangles){
  vec3 bestSolution = vec3(std::numeric_limits<float>::infinity(),0,0);
  ModelTriangle bestTri;
  int trianglessize = triangles.size();

  for(int i = 0; i < trianglessize; i++){
    vec3 e0 = triangles[i].vertices[1] - triangles[i].vertices[0];
    vec3 e1 = triangles[i].vertices[2] - triangles[i].vertices[0];
    vec3 SPVector = cameraPosition - triangles[i].vertices[0];
    mat3 DEMatrix(-r, e0, e1);
    vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;
    //check if u and v are between 0 and 1
    if((possibleSolution[1] >= 0) && (possibleSolution[1] <= 1) && (possibleSolution[2] >= 0) && (possibleSolution[2] <= 1) && ((possibleSolution[1] + possibleSolution[2]) <= 1)){
      if((bestSolution[0] == std::numeric_limits<float>::infinity()) && (possibleSolution[0] > 0) ){
        bestSolution = possibleSolution;
        bestTri = triangles[i];
      }
      else if((possibleSolution[0] < bestSolution[0]) && (possibleSolution[0] > 0) ){
        bestSolution = possibleSolution;
        bestTri = triangles[i];
      }
    }
  }

  vec3 intersectionPoint = bestTri.vertices[0] + bestSolution[1]*(bestTri.vertices[1]-bestTri.vertices[0]) + bestSolution[2]*(bestTri.vertices[2]-bestTri.vertices[0]);
  RayTriangleIntersection result = RayTriangleIntersection(intersectionPoint, abs(bestSolution[0]), bestTri);
  return result;
}

float proximityLighting(RayTriangleIntersection rTI, vec3 lightSource){
  vec3 point = rTI.intersectionPoint;
  float distance = sqrt(pow((point.x - lightSource[0]),2) + pow((point.y - lightSource[1]),2) + pow((point.z - lightSource[2]),2));
  float inverseSquare = 50 / ( PI * pow(distance, 2));
  if(inverseSquare > 1){
    return 1;
  }
  else{
  return inverseSquare;
  }
}

float angleOfIncidence(RayTriangleIntersection rTI, vec3 lightSource){
  ModelTriangle triangle = rTI.intersectedTriangle;
  vec3 normal = normalize(cross(triangle.vertices[1] - triangle.vertices[0], triangle.vertices[2] - triangle.vertices[0]));
  vec3 vecToLight = normalize(lightSource - rTI.intersectionPoint);
  float angleOfIncidence = dot(normal, vecToLight);
  if(angleOfIncidence > 0){
    return angleOfIncidence;
  }else{
    return 0;
  }
}

// Using getClosestIntersection in relation to the point, not the camera
bool inHardShaddow(RayTriangleIntersection rTI, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j){
  vec3 point = rTI.intersectionPoint;
  ModelTriangle triangle = rTI.intersectedTriangle;
  triangles = removeTriangle(triangle, triangles);
  vec3 vecToLight = lightSource - point;

  RayTriangleIntersection intersection = getClosestIntersection(point, normalize(vecToLight), triangles);
  float distanceToLight = sqrt(pow(vecToLight[0],2) + pow(vecToLight[1],2) + pow(vecToLight[2],2));
  //cout << "dist to light - " << distanceToLight << ", "  << "dist to camera - " << intersection.distanceFromCamera<< endl;

  if ( (i==391) && (j==388) ){
    cout << endl << "light source: " << lightSource.x << "," << lightSource.y << "," << lightSource.z << endl << endl;
    cout << "point: " << point.x << "," << point.y << "," << point.z << endl << endl;
    cout << "vecToLight: "<< vecToLight.x << "," << vecToLight.y << "," << vecToLight.z << endl << endl;
    cout << "triangle projecting from:" << endl << rTI.intersectedTriangle << endl;
    cout << "intersected triangle:" << endl << intersection.intersectedTriangle << endl;
  }
  if(intersection.distanceFromCamera == std::numeric_limits<float>::infinity()) return false;
  else if(distanceToLight > abs(intersection.distanceFromCamera)) return true;
  else return false;
}

 vector<ModelTriangle> removeTriangle(ModelTriangle triangle, vector<ModelTriangle> triangles){
  bool remove = false;
  int index = 100000;
  for(int i = 0; i < triangles.size(); i++){
    if(triangles[i].vertices[0] == triangle.vertices[0] && triangles[i].vertices[1] == triangle.vertices[1] && triangles[i].vertices[2] == triangle.vertices[2]){
      remove = true;
      index = i;
    }
  }
  if(remove) triangles.erase(triangles.begin()+index);
  return triangles;
}
