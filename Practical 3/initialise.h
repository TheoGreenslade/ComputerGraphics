#define WIDTH 640
#define HEIGHT 480

using namespace std;
using namespace glm;

vector<ModelTriangle> initialiseMirrors (vector<ModelTriangle> triangles);
vector<ModelTriangle> initialiseGravity (vector<ModelTriangle> triangles);
vector<vector<ModelTriangle>> initialisePlanets(vector<ModelTriangle> sphere);
vector<ModelTriangle> initialiseScaleAndPosition(vector<ModelTriangle> triangles, float scale, vec3 position);
vector<ModelTriangle> makeSeeThrough(vector<ModelTriangle> object);
vector<ModelTriangle> makeTextured(vector<ModelTriangle> object, int textureNumber);
vector<ModelTriangle> initialiseStars();
ModelTriangle initialiseStar(vec3 topLeft, vec3 bottomLeft, vec3 topRight, Colour colour, bool topOrBottom);

vector<ModelTriangle> initialiseMirrors (vector<ModelTriangle> triangles){
  int NumOfTri = triangles.size();
  vector<ModelTriangle> newTriangles;
  for (int i = 0; i <NumOfTri; i++){
    ModelTriangle triangle = triangles[i];
    if ((triangle.colour.name == "Blue")){
      triangle.reflect = true;
    }
    newTriangles.push_back(triangle);
  }
  return newTriangles;
}

vector<ModelTriangle> initialiseGravity (vector<ModelTriangle> triangles){
  int NumOfTri = triangles.size();
  vector<ModelTriangle> newTriangles;
  for (int i = 0; i <NumOfTri; i++){
    ModelTriangle triangle = triangles[i];
    if ((triangle.colour.name == "Blue") || (triangle.colour.name == "Red")){
      triangle.fall = true;
    }
    newTriangles.push_back(triangle);
  }
  return newTriangles;
}

vector<ModelTriangle> initialiseGlass (vector<ModelTriangle> triangles){
  int NumOfTri = triangles.size();
  vector<ModelTriangle> newTriangles;
  for (int i = 0; i <NumOfTri; i++){
    ModelTriangle triangle = triangles[i];
    if (triangle.colour.name == "Glass"){
      triangle.glass = true;
    }
    newTriangles.push_back(triangle);
  }
  return newTriangles;
}

// mercury, venus, earth, mars, jupiter, saturn, uranus, neptune
vector<vector<ModelTriangle>> initialisePlanets(vector<ModelTriangle> sphere){
  vector<vector<ModelTriangle>> planets;
  vector<ModelTriangle> sun = initialiseScaleAndPosition(sphere, 5, vec3(0,0,0));
  sun = makeSeeThrough(sun);
  sun = makeTextured(sun,0);
  planets.push_back(sun);
  
  vector<ModelTriangle> mercury = initialiseScaleAndPosition(sphere, 0.5, vec3(7,0,0));
  mercury = makeTextured(mercury,1);
  planets.push_back(mercury);
  
  vector<ModelTriangle> venus = initialiseScaleAndPosition(sphere, 1, vec3(10,0,0));
  venus = makeTextured(venus,2);
  planets.push_back(venus);
  
  vector<ModelTriangle> earth = initialiseScaleAndPosition(sphere, 1, vec3(13,0,0));
  earth = makeTextured(earth,3);
  planets.push_back(earth);
  
  vector<ModelTriangle> mars = initialiseScaleAndPosition(sphere, 0.4, vec3(15,0,0));
  mars = makeTextured(mars,4);
  planets.push_back(mars);
  
  vector<ModelTriangle> jupiter = initialiseScaleAndPosition(sphere, 2, vec3(19,0,0));
  jupiter = makeTextured(jupiter,5);
  planets.push_back(jupiter);

  vector<ModelTriangle> saturn = initialiseScaleAndPosition(sphere, 1.5, vec3(24,0,0));
  saturn = makeTextured(saturn, 6);
  planets.push_back(saturn);
  
  vector<ModelTriangle> uranus = initialiseScaleAndPosition(sphere, 1, vec3(28,0,0));
  uranus = makeTextured(uranus,7);
  planets.push_back(uranus);
  
  vector<ModelTriangle> neptune = initialiseScaleAndPosition(sphere, 1, vec3(31,0,0));
  neptune = makeTextured(neptune,8);
  planets.push_back(neptune);

  return planets;
}

