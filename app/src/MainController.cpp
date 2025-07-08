#include <memory>
#include <spdlog/spdlog.h>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <app/MainController.hpp>
#include <app/GUIController.hpp>

namespace engine::myapp {

void MainController::initialize() {
    // User initialization
    engine::graphics::OpenGL::enable_depth_testing();

    // ─── Lighting ─────────────────────────────────────────────────
    m_lighting.initialize();

    // 1) Directional light
    graphics::lighting::DirectionalLight dl;
    dl.base.color = glm::vec3(1.0f);
    dl.base.ambientIntensity = 0.1f;
    dl.base.diffuseIntensity = 0.25f;
    dl.direction = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
    m_lighting.setDirectionalLight(dl);

    // 2) Point light
    graphics::lighting::PointLight pl;
    pl.position = g_light_pos;
    pl.base.color = glm::vec3(1.0f, 0.8f, 0.6f);
    pl.base.ambientIntensity = 0.1f;
    pl.base.diffuseIntensity = 1.0f;
    pl.atten = {1.0f, 0.09f, 0.032f};
    m_lighting.setPointLight(pl);

    // 3) Material
    graphics::lighting::Material mat;
    mat.ambientColor = glm::vec3(1.0f);
    mat.diffuseColor = glm::vec3(1.0f);
    mat.specularColor = glm::vec3(1.0f);
    m_lighting.setMaterial(mat);

    // 4) Pocetni intenzitet svetla
    m_lighting.setLightIntensity(g_point_light_intensity);
    // ───────────────────────────────────────────────────────────────

    // ─── MSAA off-screen ─────────────────────────────────────────
    auto *platform = core::Controller::get<platform::PlatformController>();
    int width = platform->window()->width();
    int height = platform->window()->height();
    m_msaa = std::make_unique<engine::graphics::MSAA>(width, height, /*samples=*/4);
    // ───────────────────────────────────────────────────────────────
}

bool MainController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .state() == engine::platform::Key::State::JustPressed) { return false; }
    return true;
}

void MainController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    g_current_time += platform->dt();

    // 1)
    if (platform->key(engine::platform::KeyId::KEY_F1)
                .state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);
    }

    // 2)
    if (!g_action_triggered &&
        platform->key(engine::platform::KeyId::KEY_L).state() == engine::platform::Key::State::JustPressed) {
        g_action_triggered = true;
        g_action_triggered = g_current_time;// beležimo vreme pritiska
        constexpr float M = 2.0f;           // nakon 2 sekunde startujemo flicker
        g_event_queue.push_back({g_current_time + M, "START_FLICKER"});
        spdlog::info("L pritisnut → zakazujem START_FLICKER za +{:.2f}s", M);
    }
}

void MainController::update() {
    update_camera();

    std::vector<ScheduledEvent> newEvents;
    for (auto it = g_event_queue.begin(); it != g_event_queue.end();) {
        if (g_current_time >= it->triggerTime) {
            const auto name = it->eventName;
            execute_event(name);

            if (name == "START_FLICKER") {
                // 1)
                newEvents.push_back({g_current_time + g_flicker_duration, "STOP_FLICKER"});
                spdlog::info("  zakazujem STOP_FLICKER za +{:.2f}s", g_flicker_duration);
            } else if (name == "STOP_FLICKER") {
                // 2)
                newEvents.push_back({g_current_time + g_spawn_delay, "SPAWN_MODEL"});
                spdlog::info("  zakazujem SPAWN_MODEL za +{:.2f}s", g_spawn_delay);
            }

            it = g_event_queue.erase(it);
        } else { ++it; }
    }
    if (!newEvents.empty()) { g_event_queue.insert(g_event_queue.end(), newEvents.begin(), newEvents.end()); }

    if (g_flicker_active) {
        float elapsed = g_current_time - g_flicker_start_time;
        constexpr float freq = 5.0f;
        g_point_light_intensity = (sinf(2.0f * 3.14159f * freq * elapsed) + 1.0f) * 0.5f;
    }
}

void MainController::begin_draw() {
    auto *graphics = core::Controller::get<engine::graphics::GraphicsController>();

    if (g_msaa_enabled) { m_msaa->bindForWriting(); } else { graphics->bind_framebuffer(GL_FRAMEBUFFER, 0); }
    graphics->clear();
}

