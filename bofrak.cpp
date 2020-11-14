#include "bofrak.hpp"
#include "utilities/imgui/imgui_impl_opengl3.h"
#include "utilities/imgui/imgui_impl_sdl.h"
#include <glm/gtc/matrix_transform.hpp>

#ifdef _WIN32
#  include <boost/filesystem/operations.hpp>
#  include <boost/filesystem/path.hpp>
#  include <shlobj.h>
#elif defined(__APPLE__)
#  include <CoreFoundation/CoreFoundation.h>
#  include <boost/algorithm/string/replace.hpp>
#endif

Bofrak::Bofrak() {
  this->sdlWindow = NULL;
  this->glContext = NULL;
  this->appIsRunning = false;
}

Bofrak::~Bofrak() {
  this->oCamera.reset();
  this->oFractal.reset();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_GL_DeleteContext(this->glContext);
  SDL_DestroyWindow(this->sdlWindow);
  this->sdlWindow = NULL;
  SDL_Quit();
}

int Bofrak::run() {
  if (!this->init())
    return 1;

  SDL_Event ev;
  int frameCounter = 1;

  while (this->appIsRunning) {
    float fts = (1.0 * std::clock() / CLOCKS_PER_SEC);

    if (Settings::Instance()->maybeGracefullApplicationQuit) {
      this->appIsRunning = false;
      break;
    }

    while (SDL_PollEvent(&ev)) {
      this->onEvent(&ev);
    }

    glViewport(0, 0, Settings::Instance()->SDL_DrawableSize_Width, Settings::Instance()->SDL_DrawableSize_Height);
    glClearColor(Settings::Instance()->guiClearColor.r, Settings::Instance()->guiClearColor.g, Settings::Instance()->guiClearColor.b, Settings::Instance()->guiClearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // rendering
    this->managerUI->renderStart(true);
    this->renderScene();
    this->managerUI->renderEnd();
    this->managerUI->renderPostEnd();

    SDL_GL_SwapWindow(this->sdlWindow);

    if (this->appIsRunning == false)
      break;

    if (Settings::Instance()->showFrameRenderTime) {
      if (frameCounter > ImGui::GetIO().Framerate) {
        float fte = (1.0 * std::clock() / CLOCKS_PER_SEC);
        Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[TIMINGS] FRAME draw time : %f ms (%f seconds)", (fte - fts) * 1000, (fte - fts)));
        frameCounter = 1;
      }
      else
        frameCounter += 1;
    }
  }

  return 0;
}

bool Bofrak::init() {
  bool success = true;

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    printf("Error: SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  }
  else {
    this->initFolders();

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); //4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, Settings::Instance()->OpenGL_MajorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, Settings::Instance()->OpenGL_MinorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef Def_Bofrak_DEBUG_BUILD
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");

    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    if (Settings::Instance()->SDL_Window_Height == 0)
      Settings::Instance()->SDL_Window_Height = current.h - 100;
    if (Settings::Instance()->SDL_Window_Width == 0)
      Settings::Instance()->SDL_Window_Width = current.w - 100;

    this->sdlWindow = SDL_CreateWindow(WINDOW_TITLE, WINDOW_POSITION_X, WINDOW_POSITION_Y, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, Settings::Instance()->SDL_Window_Flags | SDL_WINDOW_ALLOW_HIGHDPI);
    if (this->sdlWindow == nullptr) {
      printf("Error: Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    }
    else {
      this->glContext = SDL_GL_CreateContext(this->sdlWindow);
      if (!this->glContext) {
        printf("Error: Unable to create OpenGL context! SDL Error: %s\n", SDL_GetError());
        success = false;
      }
      else {
        if (SDL_GL_MakeCurrent(this->sdlWindow, this->glContext) < 0) {
          printf("Warning: Unable to set current context! SDL Error: %s\n", SDL_GetError());
          success = false;
        }
        else {
          if (SDL_GL_SetSwapInterval(1) < 0) {
            printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
            success = false;
          }
#ifdef _WIN32
          const GLenum glewInitCode = glewInit();
          if (glewInitCode != GLEW_OK) {
            printf("Cannot initialize GLEW.\n");
            exit(EXIT_FAILURE);
          }
#endif
          Settings::Instance()->setLogFunc(std::bind(&Bofrak::doLog, this, std::placeholders::_1));

          SDL_GL_GetDrawableSize(this->sdlWindow, &Settings::Instance()->SDL_DrawableSize_Width, &Settings::Instance()->SDL_DrawableSize_Height);

          this->doLog("[INIT] GL system initialized.");

          this->managerUI = std::make_unique<UIManager>();
          this->managerUI->init(this->sdlWindow, this->glContext, std::bind(&Bofrak::guiQuit, this));
          this->doLog("[INIT] UI initialized.");

          this->managerControls = std::make_unique<Controls>();
          this->managerControls->init(this->sdlWindow);
          this->doLog("[INIT] Input Control Manager initialized.");

          this->oCamera = std::make_unique<Camera>();
          this->oCamera->initProperties();
          this->doLog("[INIT] Camera initialized.");

          this->oFractal = std::make_unique<Fractal>();
          if (!this->oFractal->init())
            this->doLog("[INIT] Fractal object err'ed.");
          else
            this->doLog("[INIT] Fractal object initialized.");

          this->appIsRunning = true;
        }
      }
    }
  }
  return success;
}

void Bofrak::onEvent(SDL_Event* ev) {
  this->managerControls->processEvents(ev);
  this->appIsRunning = this->managerControls->appIsRunning;

  // window resize
  if (ev->type == SDL_WINDOWEVENT) {
    switch (ev->window.event) {
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      Settings::Instance()->SDL_Window_Width = static_cast<int>(ev->window.data1);
      Settings::Instance()->SDL_Window_Height = static_cast<int>(ev->window.data2);
      Settings::Instance()->saveSettings();
      break;
    }
  }
}

void Bofrak::initFolders() {
  std::string homeFolder(""), iniFolder("");
#ifdef _WIN32
  char const* hdrive = getenv("HOMEDRIVE");
  char const* hpath = getenv("HOMEPATH");
  homeFolder = std::string(hdrive) + std::string(hpath);

  TCHAR szPath[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath))) {
    std::string folderLocalAppData("");
#  ifndef UNICODE
    folderLocalAppData = szPath;
#  else
    std::wstring folderLocalAppDataW = szPath;
    folderLocalAppData = std::string(folderLocalAppDataW.begin(), folderLocalAppDataW.end());
#  endif
    //std::string folderLocalAppData(szPath);
    folderLocalAppData += "\\supudo.net";
    if (!boost::filesystem::exists(folderLocalAppData))
      boost::filesystem::create_directory(folderLocalAppData);
    folderLocalAppData += "\\Bofrak";
    if (!boost::filesystem::exists(folderLocalAppData))
      boost::filesystem::create_directory(folderLocalAppData);

    std::string current_folder = boost::filesystem::current_path().string() + "\\resources";
    std::string fName("");

    fName = "Bofrak_Settings.ini";
    std::string iniFileSource(current_folder + "\\" + fName);
    std::string iniFileDestination = folderLocalAppData + "\\" + fName;
    if (!boost::filesystem::exists(iniFileDestination))
      boost::filesystem::copy(iniFileSource, iniFileDestination);

    iniFolder = folderLocalAppData;
  }
  else
    iniFolder = homeFolder;
#elif defined macintosh // OS 9
  char const* hpath = getenv("HOME");
  homeFolder = std::string(hpath);
  iniFolder = homeFolder;
#else
  char const* hpath = getenv("HOME");
  if (hpath != NULL)
    homeFolder = std::string(hpath);
  iniFolder = homeFolder;
  CFURLRef appUrlRef;
  appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("Bofrak_Settings.ini"), NULL, NULL);
  CFStringRef filePathRef = CFURLCopyPath(appUrlRef);
  const char* filePath = CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8);
  iniFolder = std::string(filePath);
  homeFolder = std::string(filePath);
  boost::replace_all(iniFolder, "Bofrak_Settings.ini", "");
  boost::replace_all(homeFolder, "Bofrak_Settings.ini", "");
  CFRelease(filePathRef);
  CFRelease(appUrlRef);
#endif
  if (Settings::Instance()->ApplicationConfigurationFolder.empty())
    Settings::Instance()->ApplicationConfigurationFolder = iniFolder;
  if (Settings::Instance()->currentFolder.empty())
    Settings::Instance()->currentFolder = homeFolder;
  Settings::Instance()->initSettings(iniFolder);
}

void Bofrak::guiQuit() {
  this->appIsRunning = false;
}

void Bofrak::doLog(std::string const& logMessage) {
  //if (this->managerUI)
  //  this->managerUI->doLog(logMessage);
  printf("[Bofrak] %s\n", logMessage.c_str());
}

void Bofrak::renderScene() {
  this->matrixProjection = glm::perspective(glm::radians(this->Setting_FOV), this->Setting_RatioWidth / this->Setting_RatioHeight, this->Setting_PlaneClose, this->Setting_PlaneFar);
  this->oCamera->render();

  this->oFractal->render(this->matrixProjection, this->oCamera->matrixCamera, SDL_GetTicks(), int(ImGui::GetIO().MousePos.x), int(ImGui::GetIO().MousePos.y));

  //this->mmmm->render(this->matrixProjection, this->oCamera->matrixCamera);
}
