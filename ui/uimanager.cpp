#include "uimanager.hpp"
#include "iconfonts/iconsfontawesome.hpp"
#include "iconfonts/iconsmaterialdesign.hpp"
#include "utilities/imgui/imgui_impl_opengl3.h"
#include "utilities/imgui/imgui_impl_sdl.h"

UIManager::UIManager() {
  this->isFrame = false;
  this->showAboutImgui = false;
  this->showAboutBofrak = false;
}

UIManager::~UIManager() {
}

void UIManager::init(SDL_Window* window, SDL_GLContext glContext, const std::function<void()>& quitApp) {
  this->sdlWindow = window;
  this->funcQuitApp = quitApp;

  this->isFrame = false;
  this->isLoadingOpen = false;
  this->showAboutImgui = false;
  this->showAboutBofrak = false;
  this->showDialogFractals = true;

  int windowWidth, windowHeight;
  SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
  int posX = 50, posY = 50;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  ImGui_ImplSDL2_InitForOpenGL(this->sdlWindow, glContext);
  ImGui_ImplOpenGL3_Init("#version 410 core");
  ImGui::StyleColorsDark();

  this->loadCustomFonts();
}

void UIManager::doLog(const std::string& message) {
  printf("%s\n", message.c_str());
}

void UIManager::renderStart(bool isFrame) {
  this->isFrame = isFrame;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(this->sdlWindow);
  ImGui::NewFrame();

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      ImGui::Separator();
#ifdef _WIN32
      if (ImGui::MenuItem(ICON_FA_POWER_OFF " Quit", "Alt+F4"))
#else
      if (ImGui::MenuItem(ICON_FA_POWER_OFF " Quit", "Cmd+Q"))
#endif
        this->funcQuitApp();
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem(ICON_FA_BARCODE " Fractals", NULL, &this->showDialogFractals);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      ImGui::MenuItem(ICON_FA_INFO_CIRCLE " About ImGui", NULL, &this->showAboutImgui);
      ImGui::MenuItem(ICON_FA_INFO_CIRCLE " About Bofrak", NULL, &this->showAboutBofrak);
      ImGui::Separator();
      ImGui::MenuItem("   ImGui Demo Window", NULL, &this->showDemoWindow);
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (this->showAboutImgui)
    this->dialogAboutImGui();

  if (this->showAboutBofrak)
    this->dialogAboutBofrak();

  if (this->isLoadingOpen)
    ImGui::OpenPopup("Bofrak Loading");
  if (ImGui::BeginPopupModal("Bofrak Loading", &this->isLoadingOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
    ImGui::Text("Calculating ... \n");
    ImGui::EndPopup();
  }

  if (this->showDemoWindow)
    ImGui::ShowDemoWindow();

  if (this->showDialogFractals)
    this->dialogFractals();
}

void UIManager::renderEnd() {
  ImGui::Render();
}

void UIManager::renderPostEnd() const {
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::showLoading() {
  this->isLoadingOpen = true;
}

void UIManager::hideLoading() {
  this->isLoadingOpen = false;
}

void UIManager::dialogAboutImGui() {
  ImGui::SetNextWindowPos(ImVec2(Settings::Instance()->SDL_Window_Width / 2, Settings::Instance()->SDL_Window_Height / 2), 0, ImVec2(0.5, 0.5));
  ImGui::Begin("About ImGui", &this->showAboutImgui, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("ImGui %s", ImGui::GetVersion());
  ImGui::Separator();
  ImGui::Text("By Omar Cornut and all github contributors.");
  ImGui::Text("ImGui is licensed under the MIT License, see LICENSE for more information.");
  ImGui::End();
}

void UIManager::dialogAboutBofrak () {
  ImGui::SetNextWindowPos(ImVec2(Settings::Instance()->SDL_Window_Width / 2, Settings::Instance()->SDL_Window_Height / 2), 0, ImVec2(0.5, 0.5));
  ImGui::Begin("About Bofrak", &this->showAboutBofrak, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("Bofrak %s", Settings::Instance()->appVersion.c_str());
  ImGui::Separator();
  ImGui::Text("By supudo.net + github.com/supudo");
  ImGui::Text("Whatever license...");
  ImGui::End();
}

void UIManager::loadCustomFonts() {
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->Clear();
  io.Fonts->AddFontDefault();

  // http://fortawesome.github.io/Font-Awesome/icons/
  std::string faFont = Settings::Instance()->appFolder() + "/fonts/fontawesome-webfont.ttf";
  static const ImWchar fa_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  ImFontConfig fa_config;
  fa_config.MergeMode = true;
  fa_config.PixelSnapH = true;
  io.Fonts->AddFontFromFileTTF(faFont.c_str(), Settings::Instance()->UIFontSize, &fa_config, fa_ranges);

  // https://design.google.com/icons/
  std::string gmFont = Settings::Instance()->appFolder() + "/fonts/material-icons-regular.ttf";
  static const ImWchar gm_ranges[] = {ICON_MIN_MD, ICON_MAX_MD, 0};
  ImFontConfig gm_config;
  gm_config.MergeMode = true;
  gm_config.PixelSnapH = true;
  io.Fonts->AddFontFromFileTTF(gmFont.c_str(), Settings::Instance()->UIFontSize + 8.00f, &gm_config, gm_ranges);
}

void UIManager::dialogFractals() {
  ImGui::SetNextWindowSize(ImVec2(500, 700), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(20, 28), ImGuiCond_FirstUseEver);
  ImGui::Begin("Fractals", &this->showDialogFractals);

  std::vector<const char*> fractals;
  fractals.push_back("[OUT] Mandelbrot");
  fractals.push_back("Mandelbrot");
  fractals.push_back("Julia");
  fractals.push_back("Mandelbulb");
  fractals.push_back("Triflake");
  fractals.push_back("Fractal Pyramid");
  fractals.push_back("Dodecahedron fractal");
  fractals.push_back("Jerusalem cube");
  ImGui::Combo("##1", &Settings::Instance()->SelectedFractalID, &fractals[0], int(fractals.size()));

  ImGui::End();
}
