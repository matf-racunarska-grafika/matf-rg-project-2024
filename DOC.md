# matf-rg-engine

This is the base project for the Computer Graphics course at the Faculty of Mathematics,
University of Belgrade for the school year of 2024/2025.

# Setup

## Linux

To setup the necessary libraries run:  
`./setup.sh`

# Engine

## Main loop

Here is the main loop of the engine. The user of this engine extends and implements the `App` class.
The `Engine` has clear stages, `setup`, `initialization`, `loop`, `poll_events`, `update`, `draw`, `terminate`, and
`on_exit`.

```C++
class App {
public:
int run(int argc, char** argv) {
  try {
    setup_(argc, argv);
    initialize();
    while (loop()) {
        poll_events();
        update();
        draw();
    }
    terminate();
  } catch(...) { ... }
  return on_exit();
}
};
```

* `setup_` - here the engine systems are setup and the App can register any of it's custom `Controller` instances
* `setup` - this the function that the user of the `App` overrides and implements a custom setup for the App
* `initialize` - `App` should gather whatever `Resources` it needs and initialize all of its state.
* `loop` - `App` can check whether it should continue to run or not. If the `loop` method returns `false`,
  the `Main loop` stops and the `App` terminates.
* `poll_events` - `App` collects information about the events that happened at the `Platform` and collects user input
  for the upcoming frame.
* `update` - `App` updates the world state, processes physics, events, and reacts to user inputs.
* `draw` - `App` uses `OpenGL` and draws the current state of the world.
* `terminate` - `App` terminates it's state
* `on_exit` -

## The `App` class

Here is the interface of the `engine/core/App.hpp` class:

```c++
class App {
    public:
        int run(int argc, char **argv);
        virtual ~App() = default;
    private:
        void setup_(int argc, char **argv);
        void initialize();
        void poll_events();
        bool loop();
        void update();
        void draw();
        void terminate();
    protected:
        virtual void setup() { // the user extends and implements setup }
        virtual int on_exit() { return 0; }
        virtual void handle_error(const Error &);
    };
```

## Project structure

Here is how the `Engine` is structured. You only need to include `<engine/core/Engine.hpp>` in your part of the project
and all the header files will be available.

```bash
├── controller
│   ├── Controller.hpp
│   ├── ControllerManager.hpp
│   └── EngineSentinelController.hpp
├── core
│   └── App.hpp
├── Declarations.hpp
├── Engine.hpp
├── graphics
│   ├── Camera.hpp
│   ├── GraphicsController.hpp
│   └── OpenGL.hpp
├── platform
│   ├── Input.hpp
│   ├── PlatformController.hpp
│   ├── PlatformEventObserver.hpp
│   └── Window.hpp
├── resources
│   ├── Mesh.hpp
│   ├── Model.hpp
│   ├── ResourcesController.hpp
│   ├── ShaderCompiler.hpp
│   ├── Shader.hpp
│   ├── Skybox.hpp
│   └── Texture.hpp
└── util
    ├── ArgParser.hpp
    ├── Configuration.hpp
    ├── Errors.hpp
    └── Utils.hpp


```

## How to...

### How to include and use engine files?

All the source files (.cpp) go into src/ directory, and all the header files go into the include/ directory.
To use the engine you just need to include `#include <engine/Engine.hpp>` and all the engine header files will be available.

### How the engine manages resources?

For every type of resource, the `ResourcesController` has a corresponding that retrieves it:

* `Model* ResourcesController::model("backpack")`
* `Shader* ResourcesController::shader("basic")`
* `Texture* ResourcesController::texture("awesomeface")`
* `Skybox* ResourcesController::skybox("skybox")`

The argument is always the resource name without the file extension. For textures and shaders the name is just the name
of
the file without the extension. For models and skyboxes it's the name of the directory, because they have multiple files
associated with them.

The pointer to the `resource` that the `ResourcesController` returns is a *non-owning pointer*, meaning you should
**never call delete on it.** All the memory is managed by the `ResourcesController.`

### How to do a basic app setup?

For a basic app setup you need to:

1. Create a class for your app, let's call it `MyApp`, in the src/.
2. Inherit from the `rg::core::App` and implement `setup()` and `App::create`.

```c++
#include <engine/Engne.hpp>
class MyApp : public rg::core::App {

public:
  void setup() override;
};

void MyApp::setup() {
    spdlog::info("Hello, setup!");
}

namespace rg {
    std::unique_ptr<core::App> core::App::create() {
        return std::make_unique<MainApp>();
    }
}
```