vector<ModelTriangle> initialiseScaleAndPosition(vector<ModelTriangle> triangles, float scale, vec3 position){
  vector<ModelTriangle> newTriangles;
  int n = triangles.size();
  for(int i = 0; i < n; i++){
    ModelTriangle newTriangle = triangles[i];
    newTriangle.vertices[0] = (triangles[i].vertices[0] * scale) + position;
    newTriangle.vertices[1] = (triangles[i].vertices[1] * scale) + position;
    newTriangle.vertices[2] = (triangles[i].vertices[2] * scale) + position;
    newTriangles.push_back(newTriangle);
    }
  return newTriangles;
}

vector<ModelTriangle> makeSeeThrough(vector<ModelTriangle> object){
  vector<ModelTriangle> newTriangles;
  int n = object.size();
  for(int i = 0; i < n; i++){
    ModelTriangle newTriangle = object[i];
    newTriangle.seethrough = true;
    newTriangles.push_back(newTriangle);
    }
  return newTriangles;
}

vector<ModelTriangle> makeTextured(vector<ModelTriangle> object, int textureNumber){
  vector<ModelTriangle> newTriangles;
  int n = object.size();
  for(int i = 0; i < n; i++){
    ModelTriangle newTriangle = object[i];
    newTriangle.textured = true;
    newTriangle.textureNum = textureNumber;
    newTriangles.push_back(newTriangle);
    }
  return newTriangles;
}

vector<ModelTriangle> initialiseStars(){
  Colour colour = Colour(255,0,255);
  float z = -100;
  float x = 100;
  float y = 100;
  vec3 topLeft = vec3(-x,y,z);
  vec3 bottomLeft = vec3(-x,-y,z);
  vec3 topRight = vec3(x,y,z);
  vec3 bottomRight = vec3(x,-y,z);
  
  vec3 topLeftFront = vec3(-x,y,50);
  vec3 bottomLeftFront = vec3(-x,-y,50);
  vec3 topRightFront = vec3(x,y,50);
  vec3 bottomRightFront = vec3(x,-y,50);

  vector<ModelTriangle> stars;
  // Back
  stars.push_back(initialiseStar(topLeft, bottomLeft, topRight, colour, true));
  stars.push_back(initialiseStar(bottomRight, topRight, bottomLeft, colour, false));
  // Left
  stars.push_back(initialiseStar(topLeftFront, bottomLeftFront, topLeft, colour, true));
  stars.push_back(initialiseStar(bottomLeft, topLeft, bottomLeftFront, colour, false));
  // Right
  stars.push_back(initialiseStar(topRight, bottomRight, topRightFront, colour, true));
  stars.push_back(initialiseStar(bottomRightFront, topRightFront, bottomRight, colour, false));
  // Top
  stars.push_back(initialiseStar(topLeftFront, topLeft, topRightFront, colour, true));
  stars.push_back(initialiseStar(topRight, topRightFront, topLeft, colour, false));
  // Bottom
  stars.push_back(initialiseStar(bottomLeft, bottomLeftFront, bottomRight, colour, true));
  stars.push_back(initialiseStar(bottomRightFront, bottomRight, bottomLeftFront, colour, false));
  stars = makeSeeThrough(stars);
  stars = makeTextured(stars, 9);
  return stars;
}

ModelTriangle initialiseStar(vec3 topLeft, vec3 bottomLeft, vec3 topRight, Colour colour, bool topOrBottom){
  ModelTriangle star = ModelTriangle(topLeft,bottomLeft,topRight, colour);
  star.shading = 'N';
  if(topOrBottom){
    star.texturePoints[0] = vec2(0,0);
    star.texturePoints[1] = vec2(0,1);
    star.texturePoints[2] = vec2(1,0);
  }else{
    star.texturePoints[0] = vec2(1,1);
    star.texturePoints[1] = vec2(1,0);
    star.texturePoints[2] = vec2(0,1);
  }
  return star;
}


