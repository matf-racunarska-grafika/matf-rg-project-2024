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
    // -------------------------------------------------------------------------------

    // ─── MSAA off-screen FBO setup ───────────────────────────────
    glEnable(GL_MULTISAMPLE);

    //
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    int width = vp[2];
    int height = vp[3];

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
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F1)
                .state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);
    }
}

void MainController::update() { update_camera(); }

void MainController::begin_draw() {
    glBindFramebuffer(GL_FRAMEBUFFER, msFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// Scena
void MainController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    auto Unlit = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("basic");
    Unlit->use();
    Unlit->set_mat4("projection", graphics->projection_matrix());
    Unlit->set_mat4("view", graphics->camera()->view_matrix());
    Unlit->set_vec3("viewPos", graphics->camera()->Position);

    auto Lit = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("lighting");
    Lit->use();
    Lit->set_mat4("projection", graphics->projection_matrix());
    Lit->set_mat4("view", graphics->camera()->view_matrix());
    Lit->set_vec3("viewPos", graphics->camera()->Position);

    // Crtaj point light-ove
    set_lights(Lit);

    // Crtaj modele
    auto backpack = engine::core::Controller::get<engine::resources::ResourcesController>()->model("backpack");
    draw_mesh(backpack, Lit, glm::vec3(-5.0f, 0.0f, -7.0f), glm::vec3(0.0f), glm::vec3(0.5f));
    draw_mesh(backpack, Lit, glm::vec3(5.0f, 0.0f, -7.0f), glm::vec3(0.0f), glm::vec3(0.5f));

    auto terrain = engine::core::Controller::get<engine::resources::ResourcesController>()->model("terrain");
    draw_mesh(terrain, Lit, glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f), glm::vec3(3.0f));

    auto tree = engine::core::Controller::get<engine::resources::ResourcesController>()->model("tree");
    draw_mesh(tree, Lit, glm::vec3(26.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, -80.0f), glm::vec3(10.0f));
    draw_mesh(tree, Lit, glm::vec3(-15.0f, 3.0f, 20.0f), glm::vec3(0.0f, 0.0f, -80.0f), glm::vec3(10.0f));
    draw_mesh(tree, Lit, glm::vec3(30.0f, 3.0f, -10.0f), glm::vec3(0.0f, 0.0f, -80.0f), glm::vec3(10.0f));
    draw_mesh(tree, Lit, glm::vec3(-20.0f, 3.0f, 10.0f), glm::vec3(0.0f, 0.0f, -80.0f), glm::vec3(10.0f));
    draw_mesh(tree, Lit, glm::vec3(26.0f, 3.0f, 10.0f), glm::vec3(0.0f, 0.0f, -80.0f), glm::vec3(10.0f));

    // Crtaj skybox
    draw_skybox();
}

void MainController::end_draw() {
    // 1)
    glBindFramebuffer(GL_READ_FRAMEBUFFER, msFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    int w = vp[2], h = vp[3];

    glBlitFramebuffer(
            0, 0, w, h,
            0, 0, w, h,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // 2)
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

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, position);

    modelMat = glm::rotate(modelMat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::rotate(modelMat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::rotate(modelMat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMat = glm::scale(modelMat, scale);
    shader->set_mat4("model", modelMat);

    model->draw(shader);
}

void MainController::draw_light_source_mesh(const glm::vec3 &lightPos, float scale) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto basicShader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("unlit_white");
    auto cubeModel = engine::core::Controller::get<engine::resources::ResourcesController>()->model("light_bulb");

    basicShader->use();
    basicShader->set_mat4("projection", graphics->projection_matrix());
    basicShader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, lightPos);

    modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::scale(modelMat, glm::vec3(scale));
    basicShader->set_mat4("model", modelMat);

    cubeModel->draw(basicShader);
}

void MainController::set_lights(auto shader) {
    // 1) Directional light
    shader->set_vec3("gDirectionalLight.Base.Color", glm::vec3(1.0f, 1.0f, 1.0f));
    shader->set_float("gDirectionalLight.Base.AmbientIntensity", 0.2f);
    shader->set_float("gDirectionalLight.Base.DiffuseIntensity", 0.5f);
    shader->set_vec3("gDirectionalLight.Direction", glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)));

    // 2) Broj point light-ova
    shader->set_int("gNumPointLights", 2);

    // 3) Parametri point light-ova
    glm::vec3 lightPos[2] = {
            glm::vec3(-10.0f, 1.0f, 2.0f),
            glm::vec3(10.0f, 3.0f, 50.0f)
    };
    for (int i = 0; i < 2; ++i) {
        std::string base = "gPointLights[" + std::to_string(i) + "]";
        shader->set_vec3(base + ".Base.Color", glm::vec3(1.0f, 0.8f, 0.6f));
        shader->set_float(base + ".Base.AmbientIntensity", 0.1f);
        shader->set_float(base + ".Base.DiffuseIntensity", 1.0f);
        shader->set_vec3(base + ".LocalPos", lightPos[i]);
        shader->set_float(base + ".Atten.Constant", 1.0f);
        shader->set_float(base + ".Atten.Linear", 0.09f);
        shader->set_float(base + ".Atten.Exp", 0.032f);
    }

    // 4) Materijal
    shader->set_vec3("gMaterial.AmbientColor", glm::vec3(1.0f));
    shader->set_vec3("gMaterial.DiffuseColor", glm::vec3(1.0f));
    shader->set_vec3("gMaterial.SpecularColor", glm::vec3(1.0f));

    // 5) Kamera u world-space za Phong specular
    {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        glm::vec3 camPos = graphics->camera()->Position;
        shader->set_vec3("gCameraLocalPos", camPos);
    }

    // 6) Teksturne jedinice
    shader->set_int("gSampler", 0);                // diffuse texture na slotu 0
    shader->set_int("gSamplerSpecularExponent", 1);// specular exponent map na slotu 1

    // 7) Mesh na poziciji izvora point light-ova
    draw_light_source_mesh(lightPos[0], 3.0f);// prvi izvor svetla
    draw_light_source_mesh(lightPos[1], 3.0f);// drugi izvor svetla
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

}