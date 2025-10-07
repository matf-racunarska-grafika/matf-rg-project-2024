//
// Created by miona on 2/17/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace app  {
	class MainController : public engine::core::Controller {

        void initialize() override;

        bool loop() override;

	    void updateCamera();

	    void update() override;

        static void drawBackpack();

	    static void drawCar();

	    void begin_draw() override;

        void drawSkybox();

        void draw() override;

	    void end_draw() override;

        public:
            std::string_view name() const override { return "app::MainController"; }
	};
}

#endif //MAINCONTROLLER_HPP
