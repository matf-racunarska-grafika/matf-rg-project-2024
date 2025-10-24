#include "../../engine/libs/glad/include/glad/glad.h"
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

#include <MainController.hpp>
#include <spdlog/spdlog.h>
#include <GL/gl.h>
#include "GuiController.hpp"


namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    if (!gui_controller->is_enabled()) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx, position.dy);
    }
}

void MainController::initialize() {
    //auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    //platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .is_down()) {
        return false;
    }
    return true;
}


void MainController::draw_manor() {
    // Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *manor = resources->model("SpookyManor");

    // Shader
    engine::resources::Shader *shader = resources->shader("basic"); // treba modelShader
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-7.0f, -2.3f, -10.0f));
    model = glm::scale(model, glm::vec3(4.0f));
    shader->set_mat4("model", model);
    manor->draw(shader);
}

void MainController::draw_street_lamp() {
    // Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *street_lamp = resources->model("Street lamp");

    // Shader
    engine::resources::Shader *shader = resources->shader("basic"); // treba modelShader
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.93f));
    model = glm::scale(model, glm::vec3(0.2f));
    shader->set_mat4("model", model);
    street_lamp->draw(shader);
}

void MainController::draw_tree() {
    // Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *tree = resources->model("Tree");

    // Shader
    engine::resources::Shader *shader = resources->shader("basic"); // treba modelShader
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());

    // Tree locations
    std::vector<glm::vec3> trees{
            glm::vec3(6.0f, -2.5f, -2.0f),
            glm::vec3(-6.5f, 0.0f, -7.51f),
            glm::vec3(-10.5f, 0.0f, 5.01f)
    };

    glm::mat4 model = glm::mat4(1.0f);
    for (int i = 0; i < trees.size(); i++) {
        model = glm::translate(model, trees[i]); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down

        shader->set_mat4("model", model);
        tree->draw(shader);
    }
}

void MainController::draw_floor() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Texture *texture = resources->texture("floor1");

    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());

    // Transform Model Matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f)); // Position the texture in the scene
    shader->set_mat4("model", model);

    // Bind the texture
    texture->bind(GL_TEXTURE0); // Binds texture to GL_TEXTURE0
    shader->set_int("texture1", 0); // Assuming the shader has a uniform 'texture1'

    // Step 3: Set up the quad (texture coordinates included)
    float quadVertices[] = {
            // positions        // texture coords
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // Bottom-left corner
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right corner
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // Top-right corner

            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // Bottom-left corner
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // Top-right corner
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // Top-left corner
    };

    // Generate VAO and VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Bind the texture and draw
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Cleanup
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("forest_skybox");

    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::update_camera() {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    if (gui_controller->is_enabled()) {
        return;
    }

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();

    if (platform->key(engine::platform::KeyId::KEY_W)
                .is_down() || platform->key(engine::platform::KeyId::KEY_UP)
                                      .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_S)
                .is_down() || platform->key(engine::platform::KeyId::KEY_DOWN)
                                      .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_A)
                .is_down() || platform->key(engine::platform::KeyId::KEY_LEFT)
                                      .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_D)
                .is_down() || platform->key(engine::platform::KeyId::KEY_RIGHT)
                                      .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    }

}

void MainController::update() {
    update_camera();
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    // clear buffer (color buffer, depth buffer)
    draw_manor();
    draw_street_lamp();
    draw_tree();
    draw_floor();
    draw_skybox();
    // swapBuffers
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

} // app