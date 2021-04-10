#ifndef UIManager_hpp
#define UIManager_hpp

#include "settings/settings.hpp"
#include "utilities/gl/glincludes.hpp"

#include "utilities/imgui/imgui.h"

class UIManager {
public:
    explicit UIManager();
    ~UIManager();
    void init(SDL_Window *window, SDL_GLContext glContext, const std::function<void()>& quitApp);

    void renderStart(bool isFrame);
    void renderEnd();
    void renderPostEnd() const;

    void doLog(const std::string& message);
    void showLoading();
    void hideLoading();

    bool isFrame, isLoadingOpen;

    int selectedFractal;

private:
    SDL_Window *sdlWindow;
    std::function<void()> funcQuitApp;

    void dialogAboutImGui();
    void dialogAboutBofrak();
    void dialogFractals();

    void loadCustomFonts();

    bool showDemoWindow;
    bool showAboutImgui;
    bool showAboutBofrak;
    bool showDialogFractals;
};

#endif /* UIManager_hpp */
