#define WIDTH 640
#define HEIGHT 480
#define PI 3.14159265

using namespace std;
using namespace glm;

void raytrace(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera, vec3 lightSource, vector<ModelTriangle> visibleTriangles, vector<PPMImage> textures);
RayTriangleIntersection getClosestIntersection(vec3 cameraPosition, vec3 r, vector<ModelTriangle> triangles);
float proximityLighting(RayTriangleIntersection RayTriangleIntersection, vec3 lightSource);
float angleOfIncidence(RayTriangleIntersection rTI, vec3 lightSource);
bool inHardShaddow(RayTriangleIntersection rTI, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j);
vector<ModelTriangle> removeTriangle(ModelTriangle triangle, vector<ModelTriangle> triangles);
void raytraceAntiAlias(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera, vec3 lightSource,vector<ModelTriangle> visibleTriangles, vector<PPMImage> textures);
float SpecularHighlight(RayTriangleIntersection rTI, vec3 lightSource, vec3 r, int shineLevel);
void raytraceTextures(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera, vec3 lightSource, vector<ModelTriangle> visibleTriangles, char*** texture);
float calcualteBrightness(RayTriangleIntersection intersection, vec3 lightSource, vec3 r, int i, int j,vector<ModelTriangle> triangles);
float softShadow(float brightness, RayTriangleIntersection intersection, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j);
vec3 scaleVector(vec3 normal, float scaleFactor);
bool inShaddow(RayTriangleIntersection rTI, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j);
bool inGlassShaddow(RayTriangleIntersection rTI, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j);
float calculateBrightnessScaler(RayTriangleIntersection intersection, vec3 shiftVector, vec3 step, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j, float stepScale, float maxShift);
vec3 findNormal(RayTriangleIntersection rTI);
vec3 reflectionColour(RayTriangleIntersection intersectionOnMirror, vec3 Ri, vector<ModelTriangle> triangles, vec3 lightSource, int i, int j, vector<PPMImage> textures);
vec3 calculateVectorOfReflection(RayTriangleIntersection intersection, vec3 Ri);
vec3 refractionColour(RayTriangleIntersection intersectionOnSurface, vec3 Ri, vector<ModelTriangle> triangles, vec3 lightSource, int i, int j, vector<PPMImage> textures);
vec3 calculateVectorOfRefraction(RayTriangleIntersection intersection, vec3 Ri);
vec3 getNormalFromBumpMap(RayTriangleIntersection intersection, vector<PPMImage> bumpMaps);

float ambientLight = 0.15;
float specIntensity = 0.2;
int specPower = 10;

// RAYTRACE FUNCTIONS //////////////////////////////////////////////////////////
void raytrace(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera, vec3 lightSource, vector<ModelTriangle> visibleTriangles, vector<PPMImage> textures){
  cout << "Raytracing..." << endl;

  for(int i = 0; i < WIDTH - 1; i++){
    cout << i << endl;
    for(int j = 0; j < HEIGHT - 1; j++){
      vec3 temp = vec3(i-(WIDTH/2),(HEIGHT/2)-j, -distanceOfImagePlaneFromCamera);
      vec3 r = normalize(temp * cameraRotation);
      RayTriangleIntersection intersection = getClosestIntersection(cameraPosition,r,visibleTriangles);

      if(intersection.distanceFromCamera != std::numeric_limits<float>::infinity()){
        uint32_t pixel_colour;
        if (intersection.intersectedTriangle.reflect){
          vec3 colour = reflectionColour(intersection,r,triangles,lightSource,i,j, textures);
          pixel_colour = (255<<24) + (std::min(int(colour.x), 255)<<16) + (std::min(int(colour.y), 255)<<8) + std::min(int(colour.z), 255);
        }
        else if (intersection.intersectedTriangle.glass){
          vec3 colour = refractionColour(intersection,r,triangles,lightSource,i,j, textures);
          pixel_colour = (255<<24) + (std::min(int(colour.x), 255)<<16) + (std::min(int(colour.y), 255)<<8) + std::min(int(colour.z), 255);
        } 
        else if (intersection.intersectedTriangle.textured){
          float brightness =  calcualteBrightness(intersection,lightSource,r,i,j,triangles);
          vec3 colour = calculateTexturePixelColour(intersection, textures, brightness);
          pixel_colour = (255<<24) + (std::min((int)colour[0], 255)<<16) + (std::min((int)colour[1], 255)<<8) + std::min((int)colour[2], 255);
        }
        else {
          float brightness =  calcualteBrightness(intersection,lightSource,r,i,j,triangles);
          Colour colour = intersection.intersectedTriangle.colour;
          int red = colour.red * brightness;
          int green = colour.green * brightness;
          int blue = colour.blue * brightness;
          pixel_colour = (255<<24) + (std::min(red, 255)<<16) + (std::min(green, 255)<<8) + std::min(blue, 255);
        }
        window.setPixelColour(i, j, pixel_colour);
      }
    }
  }
  cout << "done." << endl;
}

