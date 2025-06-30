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

    // ──────────── Point shadows ──────────────────────────────────────────────────────────────
    // 1) Depth cubemap setup
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     GL_DEPTH_COMPONENT,
                     SHADOW_WIDTH, SHADOW_HEIGHT,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) spdlog::error("Depth cubemap FBO nije complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // ──────────────────────────────────────────────────────────────

    // ------------- Compute shadow matrices -------------
    glm::mat4 shadowProj = glm::perspective(
            glm::radians(90.0f),
            float(SHADOW_WIDTH) / float(SHADOW_HEIGHT),
            near_plane,
            far_plane
            );

    shadowMatrices[0] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0));
    shadowMatrices[1] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
    shadowMatrices[2] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    shadowMatrices[3] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
    shadowMatrices[4] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));
    shadowMatrices[5] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));

    // ─── MSAA off-screen FBO setup ───────────────────────────────
    glEnable(GL_MULTISAMPLE);

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    width = vp[2];
    height = vp[3];

    // 1)
    glGenFramebuffers(1, &msFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, msFBO);

    // 2)
    glGenRenderbuffers(1, &msColorRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, msColorRBO);
    glRenderbufferStorageMultisample(
            GL_RENDERBUFFER,
            MSAA_SAMPLES,
            GL_RGBA8,
            width,
            height
            );
    glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_RENDERBUFFER,
            msColorRBO
            );

    // 3)
    glGenRenderbuffers(1, &msDepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, msDepthRBO);
    glRenderbufferStorageMultisample(
            GL_RENDERBUFFER,
            MSAA_SAMPLES,
            GL_DEPTH24_STENCIL8,
            width,
            height
            );
    glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            msDepthRBO
            );

    // 4)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { spdlog::error("MSAA FBO is not complete!"); }

    // 5)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // ───────────────────────────────────────────────────────────────

    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(
            std::move(observer));
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
    // 1)
    update_camera();

    // 2)
    std::vector<ScheduledEvent> newEvents;

    // 3)
    for (auto it = eventQueue.begin(); it != eventQueue.end(); /* nista */) {
        // DEBUG
        spdlog::info("  [debug] eventQueue[{}] = '{}' @ triggerTime={:.2f}, currentTime={:.2f}",
                     std::distance(eventQueue.begin(), it),
                     it->eventName,
                     it->triggerTime,
                     currentTime);

        if (currentTime >= it->triggerTime) {
            // izvrši event
            executeEvent(it->eventName);

            if (it->eventName == "START_FLICKER") {
                constexpr float N = 3.0f;
                newEvents.push_back({currentTime + N, "SPAWN_MODEL"});
                spdlog::info("  zakazujem SPAWN_MODEL za +{:.2f}s", N);
            }

            it = eventQueue.erase(it);
        } else { ++it; }
    }

    // 4)
    if (!newEvents.empty()) { eventQueue.insert(eventQueue.end(), newEvents.begin(), newEvents.end()); }

    // 5)
    if (flickerActive) {
        float elapsed = currentTime - flickerStartTime;
        constexpr float freq = 5.0f;
        pointLightIntensity = (sinf(2.0f * 3.14159265f * freq * elapsed) + 1.0f) * 0.5f;
    }
}

