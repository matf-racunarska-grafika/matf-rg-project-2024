#include <MyModel.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace app {

MyModel::MyModel(engine::resources::Model * model, glm::vec3 position, float scale, float yaw, float pitch, float roll)
    : m_model(model)
{
    m_transform = glm::mat4(1.0f);
    m_transform = glm::translate(m_transform, position);
    m_transform = glm::rotate(m_transform, glm::radians(yaw),   glm::vec3(0.0f, 1.0f, 0.0f));
    m_transform = glm::rotate(m_transform, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    m_transform = glm::rotate(m_transform, glm::radians(roll),  glm::vec3(0.0f, 0.0f, 1.0f));
    m_transform = glm::scale(m_transform, glm::vec3(scale, scale, scale));
}

void MyModel::prepare_for_draw(engine::resources::Shader * s) {
    s->use();
    s->set_mat4("model", m_transform);
}

void MyModel::draw(engine::resources::Shader * s) {
    prepare_for_draw(s);
    m_model->draw(s);
}

engine::resources::Model * MyModel::model() const {
    return m_model;
}

glm::mat4 MyModel::transform() const {
    return m_transform;
}

} // namespace app
