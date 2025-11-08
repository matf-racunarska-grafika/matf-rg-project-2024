#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/VisionController.hpp>
#include <spdlog/spdlog.h>

namespace engine {
namespace vision {

VisionController::~VisionController() {
    cleanup_framebuffers();

    if (m_quad_vao != 0) {
        glDeleteVertexArrays(1, &m_quad_vao);
    }
    if (m_quad_vbo != 0) {
        glDeleteBuffers(1, &m_quad_vbo);
    }
}

void VisionController::initialize() {
    spdlog::info("VisionController::initialize");

    auto platform = core::Controller::get<platform::PlatformController>();
    m_width = platform->window()->width();
    m_height = platform->window()->height();

    load_shaders();

    create_framebuffers();

    float quad_vertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_quad_vao);
    glGenBuffers(1, &m_quad_vbo);

    glBindVertexArray(m_quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    m_initialized = true;
    spdlog::info("VisionController initialized - Press 'B' to toggle bloom!");
}

void VisionController::update() {
    auto platform = core::Controller::get<platform::PlatformController>();
    int current_width = platform->window()->width();
    int current_height = platform->window()->height();

    if (current_width != m_width || current_height != m_height) {
        m_width = current_width;
        m_height = current_height;
        resize_framebuffers(m_width, m_height);
        spdlog::debug("VisionController: Resized framebuffers to {}x{}", m_width, m_height);
    }
}



void VisionController::begin_scene() {
    if (!m_initialized || !m_bloom_enabled) {
        return;
    }

    if (m_in_scene) {
        spdlog::warn("VisionController::begin_scene() called while already in scene");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_scene_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_in_scene = true;
}

void VisionController::end_scene() {
    if (!m_initialized) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    if (!m_in_scene) {
        //spdlog::warn("VisionController::end_scene() called without begin_scene()");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (m_bloom_enabled) {
        render_bright_pass();
        render_blur_pass();
        render_composite();
    } else {
        render_passthrough();
    }

    m_in_scene = false;
}

void VisionController::create_framebuffers() {
    spdlog::debug("Creating bloom framebuffers ({}x{})", m_width, m_height);

    glGenFramebuffers(1, &m_scene_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_scene_fbo);

    glGenTextures(1, &m_scene_color_buffer);
    glBindTexture(GL_TEXTURE_2D, m_scene_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_scene_color_buffer, 0);

    glGenRenderbuffers(1, &m_scene_depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_scene_depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_scene_depth_buffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::error("Scene framebuffer is not complete!");
    }

    glGenFramebuffers(1, &m_bright_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_bright_fbo);

    glGenTextures(1, &m_bright_color_buffer);
    glBindTexture(GL_TEXTURE_2D, m_bright_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bright_color_buffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::error("Bright pass framebuffer is not complete!");
    }

    glGenFramebuffers(2, m_ping_pong_fbo);
    glGenTextures(2, m_ping_pong_color_buffers);

    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ping_pong_fbo[i]);
        glBindTexture(GL_TEXTURE_2D, m_ping_pong_color_buffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ping_pong_color_buffers[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            spdlog::error("Ping-pong framebuffer {} is not complete!", i);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    spdlog::debug("Bloom framebuffers created successfully");
}

void VisionController::cleanup_framebuffers() {
    if (m_scene_fbo != 0) {
        glDeleteFramebuffers(1, &m_scene_fbo);
        glDeleteTextures(1, &m_scene_color_buffer);
        glDeleteRenderbuffers(1, &m_scene_depth_buffer);
        m_scene_fbo = 0;
        m_scene_color_buffer = 0;
        m_scene_depth_buffer = 0;
    }

    if (m_bright_fbo != 0) {
        glDeleteFramebuffers(1, &m_bright_fbo);
        glDeleteTextures(1, &m_bright_color_buffer);
        m_bright_fbo = 0;
        m_bright_color_buffer = 0;
    }

    if (m_ping_pong_fbo[0] != 0) {
        glDeleteFramebuffers(2, m_ping_pong_fbo);
        glDeleteTextures(2, m_ping_pong_color_buffers);
        m_ping_pong_fbo[0] = 0;
        m_ping_pong_fbo[1] = 0;
        m_ping_pong_color_buffers[0] = 0;
        m_ping_pong_color_buffers[1] = 0;
    }
}

void VisionController::resize_framebuffers(int width, int height) {
    cleanup_framebuffers();
    create_framebuffers();
}

void VisionController::render_bright_pass() {
    auto resources = core::Controller::get<resources::ResourcesController>();
    auto shader = resources->shader(m_bloom_shader);

    if (!shader) {
        spdlog::error("Bloom shader not found!");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_bright_fbo);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();
    shader->set_int("mode", 0);  // bright pass
    shader->set_int("sceneTexture", 0);
    shader->set_float("threshold", m_bloom_threshold);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_scene_color_buffer);

    render_quad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VisionController::render_blur_pass() {
    auto resources = core::Controller::get<resources::ResourcesController>();
    auto shader = resources->shader(m_bloom_shader);

    if (!shader) {
        spdlog::error("Bloom shader not found!");
        return;
    }

    shader->use();
    shader->set_int("mode", 1);
    shader->set_int("brightTexture", 0);

    bool horizontal = true;
    bool first_iteration = true;

    for (int i = 0; i < m_blur_passes; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ping_pong_fbo[horizontal]);
        shader->set_bool("horizontal", horizontal);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? m_bright_color_buffer : m_ping_pong_color_buffers[!horizontal]);

        render_quad();

        horizontal = !horizontal;
        if (first_iteration) {
            first_iteration = false;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VisionController::render_composite() {
    auto resources = core::Controller::get<resources::ResourcesController>();
    engine::resources::Shader* shader = resources->shader(m_bloom_shader);

    if (!shader) {
        spdlog::error("Bloom shader not found!");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();
    shader->set_int("mode", 2);  // composite
    shader->set_int("sceneTexture", 0);
    shader->set_int("bloomTexture", 1);
    shader->set_float("bloomIntensity", m_bloom_intensity);
    shader->set_float("exposure", m_exposure);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_scene_color_buffer);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_ping_pong_color_buffers[0]);

    render_quad();
}

void VisionController::render_quad() {
    glBindVertexArray(m_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void VisionController::render_passthrough() {
    auto resources = core::Controller::get<resources::ResourcesController>();
    auto shader = resources->shader(m_bloom_shader);

    if (!shader) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_scene_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height,
                         GL_COLOR_BUFFER_BIT, GL_NEAREST);
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();
    shader->set_int("mode", 3);  // passthrough mode
    shader->set_int("sceneTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_scene_color_buffer);

    render_quad();
}

void VisionController::load_shaders() {
    auto resources = core::Controller::get<resources::ResourcesController>();

    if (!resources->shader(m_bloom_shader)) {
        spdlog::warn("Bloom shader '{}' not found - bloom may not work", m_bloom_shader);
    } else {
        spdlog::info("Bloom shader loaded successfully");
    }
}

}
}