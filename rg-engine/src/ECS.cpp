
//

#include <engine/render/DrawableElement.hpp>
#include <engine/ecs/EntityController.hpp>
#include <algorithm>
namespace rg {
    std::string_view EntityController::name() const {
        return "EntityController";
    }

    void EntityController::poll_events() {
        // Remove destroyed entities to m_dead_entities
        auto alive = [](const auto &e) { return e->alive(); };
        auto alive_begin = std::partition(range(m_entities), alive);
        std::move(alive_begin, std::end(m_entities), std::back_inserter(m_dead_entities));
    }

    void EntityController::update() {
    }

    Entity *EntityController::create_entity() {
        m_entities.emplace_back(std::make_unique<Entity>());
        auto result = m_entities.back().get();
        return result;
    }

    void EntityController::draw() {
        for (auto &entity: m_entities) {
            auto drawable_component = entity->get_component<DrawableComponent>();
            if (drawable_component) {
                drawable_component->draw();
            }
        }
    }

    void DrawableComponent::draw() {
        once([] { spdlog::info("DrawableComponent::draw"); });
        drawable_element->draw(shader);
    }
}// namespace rg
