#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <string>
#include "settings/settings.hpp"
#include "objects/definitions.hpp"

class Camera {
public:
  ~Camera();
  Camera();
  void initProperties();
  void render();

  std::unique_ptr<ObjectEye> eyeSettings;
  std::unique_ptr<ObjectCoordinate> positionX, positionY, positionZ;
  std::unique_ptr<ObjectCoordinate> rotateX, rotateY, rotateZ;
  std::unique_ptr<ObjectCoordinate> rotateCenterX, rotateCenterY, rotateCenterZ;

  glm::vec3 cameraPosition;
  glm::mat4 matrixCamera;
};

#endif /* Camera_hpp */
