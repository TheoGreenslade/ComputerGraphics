#define WIDTH 600
#define HEIGHT 600

using namespace std;
using namespace glm;

vector<float> velocities;
vector<float> CORs;
float timeSpeed = 0.1;

vector<ModelTriangle> gravity(vector<ModelTriangle> triangles,vector<float> velocities);
void initialiseVelocities(vector<Colour> materials);
bool collision(vector<ModelTriangle> triangles, Colour material);
float bounce(int);
void updateVelocities(vector<ModelTriangle> triangles, vector<Colour> materials);
vector<ModelTriangle> liftCubes(vector<ModelTriangle> triangles);
void initialiseCORs(vector<Colour> materials);
vector<ModelTriangle> moveCubesAboveFloor(vector<ModelTriangle> triangles, vector<Colour> materials);

vector<ModelTriangle> gravity(vector<ModelTriangle> triangles,vector<float> velocities, vector<Colour> materials) {
    int numOfTri = triangles.size();
    vector<ModelTriangle> newTriangles;
    for (int i = 0; i < numOfTri; i++){
        ModelTriangle triangle = triangles[i];

        if((triangle.colour.name == "Red") || (triangle.colour.name == "Blue")){
            int index = 0;
            int numOfMaterials = materials.size();
            for (int k = 0; k < numOfMaterials; k++){
                if (triangle.colour.name == materials[k].name){
                    index = k;
                }
            }

            float velocity = velocities[index];
            float displacement = (velocity*timeSpeed);
            for (int j = 0; j < 3; j++){
                triangle.vertices[j].y = triangle.vertices[j].y + displacement;
            } 
        }
        newTriangles.push_back(triangle);
    }
    newTriangles = moveCubesAboveFloor(newTriangles,materials);
    updateVelocities(newTriangles,materials);
    return newTriangles;
}

void initialiseVelocities(vector<Colour> materials) {
    vector<float> initialVelocities;
    int numOfMaterials = materials.size();
    for (int i = 0; i < numOfMaterials; i++){
        Colour material = materials[i];
        float velocity = 1;
        if((material.name != "Red") && (material.name != "Blue")){
            velocity = std::numeric_limits<float>::infinity();
        }
        initialVelocities.push_back(velocity);
    }
    velocities = initialVelocities;
}

bool collision(vector<ModelTriangle> triangles, Colour material){
    int numOfTri = triangles.size();
    for (int i = 0; i < numOfTri; i++){
        if (triangles[i].colour.name == material.name){
            for (int j = 0; j < 3; j++){
                if (triangles[i].vertices[j].y <= -0.162686){
                    return true;
                }
            }    
        }
    }
    return false;
}

float bounce(int index){
    float newVelocity = - (velocities[index] * CORs[index]);
    if (newVelocity > 0.7) {
       return newVelocity; 
    } else {
        return 0;
    }
    
}

void updateVelocities(vector<ModelTriangle> triangles, vector<Colour> materials){
    int numOfMaterials = materials.size();
    for (int i = 0; i < numOfMaterials; i++){
        if((materials[i].name == "Red") || (materials[i].name == "Blue")){
            float newVelocity;
            if (collision(triangles, materials[i])){
                    newVelocity = bounce(i);
            } else {
                newVelocity = velocities[i] - (9.8 * timeSpeed);
            }
            velocities[i] = newVelocity; 
        }
    }
}

vector<ModelTriangle> liftCubes(vector<ModelTriangle> triangles){
    int numOfTri = triangles.size();
    for (int i = 0; i < numOfTri; i++){
        if((triangles[i].colour.name == "Red") || (triangles[i].colour.name == "Blue")){
            for (int j = 0; j < 3; j++){
                triangles[i].vertices[j].y = triangles[i].vertices[j].y + 2;
            }
        }
    }
    return triangles;
}

void initialiseCORs(vector<Colour> materials) {
    vector<float> initialCORs;
    int numOfMaterials = materials.size();
    for (int i = 0; i < numOfMaterials; i++){
        Colour material = materials[i];
        float COR = 0.5;
        if((material.name != "Red") && (material.name != "Blue")){
            COR = std::numeric_limits<float>::infinity();
        }
        initialCORs.push_back(COR);
    }
    CORs = initialCORs;
}

vector<ModelTriangle> moveCubesAboveFloor(vector<ModelTriangle> triangles, vector<Colour> materials) {
    int numOfTri = triangles.size();
    int numOfMaterials = materials.size();
    for (int j = 0; j < numOfMaterials; j++){
        float distanceBelowFloor = 0;
        if ((materials[j].name == "Red") || (materials[j].name == "Blue")){
            for (int i = 0; i < numOfTri; i++){
                if (triangles[i].colour.name == materials[j].name){
                    for (int k = 0; k < 3; k++){
                        if (triangles[i].vertices[k].y < -0.162686){
                            if (triangles[i].vertices[k].y < distanceBelowFloor){
                                distanceBelowFloor = triangles[i].vertices[k].y + 0.162686;
                            }
                        }
                    }    
                }
            }

            if (distanceBelowFloor < 0 ){
                for (int i = 0; i < numOfTri; i++){
                    if (triangles[i].colour.name == materials[j].name){
                        for (int k = 0; k < 3; k++){
                            triangles[i].vertices[k].y = triangles[i].vertices[k].y - distanceBelowFloor;
                        }
                    }
                }
            }
        }
    }
    return triangles;
}