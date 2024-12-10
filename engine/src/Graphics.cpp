
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine/controller/ControllerManager.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/Skybox.hpp>

namespace engine::graphics {

    void GraphicsController::initialize() {
        const int opengl_initialized = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        RG_GUARANTEE(opengl_initialized, "OpenGL failed to init!");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        auto handle = engine::controller::get<platform::PlatformController>()->window()->handle_();
        ImGui_ImplGlfw_InitForOpenGL(handle, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
    }

    void GraphicsController::update() {
        auto platform = engine::controller::get<engine::platform::PlatformController>();
        m_projection  = glm::perspective(glm::radians(45.0f),
                                        static_cast<float>(platform->window()->width()) / platform
                                        ->window()->
                                        height(),
                                        0.1f, 100.f);
    }

    std::string_view GraphicsController::name() const {
        return "GraphicsController";
    }

    void GraphicsController::begin_gui() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GraphicsController::end_gui() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void GraphicsController::draw_skybox(const resources::Shader *shader, const resources::Skybox *skybox) {
        glm::mat4 view = glm::mat4(glm::mat3(m_camera.view_matrix()));
        shader->use();
        shader->set_mat4("view", view);
        shader->set_mat4("projection", m_projection);
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(skybox->vao());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    // TODO(mspasic); Imgui terminate

}
