#include "common/state.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include "CONFIG.H"

bool State::has(const std::string& key) const { return values_.find(key) != values_.end(); }

void State::save(const std::string& path) const {
  nlohmann::json root;
  root["schema_version"] = STATE_SCHEMA_VERSION;
  root["values"] = nlohmann::json::object();

  for (const auto& pair : values_) {
    root["values"][pair.first] = value_to_json_entry(pair.second);
  }

  const std::filesystem::path out_path(path);
  if (out_path.has_parent_path()) {
    std::filesystem::create_directories(out_path.parent_path());
  }

  std::ofstream out(path);
  if (!out.is_open()) {
    throw std::runtime_error("Failed to open state file for write: " + path);
  }
  out << root.dump(2);
}

State State::load(const std::string& path) {
  std::ifstream in(path);
  if (!in.is_open()) {
    throw std::runtime_error("Failed to open state file for read: " + path);
  }

  nlohmann::json root;
  in >> root;

  if (!root.contains("schema_version") || !root["schema_version"].is_number_integer()) {
    throw std::runtime_error("Invalid state file: missing integer schema_version");
  }

  const int schema_version = root["schema_version"].get<int>();
  if (schema_version != STATE_SCHEMA_VERSION) {
    throw std::runtime_error("State schema_version mismatch: expected " +
                             std::to_string(STATE_SCHEMA_VERSION) + ", got " +
                             std::to_string(schema_version));
  }

  if (!root.contains("values") || !root["values"].is_object()) {
    throw std::runtime_error("Invalid state file: missing object values");
  }

  State state;
  for (auto it = root["values"].begin(); it != root["values"].end(); ++it) {
    const std::string key = it.key();
    const nlohmann::json& entry = it.value();

    if (!entry.is_object() || !entry.contains("type") || !entry.contains("value")) {
      throw std::runtime_error("Invalid state entry for key: " + key);
    }

    const std::string type = entry["type"].get<std::string>();
    state.values_[key] = value_from_json_entry(key, type, entry["value"]);
  }

  return state;
}

State::Value State::value_from_json_entry(const std::string& key, const std::string& type,
                                          const nlohmann::json& value_json) {
  if (type == "int") {
    if (!value_json.is_number_integer()) {
      throw std::runtime_error("Invalid int value for key: " + key);
    }
    return value_json.get<Int>();
  }

  if (type == "float") {
    if (!value_json.is_number()) {
      throw std::runtime_error("Invalid float value for key: " + key);
    }
    return value_json.get<Float>();
  }

  if (type == "bool") {
    if (!value_json.is_boolean()) {
      throw std::runtime_error("Invalid bool value for key: " + key);
    }
    return value_json.get<Bool>();
  }

  if (type == "string") {
    if (!value_json.is_string()) {
      throw std::runtime_error("Invalid string value for key: " + key);
    }
    return value_json.get<String>();
  }

  if (type == "int_list") {
    if (!value_json.is_array()) {
      throw std::runtime_error("Invalid int_list value for key: " + key);
    }
    return value_json.get<IntList>();
  }

  if (type == "float_list") {
    if (!value_json.is_array()) {
      throw std::runtime_error("Invalid float_list value for key: " + key);
    }
    return value_json.get<FloatList>();
  }

  if (type == "bool_list") {
    if (!value_json.is_array()) {
      throw std::runtime_error("Invalid bool_list value for key: " + key);
    }
    return value_json.get<BoolList>();
  }

  if (type == "string_list") {
    if (!value_json.is_array()) {
      throw std::runtime_error("Invalid string_list value for key: " + key);
    }
    return value_json.get<StringList>();
  }

  throw std::runtime_error("Unknown state value type for key " + key + ": " + type);
}

nlohmann::json State::value_to_json_entry(const Value& value) {
  nlohmann::json entry;

  if (std::holds_alternative<Int>(value)) {
    entry["type"] = "int";
    entry["value"] = std::get<Int>(value);
  } else if (std::holds_alternative<Float>(value)) {
    entry["type"] = "float";
    entry["value"] = std::get<Float>(value);
  } else if (std::holds_alternative<Bool>(value)) {
    entry["type"] = "bool";
    entry["value"] = std::get<Bool>(value);
  } else if (std::holds_alternative<String>(value)) {
    entry["type"] = "string";
    entry["value"] = std::get<String>(value);
  } else if (std::holds_alternative<IntList>(value)) {
    entry["type"] = "int_list";
    entry["value"] = std::get<IntList>(value);
  } else if (std::holds_alternative<FloatList>(value)) {
    entry["type"] = "float_list";
    entry["value"] = std::get<FloatList>(value);
  } else if (std::holds_alternative<BoolList>(value)) {
    entry["type"] = "bool_list";
    entry["value"] = std::get<BoolList>(value);
  } else if (std::holds_alternative<StringList>(value)) {
    entry["type"] = "string_list";
    entry["value"] = std::get<StringList>(value);
  } else {
    throw std::runtime_error("Unsupported state value type at serialization");
  }

  return entry;
}

[[noreturn]] void State::throw_type_error(const std::string& key, const std::string& expected) {
  throw std::runtime_error("State key '" + key + "' has incompatible type; expected " + expected);
}
