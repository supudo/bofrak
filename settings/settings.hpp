#ifndef Settings_hpp
#define Settings_hpp

#include <string>
#include <vector>
#include <iostream>
#include <type_traits>
#include <typeinfo>

#include "settingsstructs.hpp"
#include "configutils.hpp"
#include "utilities/gl/glutils.hpp"

#ifdef WIN32
#include <memory>
#endif

#define BOOST_FILESYSTEM_NO_DEPRECATED

class Settings {
public:
  static Settings* Instance();
	void initSettings(const std::string& iniFolder);
  void saveSettings();
  std::string appFolder();
  bool isAllowedFileExtension(std::string fileExtension, const std::vector<std::string>& allowedExtensions = std::vector<std::string>()) const;
  void setLogFunc(const std::function<void(std::string)>& doLog);

  unsigned short int OpenGL_MajorVersion, OpenGL_MinorVersion;
  bool maybeGracefullApplicationQuit;
  std::function<void(std::string)> funcDoLog;
  std::string ApplicationConfigurationFolder, appVersion, currentFolder, newLineDelimiter, SettingsFile;
  Color guiClearColor;
  int SDL_Window_Flags, SDL_Window_Width, SDL_Window_Height;
  int SDL_DrawableSize_Width, SDL_DrawableSize_Height;
  int UIFontSize;
	bool shouldRecompileShaders, showGLErrors;
  bool showFrameRenderTime;

  void timerStart(const std::string& msg);
  void timerEnd(const std::string& msg);
  const bool hasEnding(std::string const &fullString, std::string const &ending) const;

  std::unique_ptr<GLUtils> glUtils;
	const std::string getTimeNow() const;
  void logTimings(const std::string& file, const std::string& method);

  template <class T>
  inline void printClassAlignment(T *s) {
    std::cout << typeid(s).name() << " <-> " << std::alignment_of<T>() << '\n';
  }

  template <typename T>
  inline void printTypeAlignment(T s) {
    std::cout << "Size: " << sizeof(s) << " - " << typeid(s).name() << " <-> " << std::alignment_of<T>() << '\n';
  }

#ifdef _WIN32
  template<typename ... Args>
  std::string string_format(const std::string& format, Args ... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1);
  }
#else
  std::string string_format(const std::string& fmt_str, ...);
#endif

private:
  Settings(){}
  Settings(Settings const&){}
  Settings& operator=(Settings const&);
  static Settings* m_pInstance;

  std::unique_ptr<ConfigUtils> cfgUtils;
  void reuseLogFunc(const std::string& msg);
};

#endif /* Settings_hpp */
