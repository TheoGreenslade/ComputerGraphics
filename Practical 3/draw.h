#include "depthBuffer.h"

#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

void drawLine(DrawingWindow window, CanvasPoint start, CanvasPoint end, Colour colour);
void drawStrokedTri(DrawingWindow window, CanvasTriangle triangle);
void drawFilledTri(DrawingWindow window, CanvasTriangle triangle);
void drawFilledTopTri(DrawingWindow window, vector<CanvasPoint> list, CanvasPoint middlePoint, Colour colour);
void drawFilledBottomTri(DrawingWindow window, vector<CanvasPoint> list, CanvasPoint middlePoint, Colour colour);
float findIntersect(CanvasPoint from, CanvasPoint to, float yIntercept);

void drawLine(DrawingWindow window, CanvasPoint start, CanvasPoint end, Colour colour){
  float xDiff = end.x - start.x;
  float yDiff = end.y - start.y;
  float numberOfSteps = std::max(abs(xDiff),abs(yDiff));
  float xStepSize = xDiff/numberOfSteps;
  float yStepSize = yDiff/numberOfSteps;

  uint32_t pixel_colour = (255<<24) + (int(colour.red)<<16) + (int(colour.green)<<8) + int(colour.blue);

  for (float i=0.0; i<numberOfSteps; i++) {
    float x = start.x + (xStepSize*i);
    float y = start.y + (yStepSize*i);
    window.setPixelColour(round(x), round(y), pixel_colour);
  }
}

void drawStrokedTri(DrawingWindow window, CanvasTriangle triangle){
  drawLine(window, triangle.vertices[0],triangle.vertices[1],triangle.colour);
  drawLine(window, triangle.vertices[0],triangle.vertices[2],triangle.colour);
  drawLine(window, triangle.vertices[1],triangle.vertices[2],triangle.colour);
}

void drawFilledTri(DrawingWindow window, CanvasTriangle triangle)
{
  Colour colour = triangle.colour;
  CanvasPoint a = triangle.vertices[0];
  CanvasPoint b = triangle.vertices[1];
  CanvasPoint c = triangle.vertices[2];
  vector<CanvasPoint> list{a, b, c};
  // Sorting list, descending order, c++ lambda function
  sort(list.begin(), list.end(), [](const CanvasPoint& lhs,
       const CanvasPoint& rhs) {
    return lhs.y < rhs.y;});

  double depth = calculateDepth(list[0],list[2],list[1].y);

  CanvasPoint middlePoint = CanvasPoint(findIntersect(list[0], list[2], list[1].y), list[1].y, depth);
  CanvasTriangle temp =  CanvasTriangle(list[0], list[1], middlePoint, colour);
  CanvasTriangle temp2 =  CanvasTriangle(list[1], list[2], middlePoint, colour);
  drawStrokedTri(window, triangle);
  drawFilledBottomTri(window, list, middlePoint, colour);
  drawFilledTopTri(window, list, middlePoint, colour);

}

void drawFilledTopTri(DrawingWindow window, vector<CanvasPoint> list, CanvasPoint middlePoint, Colour colour){
  for(int y = round(list[0].y); y < list[1].y; y++){
    CanvasPoint TempA = CanvasPoint(findIntersect(list[0], middlePoint, y), y);
    CanvasPoint TempB = CanvasPoint(findIntersect(list[0], list[1], y), y);
    if(TempA.x != TempB.x){ // Causes division by 0
      drawLine(window, TempA, TempB, colour);
    }else{
      window.setPixelColour(list[0].x, y, (colour.red<<16) + (colour.green<<8) + (colour.blue));
    }
  }
}

void drawFilledBottomTri(DrawingWindow window, vector<CanvasPoint> list, CanvasPoint middlePoint, Colour colour){
  for(int y = round(list[1].y); y < list[2].y; y++){
    CanvasPoint TempA = CanvasPoint(findIntersect(middlePoint, list[2], y), y);
    CanvasPoint TempB = CanvasPoint(findIntersect(list[1], list[2], y), y);
    if(TempA.x != TempB.x){ // Causes division by 0
      drawLine(window, TempA, TempB, colour);
    }else{
      window.setPixelColour(list[2].x, y, (colour.red<<16) + (colour.green<<8) + (colour.blue));
    }
  }
}

float findIntersect(CanvasPoint from, CanvasPoint to, float yIntercept){
  float x1 = from.x;
  float y1 = from.y;
  float x2 = to.x;
  float y2 = to.y;
  float xDiff = x2 - x1;
  float yDiff = y2 - y1;
  if(round(yIntercept) == round(from.y)){
    return x1;
  }
  if(round(yIntercept) == round(to.y)){
    return x2;
  }
  float littleY = y1 - yIntercept;
  float littleX = xDiff * (littleY / yDiff);
  return x1 - littleX;
}

void displayPPMImage(DrawingWindow window, string filename){
  std::ifstream ifs;
  ifs.open (filename, ifstream::in);

  char encoding[256];
  ifs.getline(encoding,256);

  char comment[256];
  ifs.getline(comment,256);

  char dimensions[256];
  ifs.getline(dimensions,256);
  string dimensionsString(dimensions);
  size_t found = dimensionsString.find(" ");
  int width = stoi(dimensionsString.substr(0,found));
  int height = stoi(dimensionsString.substr(found+1));
  if ((width != window.width) || (height!= window.height)){
    window.destroy();
    window = DrawingWindow(width,height,false);
  }

  char maxColStr[256];
  ifs.getline(maxColStr,256);
  int maxCol = stoi(maxColStr);

  cout << maxCol << endl;

  char*** image;
  image = readPPMPayload(filename);

  for (int y = 0; y < height; y++){
    for (int x = 0; x < width; x++){
      uint32_t pixel_colour = (255<<24) + (int(image[x][y][0])<<16) + (int(image[x][y][1])<<8) + int(image[x][y][2]);
      window.setPixelColour(x, y, pixel_colour);
    }
  }
  ifs.close();
}

