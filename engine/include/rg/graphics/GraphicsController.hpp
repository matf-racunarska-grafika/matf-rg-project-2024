//
// Created by spaske on 12/7/24.
//

#ifndef GRAPHICSCONTROLLER_HPP
#define GRAPHICSCONTROLLER_HPP
#include <rg/controller/Controller.hpp>

namespace rg::graphics {
    class GraphicsController final : public rg::controller::Controller {
    public:
        std::string_view name() const override;

        void begin_gui();

        void end_gui();

    private:
        void initialize() override;
    };
}
#endif //GRAPHICSCONTROLLER_HPP
