#ifndef GAME_ENGINE_MESH_HPP
#define GAME_ENGINE_MESH_HPP

#include <Eigen/Dense>

#include <cstdint>
#include <vector>

namespace game_engine::mesh {

struct Triangle {
  std::uint32_t p1;
  std::uint32_t p2;
  std::uint32_t p3;
};

struct Mesh {
  std::vector<Eigen::Vector4d> vertices;
  std::vector<Triangle> triangles;
  std::vector<Eigen::Vector4d> normals;
};

Mesh MakeCube();

} // namespace game_engine::mesh

#endif
