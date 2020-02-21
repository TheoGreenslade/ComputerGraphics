#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 240


// Function Declarations
////////////////////////////////////////////////////////////////////////////////

vector<float> interpolate(float from, float to, int numberOfValues);
vector<vec3> interpolateVec3(vec3 from, vec3 to, int numberOfValues);
void drawLine(CanvasPoint from, CanvasPoint to, Colour colour);
void drawStrokedTriangle(CanvasTriangle a);
vector<ModelTriangle> readGeometry(string filename, vector<Colour> materials);
void drawFilledTriangle(CanvasTriangle a);
float findIntersect(CanvasPoint from, CanvasPoint to, float yIntercept);
vector<Colour> readMaterials(string filename);
void update();
void draw(bool boool);
void handleEvent(SDL_Event event);
char ***malloc3dArray(int dim1, int dim2, int dim3);
void drawTextureTriangleFlatBottom(CanvasTriangle triangle, string filename);
void drawTextureTriangleFlatTop(CanvasTriangle triangle, string filename);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);


// Main
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  // Boool is to stop the program drawing indefinitely, no looping print
  // statements
  bool boool = true;
  //SDL_Event event;
  while(true)
  {
    //if(window.pollForInputEvents(&event)) handleEvent(event);
    //update();
    vector<Colour> materials = readMaterials("cornell-box.mtl");
    vector<ModelTriangle> triangles = readGeometry("cornell-box.obj", materials);
    for(int i = 0; i < 8; i++){

    }
    //draw(boool);
    //window.renderFrame();
    boool = false;
  }
}

void update()
{
  // Function for performing animation (shifting artifacts or moving the camera)
}

void draw(bool boool){
  Colour colour = Colour(255, 20, 16);
  Colour colour2 = Colour(20, 255, 16);
  CanvasPoint b = CanvasPoint(100, 20);
  CanvasPoint c = CanvasPoint(200, 220);
  CanvasPoint a = CanvasPoint(175, 100);
  CanvasTriangle d = CanvasTriangle(a, b, c, colour);
  CanvasTriangle e = CanvasTriangle(a, b, c, colour2);
  if(boool == true){
    drawStrokedTriangle(e);
    drawFilledTriangle(d);
  }
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
    if(event.key.keysym.sym == SDLK_LEFT) cout << "LEFT" << endl;
    else if(event.key.keysym.sym == SDLK_RIGHT) cout << "RIGHT" << endl;
    else if(event.key.keysym.sym == SDLK_UP) cout << "UP" << endl;
    else if(event.key.keysym.sym == SDLK_DOWN) cout << "DOWN" << endl;
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN) cout << "MOUSE CLICKED" << endl;
}

// Read In Function
////////////////////////////////////////////////////////////////////////////////
vector<Colour> readMaterials(string filename){

  std::ifstream ifs;
  ifs.open (filename, ifstream::in);
  assert (!ifs.fail( ));

  vector<Colour> materials;

  while(!ifs.eof( )){
    Colour newColour;
    char colourName[256];
    ifs.getline(colourName,256);
    string colourNameString(colourName);
    newColour.name = colourNameString.substr(7);

    string kd;
    char colour[256];
    ifs.getline(colour,256);
    string colourString(colour);
    size_t found = colourString.find(" ");
    kd = colourString.substr(0,found);

    colourString = colourString.substr(found + 1);
    found = colourString.find(" ");
    newColour.red = round(255*stof(colourString.substr(0,found)));

    colourString = colourString.substr(found + 1);
    found = colourString.find(" ");
    newColour.green = round(255*stof(colourString.substr(0,found)));

    colourString = colourString.substr(found + 1);
    found = colourString.find(" ");
    newColour.blue = round(255*stof(colourString.substr(0,found)));

    materials.push_back(newColour);

    char endLine[256];
    ifs.getline(endLine,256);
  }
  return materials;
}

