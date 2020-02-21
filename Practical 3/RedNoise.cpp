#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>

using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 240

void draw();
void update();
void handleEvent(SDL_Event event);
void drawline(CanvasPoint start, CanvasPoint end, Colour colour);
void drawStrokedTri(CanvasTriangle triangle);
void randomStrokedTri();
void drawFilledTri(CanvasTriangle triangle);
void drawFilledTriFlat(CanvasTriangle triangle, int tri);
void randomFilledTri();
void displayPPMImage(string filename);
void drawTextureTriangle(CanvasTriangle triangle, string filename);
void drawTextureTriangleFlatBottom(CanvasTriangle triangle, string filename);
char*** readPPMImagePayload(ifstream ifs,int width,int height);
char ***malloc3dArray(int dim1, int dim2, int dim3);
float* interpolate(float start, float end, int steps);
void drawTextureTriangleFlatTop(CanvasTriangle triangle, string filename);
vector<Colour> readMaterials(string filename);
vector<ModelTriangle> readGeometry(string filename, vector<Colour> materials);
CanvasTriangle projectTriangleOnImagePlane(ModelTriangle modelTriangle, vec3 cameraPosition, mat4x4 cameraTransform);
CanvasPoint projectPointOnImagePlane(vec3 point, vec3 cameraPosition, mat4x4 cameraTransform);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

int main(int argc, char* argv[])
{
  bool boool = true;
  SDL_Event event;

  while(true)
  {
    if(window.pollForInputEvents(&event)) handleEvent(event);
    //update();
    vector<Colour> materials = readMaterials("cornell-box/cornell-box.mtl");
    vector<ModelTriangle> triangles = readGeometry("cornell-box/cornell-box.obj", materials);


    vec3 cameraPosition = vec3(0,0,10000000);
    mat4x4 cameraTransform = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,20,0);

    for(int i = 0; i < triangles.size(); i++){
      CanvasTriangle temp =  projectTriangleOnImagePlane(triangles[i], cameraPosition, cameraTransform);
      drawFilledTri(temp);
    }
    //draw(boool);
    window.renderFrame();
    boool = false;
  }

  // Need to render the frame at the end, or nothing actually gets shown on the screen !
  // window.renderFrame();
}

void draw()
{
  window.clearPixels();
  for(int y=0; y<window.height ;y++) {
    for(int x=0; x<window.width ;x++) {
      float red = rand() % 255;
      float green = 0.0;
      float blue = 0.0;
      uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
      window.setPixelColour(x, y, colour);
    }
  }
}

void update()
{
  // Function for performing animation (shifting artifacts or moving the camera)
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
    if(event.key.keysym.sym == SDLK_LEFT) cout << "LEFT" << endl;
    else if(event.key.keysym.sym == SDLK_RIGHT) cout << "RIGHT" << endl;
    else if(event.key.keysym.sym == SDLK_UP) cout << "UP" << endl;
    else if(event.key.keysym.sym == SDLK_DOWN) cout << "DOWN" << endl;
    else if(event.key.keysym.sym == SDLK_u) randomStrokedTri();
    else if(event.key.keysym.sym == SDLK_f)randomFilledTri();
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN){
   cout << "MOUSE CLICKED: x:" << event.button.x << " y:" << event.button.y << endl;
   CanvasPoint point = CanvasPoint(event.button.x,event.button.y);
   cout << point << endl;
  }
}

