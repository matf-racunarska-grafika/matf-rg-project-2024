//
// Created by spaske on 12/7/24.
//

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <rg/controller/ControllerManager.hpp>
#include <rg/graphics/GraphicsController.hpp>
#include <rg/graphics/OpenGL.hpp>
#include <rg/platform/PlatformController.hpp>

namespace rg::graphics {

    void GraphicsController::initialize() {
        const int opengl_initialized = OpenGL::initialize(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        RG_GUARANTEE(opengl_initialized, "OpenGL failed to init!");
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        auto handle = rg::controller::get<platform::PlatformController>()->window()->handle_();
        ImGui_ImplGlfw_InitForOpenGL(handle, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
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

}