void MainController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    // 0) Ажурирај светло (овај позив мора бити овде, унутар draw-а)
    engine::graphics::lighting::PointLight pl;
    pl.base.color = glm::vec3(1.0f, 0.8f, 0.6f);
    pl.base.ambientIntensity = 0.1f;
    pl.base.diffuseIntensity = 1.0f;
    pl.position = g_light_pos;// GUI-vrednost
    pl.atten.constant = 1.0f;
    pl.atten.linear = 0.09f;
    pl.atten.exp = 0.032f;
    m_lighting.setPointLight(pl);

    // ───── DEPTH PASS ────────────────────────────────────────────────────────────────
    m_lighting.beginDepthPass();
    m_lighting.renderDepthPass([&](const engine::resources::Shader *depthShader) {
        depthShader->use();

        // 1) Well
        {
            glm::vec3 wellPos(-10.0f, -7.0f, 10.0f);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), wellPos);
            model = glm::scale(model, glm::vec3(0.2f));
            depthShader->set_mat4("model", model);
            resources->model("well")->draw(depthShader);
        }

        // 2) Poles
        {
            std::vector<glm::vec3> polePositions = {
                    {-8.0f, -7.0f, -5.0f},
                    {18.0f, -7.0f, -80.0f},
                    {70.0f, -7.0f, -150.0f}
            };
            for (auto &pos: polePositions) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                model = glm::scale(model, glm::vec3(2.0f));
                depthShader->set_mat4("model", model);
                resources->model("pole")->draw(depthShader);
            }
        }

        // 3) Terrain
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, 0.0f));
            model = glm::scale(model, glm::vec3(3.0f));
            depthShader->set_mat4("model", model);
            resources->model("terrain")->draw(depthShader);
        }

        // 4) Trees
        {
            std::vector<glm::vec3> treePositions = {
                    {26.0f, 3.0f, 0.0f}, {-15.0f, 3.0f, 20.0f}, {30.0f, 3.0f, -10.0f},
                    {-20.0f, 3.0f, 10.0f}, {26.0f, 3.0f, 10.0f}, {-15.0f, 3.0f, -30.0f},
                    {16.0f, 1.0f, -30.0f}, {0.0f, 4.0f, -130.0f}, {-2.0f, 4.0f, -100.0f},
                    {10.0f, 4.0f, -140.0f}, {40.0f, 2.0f, -130.0f}, {38.0f, 1.0f, -100.0f},
                    {50.0f, 2.0f, -140.0f}, {30.0f, 4.0f, -160.0f}, {40.0f, 4.0f, -190.0f}
            };
            for (auto &pos: treePositions) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                model = glm::scale(model, glm::vec3(10.0f));
                model = glm::rotate(model, -80.0f, glm::vec3(0, 0, 1));
                depthShader->set_mat4("model", model);
                resources->model("tree")->draw(depthShader);
            }
        }

        // 5) Cottage
        {
            glm::vec3 pos(0.0f, -3.0f, -125.0f);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
            model = glm::rotate(model, 2.0f, glm::vec3(0, 1, 0));
            model = glm::scale(model, glm::vec3(0.2f));
            depthShader->set_mat4("model", model);
            resources->model("cottage")->draw(depthShader);
        }

        // 6) Medieval House
        {
            glm::vec3 pos(50.0f, -8.0f, -20.0f);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
            model = glm::rotate(model, -4.7f, glm::vec3(0, 1, 0));
            model = glm::scale(model, glm::vec3(3.0f));
            depthShader->set_mat4("model", model);
            resources->model("medieval_house")->draw(depthShader);
        }

        // 7) Event spawner
        for (const auto &obj: g_spawned_objects) {
            const auto &name = obj.name;
            const auto &pos = obj.position;
            const auto &rot = obj.rotation;
            const auto &scale = obj.scale;

            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
            model = glm::rotate(model, rot.x, glm::vec3(1, 0, 0));
            model = glm::rotate(model, rot.y, glm::vec3(0, 1, 0));
            model = glm::rotate(model, rot.z, glm::vec3(0, 0, 1));
            model = glm::scale(model, scale);

            depthShader->set_mat4("model", model);
            resources->model(name)->draw(depthShader);
        }
    });
    m_lighting.endDepthPass();

    // umesto glBindFramebuffer(...)
    if (g_msaa_enabled) { m_msaa->bindForWriting(); } else { graphics->bind_framebuffer(GL_FRAMEBUFFER, 0); }

    auto *platform = core::Controller::get<platform::PlatformController>();
    int width = platform->window()->width();
    int height = platform->window()->height();
    glViewport(0, 0, width, height);

    m_lighting.setCameraPosition(graphics->camera()->Position);
    m_lighting.setLightIntensity(g_point_light_intensity);

    // ───── LIGHTING PASS ─────────────────────────────────────────────────────────────
    m_lighting.renderLightingPass([&](const engine::resources::Shader *lightShader) {
        lightShader->use();

        // Projection & view
        lightShader->set_mat4("projection", graphics->projection_matrix());
        lightShader->set_mat4("view", graphics->camera()->view_matrix());

        // Intenzitet point svetla
        lightShader->set_float("uLightIntensity", g_point_light_intensity);

        // Pozicija point svetla
        lightShader->set_vec3("lightPos", g_light_pos);

        // 1) Well
        draw_mesh(resources->model("well"), lightShader,
                  glm::vec3(-10.0f, -7.0f, 10.0f),
                  glm::vec3(0.0f),
                  glm::vec3(0.2f));

        // 2) Poles
        std::vector<glm::vec3> polePositions = {
                {-8.0f, -7.0f, -5.0f},
                {18.0f, -7.0f, -80.0f},
                {70.0f, -7.0f, -150.0f}
        };

        for (auto &pos: polePositions) {
            draw_mesh(resources->model("pole"), lightShader,
                      pos,
                      glm::vec3(0.0f),
                      glm::vec3(2.0f));
        }

        // 3) Terrain
        draw_mesh(resources->model("terrain"), lightShader,
                  glm::vec3(0.0f, -10.0f, 0.0f),
                  glm::vec3(0.0f), glm::vec3(3.0f));

        // 4) Trees
        std::vector<glm::vec3> treePositions = {
                // Prvi set
                {26.0f, 3.0f, 0.0f},
                {-15.0f, 3.0f, 20.0f},
                {30.0f, 3.0f, -10.0f},
                {-20.0f, 3.0f, 10.0f},
                {26.0f, 3.0f, 10.0f},

                // Drugi set
                {-15.0f, 3.0f, -30.0f},
                {16.0f, 1.0f, -30.0f},

                // Treci set
                {0.0f, 4.0f, -130.0f},
                {-2.0f, 4.0f, -100.0f},
                {10.0f, 4.0f, -140.0f},
                {40.0f, 2.0f, -130.0f},
                {38.0f, 1.0f, -100.0f},
                {50.0f, 2.0f, -140.0f},

                // Cetvrti set
                {30.0f, 4.0f, -160.0f},
                {40.0f, 4.0f, -190.0f}
        };

        for (auto &pos: treePositions) {
            draw_mesh(resources->model("tree"), lightShader,
                      pos,
                      glm::vec3(0.0f, 0.0f, -80.0f),
                      glm::vec3(10.0f));
        }

        // 5) Cottage
        draw_mesh(resources->model("cottage"), lightShader,
                  glm::vec3(0.0f, -3.0f, -125.0f),
                  glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.2f));

        // 6) Medieval House
        draw_mesh(resources->model("medieval_house"), lightShader,
                  glm::vec3(50.0f, -8.0f, -20.0f),
                  glm::vec3(0.0f, -4.7f, 0.0f),
                  glm::vec3(3.0f));

        for (const auto &obj: g_spawned_objects) {
            const auto &modelName = obj.name;
            const auto &pos = obj.position;
            const auto &rot = obj.rotation;
            const auto &scale = obj.scale;

            draw_mesh(
                    resources->model(modelName),
                    lightShader,
                    pos,
                    rot,
                    scale
                    );
        }
    });

    m_lighting.renderLightBulb(g_light_pos, 3.0f);

    // ───── SKYBOX ────────────────────────────────────────────────────────────────────
    draw_skybox();
}