void raytraceAntiAlias(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera, vec3 lightSource,vector<ModelTriangle> visibleTriangles, vector<PPMImage> textures){
  cout << "Raytracing (Anti-Alias)..." << endl;

  for(int i = 0; i < WIDTH - 1; i++){
    cout << i << endl;
    for(int j = 0; j < HEIGHT - 1; j++){

      vec3 rayColour = vec3(0,0,0);

      for(int x = 0; x < 2; x++){
        for(int y = 0; y < 2; y++){
          // float AAshiftTempX = ((x*2)-3);
          // float AAshiftX = AAshiftTempX/8;
          // float AAshiftTempY = ((y*2)-3);
          // float AAshiftY = AAshiftTempY/8;
          // float tempX = i-(WIDTH/2) + AAshiftX;
          // float tempY = (HEIGHT/2)-j + AAshiftY;
          float AAshiftTempX = ((x*2)-1);
          float AAshiftX = AAshiftTempX/4;
          float AAshiftTempY = ((y*2)-1);
          float AAshiftY = AAshiftTempY/4;
          float tempX = i-(WIDTH/2) + AAshiftX;
          float tempY = (HEIGHT/2)-j + AAshiftY;

          vec3 temp = vec3(tempX,tempY, -distanceOfImagePlaneFromCamera);
          vec3 r = normalize(temp * cameraRotation);
          RayTriangleIntersection intersection = getClosestIntersection(cameraPosition,r,visibleTriangles);

          if(intersection.distanceFromCamera != std::numeric_limits<float>::infinity()){
            if (intersection.intersectedTriangle.reflect){
              vec3 rayColourNew = reflectionColour(intersection,r,triangles,lightSource,i,j,textures);
              rayColour = rayColour + rayColourNew;
            }else if (intersection.intersectedTriangle.glass){
              vec3 rayColourNew = refractionColour(intersection,r,triangles,lightSource,i,j,textures);
              rayColour = rayColour + rayColourNew;
            }else if (intersection.intersectedTriangle.textured){
              float brightness =  calcualteBrightness(intersection,lightSource,r,i,j,triangles);
              vec3 rayColourNew = calculateTexturePixelColour(intersection, textures, brightness);
              rayColour = rayColour + rayColourNew;
            }else {
              float brightness =  calcualteBrightness(intersection,lightSource,r,i,j,triangles);
              Colour colour = intersection.intersectedTriangle.colour;
              int red = colour.red * brightness;
              int green = colour.green * brightness;
              int blue = colour.blue * brightness;
              vec3 rayColourNew = vec3(std::min(red, 255), std::min(green, 255), std::min(blue, 255));
              rayColour = rayColour + rayColourNew;
            }
          }
        }
      }
      vec3 scale = vec3(4,4,4);
      rayColour = rayColour / scale;
      uint32_t pixel_colour = (255<<24) + (int(rayColour.x)<<16) + (int(rayColour.y)<<8) + int(rayColour.z);
      window.setPixelColour(i, j, pixel_colour);
    }
  }
  cout << "done." << endl;
}

