#include "../../engine/libs/glad/include/glad/glad.h"
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

#include <MainController.hpp>
#include <spdlog/spdlog.h>
#include <GL/gl.h>
#include "GuiController.hpp"
#include "../../engine/libs/glfw/include/GLFW/glfw3.h"


namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

// Lighting
glm::vec3 lightPos1(-4.2f, 25.0f, -45.0f);
glm::vec3 lightPos2(-4.33f, -0.4f, -3.93);
glm::vec3 lightPos3(-3.53f, -0.4f, -3.93f);


// Lightning colors
glm::vec3 sphereColor(0.7f, 0.8f, 1.0f);
glm::vec3 lightCubeColor = glm::vec3(1.0f, 1.0f, 0.0f);


void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (gui_controller->is_enabled()) {
        platform->set_enable_cursor(true);
        return;
    }

    // tell GLFW to capture our mouse
    platform->set_enable_cursor(false);

    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->rotate_camera(position.dx, position.dy, true);
}

void MainController::initialize() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();

    // --------------------------------floor--------------------------------
    float floorVertices[] = {
            // positions              // texture Coords     // Normal Coords
            15.0f, -2.5f, 15.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -15.0f, -2.5f, 15.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -15.0f, -2.5f, -15.0f, 0.0f, 2.0f, 0.0f, 1.0f, 0.0f,

            15.0f, -2.5f, 15.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -15.0f, -2.5f, -15.0f, 0.0f, 2.0f, 0.0f, 1.0f, 0.0f,
            15.0f, -2.5f, -15.0f, 2.0f, 2.0f, 0.0f, 1.0f, 0.0f
    };

    // Generate VAO and VBO
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);

    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // --------------------------------grass--------------------------------

    float grassVertices[] = {
            // positions         // texture Coords          // Normal Coords
            0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

            0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.5f, 0.0f, 1.0f, 0.0f

            // texture coords have swapped y coordinates because texture is flipped upside down
    };

    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);

    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // --------------------------------lightCube--------------------------------
    SphereMesh lightCube = generateSphereMesh(1.0f, 36, 18);

    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &lightCubeVBO);
    glGenBuffers(1, &lightCubeEBO);

    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, lightCube.vertices
                                           .size() * sizeof(float), &lightCube.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightCubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lightCube.indices
                                                   .size() * sizeof(unsigned int), &lightCube.indices[0],
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    lightCubeIndexCount = lightCube.indices
                                   .size();

    // --------------------------------sphere--------------------------------
    SphereMesh sphere = generateSphereMesh(1.0f, 36, 18);

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.vertices
                                        .size() * sizeof(float), &sphere.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.indices
                                                .size() * sizeof(unsigned int), &sphere.indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    sphereIndexCount = sphere.indices
                             .size();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .is_down()) {
        return false;
    }
    return true;
}

void MainController::set_model_lighting(engine::resources::Shader *shader) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->set_vec3("viewPos", graphics->camera()
                                        ->Position);

    // Light 1 (moon)
    shader->set_vec3("light1.position", lightPos1);
    shader->set_vec3("light1.ambient", sphereColor * 0.25f);//glm::vec3(0.5f));
    shader->set_vec3("light1.diffuse", sphereColor * 0.45f);//glm::vec3(0.5f));
    shader->set_vec3("light1.specular", sphereColor * 0.2f);//glm::vec3(1.0f));

    // Light 2 (lamp)
    shader->set_vec3("light2.position", lightPos2);
    shader->set_vec3("light2.ambient", lightCubeColor * 0.05f);//glm::vec3(0.15f, 0.12f, 0.05f));
    shader->set_vec3("light2.diffuse", lightCubeColor * 0.13f);//glm::vec3(0.9f, 0.75f, 0.85f));
    shader->set_vec3("light2.specular", lightCubeColor * 0.04f);//glm::vec3(1.0f, 0.85f, 0.95f));

    // Light 2 (lamp)
    shader->set_vec3("light2.position", lightPos3);
    shader->set_vec3("light2.ambient", lightCubeColor * 0.05f);//glm::vec3(0.15f, 0.12f, 0.05f));
    shader->set_vec3("light2.diffuse", lightCubeColor * 0.13f);//glm::vec3(0.9f, 0.75f, 0.85f));
    shader->set_vec3("light2.specular", lightCubeColor * 0.04f);//glm::vec3(1.0f, 0.85f, 0.95f));

    // Material
    shader->set_float("shininess", 16.0f);
}

void MainController::draw_manor() {
    // Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *manor = resources->model("SpookyManor");

    // Shader
    engine::resources::Shader *shader = resources->shader("modelShader");
    shader->use();
    set_model_lighting(shader);

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-7.0f, -2.5f, -10.0f));
    model = glm::scale(model, glm::vec3(4.0f));
    shader->set_mat4("model", model);
    manor->draw(shader);
}

void MainController::draw_street_lamp() {
    // Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *street_lamp = resources->model("StreetLamp");

    // Shader
    engine::resources::Shader *shader = resources->shader("modelShader");
    shader->use();
    set_model_lighting(shader);

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.93, -2.5f, -3.93));
    model = glm::scale(model, glm::vec3(0.0045f));
    shader->set_mat4("model", model);
    street_lamp->draw(shader);
}

