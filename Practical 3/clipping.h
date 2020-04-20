#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

vector<ModelTriangle> clipTriangles(vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation,int distanceOfImagePlaneFromCamera);
vector<BoundingBox> createBoundingBoxes(vector<ModelTriangle> triangles);
vector<BoundingBox> adjustBoundingBox(vector<BoundingBox> boxes, ModelTriangle triangle, int i);
BoundingBox createNewBoundingBox(ModelTriangle triangle, string name);
bool isBoundingBoxVisible(BoundingBox box, vec3 cameraPosition, mat3x3 cameraRotation,int distanceOfImagePlaneFromCamera);
vector<ModelTriangle> removeTrianglesInBox(BoundingBox box, vector<ModelTriangle> triangles);
bool isVertexVisible(vec3 vertex, vec3 cameraPosition, mat3x3 cameraRotation, int distanceOfImagePlaneFromCamera);

vector<ModelTriangle> clipTriangles(vector<ModelTriangle> triangles, vec3 cameraPosition, mat3x3 cameraRotation,int distanceOfImagePlaneFromCamera){
    vector<BoundingBox> boxes = createBoundingBoxes(triangles);
    int numOfBoxes = boxes.size();
    for (int i = 0; i < numOfBoxes; i++){
        BoundingBox box = boxes[i];
        if(not(isBoundingBoxVisible(box,cameraPosition,cameraRotation,distanceOfImagePlaneFromCamera))){
            triangles = removeTrianglesInBox(box,triangles);
        }
    }
    return triangles;
}

vector<BoundingBox> createBoundingBoxes(vector<ModelTriangle> triangles){
    vector<BoundingBox> boxes;
    int numOfTris = triangles.size();
    for (int i = 0; i < numOfTris; i++){
        ModelTriangle triangle = triangles[i];
        string name = triangle.colour.name;
        int numOfBoxes = boxes.size();
        bool boxFound = false;
        for (int j = 0; j < numOfBoxes; j++){
            if (boxes[j].name == name){
                boxes = adjustBoundingBox(boxes,triangle, j);
                boxFound = true;
            }
        }
        if (not(boxFound)){
            BoundingBox newBox = createNewBoundingBox(triangle,name);
            boxes.push_back(newBox);
        }
    }
    return boxes;
}

vector<BoundingBox> adjustBoundingBox(vector<BoundingBox> boxes, ModelTriangle triangle, int j){
    for(int i = 0; i < 3; i++){
        if (triangle.vertices[i].x < boxes[j].vertices[0].x){
            boxes[j].vertices[0].x = triangle.vertices[i].x;
            boxes[j].vertices[3].x = triangle.vertices[i].x;
            boxes[j].vertices[4].x = triangle.vertices[i].x;
            boxes[j].vertices[7].x = triangle.vertices[i].x;
        }
        if (triangle.vertices[i].x > boxes[j].vertices[1].x){
            boxes[j].vertices[1].x = triangle.vertices[i].x;
            boxes[j].vertices[2].x = triangle.vertices[i].x;
            boxes[j].vertices[5].x = triangle.vertices[i].x;
            boxes[j].vertices[6].x = triangle.vertices[i].x;
        }
        if (triangle.vertices[i].y < boxes[j].vertices[2].y){
            boxes[j].vertices[2].y = triangle.vertices[i].y;
            boxes[j].vertices[3].y = triangle.vertices[i].y;
            boxes[j].vertices[6].y = triangle.vertices[i].y;
            boxes[j].vertices[7].y = triangle.vertices[i].y;
        }
        if (triangle.vertices[i].y > boxes[j].vertices[0].y){
            boxes[j].vertices[0].y = triangle.vertices[i].y;
            boxes[j].vertices[1].y = triangle.vertices[i].y;
            boxes[j].vertices[4].y = triangle.vertices[i].y;
            boxes[j].vertices[5].y = triangle.vertices[i].y;
        }
        if (triangle.vertices[i].z < boxes[j].vertices[4].z){
            boxes[j].vertices[4].z = triangle.vertices[i].z;
            boxes[j].vertices[5].z = triangle.vertices[i].z;
            boxes[j].vertices[6].z = triangle.vertices[i].z;
            boxes[j].vertices[7].z = triangle.vertices[i].z;
        }
        if (triangle.vertices[i].z > boxes[j].vertices[0].z){
            boxes[j].vertices[0].z = triangle.vertices[i].z;
            boxes[j].vertices[1].z = triangle.vertices[i].z;
            boxes[j].vertices[2].z = triangle.vertices[i].z;
            boxes[j].vertices[3].z = triangle.vertices[i].z;
        }
    }
    return boxes;
}

