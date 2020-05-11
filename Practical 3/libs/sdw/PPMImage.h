#include <iostream>

class PPMImage
{
  public:
    int height;
    int width;
    char*** payload;
    glm::vec3 cameraPosition = glm::vec3(0,3,3);

    PPMImage()
    {
    }

    PPMImage(int h, int w)
    {
      height = h;
      width = w;
    }
};
