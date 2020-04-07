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