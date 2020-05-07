#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

vector<ModelTriangle> initialiseMirrors (vector<ModelTriangle> triangles);
vector<ModelTriangle> initialiseGravity (vector<ModelTriangle> triangles);
vector<vector<ModelTriangle>> initialisePlanets(vector<ModelTriangle> sphere);
vector<ModelTriangle> initialiseScaleAndPosition(vector<ModelTriangle> triangles, float scale, vec3 position);
vector<ModelTriangle> makeSeeThrough(vector<ModelTriangle> object);

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
    if (triangle.colour.name == "Red"){
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
  planets.push_back(sun);
  
  vector<ModelTriangle> mercury = initialiseScaleAndPosition(sphere, 0.5, vec3(7,0,0));
  planets.push_back(mercury);
  
  vector<ModelTriangle> venus = initialiseScaleAndPosition(sphere, 1, vec3(10,0,0));
  planets.push_back(venus);
  
  vector<ModelTriangle> earth = initialiseScaleAndPosition(sphere, 1, vec3(13,0,0));
  planets.push_back(earth);
  
  vector<ModelTriangle> mars = initialiseScaleAndPosition(sphere, 0.4, vec3(15,0,0));
  planets.push_back(mars);
  
  vector<ModelTriangle> jupiter = initialiseScaleAndPosition(sphere, 2, vec3(19,0,0));
  planets.push_back(jupiter);
  
  vector<ModelTriangle> saturn = initialiseScaleAndPosition(sphere, 1.5, vec3(24,0,0));
  planets.push_back(saturn);
  
  vector<ModelTriangle> uranus = initialiseScaleAndPosition(sphere, 1, vec3(28,0,0));
  planets.push_back(uranus);
  
  vector<ModelTriangle> neptune = initialiseScaleAndPosition(sphere, 1, vec3(31,0,0));
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