void MainController::begin_draw() {
    glBindFramebuffer(GL_FRAMEBUFFER, msFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void MainController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    // 1)
    {
        glm::mat4 shadowProj = glm::perspective(
                glm::radians(90.0f),
                float(SHADOW_WIDTH) / float(SHADOW_HEIGHT),
                near_plane,
                far_plane
                );
        const glm::vec3 &pos = lightPos;// lightPos је glm::vec3
        shadowMatrices[0] = shadowProj * glm::lookAt(pos, pos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0));
        shadowMatrices[1] = shadowProj * glm::lookAt(pos, pos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
        shadowMatrices[2] = shadowProj * glm::lookAt(pos, pos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
        shadowMatrices[3] = shadowProj * glm::lookAt(pos, pos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
        shadowMatrices[4] = shadowProj * glm::lookAt(pos, pos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));
        shadowMatrices[5] = shadowProj * glm::lookAt(pos, pos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));
    }

    // ====== DEPTH PASS ======
    glDisable(GL_MULTISAMPLE);
    auto depthShader = resources->shader("point_shadows_depth");
    depthShader->use();

    for (int face = 0; face < 6; ++face) {
        depthShader->set_mat4("shadowMatrices[" + std::to_string(face) + "]",
                              shadowMatrices[face]);
    }
    depthShader->set_vec3("lightPos", lightPos);
    depthShader->set_float("far_plane", far_plane);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);
    renderSceneDepth(depthShader);
    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, msFBO);
    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, width, height);
    // =================================================

    // ====== MAIN PASS ======
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto Lit = resources->shader("lighting");
    Lit->use();
    Lit->set_mat4("projection", graphics->projection_matrix());
    Lit->set_mat4("view", graphics->camera()->view_matrix());

    Lit->set_float("uLightIntensity", pointLightIntensity);

    set_lights(Lit);

    Lit->use();

    Lit->set_bool("shadows", true);
    Lit->set_int("gNumPointLights", 1);
    Lit->set_float("far_plane", far_plane);

    Lit->set_vec3("lightPos", lightPos);

    Lit->set_int("shadowMap", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

    renderSceneLit(Lit);

    draw_skybox();

    // =================================================
}

void MainController::end_draw() {
    // 1) Resolve MSAA FBO -> default framebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, msFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    int w = vp[2], h = vp[3];

    glBlitFramebuffer(
            0, 0, w, h,
            0, 0, w, h,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // 2) Swap
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

// USER DEFINED
// ---------------------------------------------------------------------------------------------------------------------------

void MainController::renderSceneDepth(const resources::Shader *depthShader) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    // 1) Backpack 1
    glm::vec3 backpackPos = glm::vec3(-10.0f, 0.0f, 3.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), backpackPos);
    model = glm::scale(model, glm::vec3(0.5f));
    depthShader->set_mat4("model", model);
    resources->model("backpack")->draw(depthShader);

    // 2) Pole
    glm::mat4 poleModel = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, -7.0f, -5.0f));
    // Po potrebi skaliraj/rotiraj poleModel da bude odgovarajuće visine/orijentacije.
    // Ovde ćemo pretpostaviti uniformnu skalu 1.0 (bez rotacije):
    poleModel = glm::scale(poleModel, glm::vec3(2.0f));
    depthShader->set_mat4("model", poleModel);
    resources->model("pole")->draw(depthShader);

    // 3) Terrain
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                         glm::vec3(0.0f, -10.0f, 0.0f));
        model = glm::scale(model, glm::vec3(3.0f));
        depthShader->set_mat4("model", model);
        resources->model("terrain")->draw(depthShader);
    }

    // 4) Trees
    std::vector<glm::vec3> treePositions = {
            // Prvi set
            {26.0f, 3.0f, 0.0f},
            {-15.0f, 3.0f, 20.0f},
            {30.0f, 3.0f, -10.0f},
            {-20.0f, 3.0f, 10.0f},
            {26.0f, 3.0f, 10.0f},

            // Drugi set
            {-15.0f, 3.0f, -30.0f}
    };

    for (auto &pos: treePositions) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(model, glm::vec3(10.0f));
        depthShader->set_mat4("model", model);
        resources->model("tree")->draw(depthShader);
    }
}

