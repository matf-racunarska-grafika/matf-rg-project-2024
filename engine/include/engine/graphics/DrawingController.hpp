#ifndef MATF_RG_PROJECT_DRAWINGCONTROLLER_HPP
#define MATF_RG_PROJECT_DRAWINGCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <set>
#include <algorithm>
#include <unordered_map>

namespace engine::drawing {

struct InstancedDrawable {
    std::string model_name;
    std::string shader_name;
    std::vector<glm::mat4> transforms;
    unsigned int instance_vbo = 0;
    bool visible = true;
    bool needs_update = true;

    void add_instance(const glm::mat4& transform);
    void update_instance(size_t index, const glm::mat4& transform);
    void clear_instances();
    size_t instance_count() const { return transforms.size(); }
};

class DrawingController : public core::Controller {
public:
    std::string_view name() const override { return "DrawingController"; }

    void initialize() override;
    void update() override;
    void draw() override;
    void terminate() override;

    InstancedDrawable* create_instanced_group(const std::string& id, const std::string& model_name, const std::string& shader_name);
    InstancedDrawable* get_instanced_group(const std::string& id);
    void remove_instanced_group(const std::string& id);
    void add_instance_to_group(const std::string& group_id, const glm::mat4& transform);
    void update_instance_in_group(const std::string& group_id, size_t index,
                                  const glm::mat4& transform);
    void setup_instanced_group(const std::string& group_id);

    std::vector<std::string> get_all_instanced_group_ids() const;

    void clear_all();
    int get_draw_call_count() const { return m_draw_call_count; }

private:
    std::unordered_map<std::string, InstancedDrawable> m_instanced_groups;

    int m_draw_call_count = 0;

    void draw_instanced_groups();
    void setup_instance_attributes(const std::string& model_name, unsigned int instance_vbo);
};

}

#endif//MATF_RG_PROJECT_DRAWINGCONTROLLER_HPP
