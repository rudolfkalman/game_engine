#ifndef GAME_ENGINE_TRANSFORM_HPP
#define GAME_ENGINE_TRANSFORM_HPP

#include "game_engine/mesh.hpp"

#include <Eigen/Dense>

#include <vector>

namespace game_engine::transform {

struct TransformedMesh {
  std::vector<Eigen::Vector2d> screen_vertices;
  std::vector<Eigen::Vector4d> view_vertices;
  std::vector<Eigen::Vector4d> view_normals;
};

TransformedMesh Apply(const mesh::Mesh &mesh, const Eigen::Matrix4d &model,
                      const Eigen::Matrix4d &view,
                      const Eigen::Matrix4d &projection, int screen_width,
                      int screen_height);

} // namespace game_engine::transform

#endif
