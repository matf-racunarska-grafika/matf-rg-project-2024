//
// Created by nikola on 10/8/25.
//

#ifndef MATF_RG_PROJECT_MYCONTROLLER_H
#define MATF_RG_PROJECT_MYCONTROLLER_H
#include "engine/core/Controller.hpp"
#include <vector>

#include <Drawable.hpp>
#include <PointLight.hpp>
#include <DirectionalLight.hpp>

namespace app {
class Drawable;
class PointLight;

class MyController : public engine::core::Controller {
    std::vector<Drawable*> m_drawables;
    std::vector<PointLight*> m_point_lights;

    void initialize() override;
    bool loop() override;

    void update_camera();
    void update() override;

    void begin_draw() override;
    void draw_skybox();
    void draw() override;
    void end_draw() override;

public:
    DirectionalLight directional_light;
    std::string_view name() const override {return "app::MyController";}

    void addDrawable(Drawable* drawable);
    void addPointLight(PointLight* point_light);

    void terminate() override;
};

}// namespace app

#endif//MATF_RG_PROJECT_MYCONTROLLER_H
