#ifndef SPRITE_CONTROLLER_HPP
#define SPRITE_CONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

namespace engine::resources {
class Texture;
class Shader;
}

namespace engine::graphics {

class SpriteController final : public core::Controller {
public:
    std::string_view name() const override { return "engine::graphics::SpriteController"; }

    void draw_textured_quad(resources::Texture *texture,
                            resources::Shader *shader,
                            const glm::mat4 &model,
                            const glm::mat4 &view,
                            const glm::mat4 &projection);

private:
    void initialize() override;
    void terminate() override;

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
};

}

#endif // SPRITE_CONTROLLER_HPP


