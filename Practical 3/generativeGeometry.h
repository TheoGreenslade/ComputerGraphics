#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

float RandomFloat(float a, float b);

// for testing
vector<ModelTriangle> generateSquare(){
  vector<ModelTriangle> generatedTriangles;
  Colour newColour;
  newColour.name = "white";
  newColour.red = 255;
  newColour.green = 255;
  newColour.blue = 255;
  vec3 point1 = vec3(-5,0,0);
  vec3 point2 = vec3(5,0,0);
  vec3 point3 = vec3(-5,0,-10);
  vec3 point4 = vec3(5,0,-10);
  ModelTriangle newTriangle;
  newTriangle.vertices[0] = point1;
  newTriangle.vertices[1] = point2;
  newTriangle.vertices[2] = point3;
  newTriangle.colour = newColour;
  ModelTriangle newTriangle2;
  newTriangle2.vertices[0] = point2;
  newTriangle2.vertices[1] = point3;
  newTriangle2.vertices[2] = point4;
  newTriangle2.colour = newColour;
  generatedTriangles.push_back(newTriangle);
  generatedTriangles.push_back(newTriangle2);
  return generatedTriangles;
}

vector<ModelTriangle> generatePlane(int xs, int zs){
  vector<ModelTriangle> generatedTriangles;
  Colour newColour;
  newColour.name = "white";
  newColour.red = 255;
  newColour.green = 255;
  newColour.blue = 255;
  vector<vec3> points;

  // This creates the points
  for(int x = -3; x < -3 + xs; x++){
    for(int z = -6; z < -6 + zs; z++){
      float y = RandomFloat(0, 1);
      vec3 point = vec3(x, y, z);
      points.push_back(point);
    }
  }
  // This ties the points together with triangles
  for(int i = 0; i < (int)points.size() - xs; i++){
      if((i+1)%5 != 0){
        ModelTriangle newTriangle;
        newTriangle.colour = newColour;
        newTriangle.vertices[0] = points[i];
        newTriangle.vertices[1] = points[i + xs];
        newTriangle.vertices[2] = points[i + xs + 1];
        generatedTriangles.push_back(newTriangle);
        newTriangle.vertices[0] = points[i];
        newTriangle.vertices[1] = points[i + 1];
        newTriangle.vertices[2] = points[i + xs + 1];
        generatedTriangles.push_back(newTriangle);
      }
  }
  return generatedTriangles;
}

float RandomFloat(float a, float b) {
  float random = ((float) rand()) / (float) RAND_MAX;
  float diff = b - a;
  float r = random * diff;
  return a + r;
}