4. Run the program.

*-But where is the main function?* You may ask. The `main` function is located

### How are the `resources` managed?

Resources currently include: `textures`, `shaders`, `models`, `skyboxes`.
The `ResourcesController` manages the loading, storing, and accessing the resource objects.
During the `App::initialize` the `ResourcesController` will load all the resources contained in the `resources`
directory.

### How to add a model?

All the models are stored in the `resources/models` directory. Let's add a backpack model from the course.

1. Create a directory `resources/models/backpack`.
2. Copy `backpack` model files into the `resources/models/backpack`.
3. Add a configuration for your model in the config.json

```
 "resources": {
    "models": {
      "backpack": { # <--- This will be the name of the model you use in the app
        "path": "backpack/backpack.obj", # <---- Relative path to the .obj file
        "flip_uvs": false # <---- whether the loader should filp the texture coordinates
      }
    }
  }
```

4. The `ResourcesController` will automatically load this model during `ResourcesController::initialize()`, you should
   see a log:

```
...
[2024-12-08 11:19:12.347] [info] load_model(backpack, resources/models/backpack/backpack.obj)
[2024-12-08 11:19:13.272] [info] Loading texture: resources/models/backpack/diffuse.jpg
[2024-12-08 11:19:14.114] [info] Loading texture: resources/models/backpack/specular.jpg
[2024-12-08 11:19:14.479] [info] Loading texture: resources/models/backpack/normal.png
...
```

5. Use the model in your app:

```c++
    Model* backpack = rg::controller::get<rg::resources::ResourcesController>()->model("backpack");
    Shader* shader   = ... 
    backpack->draw(shader);
```

### How to add a texture?

1. Add a texture file `awesomeface.png` to the `resources/textures` directory
2. Use it in the app (`ResourcesController` will automatically load it)

```c++
Texture* texture = rg::controller::get<ResourcesController>()->texture("awesomeface");
```

### How to add a shader?

1. Create a `your_shader.glsl` in the `resources/shaders/your_shader.glsl`.
2. Implement `vertex`, `fragment`, and `geometry` (optional), shaders in the same file.
3. Use it in the app:

```C++
Shader* shader = rg::controller::get<ResourcesController>()->shader("your_shader");
Model* backpack = ...;
backpack->draw(shader);
```

Vertex, fragment and geometry shaders are written in the same file.
Use the `// #shader vertex|fragment|geometry` to declare the start of the shader.
Here is the example:

```glsl
//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

void main() {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
```

`ResourcesController` will load and compile all the shaders located in the `resources/shaders` directory.

### How to draw a gui?

