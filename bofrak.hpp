#ifndef Bofrak_hpp
#define Bofrak_hpp

#include <thread>
#include <glm/glm.hpp>
#include "utilities/gl/glincludes.hpp"
#include "utilities/input/controls.hpp"
#include "settings/settings.hpp"
#include "objects/camera.hpp"
#include "ui/uimanager.hpp"
#include "objects/fractal.hpp"

class Bofrak {
public:
	Bofrak();
  ~Bofrak();
  int run();

private:
  bool init();
  void onEvent(SDL_Event* ev);
  void doLog(std::string const& logMessage);
  void initFolders();
  void renderScene();
  void guiQuit();

  // Screen dimension constants
  const char *WINDOW_TITLE = "Bofrak";
  const unsigned int WINDOW_POSITION_X = SDL_WINDOWPOS_CENTERED;
  const unsigned int WINDOW_POSITION_Y = SDL_WINDOWPOS_CENTERED;

  // SDLs
  SDL_Window *sdlWindow = NULL;
  SDL_GLContext glContext;

  // Variables
  bool appIsRunning = false;

  // Customs
  std::unique_ptr<Controls> managerControls;
  std::unique_ptr<UIManager> managerUI;

  // 3d
  float Setting_FOV = 45.0;
  float Setting_RatioWidth = 4.0f, Setting_RatioHeight = 3.0f;
  float Setting_PlaneClose = 0.1f, Setting_PlaneFar = 100.0f;
  glm::mat4 matrixProjection;

  // objects
  std::unique_ptr<Camera> oCamera;
  std::unique_ptr<Fractal> oFractal;
};

#endif /* Bofrak_hpp */
