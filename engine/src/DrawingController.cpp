#include <engine/graphics/DrawingController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace engine::drawing {


void InstancedDrawable::add_instance(const glm::mat4& transform) {
    transforms.push_back(transform);
    needs_update = true;
}

void InstancedDrawable::update_instance(size_t index, const glm::mat4& transform) {
    if (index < transforms.size()) {
        transforms[index] = transform;
        needs_update = true;
    }
}

void InstancedDrawable::clear_instances() {
    transforms.clear();
    needs_update = true;
}

void DrawingController::initialize() {
    spdlog::info("DrawingController initialized");
}

void DrawingController::update() {
    m_draw_call_count = 0;
}

void DrawingController::draw() {
    m_draw_call_count = 0;

    draw_instanced_groups();
}

void DrawingController::terminate() {
    auto graphics = core::Controller::get<graphics::GraphicsController>();

    // Clean up instance buffers
    for (auto& [id, group] : m_instanced_groups) {
        if (group.instance_vbo != 0) {
            glDeleteBuffers(1, &group.instance_vbo);
        }
    }

    clear_all();
    spdlog::info("DrawingController terminated");
}


InstancedDrawable* DrawingController::create_instanced_group(const std::string& id,
                                                             const std::string& model_name,
                                                             const std::string& shader_name) {
    if (m_instanced_groups.find(id) != m_instanced_groups.end()) {
        spdlog::warn("Instanced group '{}' already exists, overwriting", id);
    }

    InstancedDrawable group;
    group.model_name = model_name;
    group.shader_name = shader_name;

    m_instanced_groups[id] = group;

    spdlog::info("Created instanced group '{}' (model: {}, shader: {})", id, model_name, shader_name);

    return &m_instanced_groups[id];
}

InstancedDrawable* DrawingController::get_instanced_group(const std::string& id) {
    auto it = m_instanced_groups.find(id);
    if (it != m_instanced_groups.end()) {
        return &it->second;
    }
    return nullptr;
}

void DrawingController::remove_instanced_group(const std::string& id) {
    auto it = m_instanced_groups.find(id);
    if (it != m_instanced_groups.end()) {
        if (it->second.instance_vbo != 0) {
            glDeleteBuffers(1, &it->second.instance_vbo);
        }
        m_instanced_groups.erase(it);
        spdlog::info("Removed instanced group '{}'", id);
    }
}

void DrawingController::add_instance_to_group(const std::string& group_id, const glm::mat4& transform) {
    auto* group = get_instanced_group(group_id);
    if (group) {
        group->add_instance(transform);
    } else {
        spdlog::error("Cannot add instance: group '{}' not found", group_id);
    }
}

void DrawingController::update_instance_in_group(const std::string& group_id,
                                                 size_t index,
                                                 const glm::mat4& transform) {
    auto* group = get_instanced_group(group_id);
    if (group) {
        group->update_instance(index, transform);
    } else {
        spdlog::error("Cannot update instance: group '{}' not found", group_id);
    }
}

void DrawingController::setup_instanced_group(const std::string& group_id) {
    auto* group = get_instanced_group(group_id);
    if (!group) {
        spdlog::error("Cannot setup instancing: group '{}' not found", group_id);
        return;
    }

    if (group->transforms.empty()) {
        spdlog::warn("Setting up instanced group '{}' with no instances", group_id);
        return;
    }

    auto resources = core::Controller::get<resources::ResourcesController>();

    if (group->instance_vbo != 0) {
        glDeleteBuffers(1, &group->instance_vbo);
    }

    glGenBuffers(1, &group->instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, group->instance_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 group->transforms.size() * sizeof(glm::mat4),
                 &group->transforms[0],
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Setup instance attributes for all meshes in the model
    setup_instance_attributes(group->model_name, group->instance_vbo);

    group->needs_update = false;

    spdlog::info("Setup instanced group '{}' with {} instances",
                 group_id, group->transforms.size());
}


std::vector<std::string> DrawingController::get_all_instanced_group_ids() const {
    std::vector<std::string> ids;
    ids.reserve(m_instanced_groups.size());
    for (const auto& [id, group] : m_instanced_groups) {
        ids.push_back(id);
    }
    return ids;
}

void DrawingController::clear_all() {

    for (auto& [id, group] : m_instanced_groups) {
        if (group.instance_vbo != 0) {
            glDeleteBuffers(1, &group.instance_vbo);
        }
    }
    m_instanced_groups.clear();

    spdlog::info("Cleared all drawable objects and instanced groups");
}

void DrawingController::draw_instanced_groups() {
    auto graphics = core::Controller::get<graphics::GraphicsController>();
    auto resources = core::Controller::get<resources::ResourcesController>();

    for (auto& [id, group] : m_instanced_groups) {
        if (!group.visible || group.transforms.empty()) continue;

        if (group.needs_update && group.instance_vbo != 0) {
            glBindBuffer(GL_ARRAY_BUFFER, group.instance_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                           group.transforms.size() * sizeof(glm::mat4),
                           &group.transforms[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            group.needs_update = false;
        }

        resources::Model* model = resources->model(group.model_name);
        resources::Shader* shader = resources->shader(group.shader_name);

        if (!model || !shader) {
            spdlog::warn("Instanced group '{}': missing model or shader", id);
            continue;
        }

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());

        shader->set_vec3("viewPos", graphics->camera()->Position);

        for (const auto& mesh : model->get_m_meshes()) {
            glBindVertexArray(mesh.get_vao());
            glDrawElementsInstanced(GL_TRIANGLES,mesh.get_m_num_indices(), GL_UNSIGNED_INT,0, group.transforms.size());
            glBindVertexArray(0);
        }

        m_draw_call_count++;  // One draw call per instanced group
    }
}

void DrawingController::setup_instance_attributes(const std::string& model_name,
                                                  unsigned int instance_vbo) {
    auto resources = core::Controller::get<resources::ResourcesController>();
    resources::Model* model = resources->model(model_name);

    if (!model) {
        spdlog::error("Cannot setup instance attributes: model '{}' not found", model_name);
        return;
    }

    // For each mesh in the model
    for (const auto& mesh : model->get_m_meshes()) {
        glBindVertexArray(mesh.get_vao());
        glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);

        // mat4 takes 4 attribute locations
        std::size_t vec4_size = sizeof(glm::vec4);

        for (unsigned int i = 0; i < 4; i++) {
            unsigned int location = 3 + i;  // Locations 3, 4, 5, 6

            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE,
                                 4 * vec4_size, (void*)(i * vec4_size));
            glVertexAttribDivisor(location, 1);
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

} // namespace engine::drawing