`Engine` uses the [imgui](https://github.com/ocornut/imgui) library for drawing a GUI. See the library page for more
examples.
A GUI should be drawn last in order to be visible on top of all the other objects in the world.
Here is the example of displaying camera info in a GUI.

```C++
    auto graphics = rg::controller::get<rg::graphics::GraphicsController>();
    graphics->begin_gui();
    // Draw Camera Info window
    {
        ImGui::Begin("Camera info");
        const auto &c = *camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::End();
    }
    graphics->end_gui();
```

![img.png](extra/img.png)

### How to throw handle and errors?

The `Engine` defines several types of exceptions that it throws that are not meant to be caught or handled. They serve
as a *graceful* way to terminate the application and provide the user with some useful information on how to **fix** the
error.

For example, the `ResourcesController` will throw `AssetLoadingError` if it can't read the model file.

```c++
const aiScene *scene = importer.ReadFile(model_path, flags);
if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    throw AssetLoadingError("Assimp error while reading model. ", model_path, model_name);
}
```

The `AssetLoadingError` isn't meant to be caught, but to inform the user that something is wrong with the asset
file and that the asset file should be fixed.

### How to get key/mouse press event?

The `PlatformController` differentiates between four type of button/key states:

1. `rg::platform::Key::State::JustPressed` -> Only in the first frame when the button was registered as pressed
2. `rg::platform::Key::State::Pressed` -> Every subsequent frame if the button is still held
3. `rg::platform::Key::State::JustReleased` -> Just the first frame in which button was registered as released
4. `rg::platform::Key::State::Released` -> Button is not pressed

Use the `PlatformController::key` method to get the state of the key in a given frame:

Here is the example of enabling/disabling gui drawing when the F2 key is pressed.

```C++
void poll_events() override {
    const auto platform = rg::controller::get<rg::platform::PlatformController>();
    if (platform->key(rg::platform::KeyId::KEY_F2).state() == rg::platform::Key::State::JustPressed) {
        m_draw_gui = !m_draw_gui;
    }
}
```

All the keys are identified via `rg::platform::KeyId`.

### How to register a callback for platform events?

There is also an option to register callbacks for platform events via: `PlatformEventObserver`.

1. Implement the event observer by extending the class `rg::platform::PlatformEventObserver`, and override methods you'd
   like to have a custom operation executed once the event happens
2. Register the `observer` instance in the `PlatformController`.

```C++
class MainPlatformEventObserver final : public rg::platform::PlatformEventObserver {
public:
    void on_keyboard(rg::platform::Key key) override {
        spdlog::info("Keyboard event: key={}, state=", key, key.state());
    }
};
class MainController : public rg::controller::Controller {
protected:
    void initialize() override {
        // ... 
        auto observer = std::make_unique<MainPlatformEventObserver>();
        rg::controller::get<rg::platform::PlatformController>()->register_platform_event_observer(std::move(observer));
    }
    // ...
}
```

Now, for every keyboard event, the `PlatformController` will call `MainPlatformEventObserver::on_keyboard` and pass
the `key` on which the event occurred as an argument.

### How to get Window properties?

`PlatformController` initializes and stores the `Window` handle, you can access via:

```c++
    auto platform = rg::controller::get<rg::platform::PlatformController>();
    platform->window()->height()
    platform->window()->width()
    platform->window()->title()
```

also, the `PlatformController` will update the window properties if the size of the window changes.

### How to add new OpenGL calls?

Rendering actions that require more than one OpenGL call should be abstracted in the `rg::graphics::OpenGL` class.
For example, here is the function that compiles the GLSL shader and returns true if the compilation was successful:

```C++
bool OpenGL::compile_shader(uint32_t shader_id, const std::string &shader_source) {
    const char *shader_source_cstr = shader_source.c_str();
    glShaderSource(shader_id, 1, &shader_source_cstr, nullptr);
    glCompileShader(shader_id);
    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    return success;
}
```

Why this way? It's less error-prone, and it's easier to add debugging assertions and error checks if needed.
**There shouldn't be any raw OpenGL calls, except in the engine when they are one line long.**

### How to add a custom controller?

`Controller`s are a way to hook into the engine execution. To create a custom controller:

1. Create a custom controller class that extends the `rg::controller::Controller`
2. Implement for the phase (`initialize`, `loop`, `poll_events`, `update`, `draw`, `terminate`) for which you want to
   execute custom code.
3. Register the controller in the `YourApp::setup`.

Here is the example of creating the `MainController` that enables `depth testing`.

```c++
class MainController : public rg::controller::Controller {
protected:
    void initialize() override {
        rg::graphics::OpenGL::enable_depth_testing();
    }
};
class MainApp final : public rg::core::App {
protected:
    void setup() override {
        auto main_controller = rg::controller::register_controller<MainController>();
        /* Make sure that the main_controller executes after all the engine controllers. */
        main_controller->after(rg::controller::get<rg::controller::EngineSentinelController>());
    }
};

```

### How to add a configuration option?

You can configure some parts of the `engine` in the `config.json`. For example, we can
configure the size and the title of the window when the application starts.

```json
{
  "window": {
    "height": 600,
    "title": "Hello, window!",
    "width": 800
  }
}
```

You can also add your custom configuration options:

```json
{
  "exposure": 2.45,
  "on_exit_message": "Bye..."
}
```

and use them:

```C++
auto &config = util::Configuration::config();
float exposure = config["exposure"].get<float>();
std::string gamma = config["on_exit_message"].get<std::string>();
```

The `Configuration` class uses [nlohmann/json](https://github.com/nlohmann/json) library to load and store `json` files,
see
the library page for documentation.

### How to pass a command line argument?

You use the `ArgParser` to parse the command line arguments anywhere from the program.
For example, for the invocation command: `./matf-rg-engine ... --fps 120`, the
`parser->arg("--fps")` will return the value 60. If the argument is not present, it will return the default value
passed as the second argument to the `arg` method.

```C++
void setup() override {
    auto parser = rg::util::ArgParser()->instance();
    auto fps = parser->arg<int>("--fps", 60);
}
```