void MainController::renderSceneLit(const resources::Shader *shader) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    // 1) Backpack 1
    glm::vec3 backpackPos = glm::vec3(-10.0f, 0.0f, 3.0f);
    draw_mesh(resources->model("backpack"), shader,
              backpackPos,     // pozicija
              glm::vec3(0.0f), // nema rotacije
              glm::vec3(0.5f));// skala 0.5

    // 2) Pole
    draw_mesh(resources->model("pole"), shader,
              glm::vec3(-8.0f, -7.0f, -5.0f),
              glm::vec3(0.0f), // bez rotacije
              glm::vec3(2.0f));// skala 1

    // 3) Terrain
    draw_mesh(resources->model("terrain"), shader,
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
            {-15.0f, 3.0f, -30.0f}
    };

    for (auto &pos: treePositions) {
        draw_mesh(resources->model("tree"), shader,
                  pos,
                  glm::vec3(0.0f, 0.0f, -80.0f),
                  glm::vec3(10.0f));
    }
}

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

void MainController::draw_light_source_mesh(const glm::vec3 &lightPos, float scale) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto basicShader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("unlit_white");
    auto cubeModel = engine::core::Controller::get<engine::resources::ResourcesController>()->model("light_bulb");

    basicShader->use();
    basicShader->set_mat4("projection", graphics->projection_matrix());
    basicShader->set_mat4("view", graphics->camera()->view_matrix());

    // Kreiraj model matricu tako da se kocka postavi na lightPos i skalira
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, lightPos);
    // rotacija od 90 stepeni oko X ose
    modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::scale(modelMat, glm::vec3(scale));
    basicShader->set_mat4("model", modelMat);

    cubeModel->draw(basicShader);
}

void MainController::set_lights(auto shader) {
    // 1) Directional light
    shader->set_vec3("gDirectionalLight.Base.Color", glm::vec3(1.0f));
    shader->set_float("gDirectionalLight.Base.AmbientIntensity", 0.1f);
    shader->set_float("gDirectionalLight.Base.DiffuseIntensity", 0.25f);
    shader->set_vec3("gDirectionalLight.Direction", glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)));

    // 2)
    shader->set_int("gNumPointLights", 1);

    // 3)
    glm::vec3 lp = lightPos;
    shader->set_vec3("gPointLights[0].Base.Color", glm::vec3(1.0f, 0.8f, 0.6f));
    shader->set_float("gPointLights[0].Base.AmbientIntensity", 0.1f);
    shader->set_float("gPointLights[0].Base.DiffuseIntensity", 1.0f);
    shader->set_vec3("gPointLights[0].LocalPos", lp);
    shader->set_float("gPointLights[0].Atten.Constant", 1.0f);
    shader->set_float("gPointLights[0].Atten.Linear", 0.09f);
    shader->set_float("gPointLights[0].Atten.Exp", 0.032f);

    // 4)
    shader->set_vec3("gMaterial.AmbientColor", glm::vec3(1.0f));
    shader->set_vec3("gMaterial.DiffuseColor", glm::vec3(1.0f));
    shader->set_vec3("gMaterial.SpecularColor", glm::vec3(1.0f));

    // 5)
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->set_vec3("gCameraLocalPos", graphics->camera()->Position);

    // 6)
    shader->set_int("gSampler", 0);
    shader->set_int("gSamplerSpecularExponent", 1);

    // 7)
    draw_light_source_mesh(lp, 3.0f);
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
    // Vertikalno kretanje
    if (platform->key(engine::platform::KEY_E).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::UP, dt * speedMultiplier); }
    if (platform->key(engine::platform::KEY_Q).state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt * speedMultiplier); }

    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

void MainController::executeEvent(const std::string &eventName) {
    float now = currentTime;

    if (eventName == "START_FLICKER") {
        // 1)
        flickerActive = true;
        flickerStartTime = now;
        spdlog::info("EVENT START_FLICKER: flickerActive = true");
    } else if (eventName == "SPAWN_MODEL") {
        // 2)
        flickerActive = false;
        pointLightIntensity = 5.0f;
        spdlog::info("EVENT SPAWN_MODEL: flickerActive = false, intensity reset");

        // 3)
        spawnedObjects.emplace_back(
                "tree",
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f),
                glm::vec3(1.0f)
                );
        spdlog::info("Spawnovan novi model: tree");
    } else { spdlog::warn("executeEvent: nepoznat event '{}'", eventName); }
}

}
