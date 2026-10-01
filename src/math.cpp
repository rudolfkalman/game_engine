#include "game_engine/math.hpp"

#include <cmath>
#include <numbers>

namespace game_engine::math {

Eigen::Matrix4d MakeScale(double x, double y, double z) {
  Eigen::Matrix4d scale;
  scale << x, 0, 0, 0,
           0, y, 0, 0,
           0, 0, z, 0,
           0, 0, 0, 1;
  return scale;
}

Eigen::Matrix4d MakeRotation(double roll, double pitch, double yaw) {
  Eigen::Matrix4d yaw_matrix;
  yaw_matrix << std::cos(yaw), -std::sin(yaw), 0, 0,
                std::sin(yaw),  std::cos(yaw), 0, 0,
                0,              0,             1, 0,
                0,              0,             0, 1;

  Eigen::Matrix4d pitch_matrix;
  pitch_matrix << std::cos(pitch),  0, std::sin(pitch), 0,
                  0,                1, 0,               0,
                 -std::sin(pitch),  0, std::cos(pitch), 0,
                  0,                0, 0,               1;

  Eigen::Matrix4d roll_matrix;
  roll_matrix << 1, 0,              0,               0,
                 0, std::cos(roll), -std::sin(roll), 0,
                 0, std::sin(roll),  std::cos(roll), 0,
                 0, 0,              0,               1;

  return roll_matrix * pitch_matrix * yaw_matrix;
}

Eigen::Matrix4d MakeTranslation(double x, double y, double z) {
  Eigen::Matrix4d translation;
  translation << 1, 0, 0, x,
                 0, 1, 0, y,
                 0, 0, 1, z,
                 0, 0, 0, 1;
  return translation;
}

Eigen::Matrix4d MakeProjection(double fov_y, double aspect, double near_plane,
                               double far_plane) {
  const double height = 2.0 * near_plane * std::tan(fov_y / 2.0);
  const double width = height * aspect;

  Eigen::Matrix4d projection;
  projection << 2.0 * near_plane / width, 0, 0, 0,
                0, 2.0 * near_plane / height, 0, 0,
                0, 0,
                -(far_plane + near_plane) / (far_plane - near_plane),
                -(2.0 * far_plane * near_plane) / (far_plane - near_plane),
                0, 0, -1, 0;
  return projection;
}

double Deg2rad(double deg) { return deg * std::numbers::pi / 180.0; }

double Cross2d(const Eigen::Vector2d &a, const Eigen::Vector2d &b) {
  return a.x() * b.y() - a.y() * b.x();
}

} // namespace game_engine::math