vector<ModelTriangle> readGeometry(string filename, vector<Colour> materials){

  std::ifstream ifs;
  ifs.open (filename, ifstream::in);
  assert (!ifs.fail( ));
  char endLine[256];
  ifs.getline(endLine,256);
  ifs.getline(endLine,256);

  vector<ModelTriangle> ModelTriangles;

  vector<vec3> points;
  while(!ifs.eof( )){
    ModelTriangle newTriangle;
    char triangleName[256];
    ifs.getline(triangleName,256);
    string triangleNameString(triangleName);
    //newTriangle.name = triangleNameString.substr(1);

    char material[256];
    ifs.getline(material,256);
    string colourString(material);
    size_t found = colourString.find(" ");
    string newMaterial = colourString.substr(found + 1);
    for(int i = 0; i < materials.size(); i ++){
      if(newMaterial == materials[i].name){
        newTriangle.colour = materials[i];
      }
    }

    char point[256];
    ifs.getline(point,256);
    while(point[0] == 'v'){
      vec3 newPoint;
      string pointString(point);
      pointString = pointString.substr(2);

      found = pointString.find(" ");
      newPoint.x = stof(pointString.substr(0,found));
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newPoint.y = stof(pointString.substr(0,found));
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newPoint.z = stof(pointString.substr(0,found));

      points.push_back(newPoint);
      ifs.getline(point,256);
    }

    while(point[0] == 'f'){

      string faceString(point);
      faceString = faceString.substr(2);
      found = faceString.find("/");
      int index = stoi(faceString.substr(0,found));
      newTriangle.vertices[0] = points[index - 1];

      faceString = faceString.substr(found + 2);
      found = faceString.find("/");
      index = stoi(faceString.substr(0,found));
      newTriangle.vertices[1] = points[index - 1];

      faceString = faceString.substr(found + 2);
      found = faceString.find("/");
      index = stoi(faceString.substr(0,found));
      newTriangle.vertices[2] = points[index - 1];

      ifs.getline(point,256);
    }
    ModelTriangles.push_back(newTriangle);
  }
  return ModelTriangles;
}


// Interpolation Funtions
////////////////////////////////////////////////////////////////////////////////

vector<float> interpolate(float from, float to, int numberOfValues)
{
  float spacing = (to - from) / (numberOfValues - 1);
  vector<float> values;
  for(int i = 0; i < numberOfValues; i++){
    float value = from + (i * spacing);
    values.push_back(value);
  }
  return values;
}

vector<vec3> interpolateVec3(vec3 from, vec3 to, int numberOfValues){
  vector<vec3> a;
  float spacingx = (to.x - from.x) / (numberOfValues - 1);
  float spacingy = (to.y - from.y) / (numberOfValues - 1);
  float spacingz = (to.z - from.z) / (numberOfValues - 1);
  for(int i = 0; i <= numberOfValues; i++){
    vec3 b{from.x +(i * spacingx), from.y +(i * spacingy), from.z
           + (i * spacingz)};
    a.push_back(b);
  }
  return a;
}


// Drawing Functions
////////////////////////////////////////////////////////////////////////////////

void drawLine(CanvasPoint from, CanvasPoint to, Colour colour)
{
  float x1 = from.x;
  float y1 = from.y;
  float x2 = to.x;
  float y2 = to.y;
  uint32_t colourUint = (colour.red<<16) + (colour.green<<8) + (colour.blue);
  float xDiff = x2 - x1;
  float yDiff = y2 - y1;
  float numberOfSteps = glm::max(abs(xDiff), abs(yDiff));
  float xStepSize = xDiff/numberOfSteps;
  float yStepSize = yDiff/numberOfSteps;
  for(float i = 0.0; i<=numberOfSteps; i++){
    float x = x1 + (xStepSize*i);
    float y = y1 + (yStepSize*i);
    window.setPixelColour(round(x), round(y), colourUint);
  }
}

void drawStrokedTriangle(CanvasTriangle a)
{
  drawLine(a.vertices[0], a.vertices[1], a.colour);
  drawLine(a.vertices[1], a.vertices[2], a.colour);
  drawLine(a.vertices[2],a.vertices[0], a.colour);
}

