#include "settings.hpp"
#include <SDL2/SDL.h>
#ifdef _WIN32
#undef main
#include <boost/filesystem.hpp>
#include <iostream>
#elif __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#else
#include <boost/filesystem.hpp>
#include <iostream>
#endif
#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/importerdesc.h>
#include <boost/algorithm/string.hpp>
#include <memory>
#include <stdarg.h>

Settings* Settings::m_pInstance = NULL;

Settings* Settings::Instance() {
  if (!m_pInstance) {
    m_pInstance = new Settings;
    //m_pInstance->initSettings();
  }
  return m_pInstance;
}

void Settings::initSettings(const std::string& iniFolder) {
#ifdef Def_Bofrak_OpenGL_4x
  m_pInstance->OpenGL_MajorVersion = 4;
  m_pInstance->OpenGL_MinorVersion = 1;
#else
  m_pInstance->OpenGL_MajorVersion = 2;
  m_pInstance->OpenGL_MinorVersion = 1;
#endif

  m_pInstance->cfgUtils = std::make_unique<ConfigUtils>();
  m_pInstance->cfgUtils->init(iniFolder);

  m_pInstance->UIFontSize = 14.0f;

  m_pInstance->appVersion = m_pInstance->cfgUtils->readString("appVersion");
  m_pInstance->shouldRecompileShaders = m_pInstance->cfgUtils->readBool("shouldRecompileShaders");
  m_pInstance->showGLErrors = m_pInstance->cfgUtils->readBool("showGLErrors");
  m_pInstance->showFrameRenderTime = m_pInstance->cfgUtils->readBool("showFrameRenderTime");
  m_pInstance->SDL_Window_Width = m_pInstance->cfgUtils->readInt("SDL_Window_Width");
  m_pInstance->SDL_Window_Height = m_pInstance->cfgUtils->readInt("SDL_Window_Height");
  m_pInstance->SDL_DrawableSize_Width = m_pInstance->cfgUtils->readInt("SDL_DrawableSize_Width");
  m_pInstance->SDL_DrawableSize_Height = m_pInstance->cfgUtils->readInt("SDL_DrawableSize_Height");
  m_pInstance->guiClearColor = {70.0f / 255.0f, 70.0f / 255.0f, 70.0f / 255.0f, 255.0f / 255.0f};
  m_pInstance->SDL_Window_Flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
  m_pInstance->SelectedFractalID = 0;

#ifdef _WIN32
  m_pInstance->newLineDelimiter = "\r\n";
#elif defined macintosh // OS 9
  m_pInstance->newLineDelimiter = "\r";
#else
  m_pInstance->newLineDelimiter = "\n";
#endif

  m_pInstance->glUtils = std::make_unique<GLUtils>(std::bind(&Settings::reuseLogFunc, this, std::placeholders::_1));

  m_pInstance->ApplicationConfigurationFolder = iniFolder;
}

void Settings::reuseLogFunc(const std::string& msg) {
  m_pInstance->funcDoLog(msg);
}

std::string Settings::appFolder() {
#ifdef __APPLE__
  CFBundleRef mainBundle = CFBundleGetMainBundle();
  CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
  char folder[PATH_MAX];
  if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8*)folder, PATH_MAX))
    printf("Can't open bundle folder!\n");
  CFRelease(resourcesURL);
  return std::string(folder);
#elif _WIN32
  return boost::filesystem::current_path().string() + "/resources";
#else
  return boost::filesystem::current_path().string() + "/resources";
#endif
}

void Settings::saveSettings() {
  this->cfgUtils->writeString("appVersion", this->appVersion);
  this->cfgUtils->writeBool("shouldRecompileShaders", this->shouldRecompileShaders);
  this->cfgUtils->writeBool("showGLErrors", this->showGLErrors);
  this->cfgUtils->writeBool("showFrameRenderTime", this->showFrameRenderTime);
  this->cfgUtils->writeInt("SDL_Window_Width", this->SDL_Window_Width);
  this->cfgUtils->writeInt("SDL_Window_Height", this->SDL_Window_Height);
  this->cfgUtils->writeInt("SDL_DrawableSize_Width", this->SDL_DrawableSize_Width);
  this->cfgUtils->writeInt("SDL_DrawableSize_Height", this->SDL_DrawableSize_Height);
  this->cfgUtils->saveSettings();
}

#ifdef __APPLE__
std::string Settings::string_format(const std::string& fmt_str, ...) {
  int n = static_cast<int>(fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
  std::unique_ptr<char[]> formatted;
  va_list ap;
  while (1) {
    formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
    strcpy(&formatted[0], fmt_str.c_str());
    va_start(ap, fmt_str);
    int final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
    va_end(ap);
    if (final_n < 0 || final_n >= n)
      n += abs(final_n - n + 1);
    else
      break;
  }
  return std::string(formatted.get());
}
#endif

bool Settings::isAllowedFileExtension(std::string fileExtension, const std::vector<std::string>& allowedExtensions) const {
  if (allowedExtensions.size() > 0) {
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    return std::find(std::begin(allowedExtensions), std::end(allowedExtensions), fileExtension) != std::end(allowedExtensions);
  }
  else
    return true;
}

void Settings::setLogFunc(const std::function<void(std::string)>& doLog) {
  this->funcDoLog = doLog;
}

void Settings::timerStart(const std::string& msg) {
  this->funcDoLog(this->string_format("[Timer START] %s - %s", msg.c_str(), this->getTimeNow().c_str()));
}

void Settings::timerEnd(const std::string& msg) {
  this->funcDoLog(this->string_format("[Timer END] %s - %s", msg.c_str(), this->getTimeNow().c_str()));
}

const std::string Settings::getTimeNow() const {
  time_t t = time(0);
  const struct tm* now = localtime(&t);
  const int t_hour = static_cast<int>(now->tm_hour);
  const int t_min = static_cast<int>(now->tm_min);
  const int t_sec = static_cast<int>(now->tm_sec);
  std::string tn("");
  if (t_hour < 10)
    tn += "0";
  tn += std::to_string(t_hour) + ":";
  if (t_min < 10)
    tn += "0";
  tn += std::to_string(t_min) + ":";
  if (t_sec < 10)
    tn += "0";
  tn += std::to_string(t_sec);
  return tn;
}

const bool Settings::hasEnding(std::string const& fullString, std::string const& ending) const {
  if (fullString.length() >= ending.length())
    return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
  return false;
}

void Settings::logTimings(const std::string& file, const std::string& method) {
#ifdef Bofrak_Debug_Timings
  Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[TIMINGS] [%s - %s] : %f seconds.", file.c_str(), method.c_str(), (1.0 * std::clock() / CLOCKS_PER_SEC)));
#endif
}
