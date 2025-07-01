#include <glad/glad.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <app/MainController.hpp>
#include <app/GUIController.hpp>

namespace engine::myapp {
void MainPlatformEventObserver::on_key(engine::platform::Key key) { spdlog::info("Keyboard event: key={}, state={}", key.name(), key.state_str()); }

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) { spdlog::info("MousePosition: {} {}", position.x, position.y); }

void MainController::initialize() {
    // User initialization
    engine::graphics::OpenGL::enable_depth_testing();

    // ─── Lighting ─────────────────────────────────────────────────
    lighting.initialize();

    // 1) Directional light
    graphics::lighting::DirectionalLight dl;
    dl.base.color = glm::vec3(1.0f);
    dl.base.ambientIntensity = 0.1f;
    dl.base.diffuseIntensity = 0.25f;
    dl.direction = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
    lighting.setDirectionalLight(dl);

    // 2) Point light
    graphics::lighting::PointLight pl;
    pl.position = lightPos;
    pl.base.color = glm::vec3(1.0f, 0.8f, 0.6f);
    pl.base.ambientIntensity = 0.1f;
    pl.base.diffuseIntensity = 1.0f;
    pl.atten = {1.0f, 0.09f, 0.032f};
    lighting.setPointLight(pl);

    // 3) Material
    graphics::lighting::Material mat;
    mat.ambientColor = glm::vec3(1.0f);
    mat.diffuseColor = glm::vec3(1.0f);
    mat.specularColor = glm::vec3(1.0f);
    lighting.setMaterial(mat);

    // 4) Pocetni intenzitet svetla
    lighting.setLightIntensity(pointLightIntensity);
    // ───────────────────────────────────────────────────────────────

    // ─── MSAA off-screen ─────────────────────────────────────────
    glEnable(GL_MULTISAMPLE);
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    width = vp[2];
    height = vp[3];
    _msaa = std::make_unique<engine::graphics::MSAA>(width, height, /*samples=*/4);
    // ───────────────────────────────────────────────────────────────

    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()
            ->register_platform_event_observer(std::move(observer));
}

bool MainController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .state() == engine::platform::Key::State::JustPressed) { return false; }
    return true;
}

void MainController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    currentTime += platform->dt();

    // 1)
    if (platform->key(engine::platform::KeyId::KEY_F1)
                .state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);
    }

    // 2)
    if (!actionTriggered &&
        platform->key(engine::platform::KeyId::KEY_L).state() == engine::platform::Key::State::JustPressed) {
        actionTriggered = true;
        actionTriggerTime = currentTime;// beležimo vreme pritiska
        constexpr float M = 2.0f;       // nakon 2 sekunde startujemo flicker
        eventQueue.push_back({currentTime + M, "START_FLICKER"});
        spdlog::info("L pritisnut → zakazujem START_FLICKER za +{:.2f}s", M);
    }
}

void MainController::update() {
    update_camera();

    std::vector<ScheduledEvent> newEvents;
    for (auto it = eventQueue.begin(); it != eventQueue.end();) {
        if (currentTime >= it->triggerTime) {
            const auto name = it->eventName;
            executeEvent(name);

            if (name == "START_FLICKER") {
                // 1)
                newEvents.push_back({currentTime + flickerDuration, "STOP_FLICKER"});
                spdlog::info("  zakazujem STOP_FLICKER za +{:.2f}s", flickerDuration);
            } else if (name == "STOP_FLICKER") {
                // 2)
                newEvents.push_back({currentTime + spawnDelay, "SPAWN_MODEL"});
                spdlog::info("  zakazujem SPAWN_MODEL za +{:.2f}s", spawnDelay);
            }

            it = eventQueue.erase(it);
        } else { ++it; }
    }
    if (!newEvents.empty()) { eventQueue.insert(eventQueue.end(), newEvents.begin(), newEvents.end()); }

    if (flickerActive) {
        float elapsed = currentTime - flickerStartTime;
        constexpr float freq = 5.0f;
        pointLightIntensity = (sinf(2.0f * 3.14159f * freq * elapsed) + 1.0f) * 0.5f;
    }
}

