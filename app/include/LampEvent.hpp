//
// Created by matfrg on 7/16/25.
//

#ifndef LAMPEVENT_HPP
#define LAMPEVENT_HPP
#include "../../engine/libs/assimp/code/AssetLib/Blender/BlenderScene.h"

namespace app {

enum LampState { TRANSPARENT, GREEN, BLUE };

class LampEvent {
public:
    LampEvent();

    bool cycle_active = false;

    void update_lamp(float delta_time);

    LampState get_lamp_state() const;

    void start_automatic_cycle();// trigger from key B

    void cycle_manual_color();

private:
    LampState lamp_state;
    float t_timer = 0.0f;
    int cycle_stage = 0;// 0 = waiting for GREEN, 1 = GREEN, 2 = BLUE, 3 = done
};
}// app

#endif //LAMPEVENT_HPP
