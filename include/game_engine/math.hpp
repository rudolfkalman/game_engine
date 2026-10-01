#ifndef GAME_ENGINE_MATH_HPP
#define GAME_ENGINE_MATH_HPP

#include <Eigen/Dense>

namespace game_engine::math {

Eigen::Matrix4d MakeScale(double x, double y, double z);
Eigen::Matrix4d MakeRotation(double roll, double pitch, double yaw);
Eigen::Matrix4d MakeTranslation(double x, double y, double z);
Eigen::Matrix4d MakeProjection(double fov_y, double aspect, double near_plane,
                               double far_plane);

double Deg2rad(double deg);
double Cross2d(const Eigen::Vector2d &a, const Eigen::Vector2d &b);

} // namespace game_engine::math

#endif
