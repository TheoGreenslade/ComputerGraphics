#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

struct Pixel {
  int x;
  int y;
  float zinv;
};

// vector<CanvasPoint> interpolatePixels(Pixel pixel1, Pixel pixel2){
//   float diffX = pixel2.x - pixel1.x;
//   float diffY = pixel2.y - pixel1.y;
//   float diffZInv = pixel2.zinv - pixel1.zinv;

//   float steps = std::max(abs(diffX),abs(diffY));

//   float stepX = diffX / steps;
//   float stepY = diffY / steps;
//   float stepZInv = diffZInv / steps;

//   vector<Canv> result;
//   for (int i=0; i < steps; i++){
//     Pixel temp;
//     temp.x = pixel1.x + (stepX*i);
//     temp.y = pixel1.y + (stepY*i);
//     temp.zinv = pixel1.zinv + (stepZInv*i);
//     result.push_back(temp);
//   }
//   return result;
// }

double calculateDepth(CanvasPoint point1, CanvasPoint point2, float y){
  float bigY = point2.y - point1.y;
  float bigZ = point2.depth - point1.depth;
  float littleY = y - point1.y;
  float littleZ = bigZ * (littleY/bigY);
  double depth = littleZ + point1.depth;
  return depth;
}