void raytraceTextures(DrawingWindow window, vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation, float distanceOfImagePlaneFromCamera, vec3 lightSource, vector<ModelTriangle> visibleTriangles, char*** texture){
  cout << "Raytracing..." << endl;
  for(int i = 0; i < WIDTH - 1; i++){
    for(int j = 0; j < HEIGHT - 1; j++){
      vec3 temp = vec3(i-(WIDTH/2),(HEIGHT/2)-j, -distanceOfImagePlaneFromCamera);
      vec3 r = normalize(temp * cameraRotation);
      RayTriangleIntersection intersection = getClosestIntersection(cameraPosition,r,visibleTriangles);

      if(intersection.distanceFromCamera != std::numeric_limits<float>::infinity()){
        cout << "comment" << endl;
        // float brightness =  calcualteBrightness(intersection,lightSource,r,i,j,triangles);
        // uint32_t pixel_colour = calculateTexturePixelColour(intersection, texture, brightness);
        // window.setPixelColour(i, j, pixel_colour);
      }
    }
  }
  cout << "done." << endl;
}

// LIGHTING FUNCTIONS //////////////////////////////////////////////////////////
float proximityLighting(RayTriangleIntersection rTI, vec3 lightSource){
  vec3 point = rTI.intersectionPoint;
  float distance = sqrt(pow((point.x - lightSource[0]),2) + pow((point.y - lightSource[1]),2) + pow((point.z - lightSource[2]),2));
  float inverseSquare = 2000 / ( PI * pow(distance, 2));
  if(inverseSquare > 1){
    return 1;
  }
  else{
  return inverseSquare;
  }
}

float angleOfIncidence(RayTriangleIntersection rTI, vec3 lightSource){
  ModelTriangle triangle = rTI.intersectedTriangle;
  vec3 normal = findNormal(rTI);
  vec3 vecToLight = normalize(lightSource - rTI.intersectionPoint);
  float angleOfIncidence = dot(normal, vecToLight);
  if(angleOfIncidence > 0){
    return angleOfIncidence;
  }else{
    return 0;
  }
}

float SpecularHighlight(RayTriangleIntersection rTI, vec3 lightSource, vec3 r, int shineLevel){

  ModelTriangle triangle = rTI.intersectedTriangle;
  vec3 normal = findNormal(rTI);
  vec3 vecFromLight = normalize(rTI.intersectionPoint - lightSource);
  vec3 reflectedVec = vecFromLight - (2 * (dot(vecFromLight, normal)) * normal);
  vec3 rInverse = vec3(-r[0], -r[1], -r[2]);
  float specularValue = pow(dot(reflectedVec, rInverse), shineLevel);
  if(specularValue >= 0){
    return specularValue;
  }
  else return 0;
}

