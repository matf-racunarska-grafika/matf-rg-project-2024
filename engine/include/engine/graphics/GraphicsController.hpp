//
// Created by spaske on 12/7/24.
//

#ifndef GRAPHICSCONTROLLER_HPP
#define GRAPHICSCONTROLLER_HPP
#include <engine/graphics/Camera.hpp>
#include <engine/controller/Controller.hpp>
#include <engine/Declarations.hpp>

namespace engine::graphics {
    class GraphicsController final : public controller::Controller {
    public:
        std::string_view name() const override;

        void begin_gui();

        void draw_skybox(const resources::Shader *shader, const resources::Skybox *skybox);

        void end_gui();

        Camera *camera() {
            return &m_camera;
        }

        const glm::mat4 &projection_matrix() const {
            return m_projection;
        }

    private:
        void initialize() override;

        void update() override;

        glm::mat4 m_projection{};
        Camera m_camera{};
    };
}
#endif //GRAPHICSCONTROLLER_HPP
