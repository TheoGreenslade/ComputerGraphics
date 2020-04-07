#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

float RandomFloat(float a, float b);
//vector<vec3> diamondSquare(vector<vec3> points, int size);

vector<ModelTriangle> generatePlane(int xs, int zs, int startx, int startz){
  vector<ModelTriangle> generatedTriangles;
  Colour newColour;
  newColour.name = "white";
  newColour.red = 255;
  newColour.green = 255;
  newColour.blue = 255;
  vector<vec3> points;

  // This creates the points
  for(int x = startx; x < startx + xs; x++){
    for(int z = startz; z < startz + zs; z++){
      float y = RandomFloat(0, 1);
      vec3 point = vec3(x, y, z);
      points.push_back(point);
    }
  }
  //vector<vec3> smoothedPoints = diamondSquare(points, sqrt(points.size()));

  // This ties the points together with triangles
  for(int i = 0; i < (int)points.size() - xs; i++){
      // This is so the edges dont connect
      if((i+1)%xs != 0){
        ModelTriangle newTriangle;
        newTriangle.colour = newColour;
        newTriangle.vertices[0] = points[i];
        newTriangle.vertices[2] = points[i + xs];
        newTriangle.vertices[1] = points[i + xs + 1];
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

// vector<vec3> diamondSquare(vector<vec3> points, int size){
//   int cutsize = size/2;
// }
