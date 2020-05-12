#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

char ***malloc3dArray(int dim1, int dim2, int dim3);

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

vector<ModelTriangle> readGeometry(string filename, vector<Colour> materials, float  scalingFactor){

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
    int materialssize = materials.size();
    for(int i = 0; i < materialssize; i ++){
      if(newMaterial == materials[i].name){
        newTriangle.colour = materials[i];
      }
    }
    // cout << newTriangle.colour << newl;

    char point[256];
    ifs.getline(point,256);
    while(point[0] == 'v'){
      vec3 newPoint;
      string pointString(point);
      pointString = pointString.substr(2);

      found = pointString.find(" ");
      newPoint.x = stof(pointString.substr(0,found))*scalingFactor;
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newPoint.y = stof(pointString.substr(0,found))*scalingFactor;
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newPoint.z = stof(pointString.substr(0,found))*scalingFactor;

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
      newTriangle.shading = 'N';
      ModelTriangles.push_back(newTriangle);
    }

  }
  return ModelTriangles;
}

vector<ModelTriangle> readGeometrySphere(string filename, float  scalingFactor){

  std::ifstream ifs;
  ifs.open (filename, ifstream::in);
  assert (!ifs.fail( ));

  vector<ModelTriangle> ModelTriangles;
  vector<vec3> points;
  vector<vec3> normals;
  vector<vec2> texturePoints;

  Colour newColour;
  newColour.name = "Red";
  newColour.red = 255;
  newColour.green = 0;
  newColour.blue = 0;

  while(!ifs.eof( )){
    char point[256];
    ifs.getline(point,256);
    if(point[0] == 'v' && point[1] == ' '){
      vec3 newPoint;
      string pointString(point);
      pointString = pointString.substr(3);

      size_t found = pointString.find(" ");
      newPoint.x = stof(pointString.substr(0,found))*scalingFactor;
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newPoint.y = stof(pointString.substr(0,found))*scalingFactor;
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newPoint.z = stof(pointString.substr(0,found))*scalingFactor;

      points.push_back(newPoint);
    }
    else if(point[0] == 'v' && point[1] == 'n'){
      vec3 newNormal;
      string pointString(point);
      pointString = pointString.substr(4);

      size_t found = pointString.find(" ");
      newNormal.x = stof(pointString.substr(0,found));
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newNormal.y = stof(pointString.substr(0,found));
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newNormal.z = stof(pointString.substr(0,found));
      normals.push_back(newNormal);
    }
    else if(point[0] == 'v' && point[1] == 't'){
      vec2 newTexturePoint;
      string pointString(point);
      pointString = pointString.substr(4);

      size_t found = pointString.find(" ");
      newTexturePoint.x = stof(pointString.substr(0,found));
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newTexturePoint.y = stof(pointString.substr(0,found));
      texturePoints.push_back(newTexturePoint);
    }
    else if(point[0] == 'f'){
      ModelTriangle newTriangle;
      newTriangle.colour = newColour;
      // point 1
      string faceString(point);
      faceString = faceString.substr(2);
      size_t found = faceString.find("/");
      int index = stoi(faceString.substr(0,found));
      newTriangle.vertices[0] = points[index - 1];
      // texture 1
      faceString = faceString.substr(found + 1);
      found = faceString.find("/");
      index = stoi(faceString.substr(0,found));
      newTriangle.texturePoints[0] = texturePoints[index -1];
      // normal 1
      faceString = faceString.substr(found + 1);
      found = faceString.find(" ");
      index = stoi(faceString.substr(0,found));
      newTriangle.normals[0] = normals[index - 1];

      // point 2
      faceString = faceString.substr(found + 1);
      found = faceString.find("/");
      index = stoi(faceString.substr(0,found));
      newTriangle.vertices[1] = points[index - 1];
      // texture 2
      faceString = faceString.substr(found + 1);
      found = faceString.find("/");
      index = stoi(faceString.substr(0,found));
      newTriangle.texturePoints[1] = texturePoints[index -1];
      // normal 2
      faceString = faceString.substr(found + 1);
      found = faceString.find(" ");
      index = stoi(faceString.substr(0,found));
      newTriangle.normals[1] = normals[index - 1];

      // point 3
      faceString = faceString.substr(found + 1);
      found = faceString.find("/");
      index = stoi(faceString.substr(0,found));
      newTriangle.vertices[2] = points[index - 1];
      // texture 3
      faceString = faceString.substr(found + 1);
      found = faceString.find("/");
      index = stoi(faceString.substr(0,found));
      newTriangle.texturePoints[2] = texturePoints[index -1];
      // normal 3
      faceString = faceString.substr(found + 1);
      found = faceString.find(" ");
      index = stoi(faceString.substr(0,found));
      newTriangle.normals[2] = normals[index - 1];

      newTriangle.shading = 'P';
      ModelTriangles.push_back(newTriangle);
    }
  }
  return ModelTriangles;
}

