
#ifndef GRAPHICSCONTROLLER_HPP
#define GRAPHICSCONTROLLER_HPP
#include <engine/graphics/Camera.hpp>
#include <engine/controller/Controller.hpp>
#include <engine/Declarations.hpp>

namespace engine::graphics {
    /**
    * @class GraphicsController
    * @brief Implements basic drawing methods that the @ref core::App implementation uses.
    *
    * This class should implement all the complex functions needed for drawing an entity in the scene.
    * For example @ref GraphicsController::draw_skybox.
    */
    class GraphicsController final : public controller::Controller {
    public:
        std::string_view name() const override;

        /**
        * @brief Calls internal methods for the beginning of gui drawing. Should be called in pair with @ref GraphicsController::end_gui.
        *
        * Example:
        * @code
        * auto graphics = engine::controller::get<engine::graphics::GraphicsController>();
        * graphics->begin_gui();
        * ImGui::Begin("Camera info");
        * const auto &c = ...;
        * ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        * ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        * ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        * ImGui::End();
        * graphics->end_gui();
        * @endcode
        */
        void begin_gui();

        /**
        * @brief Calls internal method for the ending of gui drawing. Should be called in pair with @ref GraphicsController::begin_gui.
        */
        void end_gui();

        /**
        * @brief Draws a @ref resources::Skybox with the @ref resources::Shader.
        */
        void draw_skybox(const resources::Shader *shader, const resources::Skybox *skybox);

        Camera *camera() {
            return &m_camera;
        }

        /**
        * @brief The projection matrix will be updated in the window size changes.
        * @returns The projection matrix.
        */
        const glm::mat4 &projection_matrix() const {
            return m_projection;
        }

    private:
        /**
        * @brief Initializes OpenGL and ImGUI.
        */
        void initialize() override;

        void update() override;

        glm::mat4 m_projection{};
        Camera m_camera{};
    };
}
#endif //GRAPHICSCONTROLLER_HPP