void MainController::begin_draw() {
    if (msaaEnabled) { _msaa->bindForWriting(); } else { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void MainController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    // 0) Ажурирај светло (овај позив мора бити овде, унутар draw-а)
    engine::graphics::lighting::PointLight pl;
    pl.base.color = glm::vec3(1.0f, 0.8f, 0.6f);
    pl.base.ambientIntensity = 0.1f;
    pl.base.diffuseIntensity = 1.0f;
    pl.position = lightPos;// GUI-vrednost
    pl.atten.constant = 1.0f;
    pl.atten.linear = 0.09f;
    pl.atten.exp = 0.032f;
    lighting.setPointLight(pl);

    // ───── DEPTH PASS ────────────────────────────────────────────────────────────────
    lighting.beginDepthPass();
    lighting.renderDepthPass([&](const engine::resources::Shader *depthShader) {
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
                    {50.0f, 2.0f, -140.0f}
            };
            for (auto &pos: treePositions) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                model = glm::scale(model, glm::vec3(10.0f));
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
        for (auto &entry: spawnedObjects) {
            const auto &name = std::get<0>(entry);
            const auto &pos = std::get<1>(entry);
            const auto &rot = std::get<2>(entry);
            const auto &scale = std::get<3>(entry);

            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
            model = glm::rotate(model, rot.x, glm::vec3(1, 0, 0));
            model = glm::rotate(model, rot.y, glm::vec3(0, 1, 0));
            model = glm::rotate(model, rot.z, glm::vec3(0, 0, 1));
            model = glm::scale(model, scale);

            depthShader->set_mat4("model", model);
            resources->model(name)->draw(depthShader);
        }
    });
    lighting.endDepthPass();

    if (msaaEnabled) { _msaa->bindForWriting(); } else { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    glViewport(0, 0, width, height);

    lighting.setCameraPosition(graphics->camera()->Position);
    lighting.setLightIntensity(pointLightIntensity);

    // ───── LIGHTING PASS ─────────────────────────────────────────────────────────────
    lighting.renderLightingPass([&](const engine::resources::Shader *lightShader) {
        lightShader->use();

        // Projection & view
        lightShader->set_mat4("projection", graphics->projection_matrix());
        lightShader->set_mat4("view", graphics->camera()->view_matrix());

        // Intenzitet point svetla
        lightShader->set_float("uLightIntensity", pointLightIntensity);

        // Pozicija point svetla
        lightShader->set_vec3("lightPos", lightPos);

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
                {50.0f, 2.0f, -140.0f}
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

        // Event spawner
        for (auto &entry: spawnedObjects) {
            const std::string &modelName = std::get<0>(entry);
            const glm::vec3 &pos = std::get<1>(entry);
            const glm::vec3 &rot = std::get<2>(entry);
            const glm::vec3 &scale = std::get<3>(entry);

            draw_mesh(
                    resources->model(modelName),
                    lightShader,
                    pos,
                    rot,
                    scale
                    );
        }
    });

    lighting.renderLightBulb(lightPos, 3.0f);

    // ───── SKYBOX ────────────────────────────────────────────────────────────────────
    draw_skybox();
}

void MainController::end_draw() {
    // 1) Resolve MSAA FBO -> default framebuffer
    if (msaaEnabled) { _msaa->resolveToDefault(); }

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

void MainController::executeEvent(const std::string &eventName) {
    if (eventName == "START_FLICKER") {
        flickerActive = true;
        flickerStartTime = currentTime;
        spdlog::info("EVENT START_FLICKER");
    } else if (eventName == "STOP_FLICKER") {
        flickerActive = false;
        pointLightIntensity = 7.0f;// resetujemo intenzitet
        spdlog::info("EVENT STOP_FLICKER");
    } else if (eventName == "SPAWN_MODEL") {
        spawnedObjects.emplace_back(
                "police_car",
                glm::vec3{0.0f, -8.0f, -3.0f},
                glm::vec3(0.0f),
                glm::vec3(1.0f)
                );
        spdlog::info("EVENT SPAWN_MODEL: spawnovan tree");
    } else { spdlog::warn("executeEvent: nepoznat event '{}'", eventName); }
}

}
