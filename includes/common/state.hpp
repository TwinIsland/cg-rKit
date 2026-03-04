#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "CONFIG.H"

#include <nlohmann/json.hpp>

namespace state_detail {
template <typename T>
struct is_std_vector : std::false_type {};

template <typename U, typename Alloc>
struct is_std_vector<std::vector<U, Alloc>> : std::true_type {};

template <typename T>
struct is_integral_vector : std::false_type {};

template <typename U, typename Alloc>
struct is_integral_vector<std::vector<U, Alloc>>
    : std::integral_constant<bool, std::is_integral<U>::value && !std::is_same<U, bool>::value> {};

template <typename T>
struct is_floating_vector : std::false_type {};

template <typename U, typename Alloc>
struct is_floating_vector<std::vector<U, Alloc>> : std::is_floating_point<U> {};
}  // namespace state_detail

class State {
 public:
  using Int = std::int64_t;
  using Float = double;
  using Bool = bool;
  using String = std::string;
  using IntList = std::vector<Int>;
  using FloatList = std::vector<Float>;
  using BoolList = std::vector<Bool>;
  using StringList = std::vector<String>;

  using Value = std::variant<Int, Float, Bool, String, IntList, FloatList, BoolList, StringList>;

  State() = default;

  template <typename T, typename std::enable_if_t<std::is_enum<T>::value, int> = 0>
  void set(const std::string& key, T value) {
    values_[key] = static_cast<Int>(value);
  }

  template <typename T, typename std::enable_if_t<!std::is_enum<T>::value, int> = 0>
  void set(const std::string& key, const T& value) {
    values_[key] = normalize_for_store(value);
  }

  template <typename T>
  std::optional<T> get(const std::string& key) const {
    const auto it = values_.find(key);
    if (it == values_.end()) {
#if STATE_GET_MISSING_KEY_RAISE_ERROR
      throw std::runtime_error("State key not found: " + key);
#else
      return std::nullopt;
#endif
    }

    try {
      return convert_for_read<T>(it->second, key);
    } catch (const std::runtime_error&) {
#if STATE_GET_TYPE_MISMATCH_RAISE_ERROR
      throw;
#else
      return std::nullopt;
#endif
    }
  }

  bool has(const std::string& key) const;
  void save(const std::string& path) const;
  static State load(const std::string& path);

 private:
  std::map<std::string, Value> values_;

  static Value value_from_json_entry(const std::string& key, const std::string& type,
                                     const nlohmann::json& value_json);
  static nlohmann::json value_to_json_entry(const Value& value);

  template <typename T>
  static Value normalize_for_store(const T& value) {
    using Decayed = std::decay_t<T>;

    if constexpr (std::is_same<Decayed, String>::value) {
      return value;
    } else if constexpr (std::is_same<Decayed, const char*>::value) {
      return String(value);
    } else if constexpr (std::is_same<Decayed, Bool>::value) {
      return value;
    } else if constexpr (std::is_integral<Decayed>::value) {
      return static_cast<Int>(value);
    } else if constexpr (std::is_floating_point<Decayed>::value) {
      return static_cast<Float>(value);
    } else if constexpr (state_detail::is_integral_vector<Decayed>::value) {
      IntList out;
      out.reserve(value.size());
      for (const auto& item : value) {
        out.push_back(static_cast<Int>(item));
      }
      return out;
    } else if constexpr (state_detail::is_floating_vector<Decayed>::value) {
      FloatList out;
      out.reserve(value.size());
      for (const auto& item : value) {
        out.push_back(static_cast<Float>(item));
      }
      return out;
    } else if constexpr (std::is_same<Decayed, BoolList>::value) {
      return value;
    } else if constexpr (std::is_same<Decayed, StringList>::value) {
      return value;
    } else {
      static_assert(sizeof(T) == 0, "Unsupported type for State::set");
    }
  }

  template <typename T>
  static T convert_for_read(const Value& value, const std::string& key) {
    using Decayed = std::decay_t<T>;

    if constexpr (std::is_enum<Decayed>::value) {
      if (!std::holds_alternative<Int>(value)) {
        throw_type_error(key, "enum");
      }
      return static_cast<Decayed>(std::get<Int>(value));
    } else if constexpr (std::is_same<Decayed, String>::value) {
      if (!std::holds_alternative<String>(value)) {
        throw_type_error(key, "string");
      }
      return std::get<String>(value);
    } else if constexpr (std::is_same<Decayed, Bool>::value) {
      if (!std::holds_alternative<Bool>(value)) {
        throw_type_error(key, "bool");
      }
      return std::get<Bool>(value);
    } else if constexpr (std::is_integral<Decayed>::value) {
      if (!std::holds_alternative<Int>(value)) {
        throw_type_error(key, "int");
      }
      return static_cast<Decayed>(std::get<Int>(value));
    } else if constexpr (std::is_floating_point<Decayed>::value) {
      if (!std::holds_alternative<Float>(value)) {
        throw_type_error(key, "float");
      }
      return static_cast<Decayed>(std::get<Float>(value));
    } else if constexpr (state_detail::is_integral_vector<Decayed>::value) {
      if (!std::holds_alternative<IntList>(value)) {
        throw_type_error(key, "int list");
      }
      Decayed out;
      const auto& in = std::get<IntList>(value);
      out.reserve(in.size());
      for (const auto& item : in) {
        out.push_back(static_cast<typename Decayed::value_type>(item));
      }
      return out;
    } else if constexpr (state_detail::is_floating_vector<Decayed>::value) {
      if (!std::holds_alternative<FloatList>(value)) {
        throw_type_error(key, "float list");
      }
      Decayed out;
      const auto& in = std::get<FloatList>(value);
      out.reserve(in.size());
      for (const auto& item : in) {
        out.push_back(static_cast<typename Decayed::value_type>(item));
      }
      return out;
    } else if constexpr (std::is_same<Decayed, BoolList>::value) {
      if (!std::holds_alternative<BoolList>(value)) {
        throw_type_error(key, "bool list");
      }
      return std::get<BoolList>(value);
    } else if constexpr (std::is_same<Decayed, StringList>::value) {
      if (!std::holds_alternative<StringList>(value)) {
        throw_type_error(key, "string list");
      }
      return std::get<StringList>(value);
    } else {
      static_assert(sizeof(T) == 0, "Unsupported type for State::get");
    }
  }

  [[noreturn]] static void throw_type_error(const std::string& key, const std::string& expected);
};