float softShadow(float brightness, RayTriangleIntersection intersection, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j){
  if(inGlassShaddow(intersection,lightSource,triangles,i,j)) return brightness * 0.75;
  float maxShift = 0.1;
  float stepScale = 0.01;
  ModelTriangle triangle = intersection.intersectedTriangle;
  vec3 normal = findNormal(intersection);
  vec3 shiftVector = scaleVector(normal,maxShift);
  vec3 raisedPoint = intersection.intersectionPoint + shiftVector;
  RayTriangleIntersection raisedIntersection = RayTriangleIntersection(raisedPoint,intersection.distanceFromCamera, intersection.intersectedTriangle);
  bool raisedPointInHardShadow = inShaddow(raisedIntersection,lightSource,triangles,i,j);

  vec3 loweredPoint = intersection.intersectionPoint - shiftVector;
  RayTriangleIntersection loweredIntersection = RayTriangleIntersection(loweredPoint,intersection.distanceFromCamera, intersection.intersectedTriangle);
  bool loweredPointInHardShadow = inShaddow(loweredIntersection,lightSource,triangles,i,j);

  vec3 step = scaleVector(normal,stepScale);
  
  if (raisedPointInHardShadow && loweredPointInHardShadow){
    brightness = ambientLight;

  } else if (loweredPointInHardShadow){
    float scale = calculateBrightnessScaler(intersection,scaleVector(shiftVector,-1),step,lightSource,triangles,i,j,stepScale,maxShift);
    brightness = brightness * scale;

  } else if (raisedPointInHardShadow){
    float scale = calculateBrightnessScaler(intersection,shiftVector,scaleVector(step,-1),lightSource,triangles,i,j,stepScale,maxShift);
    brightness = brightness * scale;
  }
  return std::max(brightness,ambientLight);
}

bool inShaddow(RayTriangleIntersection rTI, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j){
  vec3 point = rTI.intersectionPoint;
  ModelTriangle triangle = rTI.intersectedTriangle;
  triangles = removeTriangle(triangle, triangles);
  vec3 vecToLight = lightSource - point;
  point = rTI.intersectionPoint + scaleVector(vecToLight,0.08);

  RayTriangleIntersection intersection = getClosestIntersection(point, normalize(vecToLight), triangles);
  float distanceToLight = sqrt(pow(vecToLight[0],2) + pow(vecToLight[1],2) + pow(vecToLight[2],2));

  if(intersection.distanceFromCamera == std::numeric_limits<float>::infinity()) return false;
  else if(distanceToLight > abs(intersection.distanceFromCamera) && !intersection.intersectedTriangle.glass && !intersection.intersectedTriangle.seethrough) return true;
  else return false;
}

bool inGlassShaddow(RayTriangleIntersection rTI, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j){
  vec3 point = rTI.intersectionPoint;
  ModelTriangle triangle = rTI.intersectedTriangle;
  triangles = removeTriangle(triangle, triangles);
  vec3 vecToLight = lightSource - point;
  point = rTI.intersectionPoint + scaleVector(vecToLight,0.08);

  RayTriangleIntersection intersection = getClosestIntersection(point, normalize(vecToLight), triangles);
  float distanceToLight = sqrt(pow(vecToLight[0],2) + pow(vecToLight[1],2) + pow(vecToLight[2],2));

  if(intersection.distanceFromCamera == std::numeric_limits<float>::infinity()) return false;
  else if(distanceToLight > abs(intersection.distanceFromCamera) && intersection.intersectedTriangle.glass && !intersection.intersectedTriangle.seethrough) return true;
  else return false;
}

float calcualteBrightness(RayTriangleIntersection intersection, vec3 lightSource, vec3 r, int i, int j,vector<ModelTriangle> triangles){
  if(intersection.intersectedTriangle.seethrough){
    return 1;
  }
  float b = proximityLighting(intersection, lightSource);
  float a = angleOfIncidence(intersection, lightSource);
  float spec = SpecularHighlight(intersection, lightSource, r, specPower);
  float brightness = std::max((b * a) + (spec * specIntensity), ambientLight);
  brightness = softShadow(brightness,intersection,lightSource,triangles,i,j);
  return brightness;
}

float calculateBrightnessScaler(RayTriangleIntersection intersection, vec3 shiftVector, vec3 step, vec3 lightSource, vector<ModelTriangle> triangles, int i, int j, float stepScale, float maxShift){
  int count = 0;
  vec3 point = intersection.intersectionPoint + shiftVector;
  RayTriangleIntersection pointIntersection = RayTriangleIntersection(point,intersection.distanceFromCamera, intersection.intersectedTriangle);

  while (inShaddow(pointIntersection,lightSource,triangles,i,j)){
      point = point + step;
      pointIntersection = RayTriangleIntersection(point,intersection.distanceFromCamera, intersection.intersectedTriangle);
      count++;
  }
  float scale = std::min(1 - ((count*stepScale)/(2*maxShift)) ,1.0f);
  return scale;
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
  result.u = bestSolution[1];
  result.v = bestSolution[2];
  return result;
}