vector<ModelTriangle> readGeometryLogo(string filename, float  scalingFactor){

  std::ifstream ifs;
  ifs.open (filename, ifstream::in);
  assert (!ifs.fail( ));

  vector<ModelTriangle> ModelTriangles;
  vector<vec3> points;
  vector<vec3> normals;
  vector<vec2> texturePoints;

  Colour newColour;
  newColour.name = "Glass";
  newColour.red = 0;
  newColour.green = 255;
  newColour.blue = 0;

  while(!ifs.eof( )){
    char point[256];
    ifs.getline(point,256);
    if(point[0] == 'v' && point[1] == ' '){
      vec3 newPoint;
      string pointString(point);
      pointString = pointString.substr(2);

      size_t found = pointString.find(" ");
      newPoint.x = stof(pointString.substr(0,found))*scalingFactor;
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newPoint.y = stof(pointString.substr(0,found))*scalingFactor;
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newPoint.z = stof(pointString.substr(0,found))*scalingFactor;
      points.push_back(newPoint);
    }
    if(point[0] == 'v' && point[1] == 't'){
      vec2 newTexturePoint;
      string pointString(point);
      pointString = pointString.substr(3);

      size_t found = pointString.find(" ");
      newTexturePoint.x = stof(pointString.substr(0,found));
      pointString = pointString.substr(found+1);

      found = pointString.find(" ");
      newTexturePoint.y = stof(pointString.substr(0,found));
      texturePoints.push_back(newTexturePoint);
    }
    else if(point[0] == 'f'){
      ModelTriangle newTriangle;
      newTriangle.colour = newColour;
      // point 1
      string faceString(point);
      faceString = faceString.substr(2);
      size_t found = faceString.find("/");
      int index = stoi(faceString.substr(0,found));
      newTriangle.vertices[0] = points[index - 1];
      // texture 1
      faceString = faceString.substr(found + 1);
      found = faceString.find(" ");
      index = stoi(faceString.substr(0,found));
      newTriangle.texturePoints[0] = texturePoints[index -1];

      // point 2
      faceString = faceString.substr(found + 1);
      found = faceString.find("/");
      index = stoi(faceString.substr(0,found));
      newTriangle.vertices[1] = points[index - 1];
      // texture 2
      faceString = faceString.substr(found + 1);
      found = faceString.find(" ");
      index = stoi(faceString.substr(0,found));
      newTriangle.texturePoints[1] = texturePoints[index -1];

      // point 3
      faceString = faceString.substr(found + 1);
      found = faceString.find("/");
      index = stoi(faceString.substr(0,found));
      newTriangle.vertices[2] = points[index - 1];
      // texture 3
      faceString = faceString.substr(found + 1);
      found = faceString.find(" ");
      index = stoi(faceString.substr(0,found));
      newTriangle.texturePoints[2] = texturePoints[index -1];
      newTriangle.shading = 'N';
      newTriangle.textured = false;
      ModelTriangles.push_back(newTriangle);
    }
  }
  return ModelTriangles;
}

char*** readPPMPayload(string filename){
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

void writePPMFile(DrawingWindow window){
  ofstream myfile;
  myfile.open ("image.ppm");
  myfile << "P6\n";
  myfile << "# output from renderer\n";
  myfile << WIDTH << " " << HEIGHT << "\n";
  myfile << "255\n";

  for (int y = 0; y < HEIGHT; y++){
    for (int x = 0; x < WIDTH; x++){
      // uint32_t colour = window.getPixelColour(x,y);
      // // cout << colour << endl;
      // std::string binaryColour = std::bitset<32>(colour).to_string();
      // int add = 0;

      // char red = static_cast<char>(std::stoi(binaryColour.substr(8,8),0,2)+add);
      // char green = static_cast<char>(std::stoi(binaryColour.substr(16,8),0,2)+add);
      // char blue = static_cast<char>(std::stoi(binaryColour.substr(24),0,2)+add);
      // // cout << binaryColour << endl << binaryColour.substr(8,8) << endl << binaryColour.substr(16,8) << endl << binaryColour.substr(24) << endl;
      // myfile << red << green << blue;

      uint32_t colour = window.getPixelColour(x,y);
      uint32_t red = (colour >> 16) & 0xff;
      uint32_t green = (colour >> 8) & 0xff;
      uint32_t blue = colour & 0xff;
      char redChar = static_cast<char>(red);
      char greenChar = static_cast<char>(green);
      char blueChar = static_cast<char>(blue);
      myfile << redChar << greenChar << blueChar;
    }
  }
  myfile.close();
  cout << "Saved to image.ppm" << endl;
}

PPMImage readPPMImage(string filename){
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

  PPMImage image = PPMImage(height,width);

  char maxColStr[256];
  ifs.getline(maxColStr,256);

  char*** payload;
  payload = malloc3dArray(width,height,3);
  for (int y = 0; y < height; y++){
    for (int x = 0; x < width; x++){
      ifs.get(payload[x][y][0]);
      ifs.get(payload[x][y][1]);
      ifs.get(payload[x][y][2]);
      // cout << (float)payload[x][y][0] << "," << (float)payload[x][y][1] << "," << (float)payload[x][y][2] << endl;
    }
  }
  image.payload = payload;

  return image;
}

vector<PPMImage> readPlanetTextures(){
  vector<PPMImage> textures;

  PPMImage texture0 = readPPMImage("planetTextures/sun.ppm");
  PPMImage texture1 = readPPMImage("planetTextures/mercury.ppm");
  PPMImage texture2 = readPPMImage("planetTextures/venus.ppm");
  PPMImage texture3 = readPPMImage("planetTextures/earth.ppm");
  PPMImage texture4 = readPPMImage("planetTextures/mars.ppm");
  PPMImage texture5 = readPPMImage("planetTextures/jupiter.ppm");
  PPMImage texture6 = readPPMImage("planetTextures/saturn.ppm");
  PPMImage texture7 = readPPMImage("planetTextures/uranus.ppm");
  PPMImage texture8 = readPPMImage("planetTextures/venus.ppm");
  PPMImage texture9 = readPPMImage("planetTextures/stars3.ppm");

  textures.push_back(texture0);
  textures.push_back(texture1);
  textures.push_back(texture2);
  textures.push_back(texture3);
  textures.push_back(texture4);
  textures.push_back(texture5);
  textures.push_back(texture6);
  textures.push_back(texture7);
  textures.push_back(texture8);
  textures.push_back(texture9);

  return textures;
}
