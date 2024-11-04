
//

#ifndef MATF_RG_PROJECT_ENTITY_HPP
#define MATF_RG_PROJECT_ENTITY_HPP
#include <engine/util/Errors.hpp>
#include <engine/ecs/Component.hpp>
#include <engine/util/Utils.hpp>
#include <memory>
#include <vector>
#include <algorithm>
#include "spdlog/spdlog.h"
namespace rg {
    using EntityId = size_t;
    class Entity {
        friend class EntityController;

    public:
        Entity() : m_id(++m_next_id) {
        }

        EntityId id() const {
            return m_id;
        }

        static std::unique_ptr<Entity> create() {
            return std::make_unique<Entity>();
        }

        template<typename TComponent, typename... TArgs>
        TComponent *add_component(TArgs &&...args) {
            static_assert(std::is_base_of_v<Component, TComponent>);
            RG_GUARANTEE(alive(), "Accessing entity {} that is dead.", m_id);
            RG_GUARANTEE(!has_component<TComponent>(), "Entity {} already has component {}.", m_id,
                         TComponent::type_name());
            auto component = std::make_unique<TComponent>(std::forward<TArgs>(args)...);
            component->template initialize_type_id<TComponent>();
            component->m_owner = this;
            auto result = component.get();
            m_components.emplace_back(std::move(component));
            return result;
        }


        template<typename TComponent, typename... TComponents>
        bool has_component() {
            for (const auto &component: m_components) {
                if (component->type_of<TComponent>()) {
                    if constexpr (sizeof...(TComponents) == 0) {
                        return true;
                    } else {
                        return (has_component<TComponents>() && ...);
                    }
                }
            }
            return false;
        }

        template<typename TComponent>
        void remove_component() {
            if (!has_component<TComponent>()) {
                spdlog::warn("Removing component that doesn't exist! EntityID: {}, Component: {}", m_id,
                             TComponent::type_name());
                return;
            }
            m_components.erase(std::remove_if(range(m_components), [](const auto &component) {
                return component->template type_of<TComponent>();
            }));
        }

        void destroy() {
            m_alive = false;
        }

        bool alive() const {
            return m_alive;
        }

    protected:
        inline static EntityId m_next_id = 0;
        std::vector<std::unique_ptr<Component>> m_components;

    private:
        bool should_destroy() const {
            return !m_alive;
        }

        EntityId m_id;
        bool m_alive = true;
    };

}// namespace rg
#endif//MATF_RG_PROJECT_ENTITY_HPP
