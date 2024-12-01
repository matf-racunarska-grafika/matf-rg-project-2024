
//

#ifndef MATF_RG_PROJECT_ENTITYCONTROLLER_HPP
#define MATF_RG_PROJECT_ENTITYCONTROLLER_HPP
#include <engine/controller/Controller.hpp>
#include <engine/ecs/Entity.hpp>

namespace rg {
    class EntityController : public rg::Controller {
    public:
        static std::unique_ptr<EntityController> create() {
            return std::make_unique<EntityController>();
        }

        std::string_view name() const override;

        Entity *create_entity();
        void destroy_entity(Entity *entity);

        void draw() override;

    private:
        void poll_events() override;
        void update() override;

        std::vector<std::unique_ptr<Entity>> m_entities;
        std::vector<std::unique_ptr<Entity>> m_dead_entities;
    };

}// namespace rg

#endif//MATF_RG_PROJECT_ENTITYCONTROLLER_HPP
