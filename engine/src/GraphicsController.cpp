
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/Skybox.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/PointShadowCaster.hpp>
#include <spdlog/spdlog.h>

namespace engine::graphics {

uint32_t Framebuffer::m_quadVAO = 0;

uint32_t Framebuffer::m_fbo = 0;
uint32_t Framebuffer::m_textureColorBufferMultiSampled = 0;
uint32_t Framebuffer::m_rbo = 0;

uint32_t Framebuffer::m_intermediateFBO = 0;
uint32_t Framebuffer::m_screenTexture = 0;

std::vector<PointShadowCaster> m_shadowCasters;

void GraphicsController::initialize() {
    const int opengl_initialized = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    RG_GUARANTEE(opengl_initialized, "OpenGL failed to init!");

    auto platform = engine::core::Controller::get<platform::PlatformController>();
    auto handle = platform->window()
                          ->handle_();
    m_perspective_params.FOV = glm::radians(m_camera.Zoom);
    m_perspective_params.Width = static_cast<float>(platform->window()
                                                            ->width());
    m_perspective_params.Height = static_cast<float>(platform->window()
                                                             ->height());
    m_perspective_params.Near = 0.1f;
    m_perspective_params.Far = 100.f;

    m_ortho_params.Bottom = 0.0f;
    m_ortho_params.Top = static_cast<float>(platform->window()
                                                    ->height());
    m_ortho_params.Left = 0.0f;
    m_ortho_params.Right = static_cast<float>(platform->window()
                                                      ->width());
    m_ortho_params.Near = 0.1f;
    m_ortho_params.Far = 100.0f;
    platform->register_platform_event_observer(
            std::make_unique<GraphicsPlatformEventObserver>(this));
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    RG_GUARANTEE(ImGui_ImplGlfw_InitForOpenGL(handle, true), "ImGUI failed to initialize for OpenGL");
    RG_GUARANTEE(ImGui_ImplOpenGL3_Init("#version 330 core"), "ImGUI failed to initialize for OpenGL");
}

void GraphicsController::terminate() {
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

void GraphicsPlatformEventObserver::on_window_resize(int width, int height) {
    m_graphics->perspective_params()
              .Width = static_cast<float>(width);
    m_graphics->perspective_params()
              .Height = static_cast<float>(height);

    m_graphics->orthographic_params()
              .Right = static_cast<float>(width);
    m_graphics->orthographic_params()
              .Top = static_cast<float>(height);
}

std::string_view GraphicsController::name() const { return "GraphicsController"; }

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
    shader->set_mat4("projection", projection_matrix<>());
    CHECKED_GL_CALL(glDepthFunc, GL_LEQUAL);
    CHECKED_GL_CALL(glBindVertexArray, skybox->vao());
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, skybox->texture());
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 36);
    CHECKED_GL_CALL(glBindVertexArray, 0);
    CHECKED_GL_CALL(glDepthFunc, GL_LESS);// set depth function back to default
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, 0);
}

void GraphicsController::init_msaa_framebuffer(int width, int height, int samples) {
    float quadVertices[] = {
            // positions   // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
    };

    uint32_t quadVBO;
    glGenVertexArrays(1, &Framebuffer::m_quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(Framebuffer::m_quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

    glGenFramebuffers(1, &Framebuffer::m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::m_fbo);
    glGenTextures(1, &Framebuffer::m_textureColorBufferMultiSampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, Framebuffer::m_textureColorBufferMultiSampled);

    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D_MULTISAMPLE, Framebuffer::m_textureColorBufferMultiSampled, 0);

    glGenRenderbuffers(1, &Framebuffer::m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, Framebuffer::m_rbo);

    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Framebuffer::m_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { spdlog::info("Framebuffer is not complete"); }

    glGenFramebuffers(1, &Framebuffer::m_intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::m_intermediateFBO);

    glGenTextures(1, &Framebuffer::m_screenTexture);
    glBindTexture(GL_TEXTURE_2D, Framebuffer::m_screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Framebuffer::m_screenTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { spdlog::info("Framebuffer is not complete"); }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsController::bind_msaa_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::m_fbo);
}

void GraphicsController::unbind_msaa_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsController::resolve_msaa(int width, int height) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, Framebuffer::m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Framebuffer::m_intermediateFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0,
                      width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void GraphicsController::draw_quad(const resources::Shader *shader) {
    shader->use();
    glBindVertexArray(Framebuffer::m_quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Framebuffer::m_screenTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GraphicsController::add_point_shadow_caster(int shadowWidth, int shadowHeight, float nearPlane, float farPlane) {
    auto caster = PointShadowCaster();
    caster.init(shadowWidth, shadowHeight, nearPlane, farPlane);
    m_shadowCasters.push_back(PointShadowCaster());
}

void GraphicsController::render_point_light_shadows(resources::Shader *shader, int i) {
    m_shadowCasters[i].set_shader(shader);
    m_shadowCasters[i].render_pass();
}

void GraphicsController::bind_point_light_shadows_to_shader(resources::Shader *shader, int i, const std::string &name) {
    int texUnitBase = 5;

    std::string namePos = name + ".position";
    std::string nameFar = name + ".farPlane";
    std::string nameMap = name + ".shadowMap";

    m_shadowCasters[i].bind_to_shader(shader, namePos, nameFar, nameMap, texUnitBase + i);
}

void GraphicsController::set_shadow_caster_position(glm::vec3 &position, int i) { if (i != -1) { m_shadowCasters[i].set_position(position); } else { m_shadowCasters.back().set_position(position); } }

}
