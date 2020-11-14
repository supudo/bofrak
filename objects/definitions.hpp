#ifndef ObjectDefinitions_hpp
#define ObjectDefinitions_hpp

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct ObjectCoordinate {
  bool animate;
  float point;
  ObjectCoordinate() : animate(false), point(0.0f) { }
  ObjectCoordinate(bool a, float p) : animate(a), point(p) { }
};

struct ObjectEye {
  glm::vec3 View_Eye;
  glm::vec3 View_Center;
  glm::vec3 View_Up;
};

class Vertex {
  public:
    Vertex(const glm::vec3& pos, const glm::vec2& texCoord) {
      this->pos = pos;
      this->texCoord = texCoord;
    }

    Vertex() {
      this->pos = glm::vec3(0, 0, 0);
      this->texCoord = glm::vec2(0, 0);
    }
    inline glm::vec3* GetPos() { return &pos; }
    inline glm::vec2* GetTextCoord() { return &texCoord; }
  private:
    glm::vec3 pos;
    glm::vec2 texCoord;
};

#endif /* ObjectDefinitions_hpp */
