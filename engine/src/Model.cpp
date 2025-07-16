
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::resources {

void Model::draw(const Shader *shader) {
    shader->use();
    for (auto &mesh: m_meshes) { mesh.draw(shader); }
}

void Model::set_texture(Texture *texture) { for (auto &mesh: m_meshes) { mesh.add_texture(texture); } }

void Model::destroy() { for (auto &mesh: m_meshes) { mesh.destroy(); } }
}