vector<ModelTriangle> removeTriangle(ModelTriangle triangle, vector<ModelTriangle> triangles){
  vector<ModelTriangle> newTriangles;
  for(int i = 0; i < (int) triangles.size(); i++){
    if(not(triangles[i].vertices[0] == triangle.vertices[0] && triangles[i].vertices[1] == triangle.vertices[1] && triangles[i].vertices[2] == triangle.vertices[2])){
      newTriangles.push_back(triangles[i]);
    }
  }
  return newTriangles;
}

vec3 scaleVector(vec3 normal, float scaleFactor){
  vec3 scaledVector = vec3(normal.x * scaleFactor,normal.y * scaleFactor,normal.z * scaleFactor);
  return scaledVector;
}

vec3 findNormal(RayTriangleIntersection rTI){
  ModelTriangle triangle = rTI.intersectedTriangle;
  vec3 normal;
  if(triangle.shading == 'N'){
    normal = cross(triangle.vertices[1] - triangle.vertices[0], triangle.vertices[2] - triangle.vertices[0]);
  }
  if(triangle.shading == 'P'){
    float u = rTI.u;
    float v = rTI.v;
    vec3 normal1 = rTI.intersectedTriangle.normals[0];
    vec3 normal2 = rTI.intersectedTriangle.normals[1];
    vec3 normal3 = rTI.intersectedTriangle.normals[2];
    normal = normal1 + (u*(normal2 - normal1)) + (v*(normal3 - normal1));
  }
  return normalize(normal);
}

////////////////////////////////////////////////////////////////////////
// Reflection Functions (MIRRORS)

vec3 reflectionColour(RayTriangleIntersection intersectionOnMirror, vec3 Ri, vector<ModelTriangle> triangles, vec3 lightSource, int i, int j, vector<PPMImage> textures){
  vec3 Rr = calculateVectorOfReflection(intersectionOnMirror,Ri);
  triangles = removeTriangle(intersectionOnMirror.intersectedTriangle, triangles);
  RayTriangleIntersection intersection = getClosestIntersection(intersectionOnMirror.intersectionPoint,Rr,triangles);
  triangles.push_back(intersectionOnMirror.intersectedTriangle);
  vec3 colourfinal;
  if(intersection.distanceFromCamera != std::numeric_limits<float>::infinity()){
      if (intersection.intersectedTriangle.reflect){
         colourfinal = reflectionColour(intersection,Rr,triangles,lightSource,i,j, textures);
      }else if (intersection.intersectedTriangle.glass){
         colourfinal = refractionColour(intersection,Rr,triangles,lightSource,i,j, textures);
      }else if (intersection.intersectedTriangle.textured){
         float brightness =  calcualteBrightness(intersection,lightSource,Rr,i,j,triangles);
         colourfinal = calculateTexturePixelColour(intersection, textures, brightness);
      }
      else{
      float brightness =  calcualteBrightness(intersection,lightSource,Rr,i,j,triangles);
      Colour colour = intersection.intersectedTriangle.colour;
      colourfinal = vec3(colour.red * brightness, colour.green * brightness, colour.blue * brightness);
      }
  }else{
    colourfinal = vec3(0,0,0);
  }
  return colourfinal;
}

vec3 calculateVectorOfReflection(RayTriangleIntersection intersection, vec3 Ri){
  vec3 Rr = vec3(0,0,0);
  ModelTriangle triangle = intersection.intersectedTriangle;
  vec3 N =  normalize(cross(triangle.vertices[1] - triangle.vertices[0], triangle.vertices[2] - triangle.vertices[0]));
  float dotProd = dot(Ri,N);
  vec3 temp = scaleVector(N, 2*dotProd);
  Rr = normalize(Ri - temp);
  return Rr;
} 

