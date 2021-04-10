#include "objects/fractal.hpp"
#include <vector>
#include "settings/settings.hpp"

//#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "utilities/stb/stb_image.h"

Fractal::Fractal() {
}

Fractal::~Fractal() {
  glDeleteBuffers(1, &this->vboVertices);
  glDeleteBuffers(1, &this->vboTexture);
  glDeleteVertexArrays(1, &this->glVAO);
  glDeleteProgram(this->shaderProgram);
}

bool Fractal::init() {
  this->shaderProgram = glCreateProgram();

  this->currentFractal = Settings::Instance()->SelectedFractalID;

  GLuint shaderVertex, shaderFragment;

  std::string fractalFilename("");
  switch (this->currentFractal) {
    case 0:
      fractalFilename = "out_mandelbrot";
      break;
    case 1:
      fractalFilename = "mandelbrot";
      break;
    case 2:
      fractalFilename = "julia";
      break;
    case 3:
      fractalFilename = "mandelbulb";
      break;
    case 4:
      fractalFilename = "triflake";
      break;
    case 5:
      fractalFilename = "fractal_pyramid";
      break;
    case 6:
      fractalFilename = "dodecahedron";
      break;
    case 7:
      fractalFilename = "jerusalem_cube";
      break;
    default:
      fractalFilename = "out_mandelbrot";
      break;
  }

  std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/" + fractalFilename + ".vs";
  std::string shaderSourceVertex = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_vertex = shaderSourceVertex.c_str();

  shaderPath = Settings::Instance()->appFolder() + "/shaders/" + fractalFilename + ".fs";
  std::string shaderSourceFragment = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_fragment = shaderSourceFragment.c_str();

  bool shaderCompilation = true;
  shaderCompilation &= Settings::Instance()->glUtils->compileAndAttachShader(this->shaderProgram, shaderVertex, GL_VERTEX_SHADER, shader_vertex);
  shaderCompilation &= Settings::Instance()->glUtils->compileAndAttachShader(this->shaderProgram, shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

  if (!shaderCompilation)
    return false;

  glLinkProgram(this->shaderProgram);

  GLint programSuccess = GL_TRUE;
  glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
  if (programSuccess != GL_TRUE) {
    Settings::Instance()->funcDoLog("[Fractal] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
    Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
    return false;
  }
  else {
    this->svTicks = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "vvTicks");
    this->svResolution = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "vvResolution");
    this->svCameraPosition = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "vvCameraPosition");
    this->svMouse = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "vvMouse");
    this->svTexture01 = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "fvTexture01");
  }

  glGenVertexArrays(1, &this->glVAO);
  glBindVertexArray(this->glVAO);

  const GLfloat vertices[] = {
    -1.00,  1.00, 0.00,
     1.00,  1.00, 0.00,
    -1.00, -1.00, 0.00,
     1.00, -1.00, 0.00
  };

  const GLfloat texcoords[] = {
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.0
  };

  // vertices
  glGenBuffers(1, &this->vboVertices);
  glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

  glGenBuffers(1, &this->vboTexture);
  glBindBuffer(GL_ARRAY_BUFFER, this->vboTexture);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), &texcoords[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

  addTexture(Settings::Instance()->appFolder() + "/textures/pal.png", &this->svTexture01, 0);

  glDeleteShader(shaderVertex);
  glDeleteShader(shaderFragment);

  glBindVertexArray(0);

  Settings::Instance()->glUtils->CheckForGLErrors(Settings::Instance()->string_format("%s - %s", __FILE__, __func__));

  return true;
}

void Fractal::addTexture(std::string const& textureImage, GLuint* vboTexture, const int textureID) {
  int tWidth, tHeight, tChannels;
  unsigned char* tPixels = stbi_load(textureImage.c_str(), &tWidth, &tHeight, &tChannels, 0);
  if (tPixels) {
    glGenTextures(1, vboTexture);
    glBindTexture(GL_TEXTURE_2D, *vboTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   /*
	  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    */

    GLenum textureFormat = 0;
    switch (tChannels) {
      case 1:
        textureFormat = GL_LUMINANCE;
        break;
      case 2:
        textureFormat = GL_LUMINANCE_ALPHA;
        break;
      case 3:
        textureFormat = GL_RGB;
        break;
      case 4:
        textureFormat = GL_RGBA;
        break;
      default:
        textureFormat = GL_RGB;
        break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(textureFormat), tWidth, tHeight, 0, textureFormat, GL_UNSIGNED_BYTE, tPixels);
    stbi_image_free(tPixels);
  }

  Settings::Instance()->glUtils->CheckForGLErrors(Settings::Instance()->string_format("%s - %s", __FILE__, __func__));
}

void Fractal::render(glm::mat4 mtxProject, glm::mat4 mtxCamera, unsigned const int ticks, unsigned const int mouseX, unsigned const int mouseY) {
  if (this->currentFractal != Settings::Instance()->SelectedFractalID)
    this->init();
  if (this->glVAO > 0) {
    glUseProgram(this->shaderProgram);

    glUniform1f(this->svTicks, ticks);
    glUniform2f(this->svResolution, Settings::Instance()->SDL_DrawableSize_Width, Settings::Instance()->SDL_DrawableSize_Height);
    glUniform3f(this->svCameraPosition, mtxCamera[3][0], mtxCamera[3][1], mtxCamera[3][2]);
    glUniform4f(this->svMouse, mouseX, mouseY, 0, 0);

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->svTexture01);

    //Settings::Instance()->glUtils->dumpTexture("out.png", &this->svTexture01);

    // draw
    glBindVertexArray(this->glVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);
  }
}