BoundingBox createNewBoundingBox(ModelTriangle triangle, string name){
    BoundingBox newBox = BoundingBox(name);
    float biggestX = std::max(std::max(triangle.vertices[0].x,triangle.vertices[1].x),triangle.vertices[2].x);
    float biggestY = std::max(std::max(triangle.vertices[0].y,triangle.vertices[1].y),triangle.vertices[2].y);
    float biggestZ = std::max(std::max(triangle.vertices[0].z,triangle.vertices[1].z),triangle.vertices[2].z);
    float smallestX = std::min(std::min(triangle.vertices[0].x,triangle.vertices[1].x),triangle.vertices[2].x);
    float smallestY = std::min(std::min(triangle.vertices[0].y,triangle.vertices[1].y),triangle.vertices[2].y);
    float smallestZ = std::min(std::min(triangle.vertices[0].z,triangle.vertices[1].z),triangle.vertices[2].z);

    newBox.vertices[0] = vec3(smallestX,biggestY,biggestZ);
    newBox.vertices[1] = vec3(biggestX,biggestY,biggestZ);
    newBox.vertices[2] = vec3(biggestX,smallestY,biggestZ);
    newBox.vertices[3] = vec3(smallestX,smallestY,biggestZ);
    newBox.vertices[4] = vec3(smallestX,biggestY,smallestZ);
    newBox.vertices[5] = vec3(biggestX,biggestY,smallestZ);
    newBox.vertices[6] = vec3(biggestX,smallestY,smallestZ);
    newBox.vertices[7] = vec3(smallestX,smallestY,smallestZ);

    return newBox;
}

bool isBoundingBoxVisible(BoundingBox box, vec3 cameraPosition, mat3x3 cameraRotation,int distanceOfImagePlaneFromCamera){
    bool visible = false;
    for (int i = 0; i < 8; i++){
        if (isVertexVisible(box.vertices[i],cameraPosition,cameraRotation,distanceOfImagePlaneFromCamera)){
            visible = true;
        }
    }
    return visible;
}

vector<ModelTriangle> removeTrianglesInBox(BoundingBox box,vector<ModelTriangle> triangles){
    vector<ModelTriangle> visibleTriangles;
    int numOfTris = triangles.size();
    for (int i = 0; i < numOfTris; i++){
        if (not(triangles[i].colour.name == box.name)){
            visibleTriangles.push_back(triangles[i]);
        }
    }
    return visibleTriangles;
}

bool isVertexVisible(vec3 vertex, vec3 cameraPosition, mat3x3 cameraRotation, int distanceOfImagePlaneFromCamera){
  float bigZ = vertex.z - cameraPosition.z;
  float smallZ = distanceOfImagePlaneFromCamera;
  float bigX = vertex.x - cameraPosition.x;
  float bigY = vertex.y - cameraPosition.y;
  vec3 p = vec3(bigX, bigY, bigZ);
  vec3 newP = rotatePoint(cameraRotation, p);
  float proportion = smallZ/newP.z;
  float littleX = newP.x * proportion;
  float littleY = newP.y * proportion;

  float x = -littleX + WIDTH/2;
  float y = littleY + HEIGHT/2;

  if((x < WIDTH-1) && (y < HEIGHT-1) && (x >= 0) && (y >= 0)){
      return true;
  } else{
      return false;
  }
}