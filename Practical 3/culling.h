#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

vector<ModelTriangle> cullTriangles(vector<ModelTriangle> triangles, vec3 cameraPosition);
bool backFacing(ModelTriangle triangle, vec3 cameraPosition);
bool farPlane(ModelTriangle triangle, vec3 cameraPosition);
bool nearPlane(ModelTriangle triangle, vec3 cameraPosition);

vector<ModelTriangle> cullTriangles(vector<ModelTriangle> triangles, vec3 cameraPosition){
    vector<ModelTriangle> visibleTriangles;
    int numOfTri = triangles.size();
    for (int i = 0; i < numOfTri; i++){
        ModelTriangle triangle = triangles[i];
        if(!(backFacing(triangle, cameraPosition)) && !(farPlane(triangle,cameraPosition)) && !(nearPlane(triangle, cameraPosition))){
            visibleTriangles.push_back(triangle);
        }
    }
    return visibleTriangles;
}

bool backFacing(ModelTriangle triangle, vec3 cameraPosition){
    vec3 normal = normalize(cross(triangle.vertices[1] - triangle.vertices[0], triangle.vertices[2] - triangle.vertices[0]));
    vec3 vecToCamera = normalize(cameraPosition - triangle.vertices[0]);
    float angleOfIncidence = dot(normal, vecToCamera);
    if (angleOfIncidence >= 0){
        return false;
    } else {
        return true;
    }
}

bool farPlane(ModelTriangle triangle, vec3 cameraPosition){
    for (int i = 0; i < 3; i++){
        vec3 vecToVertex = cameraPosition - triangle.vertices[i];
        float distanceFromCamera = sqrt(pow(vecToVertex.x,2) + pow(vecToVertex.y,2) + pow(vecToVertex.z,2));
        if (distanceFromCamera < 200) return false;
    }
    return true;
}

bool nearPlane(ModelTriangle triangle, vec3 cameraPosition){
    for (int i = 0; i < 3; i++){
        vec3 vecToVertex = cameraPosition - triangle.vertices[i];
        float distanceFromCamera = sqrt(pow(vecToVertex.x,2) + pow(vecToVertex.y,2) + pow(vecToVertex.z,2));
        if (distanceFromCamera > 1) return false;
    }
    return true;
}