void MainController::draw_tree() {
    // Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *tree = resources->model("Tree");

    // Shader
    engine::resources::Shader *shader = resources->shader("modelShader");
    shader->use();
    set_model_lighting(shader);

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());

    // Tree locations
    std::vector<glm::vec3> trees{
            glm::vec3(6.0f, -2.5f, -2.0f),
            glm::vec3(-6.5f, 0.0f, -7.51f),
            glm::vec3(-10.5f, 0.0f, 5.01f),
            glm::vec3(-3.5f, 0.0f, -17.51f),
            glm::vec3(-0.5f, 0.0f, 11.01f)
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
    engine::resources::Texture *texture = resources->texture("floor");

    engine::resources::Shader *shader = resources->shader("floorShader");
    shader->use();
    set_model_lighting(shader);

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());

    // Transform Model Matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f)); // Position the texture in the scene
    model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));    // it's a bit too big for our scene, so scale it down
    shader->set_mat4("model", model);

    // Bind the texture
    texture->bind(GL_TEXTURE0); // Binds texture to GL_TEXTURE0
    shader->set_int("texture1", 0); // Assuming the shader has a uniform 'texture1'

    // Bind the texture and draw
    glBindVertexArray(floorVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void MainController::draw_grass() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Texture *texture = resources->texture("grass");

    engine::resources::Shader *shader = resources->shader("grassShader");
    shader->use();

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());

    // Bind the texture
    texture->bind(GL_TEXTURE0); // Binds texture to GL_TEXTURE0
    shader->set_int("grassTexture", 0); // Assuming the shader has a uniform 'texture1'

    std::vector<glm::vec3> vegetation{
            glm::vec3(-1.5f, -2.0f, -4.48f),
            glm::vec3(1.5f, -2.0f, 0.51f),
            glm::vec3(6.0f, -2.0f, 3.7f),
            glm::vec3(-2.3f, -2.0f, -7.3f),
            glm::vec3(5.5f, -2.0f, -5.6f),
            glm::vec3(2.5f, -2.0f, 5.6f),
            glm::vec3(3.7f, -2.0f, -3.6f),
            glm::vec3(-1.8f, -2.0f, 2.6f),
            glm::vec3(-5.0f, -2.0f, -7.6f),
            glm::vec3(0.0f, -2.0f, 0.0f)
    };

    for (unsigned int i = 0; i < vegetation.size(); i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, vegetation[i]); // Position the texture in the scene
        shader->set_mat4("model", model);

        // Bind the texture and draw
        glBindVertexArray(grassVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("forest_skybox");

    auto shader = resources->shader("skybox");
    set_model_lighting(shader);
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw_light_cube() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Shader *shader = resources->shader("lightCubeShader");
    shader->use();
    shader->set_vec3("lightCubeColor", lightCubeColor);

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());

    std::vector<glm::vec3> lightPos;
    lightPos.push_back(lightPos2);
    lightPos.push_back(lightPos3);
    for (int i = 0; i < lightPos.size(); i++) {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, lightPos[i]);
        model = glm::scale(model, glm::vec3(0.05f));
        shader->set_mat4("model", model);


        glBindVertexArray(lightCubeVAO);
        glDrawElements(GL_TRIANGLES, lightCubeIndexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void MainController::draw_sphere() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Shader *shader = resources->shader("sphereShader");
    shader->use();
    shader->set_vec3("sphereColor", sphereColor);

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos1);
    model = glm::scale(model, glm::vec3(5.0f)); // a smaller cube
    shader->set_mat4("model", model);


    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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

    if (platform->key(engine::platform::KeyId::KEY_1)
                .is_down()) {
        sphereColor = glm::vec3(0.7f, 0.8f, 1.0f);
    }
    if (platform->key(engine::platform::KeyId::KEY_2)
                .is_down()) {
        sphereColor = glm::vec3(0.541f, 0.0118f, 0.0118f);
    }

    if (platform->key(engine::platform::KeyId::KEY_3)
                .is_down()) {
        lightCubeColor = glm::vec3(1.0f, 1.0f, 0.0f);
    }
    if (platform->key(engine::platform::KeyId::KEY_4)
                .is_down()) {
        lightCubeColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }
}

MainController::SphereMesh MainController::generateSphereMesh(float radius, unsigned int sectors, unsigned int stacks) {
    SphereMesh mesh;

    float x, y, z, xy;
    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;

    for (unsigned int i = 0; i <= stacks; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (unsigned int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            mesh.vertices
                .push_back(x);
            mesh.vertices
                .push_back(y);
            mesh.vertices
                .push_back(z);
        }
    }

    // GENERISANJE INDEKSA
    for (unsigned int i = 0; i < stacks; ++i) {
        unsigned int k1 = i * (sectors + 1);
        unsigned int k2 = k1 + sectors + 1;

        for (unsigned int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                mesh.indices
                    .push_back(k1);
                mesh.indices
                    .push_back(k2);
                mesh.indices
                    .push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                mesh.indices
                    .push_back(k1 + 1);
                mesh.indices
                    .push_back(k2);
                mesh.indices
                    .push_back(k2 + 1);
            }
        }
    }
    return mesh;
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
    draw_grass();
    draw_skybox();
    draw_light_cube();
    draw_sphere();
    // swapBuffers
}

void MainController::deinitialize() {
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);

    glDeleteVertexArrays(1, &grassVAO);
    glDeleteBuffers(1, &grassVBO);

    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &lightCubeVBO);

    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();

    //deinitialize();
}

} // app