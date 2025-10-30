//
// Created by matija on 10/30/25.
//

#ifndef MATF_RG_PROJECT_MYMODEL_HPP
#define MATF_RG_PROJECT_MYMODEL_HPP
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>
#include <glm/glm.hpp>

namespace app {

class MyModel {

private:
    engine::resources::Model * m_model;
    glm::mat4 m_transform;
    void prepare_for_draw(engine::resources::Shader * s);

public:
    void draw(engine::resources::Shader * s);
    MyModel(engine::resources::Model * model, glm::vec3 position, float scale, float yaw, float pitch, float roll);

    engine::resources::Model * model() const;
    glm::mat4 transform() const;
};

}// namespace app

#endif//MATF_RG_PROJECT_MYMODEL_HPP
