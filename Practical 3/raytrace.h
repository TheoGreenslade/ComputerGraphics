#define WIDTH 600
#define HEIGHT 600
#define PI 3.14159265

using namespace std;
using namespace glm;

void raytrace(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera, vec3 lightSource, vector<ModelTriangle> visibleTriangles);
RayTriangleIntersection getClosestIntersection(vec3 cameraPosition, vec3 r, vector<ModelTriangle> triangles);
float proximityLighting(RayTriangleIntersection RayTriangleIntersection, vec3 lightSource);
float angleOfIncidence(RayTriangleIntersection rTI, vec3 lightSource);
bool inHardShaddow(RayTriangleIntersection rTI, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j);
vector<ModelTriangle> removeTriangle(ModelTriangle triangle, vector<ModelTriangle> triangles);
void raytraceAntiAlias(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera, vec3 lightSource);
float SpecularHighlight(RayTriangleIntersection rTI, vec3 lightSource, vec3 r, int shineLevel);

// RAYTRACE FUNCTIONS //////////////////////////////////////////////////////////
void raytrace(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera, vec3 lightSource, vector<ModelTriangle> visibleTriangles){
  float ambientLight = 0.2;
  float specIntensity = 0.2;
  int specPower = 10;
  cout << "Raytracing..." << endl;

  for(int i = 0; i < WIDTH - 1; i++){
    for(int j = 0; j < HEIGHT - 1; j++){
      vec3 temp = vec3(i-(WIDTH/2),(HEIGHT/2)-j, -distanceOfImagePlaneFromCamera);
      vec3 r = normalize(temp * cameraRotation);
      RayTriangleIntersection intersection = getClosestIntersection(cameraPosition,r,visibleTriangles);

      if(intersection.distanceFromCamera != std::numeric_limits<float>::infinity()){
        float b = proximityLighting(intersection, lightSource);
        float a = angleOfIncidence(intersection, lightSource);
        float spec = SpecularHighlight(intersection, lightSource, r, specPower);
        // cout << spec << endl;
        float brightness = std::max((b * a) + (spec * specIntensity), ambientLight);
        if(inHardShaddow(intersection, lightSource, triangles, i, j)){
          brightness = ambientLight;
        }
        Colour colour = intersection.intersectedTriangle.colour;
        int red = colour.red * brightness;
        int green = colour.green * brightness;
        int blue = colour.blue * brightness;
        uint32_t pixel_colour = (255<<24) + (std::min(red, 255)<<16) + (std::min(green, 255)<<8) + std::min(blue, 255);
        window.setPixelColour(i, j, pixel_colour);
      }
    }
  }
  cout << "done." << endl;
}

void raytraceAntiAlias(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera, vec3 lightSource){
  float ambientLight = 0.2;
  float specIntensity = 0.2;
  int specPower = 10;
  cout << "Raytracing..." << endl;

  for(int i = 0; i < WIDTH - 1; i++){
    for(int j = 0; j < HEIGHT - 1; j++){

      vec3 rayColour = vec3(0,0,0);

      for(int x = 0; x < 4; x++){
        for(int y = 0; y < 4; y++){
          float AAshiftTempX = ((x*2)-3);
          float AAshiftX = AAshiftTempX/8;
          float AAshiftTempY = ((y*2)-3);
          float AAshiftY = AAshiftTempY/8;
          float tempX = i-(WIDTH/2) + AAshiftX;
          float tempY = (HEIGHT/2)-j + AAshiftY;

          vec3 temp = vec3(tempX,tempY, -distanceOfImagePlaneFromCamera);
          vec3 r = normalize(temp * cameraRotation);
          RayTriangleIntersection intersection = getClosestIntersection(cameraPosition,r,triangles);

          if(intersection.distanceFromCamera != std::numeric_limits<float>::infinity()){
            float b = proximityLighting(intersection, lightSource);
            float a = angleOfIncidence(intersection, lightSource);
            float spec = SpecularHighlight(intersection, lightSource, r, specPower);
            float brightness = std::max((b * a) + (spec * specIntensity), ambientLight);
            if(inHardShaddow(intersection, lightSource, triangles, i, j)){
              brightness = ambientLight;
            }
            Colour colour = intersection.intersectedTriangle.colour;
            int red = colour.red * brightness;
            int green = colour.green * brightness;
            int blue = colour.blue * brightness;
            vec3 rayColourNew = vec3(std::min(red, 255), std::min(green, 255), std::min(blue, 255));
            rayColour = rayColour + rayColourNew;
          }
        }
      }
      vec3 scale = vec3(16,16,16);
      rayColour = rayColour / scale;
      uint32_t pixel_colour = (255<<24) + (int(rayColour.x)<<16) + (int(rayColour.y)<<8) + int(rayColour.z);
      window.setPixelColour(i, j, pixel_colour);
    }
  }
  cout << "done." << endl;
}

// LIGHTING FUNCTIONS //////////////////////////////////////////////////////////
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

float SpecularHighlight(RayTriangleIntersection rTI, vec3 lightSource, vec3 r, int shineLevel){

  ModelTriangle triangle = rTI.intersectedTriangle;
  vec3 normal = normalize(cross(triangle.vertices[1] - triangle.vertices[0], triangle.vertices[2] - triangle.vertices[0]));
  vec3 vecFromLight = normalize(rTI.intersectionPoint - lightSource);
  vec3 reflectedVec = vecFromLight - (2 * (dot(vecFromLight, normal)) * normal);
  vec3 rInverse = vec3(-r[0], -r[1], -r[2]);
  float specularValue = pow(dot(reflectedVec, rInverse), shineLevel);
  if(specularValue >= 0){
    return specularValue;
  }
  else return 0;
}

bool inHardShaddow(RayTriangleIntersection rTI, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j){
  vec3 point = rTI.intersectionPoint;
  ModelTriangle triangle = rTI.intersectedTriangle;
  triangles = removeTriangle(triangle, triangles);
  vec3 vecToLight = lightSource - point;

  RayTriangleIntersection intersection = getClosestIntersection(point, normalize(vecToLight), triangles);
  float distanceToLight = sqrt(pow(vecToLight[0],2) + pow(vecToLight[1],2) + pow(vecToLight[2],2));

  if(intersection.distanceFromCamera == std::numeric_limits<float>::infinity()) return false;
  else if(distanceToLight > abs(intersection.distanceFromCamera)) return true;
  else return false;
}

// HELPER FUNCTIONS ////////////////////////////////////////////////////////////
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

vector<ModelTriangle> removeTriangle(ModelTriangle triangle, vector<ModelTriangle> triangles){
  bool remove = false;
  int index = 100000;
  for(int i = 0; i < (int) triangles.size(); i++){
    if(triangles[i].vertices[0] == triangle.vertices[0] && triangles[i].vertices[1] == triangle.vertices[1] && triangles[i].vertices[2] == triangle.vertices[2]){
      remove = true;
      index = i;
    }
  }
  if(remove) triangles.erase(triangles.begin()+index);
  return triangles;
}
