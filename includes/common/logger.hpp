#pragma once

#include <fstream>
#include <mutex>
#include <string>

class Logger {
 public:
  enum class Level {
    kInfo,
    kCommon,
    kWarning,
    kDanger,
    kDebug,
  };

  explicit Logger(std::string module = "default");
  ~Logger();

  void common(const std::string& message);
  void info(const std::string& message);
  void warning(const std::string& message);
  void danger(const std::string& message);
  void debug(const std::string& message);

  void log(Level level, const std::string& message);
  void log(Level level, const std::string& message, const char* file, int line,
           const char* function);

  void common_at(const std::string& message, const char* file, int line, const char* function);
  void info_at(const std::string& message, const char* file, int line, const char* function);
  void warning_at(const std::string& message, const char* file, int line, const char* function);
  void danger_at(const std::string& message, const char* file, int line, const char* function);
  void debug_at(const std::string& message, const char* file, int line, const char* function);

 private:
  std::string module_;
  std::mutex mutex_;
  std::ofstream file_stream_;

  static std::string current_time_string(const char* format);
  static const char* level_label(Level level);
  static const char* level_color(Level level);
  static bool should_emit(Level level);
  std::string compose_prefix(Level level, const char* file, int line, const char* function) const;

  void emit_line(Level level, const std::string& message, const char* file, int line,
                 const char* function);
};

#define LOG_COMMON(logger, message) (logger).common_at((message), __FILE__, __LINE__, __func__)
#define LOG_INFO(logger, message) (logger).info_at((message), __FILE__, __LINE__, __func__)
#define LOG_WARNING(logger, message) (logger).warning_at((message), __FILE__, __LINE__, __func__)
#define LOG_DANGER(logger, message) (logger).danger_at((message), __FILE__, __LINE__, __func__)
#define LOG_DEBUG(logger, message) (logger).debug_at((message), __FILE__, __LINE__, __func__)
