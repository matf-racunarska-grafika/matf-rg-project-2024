#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace app {

    class GuiController : public engine::core::Controller {
    private:
        void initialize() override;
        void draw() override;
    public:
        std::string_view name() const override { return "app::GuiController"; }
    };

} // app

#endif //GUICONTROLLER_HPP
