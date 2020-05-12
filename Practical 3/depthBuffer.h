#define WIDTH 640
#define HEIGHT 480
using namespace std;
using namespace glm;

struct Pixel {
  int x;
  int y;
  float zinv;
};

double calculateDepth(CanvasPoint point1, CanvasPoint point2, float y){
  float bigY = point2.y - point1.y;
  float bigZ = point2.depth - point1.depth;
  float littleY = y - point1.y;
  float littleZ = bigZ * (littleY/bigY);
  double depth = littleZ + point1.depth;
  return depth;
}
