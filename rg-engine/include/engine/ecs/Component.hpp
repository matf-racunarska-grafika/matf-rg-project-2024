
//

#ifndef MATF_RG_PROJECT_COMPONENT_HPP
#define MATF_RG_PROJECT_COMPONENT_HPP
#include <type_traits>
#include <glm/glm.hpp>
#include <string_view>
#include <vector>
#include <memory>
#include "engine/render/Shader.hpp"
namespace rg {
    using ComponentTypeId = uint32_t;
    class Entity;
    class Model;
    class Shader;

    struct Component {
        friend class Entity;

    public:
        template<typename T>
        static ComponentTypeId get_component_type_id() {
            static_assert(std::is_base_of_v<Component, T>);
            static ComponentTypeId type_id = Component::m_next_component_type_id++;
            return type_id;
        }

        static std::string_view type_name() {
            return "Component";
        }

        virtual std::string_view name() const = 0;

        Entity *owner() const {
            return m_owner;
        }
        bool active() const {
            return m_active;
        }
        ComponentTypeId type_id() const {
            return m_type_id;
        }
        virtual ~Component() = default;

        template<typename TComponent>
        bool type_of() const {
            return get_component_type_id<TComponent>() == type_id();
        }
    private:
        template<typename TComponent>
        void initialize_type_id() {
            m_type_id = Component::get_component_type_id<TComponent>();
        }
        static inline ComponentTypeId m_next_component_type_id{1};
        Entity *m_owner{};
        ComponentTypeId m_type_id{0};
        bool m_active{false};
    };

    struct PhysicsComponent : public Component {
        glm::vec3 position;
        glm::vec3 speed;
        glm::vec3 acceleration;

        PhysicsComponent(const glm::vec3 &position, const glm::vec3 &speed, const glm::vec3 &acceleration)
            : position(position), speed(speed), acceleration(acceleration) {
        }

        static std::string_view type_name() {
            return "PhysicsComponent";
        }

        std::string_view name() const override {
            return type_name();
        }
    };


    enum class RenderingLevel { Normal, AlwaysOnTop };

    class DrawableElement;
    struct DrawableComponent : public Component {
        RenderingLevel rendering_level{RenderingLevel::Normal};
        DrawableElement *drawable_element = nullptr;
        ShaderProgram *shader{nullptr};

        DrawableComponent(DrawableElement *drawable_element, ShaderProgram *shader,
                          RenderingLevel rendering_level = RenderingLevel::Normal)
            : rendering_level(rendering_level), drawable_element(drawable_element), shader(shader) {
        }

        void draw();

        static std::string_view type_name() {
            return "DrawableComponent";
        }

        std::string_view name() const override {
            return type_name();
        }
    };

}// namespace rg

#endif//MATF_RG_PROJECT_COMPONENT_HPP
