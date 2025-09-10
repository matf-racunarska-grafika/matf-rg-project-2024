/**
 * @file Camera.hpp
 * @brief Defines the Camera class for rendering.
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace engine::graphics {
/**
 *  @class Camera
 *  @brief Camera processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL.
 */
class Camera {
public:
    /**
     * @brief  Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
     */
    enum Movement {
        None,
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    /**
     * @brief  Default camera values
     */

    static constexpr float YAW = -90.0f;
    static constexpr float PITCH = 0.0f;
    static constexpr float WALK = 1.5f;
    static constexpr float RUN = 2.0f;
    static constexpr float SENSITIVITY = 0.1f;
    static constexpr float ZOOM = 45.0f;
    static constexpr float JUMP_SPEED = 3.0f;
    static constexpr float G_FORCE = -9.81f;

    /**
    * @brief Indicates if a jump can happen
    */
    bool Jump{};

    /**
     * @brief Defines speed using for going upwards
     */
    float JumpVelocity{};

    /**
    * @brief Defines a position in World-Space of where the Camera is located.
    */
    glm::vec3 Position{};

    /**
    * @brief Defines a vector in which the camera is pointed relative to the @ref Camera::Position.
    */
    glm::vec3 Front{};

    /**
    * @brief Defines the Up vector in the World-Space.
    */
    glm::vec3 Up{};

    /**
    * @brief Defines the camera Right vector.
    */
    glm::vec3 Right{};

    /**
    * @brief Defines the up orientation of the World-Space.
    */
    glm::vec3 WorldUp{};

    /**
     * @brief Defines direction in which FPS camera can move.
     */
    glm::vec3 Move{};

    float Yaw{};
    float Pitch{};

    /**
    * @brief Used in @ref Camera::move_camera. The higher the value the faster the camera will move on keyboard action.
    */
    float MovementSpeed{};

    /**
    * @brief Used in @ref Camera::rotate_camera. The higher the value the faster the camera will move on mouse action.
    */
    float MouseSensitivity{};

    /**
    * @brief Defines camera FOV.
    */
    float Zoom{};

    /**
     * @brief  constructor with vectors.
     */
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    float yaw = YAW, float pitch = PITCH);

    /**
     * @returns  returns the view matrix calculated using Euler Angles and the LookAt Matrix.
     */
    glm::mat4 view_matrix() const;

    /**
     * @brief Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems).
     */
    void move_camera(Movement direction, float delta_time);

    /**
     * @brief Processes input received from keyboard-like input system for FPS-style camera
     */
    void move_fps_camera(bool forward, bool backward, bool right, bool left, float delta_time);


    /**
     * @brief updates height of camera while jumping and coming down due g-force
     */
    void update_jump(float delta_time);

    /**
     * @brief Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
     */
    void rotate_camera(float x_offset, float y_offset, bool constrain_pitch = true);

    /**
     * @brief Processes input received from a mouse scroll-wheel event. Only requires to be input on the vertical wheel-axis
     */
    void zoom(float offset);

private:
    /**
     * @brief Calculates the front vector from the Camera's (updated) Euler Angles
     */
    void update_camera_vectors();

    /**
     * @brief Calculates the direction vector for FPS style camera from Camera's (updated) Euler Angles
     */
    void update_fps_camera_vectors();
};
}
#endif