void drawline(CanvasPoint start, CanvasPoint end, Colour colour){
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

void drawStrokedTri(CanvasTriangle triangle){
  drawline(triangle.vertices[0],triangle.vertices[1],triangle.colour);
  drawline(triangle.vertices[0],triangle.vertices[2],triangle.colour);
  drawline(triangle.vertices[1],triangle.vertices[2],triangle.colour);
}

void randomStrokedTri(){
  CanvasPoint point1 = CanvasPoint(rand() % window.width,rand() % window.height);
  CanvasPoint point2 = CanvasPoint(rand() % window.width,rand() % window.height);
  CanvasPoint point3 = CanvasPoint(rand() % window.width,rand() % window.height);
  Colour colour = Colour(rand() % 255,rand() % 255,rand() % 255);
  CanvasTriangle triangle = CanvasTriangle(point1,point2,point3,colour);
  drawStrokedTri(triangle);
}

void drawFilledTri(CanvasTriangle triangle){
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

  CanvasTriangle triangle1 = CanvasTriangle(triangle.vertices[0],triangle.vertices[1],p,triangle.colour);
  CanvasTriangle triangle2 = CanvasTriangle(triangle.vertices[1],p,triangle.vertices[2],triangle.colour);

  drawFilledTriFlat(triangle1,1);
  drawFilledTriFlat(triangle2,2);

  // drawStrokedTri(triangle1);
  // drawStrokedTri(triangle2);
}

void drawFilledTriFlat(CanvasTriangle triangle, int tri) {
  if (triangle.vertices[0].y == triangle.vertices[1].y) {
    swap(triangle.vertices[0],triangle.vertices[2]);
  }
  else if (triangle.vertices[0].y== triangle.vertices[2].y) {
    swap(triangle.vertices[0],triangle.vertices[1]);
  }
  
  float x0 = triangle.vertices[0].x;
  float x1 = triangle.vertices[1].x;
  float x2 = triangle.vertices[2].x;
  float y0 = triangle.vertices[0].y;
  float y1 = triangle.vertices[1].y;
  float y2 = triangle.vertices[2].y;

  int top = y0;
  int bottom = y1;
  if (y0 > y1) {
    top = y1;
    bottom = y0;
  }

  for (int y = top; y < bottom; y++){
    float a = x0 + ((x1 - x0)/(y1-y0))*(y-y0);
    float b = x0 + ((x2 - x0)/(y2-y0))*(y-y0);

    if (a > b) swap(a,b);

    for (int x = round(a); x < round(b) ; x ++){
      uint32_t pixel_colour = (255<<24) + (int(triangle.colour.red)<<16) + (int(triangle.colour.green)<<8) + int(triangle.colour.blue);
      window.setPixelColour(x, y, pixel_colour);
    }
  }
}

void randomFilledTri(){
  CanvasPoint point1 = CanvasPoint(rand() % window.width,rand() % window.height);
  CanvasPoint point2 = CanvasPoint(rand() % window.width,rand() % window.height);
  CanvasPoint point3 = CanvasPoint(rand() % window.width,rand() % window.height);
  Colour colour = Colour(rand() % 255,rand() % 255,rand() % 255);
  CanvasTriangle triangle = CanvasTriangle(point1,point2,point3,colour);
  drawFilledTri(triangle);
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
  drawStrokedTri(triangle);

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

  float* edgeXScale1 = interpolate(triangle.vertices[0].x,triangle.vertices[1].x,numRows);
  float* textureEdgeXScale1 = interpolate(triangle.vertices[0].texturePoint.x,triangle.vertices[1].texturePoint.x,numRows);
  float* textureEdgeYScale1 = interpolate(triangle.vertices[0].texturePoint.y,triangle.vertices[1].texturePoint.y,numRows);
  float* edgeXScale2 = interpolate(triangle.vertices[0].x,triangle.vertices[2].x,numRows);
  float* textureEdgeXScale2 = interpolate(triangle.vertices[0].texturePoint.x,triangle.vertices[2].texturePoint.x,numRows);
  float* textureEdgeYScale2 = interpolate(triangle.vertices[0].texturePoint.y,triangle.vertices[2].texturePoint.y,numRows);

  for (int i = 0; i < numRows; i++){
    int numPixelsInRow = edgeXScale2[i] - edgeXScale1[i];

    if (numPixelsInRow == 1){
      int red = int(image[int(triangle.vertices[0].texturePoint.x)][int(triangle.vertices[0].texturePoint.y)][0]);
      int green = int(image[int(triangle.vertices[0].texturePoint.x)][int(triangle.vertices[0].texturePoint.y)][1]);
      int blue = int(image[int(triangle.vertices[0].texturePoint.x)][int(triangle.vertices[0].texturePoint.y)][2]);
      uint32_t pixel_colour = (255<<24) + (red<<16) + (green<<8) + blue;
      window.setPixelColour(triangle.vertices[0].x, triangle.vertices[0].y, pixel_colour);
    } else {
    float* xScaleTexture = interpolate(textureEdgeXScale1[i],textureEdgeXScale2[i],numPixelsInRow);
    float* yScaleTexture = interpolate(textureEdgeYScale1[i],textureEdgeYScale2[i],numPixelsInRow);
    
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

  float* edgeXScale1 = interpolate(triangle.vertices[0].x,triangle.vertices[2].x,numRows);
  float* textureEdgeXScale1 = interpolate(triangle.vertices[0].texturePoint.x,triangle.vertices[2].texturePoint.x,numRows);
  float* textureEdgeYScale1 = interpolate(triangle.vertices[0].texturePoint.y,triangle.vertices[2].texturePoint.y,numRows);
  float* edgeXScale2 = interpolate(triangle.vertices[1].x+1,triangle.vertices[2].x+1,numRows);
  float* textureEdgeXScale2 = interpolate(triangle.vertices[1].texturePoint.x,triangle.vertices[2].texturePoint.x,numRows);
  float* textureEdgeYScale2 = interpolate(triangle.vertices[1].texturePoint.y,triangle.vertices[2].texturePoint.y,numRows);

  for (int i = 0; i < numRows; i++){
    int numPixelsInRow = edgeXScale2[i] - edgeXScale1[i];

    if (numPixelsInRow == 1){
      int red = int(image[int(triangle.vertices[2].texturePoint.x)][int(triangle.vertices[2].texturePoint.y)][0]);
      int green = int(image[int(triangle.vertices[2].texturePoint.x)][int(triangle.vertices[2].texturePoint.y)][1]);
      int blue = int(image[int(triangle.vertices[2].texturePoint.x)][int(triangle.vertices[2].texturePoint.y)][2]);
      uint32_t pixel_colour = (255<<24) + (red<<16) + (green<<8) + blue;
      window.setPixelColour(triangle.vertices[2].x, triangle.vertices[2].y, pixel_colour);
    } else {
    float* xScaleTexture = interpolate(textureEdgeXScale1[i],textureEdgeXScale2[i],numPixelsInRow);
    float* yScaleTexture = interpolate(textureEdgeYScale1[i],textureEdgeYScale2[i],numPixelsInRow);
    
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

float* interpolate(float start, float end, int steps){
  float step = (end - start)/(steps-1);
  float *result;
  result = (float *)malloc(steps * sizeof(float));
  for (int i=0; i < steps; i++){
    result[i] = start + (step*i);
  }
  return result;
}

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

CanvasTriangle projectTriangleOnImagePlane(ModelTriangle modelTriangle, vec3 cameraPosition, mat4x4 cameraTransform){
  CanvasPoint x = projectPointOnImagePlane(modelTriangle.vertices[0], cameraPosition, cameraTransform);
  CanvasPoint y = projectPointOnImagePlane(modelTriangle.vertices[1], cameraPosition, cameraTransform);
  CanvasPoint z = projectPointOnImagePlane(modelTriangle.vertices[2], cameraPosition, cameraTransform);

  CanvasTriangle result = CanvasTriangle(x,y,z,modelTriangle.colour);
  return result;
}

CanvasPoint projectPointOnImagePlane(vec3 point, vec3 cameraPosition, mat4x4 cameraTransform){
  // vec3 imagePlane = cameraPosition - vec3(0,0,5);
  float bigZ = cameraPosition[2]-point[2];
  float smallZ = cameraPosition[2];
  float bigX = point[0];
  float bigY = point[1];
  float littleX = ((smallZ/bigZ)*bigX)*10;
  float littleY = ((smallZ/bigZ)*bigY)*10;
  CanvasPoint result = CanvasPoint(littleX + WIDTH/2,-littleY + HEIGHT/2);
  return result;
}