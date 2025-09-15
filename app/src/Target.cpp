//
// Created by boce on 9/14/25.
//

#include "../include/Target.hpp"

#include "engine/core/App.hpp"
#include "engine/core/Controller.hpp"
#include "engine/graphics/GraphicsController.hpp"
#include "engine/resources/ResourcesController.hpp"

namespace app {

Target::Target(engine::resources::Model *model, const glm::vec3 &position) {
    m_model = model;
    m_position = position;
}

void Target::draw(const engine::resources::Shader *shader, const DirectionalLight &dirlight, const SpotLight &spotlight) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_position);
    model = glm::rotate(model, glm::radians(m_angle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(m_scale));
    shader->set_mat4("model", model);
    glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
    shader->set_mat3("invNormal", normal_matrix);

    shader->set_float("shininess", 32.0f);
    shader->set_vec3("viewPos", graphics->camera()->Position);

    dirlight.apply(shader, "dirlight");
    spotlight.apply(shader, "spotlight");

    m_model->draw(shader);
}

void Target::put_up(float dt) {
    if (m_angle < ANGLE_UPPER) {
        float angle = m_angle + ANGLE_SPEED * dt;
        m_angle = std::min(angle, ANGLE_UPPER);
    }
}

void Target::put_down(float dt) {
    if (m_angle > ANGLE_LOWER) {
        float angle = m_angle - ANGLE_SPEED * dt;
        m_angle = std::max(angle, ANGLE_LOWER);
    }
}

void Target::update(float dt) { if (m_active) { put_up(dt); } else { put_down(dt); } }

void Target::calculate_bounding_box() {
    auto meshes = m_model->meshes();
    unsigned int n = meshes.size();
    glm::vec3 local_min = meshes[0].min_vertex;
    glm::vec3 local_max = meshes[0].max_vertex;

    for (unsigned int i = 1; i < n; i++) {
        auto &min_vec = meshes[i].min_vertex;
        local_min.x = std::min(local_min.x, min_vec.x);
        local_min.y = std::min(local_min.y, min_vec.y);
        local_min.z = std::min(local_min.z, min_vec.z);

        auto &max_vec = meshes[i].max_vertex;
        local_max.x = std::max(local_max.x, max_vec.x);
        local_max.y = std::max(local_max.y, max_vec.y);
        local_max.z = std::max(local_max.z, max_vec.z);
    }

    box_min = m_position + m_scale * local_min;
    box_max = m_position + m_scale * local_max;
}

bool Target::check_boundingbox_intersect(const glm::vec3 &raycast_origin, const glm::vec3 &raycast_dir) {
    float t_min, t_max;
    float ty_min, ty_max;
    float tz_min, tz_max;

    if (raycast_dir.x != 0.0f) {
        t_min = (box_min.x - raycast_origin.x) / raycast_dir.x;
        t_max = (box_max.x - raycast_origin.x) / raycast_dir.x;
        if (t_min > t_max) { std::swap(t_min, t_max); }
    } else {
        if (raycast_origin.x > box_max.x || raycast_origin.x < box_min.x) { return false; }

        t_min = -std::numeric_limits<float>::infinity();
        t_max = std::numeric_limits<float>::infinity();
    }

    if (raycast_dir.y != 0.0f) {
        ty_min = (box_min.y - raycast_origin.y) / raycast_dir.y;
        ty_max = (box_max.y - raycast_origin.y) / raycast_dir.y;
        if (ty_min > ty_max) { std::swap(ty_min, ty_max); }

        if (ty_min > t_max || ty_max < t_min) { return false; }

        t_min = std::max(t_min, ty_min);
        t_max = std::min(t_max, ty_max);
    } else { if (raycast_origin.y > box_max.y || raycast_origin.y < box_min.y) { return false; } }

    if (raycast_dir.z != 0.0f) {
        tz_min = (box_min.z - raycast_origin.z) / raycast_dir.z;
        tz_max = (box_max.z - raycast_origin.z) / raycast_dir.z;
        if (tz_min > tz_max) { std::swap(tz_min, tz_max); }

        if (tz_min > t_max || tz_max < t_min) { return false; }

        t_min = std::max(t_min, tz_min);
        t_max = std::min(t_max, tz_max);
    } else { if (raycast_origin.z > box_max.z || raycast_origin.z < box_min.z) { return false; } }

    if (t_max < 0.0f) { return false; }

    return true;
}


}// app