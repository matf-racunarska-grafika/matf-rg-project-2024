//
// Created by matfrg on 10/11/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include<engine/core/Controller.hpp>
#include "glm/vec3.hpp"

namespace app {

class MainController : public engine::core::Controller{
    void initialize() override;
    bool loop() override;
    void draw_island();
    void draw_model(const std::string& model_name,
        const std::string& shader_name,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(1.0f),
        const glm::vec3& rotation_axis = glm::vec3(0.0f),
        float rotation_angle = 0.0f);
    void update_camera();
    void update() override;
    void begin_draw() override;
    void draw_skybox();
    void draw() override;
    void end_draw() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

} // app

#endif //MAINCONTROLLER_HPP
