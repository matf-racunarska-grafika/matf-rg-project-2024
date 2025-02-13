#include <glad/glad.h>

#include <engine/core/App.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/util/Errors.hpp>
#include <spdlog/spdlog.h>

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/util/ArgParser.hpp>
#include <engine/util/Configuration.hpp>
#include <engine/util/Utils.hpp>

#include <iostream>

namespace engine::core {
    int App::run(int argc, char **argv) {
        try {
            engine_setup(argc, argv);
            app_setup();
            initialize();
            while (loop()) {
                poll_events();
                update();
                glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
                glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                draw();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                bloomset();
                auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
                platform->swap_buffers();
            }
            terminate();
        } catch (const util::Error &e) {
            handle_error(e);
            terminate();
        }
        return on_exit();
    }

    void App::engine_setup(int argc, char **argv) {
        util::ArgParser::instance()->initialize(argc, argv);
        util::Configuration::instance()->initialize();

        // register engine controllers
        auto begin     = register_controller<EngineControllersBegin>();
        auto platform  = register_controller<platform::PlatformController>();
        auto graphics  = register_controller<graphics::GraphicsController>();
        auto resources = register_controller<resources::ResourcesController>();
        auto end       = register_controller<EngineControllersEnd>();
        begin->before(platform);
        platform->before(graphics);
        graphics->before(resources);
        resources->before(end);
    }

    void App::renderQuad() {

        if (quadVAO == 0) {
            float quadVertices[] = {
                    // positions        // texture Coords
                    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                    1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void App::initialize() {
        // Topologically sort controllers based on their dependency graph formed by before/after methods.
        {
            auto adjacent_controllers = [](Controller *curr) { return curr->next(); };
            RG_GUARANTEE(!util::alg::has_cycle(range(m_controllers), adjacent_controllers),
                         "Please make sure that there are no cycles in the controller dependency graph.");
            util::alg::topological_sort(range(m_controllers), adjacent_controllers);
        }
        for (auto controller: m_controllers) {
            spdlog::info("{}::initialize", controller->name());
            controller->initialize();
        }

        // bloom
        glGenFramebuffers(1, &hdrFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

        glGenTextures(2, colorBuffers);

        for (unsigned int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
        }

        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, attachments);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete!\n";
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(2, pingpongFBO);
        glGenTextures(2, pingpongColorbuffers);
        for (unsigned int i = 0; i < 2; ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "Framebuffer not complete!\n";
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Shader *shader = resources->shader("basic");
        engine::resources::Shader *blur_shader = resources->shader("blur");
        engine::resources::Shader *bloom_final = resources->shader("bloom_final");
        engine::resources::Shader *light       = resources->shader("light");

        shader->use();
        shader->set_int("texture_diffuse1", 0);
        blur_shader->use();
        blur_shader->set_int("image",0);
        bloom_final->use();
        bloom_final->set_int("scene", 0);
        bloom_final->set_int("bloomBlur", 1);
    }

    void App::bloomset() {
        auto resources                    = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Shader *shader = resources->shader("basic");
        engine::resources::Shader *blur_shader = resources->shader("blur");
        engine::resources::Shader *bloom_final = resources->shader("bloom_final");
        engine::resources::Shader *light       = resources->shader("light");


        bool horizontal      = true;
        bool first_iteration = true;
        blur_shader->use();
        unsigned int amount = 10;
        for (unsigned int i = 0; i < amount; ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blur_shader->set_int("horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration) {
                first_iteration = false;
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        bloom_final->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        bloom_final->set_int("bloom", 1);
        bloom_final->set_float("exposure", 1.0f);
        renderQuad();
    }

    bool App::loop() {
        for (auto controller: m_controllers) {
            if (controller->is_enabled() && !controller->loop()) {
                return false;
            }
        }
        return true;
    }

    void App::poll_events() {
        for (auto controller: m_controllers) {
            // We don't check if the controller is enabled for poll_events because the controller may enable itself in
            // the poll_events if it needs to. For example, a GUIController may enable itself in the poll_events method
            // if a button to enable/disable the GUI was pressed.
            controller->poll_events();
        }
    }

    void App::update() {
        for (auto controller: m_controllers) {
            if (controller->is_enabled()) {
                controller->update();
            }
        }
    }

    void App::draw() {
        for (auto controller: m_controllers) {
            if (controller->is_enabled()) {
                controller->begin_draw();
            }
        }
        for (auto controller: m_controllers) {
            if (controller->is_enabled()) {
                controller->draw();
            }
        }
        for (auto controller: m_controllers) {
            if (controller->is_enabled()) {
                controller->end_draw();
            }
        }
    }

    void App::terminate() {
        // We terminate controllers in reverse order of their registration to ensure that controllers that depend on
        // other controllers are terminated last.
        for (auto it = m_controllers.rbegin(); it != m_controllers.rend(); ++it) {
            auto controller = *it;
            controller->terminate();
            spdlog::info("{}::terminate", controller->name());
        }
    }

    void App::app_setup() {
        RG_UNIMPLEMENTED("You should override App::app_setup in your App implementation.");
    }

    void App::handle_error(const util::Error &e) {
        spdlog::error(e.report());
    }
} // namespace engine::core
