#include "camera.hpp"
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/matrix_decompose.hpp>

Camera::Camera() {}

Camera::~Camera() {
  this->eyeSettings.reset();

  this->positionX.reset();
  this->positionY.reset();
  this->positionZ.reset();

  this->rotateX.reset();
  this->rotateY.reset();
  this->rotateZ.reset();

  this->rotateCenterX.reset();
  this->rotateCenterY.reset();
  this->rotateCenterZ.reset();
}

void Camera::initProperties() {
  this->eyeSettings = std::make_unique<ObjectEye>();

  this->eyeSettings->View_Eye = glm::vec3(0, 0, 10);
  this->eyeSettings->View_Center = glm::vec3(0, 0, 0);
  this->eyeSettings->View_Up = glm::vec3(0, -1, 0);

  this->positionX = std::make_unique<ObjectCoordinate>(false, 0.0f);
  this->positionY = std::make_unique<ObjectCoordinate>(false, 0.0f);
  this->positionZ = std::make_unique<ObjectCoordinate>(false, -16.0f);

  this->rotateX = std::make_unique<ObjectCoordinate>(false, 160.0f);
  this->rotateY = std::make_unique<ObjectCoordinate>(false, 140.0f);
  this->rotateZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

  this->rotateCenterX = std::make_unique<ObjectCoordinate>(false, 0.0f);
  this->rotateCenterY = std::make_unique<ObjectCoordinate>(false, 0.0f);
  this->rotateCenterZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

  this->matrixCamera = glm::mat4(1.0);
}

void Camera::render() {
  this->matrixCamera = glm::lookAt(this->eyeSettings->View_Eye, this->eyeSettings->View_Center, this->eyeSettings->View_Up);

  this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(this->positionX->point, this->positionY->point, this->positionZ->point));

  this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(0, 0, 0));
  this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateX->point), glm::vec3(1, 0, 0));
  this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateY->point), glm::vec3(0, 1, 0));
  this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateZ->point), glm::vec3(0, 0, 1));
  this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(0, 0, 0));

  this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateCenterX->point), glm::vec3(1, 0, 0));
  this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateCenterY->point), glm::vec3(0, 1, 0));
  this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateCenterZ->point), glm::vec3(0, 0, 1));

  this->cameraPosition = glm::vec3(this->matrixCamera[3].x, this->matrixCamera[3].y, this->matrixCamera[3].z);
}
