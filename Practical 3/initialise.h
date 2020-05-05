#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

vector<ModelTriangle> initialiseMirrors (vector<ModelTriangle> triangles);
vector<ModelTriangle> initialiseGravity (vector<ModelTriangle> triangles);

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
