#ifndef Fractal_hpp
#define Fractal_hpp

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengl.h>
#include "objects/definitions.hpp"

class Fractal {
  public:
    Fractal();
    virtual ~Fractal();
    bool init();
    void render(glm::mat4 mtxProject, glm::mat4 mtxCamera, unsigned const int ticks, unsigned const int mouseX, unsigned const int mouseY);

  private:
    GLuint shaderProgram, glVAO, vboVertices, vboTexture;
    GLuint svTicks, svResolution, svCameraPosition, svMouse;
    GLuint svTexture01;

    void addTexture(std::string const& textureImage, GLuint* vboTexture, const int textureID);
};

#endif // Fractal_hpp
