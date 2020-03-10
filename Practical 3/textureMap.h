#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

void drawTextureTriangle(DrawingWindow window, CanvasTriangle triangle, string filename);
void drawTextureTriangleFlatBottom(DrawingWindow window, CanvasTriangle triangle, string filename);
void drawTextureTriangleFlatTop(DrawingWindow window, CanvasTriangle triangle, string filename);
void drawTextureTriangleFlatTop(DrawingWindow window, CanvasTriangle triangle, string filename);
float* interpolate(float start, float end, int steps);

void drawTextureTriangle(DrawingWindow window, CanvasTriangle triangle, string filename){
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

  drawTextureTriangleFlatBottom(window, triangle1, filename);
  drawTextureTriangleFlatTop(window, triangle2, filename);
}

void drawTextureTriangleFlatBottom(DrawingWindow window, CanvasTriangle triangle, string filename){
  char*** image;
  image = readPPMPayload(filename);

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

void drawTextureTriangleFlatTop(DrawingWindow window, CanvasTriangle triangle, string filename){
  char*** image;
  image = readPPMPayload(filename);

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

float* interpolate(float start, float end, int steps){
  float step = (end - start)/(steps-1);
  float *result;
  result = (float *)malloc(steps * sizeof(float));
  for (int i=0; i < steps; i++){
    result[i] = start + (step*i);
  }
  return result;
}