void MainController::end_draw() {
    // 1) Resolve MSAA FBO -> default framebuffer
    if (g_msaa_enabled) { m_msaa->resolveToDefault(); }

    // 2) Swap
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

// USER DEFINED
// ---------------------------------------------------------------------------------------------------------------------------

void MainController::draw_mesh(auto model, auto shader,
                               const glm::vec3 &position,
                               const glm::vec3 &rotation,// Eulerovi uglovi u radijanima: (rotX, rotY, rotZ)
                               const glm::vec3 &scale) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    // Kreiramo model matricu
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, position);
    // Rotacija – prvo rotiramo oko X, pa Y, pa Z ose
    modelMat = glm::rotate(modelMat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::rotate(modelMat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::rotate(modelMat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMat = glm::scale(modelMat, scale);
    shader->set_mat4("model", modelMat);

    // Crtamo model
    model->draw(shader);
}

void MainController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

void MainController::update_camera() {
    auto gui = engine::core::Controller::get<GUIController>();
    if (gui->is_enabled()) { return; }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();

    // Ako je SHIFT pritisnut, povecaj faktor brzine
    float speedMultiplier = 2.0f;
    if (platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT).state() == engine::platform::Key::State::Pressed) { speedMultiplier = 4.0f; }

    if (platform->key(engine::platform::KEY_W).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt * speedMultiplier); }
    if (platform->key(engine::platform::KEY_S).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt * speedMultiplier); }
    if (platform->key(engine::platform::KEY_A).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt * speedMultiplier); }
    if (platform->key(engine::platform::KEY_D).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt * speedMultiplier); }
    // Vertikalno kretanje:
    if (platform->key(engine::platform::KEY_E).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::UP, dt * speedMultiplier); }
    if (platform->key(engine::platform::KEY_Q).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt * speedMultiplier); }

    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

void MainController::execute_event(const std::string &eventName) {
    if (eventName == "START_FLICKER") {
        g_flicker_active = true;
        g_flicker_active = g_current_time;
        spdlog::info("EVENT START_FLICKER");
    } else if (eventName == "STOP_FLICKER") {
        g_flicker_active = false;
        g_point_light_intensity = 7.0f;// resetujemo intenzitet
        spdlog::info("EVENT STOP_FLICKER");
    } else if (eventName == "SPAWN_MODEL") {
        g_spawned_objects.emplace_back(
                "police_car",
                glm::vec3{0.0f, -8.0f, -3.0f},
                glm::vec3(0.0f),
                glm::vec3(1.0f)
                );
        spdlog::info("EVENT SPAWN_MODEL: spawnovan tree");
    } else { spdlog::warn("execute_event: nepoznat event '{}'", eventName); }
}

}
