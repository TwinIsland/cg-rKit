#include "common/logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

#include "CONFIG.H"

namespace {
constexpr const char* kColorReset = "\033[0m";
}

Logger::Logger(std::string module) : module_(std::move(module)) {
  if (module_.empty()) {
    module_ = "default";
  }
#if LOG_DUMP_TO_FILE
  try {
    std::filesystem::create_directories(LOG_OUTPUT_DIR);
    const std::string file_name = current_time_string("%Y%m%d_%H%M%S") + ".log";
    const std::filesystem::path log_path = std::filesystem::path(LOG_OUTPUT_DIR) / file_name;
    file_stream_.open(log_path, std::ios::out | std::ios::app);
  } catch (...) {
    // Keep console logging functional even if file logging setup fails.
  }
#endif
}

Logger::~Logger() {
  if (file_stream_.is_open()) {
    file_stream_.close();
  }
}

void Logger::common(const std::string& message) { log(Level::kCommon, message); }

void Logger::info(const std::string& message) { log(Level::kInfo, message); }

void Logger::warning(const std::string& message) { log(Level::kWarning, message); }

void Logger::danger(const std::string& message) { log(Level::kDanger, message); }

void Logger::debug(const std::string& message) { log(Level::kDebug, message); }

void Logger::log(Level level, const std::string& message) {
  log(level, message, nullptr, 0, nullptr);
}

void Logger::log(Level level, const std::string& message, const char* file, int line,
                 const char* function) {
  if (!should_emit(level)) {
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  emit_line(level, message, file, line, function);
}

std::string Logger::current_time_string(const char* format) {
  const auto now = std::chrono::system_clock::now();
  const std::time_t now_time = std::chrono::system_clock::to_time_t(now);

  std::tm tm_snapshot {};
#if defined(_WIN32)
  localtime_s(&tm_snapshot, &now_time);
#else
  localtime_r(&now_time, &tm_snapshot);
#endif

  std::ostringstream oss;
  oss << std::put_time(&tm_snapshot, format);
  return oss.str();
}

const char* Logger::level_label(Level level) {
  switch (level) {
    case Level::kInfo:
      return "INFO";
    case Level::kCommon:
      return "COMMON";
    case Level::kWarning:
      return "WARNING";
    case Level::kDanger:
      return "DANGER";
    case Level::kDebug:
      return "DEBUG";
    default:
      return "LOG";
  }
}

const char* Logger::level_color(Level level) {
  switch (level) {
    case Level::kInfo:
      return "";
    case Level::kCommon:
      return "\033[32m";  // Green
    case Level::kWarning:
      return "\033[33m";  // Yellow
    case Level::kDanger:
      return "\033[31m";  // Red
    case Level::kDebug:
      return "\033[36m";  // Cyan
    default:
      return "\033[37m";  // White
  }
}

bool Logger::should_emit(Level level) {
#if LOG_ENABLE_DEBUG
  return true;
#else
  return level != Level::kDebug;
#endif
}

std::string Logger::compose_prefix(Level level, const char* file, int line,
                                   const char* function) const {
  std::ostringstream prefix;
  prefix << '[' << level_label(level) << ']';
#if LOG_SHOW_TIME
  prefix << '[' << current_time_string("%Y-%m-%d %H:%M:%S") << "] ";
#endif
  prefix << '[' << PROJECT_NAME << ']';
  prefix << '[' << module_ << ']';
#if LOG_SHOW_SOURCE_LOCATION
  if (file != nullptr && function != nullptr && line > 0) {
    prefix << '[' << file << ':' << line << ':' << function << ']';
  }
#endif
  return prefix.str();
}

void Logger::emit_line(Level level, const std::string& message, const char* file, int line,
                       const char* function) {
  const std::string prefix = compose_prefix(level, file, line, function);
  const char* color = level_color(level);

  if (color[0] == '\0') {
    std::cout << prefix << ' ' << message << '\n';
  } else {
    std::cout << color << prefix << ' ' << message << kColorReset << '\n';
  }

#if LOG_DUMP_TO_FILE
  if (file_stream_.is_open()) {
    file_stream_ << prefix << ' ' << message << '\n';
    file_stream_.flush();
  }
#endif
}
void Logger::common_at(const std::string& message, const char* file, int line,
                       const char* function) {
  log(Level::kCommon, message, file, line, function);
}

void Logger::info_at(const std::string& message, const char* file, int line,
                     const char* function) {
  log(Level::kInfo, message, file, line, function);
}

void Logger::warning_at(const std::string& message, const char* file, int line,
                        const char* function) {
  log(Level::kWarning, message, file, line, function);
}

void Logger::danger_at(const std::string& message, const char* file, int line,
                       const char* function) {
  log(Level::kDanger, message, file, line, function);
}

void Logger::debug_at(const std::string& message, const char* file, int line,
                      const char* function) {
  log(Level::kDebug, message, file, line, function);
}
