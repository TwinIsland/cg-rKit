#pragma once

#include <string>

#include "CONFIG.H"           // IWYU pragma: export
#include "common/csv.hpp"     // IWYU pragma: export
#include "common/logger.hpp"  // IWYU pragma: export
#include "common/state.hpp"   // IWYU pragma: export
#include "common/welcome.hpp" // IWYU pragma: export


inline void ensure_directory(std::string_view path) {
  std::error_code ec;

  if (!std::filesystem::exists(path)) {
    if (!std::filesystem::create_directories(path, ec)) {
      if (ec) {
        throw std::runtime_error("Failed to create directory: " +
                                 std::string(path));
      }
    }
  }
}

inline std::filesystem::path make_absolute_path(const char *child_path) {
  return std::filesystem::path(WORKSPACE_PATH) / child_path;
}

inline void check_and_prepare_environment() {
  ensure_directory(WORKSPACE_PATH);

  if (!std::filesystem::exists(KNITRO_OPT_CONFIG_PATH)) {
    throw std::runtime_error("Knitro config file not found");
  }
}

#include <Eigen/Core>

using Vec2 = Eigen::Matrix<double, 2, 1>;
using MatX = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
using VecX = Eigen::Matrix<double, Eigen::Dynamic, 1>;

struct Eigenpair {
  double eigenvalue = 0.0;
  VecX coefficients; // basis coefficients alpha
};

constexpr double kPi = 3.141592653589793238462643383279502884;

inline double sqr(double x) { return x * x; }
inline double norm_sq(const Vec2 &v) { return v.squaredNorm(); }
inline double norm(const Vec2 &v) { return v.norm(); }

inline Vec2 unit_from_angle(double theta) {
  return Vec2(std::cos(theta), std::sin(theta));
}

inline std::complex<double> to_complex(const Vec2 &x) { return {x.x(), x.y()}; }
