#include "configutils.hpp"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <mutex>
#include <stdexcept>

ConfigUtils::~ConfigUtils() {
  this->configData = {};
}

void ConfigUtils::init(std::string const& appFolder) {
  this->configFile = appFolder + "/Bofrak_Settings.ini";
  this->regex_equalsSign = "=";
  this->readFile();
}

void ConfigUtils::saveSettings() {
#ifdef _WIN32
  std::string nlDelimiter = "\n";
#elif defined macintosh // OS 9
  std::string nlDelimiter = "\r";
#else
  std::string nlDelimiter = "\n";
#endif
  std::string configLines("");
  for (std::map<std::string, std::string>::iterator iter = this->configData.begin(); iter != this->configData.end(); ++iter) {
    std::string cfgKey = iter->first;
    std::string cfgValue = iter->second;
    configLines += cfgKey + " = " + cfgValue + nlDelimiter;
  }
  static std::mutex mutex;
  std::lock_guard<std::mutex> lock(mutex);

  std::ofstream out(this->configFile);
  if (!out.is_open())
    throw std::runtime_error("[Bofrag] Unable to save settings file!");
  out << configLines << std::endl;
  out.close();
}

bool ConfigUtils::readBool(std::string const& configKey) {
  return this->configData[configKey] == "true";
}

int ConfigUtils::readInt(std::string const& configKey) {
  std::string val = this->configData[configKey];
  if (!val.empty())
    return std::stoi(val);
  else
    return 0;
}

float ConfigUtils::readFloat(std::string const& configKey) {
  std::string val = this->configData[configKey];
  if (!val.empty())
    return std::stof(val);
  else
    return 0;
}

std::string ConfigUtils::readString(std::string const& configKey) {
  return this->configData[configKey];
}

void ConfigUtils::writeBool(std::string const& configKey, bool const& configValue) {
  this->configData[configKey] = configValue ? "true" : "false";
}

void ConfigUtils::writeInt(std::string const& configKey, int const& configValue) {
  this->configData[configKey] = std::to_string(configValue);
}

void ConfigUtils::writeFloat(std::string const& configKey, float const& configValue) {
  this->configData[configKey] = std::to_string(configValue);
}

void ConfigUtils::writeString(std::string const& configKey, std::string const& configValue) {
  this->configData[configKey] = configValue;
}

void ConfigUtils::readFile() {
  std::FILE* fp = std::fopen(this->configFile.c_str(), "rb");
  if (fp) {
#ifdef _WIN32
    std::string nlDelimiter = "\n";
#elif defined macintosh // OS 9
    std::string nlDelimiter = "\r";
#else
    std::string nlDelimiter = "\n";
#endif

    std::string fileContents;
    std::fseek(fp, 0, SEEK_END);
    fileContents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&fileContents[0], 1, fileContents.size(), fp);
    std::fclose(fp);

    size_t pos = 0;
    std::string singleLine;

    this->configData = {};

    while ((pos = fileContents.find(nlDelimiter)) != std::string::npos) {
      singleLine = fileContents.substr(0, pos);

      if (singleLine.empty() || singleLine.at(0) == '#' || singleLine.at(0) == '\n' || singleLine.at(0) == '\r' || singleLine.at(0) == '\r\n') {
        fileContents.erase(0, pos + nlDelimiter.length());
        continue;
      }
      else {
        std::string opKey, opValue;
        std::vector<std::string> lineElements = this->splitString(singleLine, this->regex_equalsSign);
        opKey = lineElements[0];
        boost::algorithm::trim(opKey);

        if (lineElements.size() > 1) {
          opValue = lineElements[1];
          boost::algorithm::trim(opValue);
        }
        else
          opValue.clear();
        this->configData[opKey] = opValue;
      }

      fileContents.erase(0, pos + nlDelimiter.length());
    }
  }
}

const std::vector<std::string> ConfigUtils::splitString(const std::string& s, std::regex const& delimiter) const {
  std::vector<std::string> elements;
  std::sregex_token_iterator iter(s.begin(), s.end(), delimiter, -1);
  std::sregex_token_iterator end;
  for (; iter != end; ++iter)
    elements.push_back(*iter);
  return elements;
}