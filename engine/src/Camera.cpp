
#include "spdlog/spdlog.h"

#include <engine/graphics/Camera.hpp>

namespace engine::graphics {
// constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(WALK), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Move = Front;
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    Jump = false;
    JumpVelocity = JUMP_SPEED;
    update_camera_vectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::view_matrix() const { return glm::lookAt(Position, Position + Front, Up); }

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::move_camera(Movement direction, float delta_time) {
    float velocity = MovementSpeed * delta_time;
    if (direction == FORWARD) { Position += Front * velocity; }
    if (direction == BACKWARD) { Position -= Front * velocity; }
    if (direction == LEFT) { Position -= Right * velocity; }
    if (direction == RIGHT) { Position += Right * velocity; }
    if (direction == UP) { Position += Up * velocity; }
    if (direction == DOWN) { Position -= Up * velocity; }
}

void Camera::move_fps_camera(bool forward, bool backward, bool right, bool left, float delta_time) {
    glm::vec3 move_dir = glm::vec3(0.0f);
    glm::vec3 move_right = glm::normalize(glm::cross(Move, WorldUp));
    float velocity = MovementSpeed * delta_time;

    if (forward) { move_dir += glm::normalize(Move); }

    if (backward) { move_dir -= glm::normalize(Move); }

    if (right) { move_dir += glm::normalize(move_right); }

    if (left) { move_dir -= glm::normalize(move_right); }

    if (glm::length(move_dir) > 0.0f) {
        move_dir = glm::normalize(move_dir);
        Position += move_dir * velocity;
    }
}

void Camera::update_jump(float delta_time) {
    if (Jump || Position.y > 0.0f) {
        JumpVelocity += G_FORCE * delta_time;
        float y = Position.y + JumpVelocity * delta_time;
        Position.y = std::max(0.0f, y);
    }

    if (Position.y == 0.0f) {
        JumpVelocity = JUMP_SPEED;
        Jump = false;
    }
}


// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::rotate_camera(float x_offset, float y_offset, bool constrain_pitch) {
    x_offset *= MouseSensitivity;
    y_offset *= MouseSensitivity;

    Yaw += x_offset;
    Pitch += y_offset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch) {
        if (Pitch > 89.0f) { Pitch = 89.0f; }
        if (Pitch < -89.0f) { Pitch = -89.0f; }
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    update_camera_vectors();
    update_fps_camera_vectors();
}

// processes input received from a mouse scroll-wheel event. Only requires to be input on the vertical wheel-axis
void Camera::zoom(float offset) {
    Zoom -= (float) offset;
    if (Zoom < 1.0f) { Zoom = 1.0f; }
    if (Zoom > 45.0f) { Zoom = 45.0f; }
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::update_camera_vectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cosf(glm::radians(Yaw)) * cosf(glm::radians(Pitch));
    front.y = sinf(glm::radians(Pitch));
    front.z = sinf(glm::radians(Yaw)) * cosf(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));
    // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::update_fps_camera_vectors() {
    glm::vec3 camera_move;
    camera_move.y = 0.0f;
    camera_move.x = cosf(glm::radians(Yaw));
    camera_move.z = sinf(glm::radians(Yaw));
    Move = glm::normalize(camera_move);
}

}