////////////////////////////////////////////////////////////////////////
// Refraction Functions (GLASS)

vec3 refractionColour(RayTriangleIntersection intersectionOnSurface, vec3 Ri, vector<ModelTriangle> triangles, vec3 lightSource, int i, int j, vector<PPMImage> textures){
  vec3 Rr = calculateVectorOfRefraction(intersectionOnSurface,Ri);
  triangles = removeTriangle(intersectionOnSurface.intersectedTriangle, triangles);
  RayTriangleIntersection intersection = getClosestIntersection(intersectionOnSurface.intersectionPoint,Rr,triangles);
  triangles.push_back(intersectionOnSurface.intersectedTriangle);
  vec3 colourfinal;
  if(intersection.distanceFromCamera != std::numeric_limits<float>::infinity()){
      if (intersection.intersectedTriangle.reflect){
         colourfinal = reflectionColour(intersection,Rr,triangles,lightSource,i,j, textures);
      }
      else if (intersection.intersectedTriangle.glass){
         colourfinal = refractionColour(intersection,Rr,triangles,lightSource,i,j, textures);
      }
      else if (intersection.intersectedTriangle.textured){
         float brightness =  calcualteBrightness(intersection,lightSource,Rr,i,j,triangles)*0.6;
         colourfinal = calculateTexturePixelColour(intersection, textures, brightness);
      }
      else{
        float brightness =  calcualteBrightness(intersection,lightSource,Rr,i,j,triangles)*0.6;
        Colour colour = intersection.intersectedTriangle.colour;
        colourfinal = vec3(colour.red * brightness, colour.green * brightness, colour.blue * brightness);
      }
  } else {
    colourfinal = vec3 (0,0,0);
  }
  return colourfinal;
}

vec3 calculateVectorOfRefraction(RayTriangleIntersection intersection, vec3 I){
  float ior = 1.5; // Glass
  float ior2 = 1;  // Air
  vec3 Ri = normalize(I);
  vec3 normal = normalize(findNormal(intersection));
  vec3 n;
  float NdotI = dot(normal, Ri);
  if(NdotI < 0){
    // Outside surface 
    NdotI = - NdotI;
    n = normal;
  }else{
    // Inside surface
    float iortemp = ior;
    ior = ior2;
    ior2 = iortemp;
    n = -normal;
  }
  float eta = ior / ior2;
  return eta * Ri - (eta * NdotI) * n;
} 

vec3 getNormalFromBumpMap(RayTriangleIntersection intersection, vector<PPMImage> bumpMaps){
  vec2 mapPoint0 = intersection.intersectedTriangle.bumpMapPoints[0];
  vec2 mapPoint1 = intersection.intersectedTriangle.bumpMapPoints[1];
  vec2 mapPoint2 = intersection.intersectedTriangle.bumpMapPoints[2];
  vec2 mapPoint = mapPoint0 + (intersection.u * (mapPoint1-mapPoint0)) + (intersection.v * (mapPoint2-mapPoint0));

  float mapX = bumpMaps[0].width*mapPoint.x;
  float mapY = bumpMaps[0].height*mapPoint.y;

  float x = (float)bumpMaps[0].payload[int(round(mapX))][int(round(mapY))][0];
  float y = (float)bumpMaps[0].payload[int(round(mapX))][int(round(mapY))][1];
  float z = (float)bumpMaps[0].payload[int(round(mapX))][int(round(mapY))][2];

  vec3 normal = vec3(x,y,z);
  

  vec3 camDir = intersection.intersectionPoint - bumpMaps[0].cameraPosition;
  if (dot(normal,camDir) < 0){
    normal *= -1;
  }

  normal = normalize(normal);

  return normal;
}
