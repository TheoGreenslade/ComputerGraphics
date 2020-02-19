#include <iostream>

class TexturePoint
{
  public:
    float x;
    float y;

    TexturePoint()
    {
    }

    TexturePoint(float xPos, float yPos)
    {
      x = xPos;
      y = yPos;
    }

    void print()
    {
    }
};

std::ostream& operator<<(std::ostream& os, const TexturePoint& point)
{
    os << "(" << point.x << ", " << point.y << ")" << std::endl;
    return os;
}
