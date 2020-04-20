#include <iostream>

class BoundingBox
{
  public:
    std::string name;
    glm::vec3 vertices[8];

    BoundingBox()
    {
    }

    BoundingBox(std::string n)
    {
      name = n;
      for (int i = 0; i < 8; i++){
          vertices[i] = glm::vec3(0,0,0);
      }
    }
};
