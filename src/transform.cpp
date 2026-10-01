#include "game_engine/transform.hpp"

namespace game_engine::transform {

TransformedMesh Apply(const mesh::Mesh &mesh, const Eigen::Matrix4d &model,
                      const Eigen::Matrix4d &view,
                      const Eigen::Matrix4d &projection, int screen_width,
                      int screen_height) {
  TransformedMesh result;
  result.screen_vertices.reserve(mesh.vertices.size());
  result.view_vertices.reserve(mesh.vertices.size());
  result.view_normals.reserve(mesh.normals.size());

  const Eigen::Matrix4d model_view = view * model;

  for (const Eigen::Vector4d &vertex : mesh.vertices) {
    const Eigen::Vector4d view_vertex = model_view * vertex;
    Eigen::Vector4d clip_vertex = projection * view_vertex;

    clip_vertex[0] /= clip_vertex[3];
    clip_vertex[1] /= clip_vertex[3];
    clip_vertex[2] /= clip_vertex[3];

    const double screen_x =
        (clip_vertex[0] + 1.0) * 0.5 * static_cast<double>(screen_width);
    const double screen_y =
        (1.0 - clip_vertex[1]) * 0.5 * static_cast<double>(screen_height);

    result.view_vertices.push_back(view_vertex);
    result.screen_vertices.emplace_back(screen_x, screen_y);
  }

  for (const Eigen::Vector4d &normal : mesh.normals) {
    result.view_normals.push_back(model_view * normal);
  }

  return result;
}

} // namespace game_engine::transform
