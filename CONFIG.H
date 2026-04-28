#ifndef CONFIG_H
#define CONFIG_H

// Project metadata
#define PROJECT_NAME "Ball Minimal"
#define PROJECT_VERSION "0.1.0"

// Logging configuration
// 1 = enable debug logs, 0 = hide debug logs
#define LOG_ENABLE_DEBUG 0

// 1 = include timestamps in console/file messages, 0 = no timestamps
#define LOG_SHOW_TIME 1

// 1 = include file:line:function in logs (when using LOG_* macros), 0 = hide source location
#define LOG_SHOW_SOURCE_LOCATION 1

// 1 = write logs to logs/<timestamp>.log, 0 = console only
#define LOG_DUMP_TO_FILE 0

// Directory for log files when LOG_DUMP_TO_FILE == 1
#define LOG_OUTPUT_DIR "logs"

// State behavior
// JSON schema version used by State::save/load
#define STATE_SCHEMA_VERSION 1

// 1 = throw error when State::get() key is missing, 0 = return std::nullopt
#define STATE_GET_MISSING_KEY_RAISE_ERROR 0

// 1 = throw error when State::get<T>() type mismatches, 0 = return std::nullopt
#define STATE_GET_TYPE_MISMATCH_RAISE_ERROR 1

#endif  // CONFIG_H