void drawFilledTriangle(CanvasTriangle triangle)
{
  Colour colour = triangle.colour;
  uint32_t colourUint = (colour.red<<16) + (colour.green<<8) + (colour.blue);
  CanvasPoint a = triangle.vertices[0];
  CanvasPoint b = triangle.vertices[1];
  CanvasPoint c = triangle.vertices[2];
  vector<CanvasPoint> list{a, b, c};
  // Sorting list, descending order, c++ lambda function
  sort(list.begin(), list.end(), [](const CanvasPoint& lhs,
       const CanvasPoint& rhs) {
    return lhs.y < rhs.y;});
  CanvasPoint middlePoint = CanvasPoint(findIntersect(list[0], list[2],
                                                      list[1].y), list[1].y);
  // Top triangle
  for(int y = list[0].y; y < list[1].y; y++){
    CanvasPoint TempA = CanvasPoint(findIntersect(list[0], middlePoint, y), y);
    CanvasPoint TempB = CanvasPoint(findIntersect(list[0], list[1], y), y);
    if(TempA.x != TempB.x){ // Causes division by 0
      drawLine(TempA, TempB, triangle.colour);
    }else{
      window.setPixelColour(TempA.x, y, colourUint);
    }
  }
  // Bottom Triangle
  for(int y = list[1].y; y < list[2].y; y++){
    CanvasPoint TempA = CanvasPoint(findIntersect(middlePoint, list[2], y), y);
    CanvasPoint TempB = CanvasPoint(findIntersect(list[1], list[2], y), y);
    if(TempA.x != TempB.x){ // Causes division by 0
      drawLine(TempA, TempB, triangle.colour);
    }else{
      window.setPixelColour(TempA.x, y, colourUint);
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
  float numberOfSteps = glm::max(abs(xDiff), abs(yDiff));
  float xStepSize = xDiff/numberOfSteps;
  float yStepSize = yDiff/numberOfSteps;
  for(int i = 0; i<numberOfSteps; i++){
    int x = round(x1 + (xStepSize*i));
    int y = round(y1 + (yStepSize*i));
    if(y == yIntercept){
      return x;
    }
  }
return 1; // Error code
}

char*** readPayload(string filename){
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

  char maxColStr[256];
  ifs.getline(maxColStr,256);

  char*** image;
  image = malloc3dArray(width,height,3);
  for (int y = 0; y < height; y++){
    for (int x = 0; x < width; x++){
      ifs.get(image[x][y][0]);
      ifs.get(image[x][y][1]);
      ifs.get(image[x][y][2]);
    }
  }
  return image;
}

void displayPPMImage(string filename){
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
  image = readPayload(filename);

  for (int y = 0; y < height; y++){
    for (int x = 0; x < width; x++){
      uint32_t pixel_colour = (255<<24) + (int(image[x][y][0])<<16) + (int(image[x][y][1])<<8) + int(image[x][y][2]);
      window.setPixelColour(x, y, pixel_colour);
    }
  }
  ifs.close();
}

void drawTextureTriangle(CanvasTriangle triangle, string filename){
  drawStrokedTriangle(triangle);

  if (triangle.vertices[0].y > triangle.vertices[1].y){
    swap(triangle.vertices[0],triangle.vertices[1]);
  }
  if (triangle.vertices[1].y > triangle.vertices[2].y){
    swap(triangle.vertices[1],triangle.vertices[2]);
  }

  float x0 = triangle.vertices[0].x;
  float x2 = triangle.vertices[2].x;
  float y0 = triangle.vertices[0].y;
  float y1 = triangle.vertices[1].y;
  float y2 = triangle.vertices[2].y;

  float x = round(x0 + ((x2 - x0)*(y1 - y0)/(y2 - y0)));
  CanvasPoint p = CanvasPoint(x,y1);

  float topToBottomEdge = sqrt(pow((x2 - x0),2) + pow((y2 - y0),2));
  float topToPEdge = sqrt(pow((p.x - x0),2) + pow((p.y - y0),2));
  float proportionPDownEdge = topToPEdge/topToBottomEdge;

  float texturePX = ((triangle.vertices[2].texturePoint.x - triangle.vertices[0].texturePoint.x) * proportionPDownEdge) + triangle.vertices[0].texturePoint.x;
  float texturePY = ((triangle.vertices[2].texturePoint.y - triangle.vertices[0].texturePoint.y) * proportionPDownEdge) + triangle.vertices[0].texturePoint.y;
  TexturePoint textureP = TexturePoint(texturePX,texturePY);
  p.texturePoint = textureP;

  CanvasTriangle triangle1 = CanvasTriangle(triangle.vertices[0],triangle.vertices[1],p,triangle.colour);
  CanvasTriangle triangle2 = CanvasTriangle(triangle.vertices[1],p,triangle.vertices[2],triangle.colour);

  drawTextureTriangleFlatBottom(triangle1, filename);
  drawTextureTriangleFlatTop(triangle2, filename);
}

void drawTextureTriangleFlatBottom(CanvasTriangle triangle, string filename){
  char*** image;
  image = readPayload(filename);

  int numRows = triangle.vertices[2].y - triangle.vertices[0].y;

  vector<float> edgeXScale1 = interpolate(triangle.vertices[0].x,triangle.vertices[1].x,numRows);
  vector<float> textureEdgeXScale1 = interpolate(triangle.vertices[0].texturePoint.x,triangle.vertices[1].texturePoint.x,numRows);
  vector<float> textureEdgeYScale1 = interpolate(triangle.vertices[0].texturePoint.y,triangle.vertices[1].texturePoint.y,numRows);
  vector<float> edgeXScale2 = interpolate(triangle.vertices[0].x,triangle.vertices[2].x,numRows);
  vector<float> textureEdgeXScale2 = interpolate(triangle.vertices[0].texturePoint.x,triangle.vertices[2].texturePoint.x,numRows);
  vector<float> textureEdgeYScale2 = interpolate(triangle.vertices[0].texturePoint.y,triangle.vertices[2].texturePoint.y,numRows);

  for (int i = 0; i < numRows; i++){
    int numPixelsInRow = edgeXScale2[i] - edgeXScale1[i];

    if (numPixelsInRow == 1){
      int red = int(image[int(triangle.vertices[0].texturePoint.x)][int(triangle.vertices[0].texturePoint.y)][0]);
      int green = int(image[int(triangle.vertices[0].texturePoint.x)][int(triangle.vertices[0].texturePoint.y)][1]);
      int blue = int(image[int(triangle.vertices[0].texturePoint.x)][int(triangle.vertices[0].texturePoint.y)][2]);
      uint32_t pixel_colour = (255<<24) + (red<<16) + (green<<8) + blue;
      window.setPixelColour(triangle.vertices[0].x, triangle.vertices[0].y, pixel_colour);
    } else {
    vector<float> xScaleTexture = interpolate(textureEdgeXScale1[i],textureEdgeXScale2[i],numPixelsInRow);
    vector<float> yScaleTexture = interpolate(textureEdgeYScale1[i],textureEdgeYScale2[i],numPixelsInRow);

      for (int j = 0; j < numPixelsInRow; j++){
        int red = int(image[int(xScaleTexture[j])][int(yScaleTexture[j])][0]);
        int green = int(image[int(xScaleTexture[j])][int(yScaleTexture[j])][1]);
        int blue = int(image[int(xScaleTexture[j])][int(yScaleTexture[j])][2]);
        uint32_t pixel_colour = (255<<24) + (red<<16) + (green<<8) + blue;
        window.setPixelColour(edgeXScale1[i] + j + 1, triangle.vertices[0].y + i, pixel_colour);
      }
    }
  }
}

void drawTextureTriangleFlatTop(CanvasTriangle triangle, string filename){
  char*** image;
  image = readPayload(filename);

  int numRows = triangle.vertices[2].y - triangle.vertices[0].y;

  vector<float> edgeXScale1 = interpolate(triangle.vertices[0].x,triangle.vertices[2].x,numRows);
  vector<float> textureEdgeXScale1 = interpolate(triangle.vertices[0].texturePoint.x,triangle.vertices[2].texturePoint.x,numRows);
  vector<float> textureEdgeYScale1 = interpolate(triangle.vertices[0].texturePoint.y,triangle.vertices[2].texturePoint.y,numRows);
  vector<float> edgeXScale2 = interpolate(triangle.vertices[1].x+1,triangle.vertices[2].x+1,numRows);
  vector<float> textureEdgeXScale2 = interpolate(triangle.vertices[1].texturePoint.x,triangle.vertices[2].texturePoint.x,numRows);
  vector<float> textureEdgeYScale2 = interpolate(triangle.vertices[1].texturePoint.y,triangle.vertices[2].texturePoint.y,numRows);

  for (int i = 0; i < numRows; i++){
    int numPixelsInRow = edgeXScale2[i] - edgeXScale1[i];

    if (numPixelsInRow == 1){
      int red = int(image[int(triangle.vertices[2].texturePoint.x)][int(triangle.vertices[2].texturePoint.y)][0]);
      int green = int(image[int(triangle.vertices[2].texturePoint.x)][int(triangle.vertices[2].texturePoint.y)][1]);
      int blue = int(image[int(triangle.vertices[2].texturePoint.x)][int(triangle.vertices[2].texturePoint.y)][2]);
      uint32_t pixel_colour = (255<<24) + (red<<16) + (green<<8) + blue;
      window.setPixelColour(triangle.vertices[2].x, triangle.vertices[2].y, pixel_colour);
    } else {
    vector<float> xScaleTexture = interpolate(textureEdgeXScale1[i],textureEdgeXScale2[i],numPixelsInRow);
    vector<float> yScaleTexture = interpolate(textureEdgeYScale1[i],textureEdgeYScale2[i],numPixelsInRow);

      for (int j = 0; j < numPixelsInRow; j++){
        int red = int(image[int(xScaleTexture[j])][int(yScaleTexture[j])][0]);
        int green = int(image[int(xScaleTexture[j])][int(yScaleTexture[j])][1]);
        int blue = int(image[int(xScaleTexture[j])][int(yScaleTexture[j])][2]);
        uint32_t pixel_colour = (255<<24) + (red<<16) + (green<<8) + blue;
        window.setPixelColour(edgeXScale1[i] + j, triangle.vertices[0].y + i, pixel_colour);
      }
    }
  }
}

char ***malloc3dArray(int dim1, int dim2, int dim3){
    int i, j;
    char ***array = (char ***) malloc(dim1 * sizeof(char **));

    for (i = 0; i < dim1; i++) {
      array[i] = (char **) malloc(dim2 * sizeof(char *));
	    for (j = 0; j < dim2; j++) {
  	    array[i][j] = (char *) malloc(dim3 * sizeof(char));
	    }
    }
    return array;
}
