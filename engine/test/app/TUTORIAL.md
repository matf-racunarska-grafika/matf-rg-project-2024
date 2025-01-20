# Test App Tutorial

To demonstrate most of the `engine` features, let's recreate part of the `engine/test/app` in this tutorial.

## 1. The TestApp class

The first thing we need to do is to create a class that extends the `engine::app::App` class and override the
`App::app_setup` method, let's call it `TestApp`.

In the `include/app` create `TestApp.hpp`

```cpp
#include <engine/core/Engine.hpp>

namespace engine::test::app {
    class TestApp final : public engine::core::App {
        void setup() override;
    };
}
```

The only file we need to include to use the `engine` is the `<engine/core/Engine.hpp>`.

And in the `src/` create `TestApp.cpp`, define `TestApp::app_setup()` and `main`:

```cpp
#include <app/TestApp.hpp>
#include <spdlog/spdlog.h>

namespace engine::test::app {
    void TestApp::app_setup() {
        spdlog::info("Hello, TestApp::app_setup");
    }
}

int main(int argc, char **argv) {
    return std::make_unique<engine::test::app::TestApp>()->run(argc, argv);
}
```

The `TestApp::app_setup()` prints "Hello, TestApp::app_setup".
The main function creates a new instance of our app `std::make_unique<engine::test::app::TestApp>()`
and calls `engine::core::App::run`.

Now lets build and run this app and we should see a trace and an empty window:

```
[2024-12-14 15:57:40.098] [info] ArgParser initialized.
[2024-12-14 15:57:40.098] [info] Configuration initialized.
[2024-12-14 15:57:40.098] [info] Hello, TestApp::app_setup <--- Here is our `TestApp::app_setup()` .
[2024-12-14 15:57:40.098] [info] PlatformGLFW3Controller::initialize
[2024-12-14 15:57:40.527] [info] Platform[GLFW 3.5.0]
[2024-12-14 15:57:40.527] [info] GraphicsController::initialize
[2024-12-14 15:57:40.528] [info] ResourcesController::initialize
```

The method `App::run` runs the application:

```cpp
int run(int argc, char** argv) {
  try {
    engine_setup(argc, argv); <--- This where `TestApp::app_setup` is called
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
```

Notice that the `engine::core::App::engine_setup` with the suffix `_` and `engine::core::App::app_setup` are different
methods.
The `engine_setup` first executes the engine
setup code and then calls the user implementation of the `setup`.

## The MainController

`Controllers` are the way to hook into the `engine::core::App::run` method. The `engine::core::Controller` class
has virtual
methods: `engine::core::Controller::initialize`, `engine::core::Controller::loop`,
`engine::core::Controller::poll_events`,
`engine::core::Controller::update`, `engine::core::Controller::draw`, and
`engine::core::Controller::terminate` that we
can override in our custom Controller implementation and have the `engine` execute them.

Let's create an `engine::core::Controller` implementation, call it `MainController`:

Create `MainController.hpp` in the `include/` directory:

```cpp
class MainController final : public engine::core::Controller {
    void initialize() override;
};
```

and `MainController.cpp` in the `src/` directory:

```cpp
#include <app/MainController.hpp>
#include <spdlog/spdlog.h>
void MainController::initialize() {
    spdlog::info("Hello, from the main controller initialization!");
}
```

If we ran the app at this point, we won't see anything change in log, because although we created an
`engine::core::Controller` implementation
we didn't register our implementation to the `engine`.
To register an `engine::core::Controller` implementation and have the `engine` execute it, we have to call
`engine::core::App::register_controller` during
`TestApp::app_setup`:

```cpp
void TestApp::app_setup() {
    spdlog::info("Hello, from the main controller initialization!");
    auto main_controller = engine::core::App::register_controller<MainController>();
    main_controller->after(engine::core::App::get_controller<controller::EngineControllersEnd>());
}
```

The `engine::core::App::register_controller<T>` returns a `T*` to the instance of a controller. Since there can be
multiple controllers
in the `app`, as we'll see later, the `engine` provides a mechanism to order their execution.
By calling:

```cpp
main_controller->after(engine::core::App::get_controller<controller::EngineControllersEnd>());
```

we ensure that our `MainController` will always execute after all the `engine` controllers. The
`engine::core::App::get_controller<controller::EngineControllersEnd>()`
retrieves an instance of a `controller::EngineControllersEnd` that serves as a sentinel between engine controllers and
our controllers, and the call
to `main_controller->after(c)` will tell the `engine` that the `main_controller` should always execute after whatever is
the controller `c`.

When we run the app now, in the log we should find the following:

```
...
[2024-12-14 19:01:07.471] [info] EngineControllersEnd::initialize
[2024-12-14 19:01:07.471] [info] N6engine4test3app14MainControllerE::initialize
[2024-12-14 19:01:07.471] [info] Hello, from the main controller initialization!
...
```

`EngineControllersEnd::initialize` means that all the engine controllers are initialized at this point.

`N6engine4test3app14MainControllerE::initialize` is the engine informing us that it called the
`MainController::initialize` at this point,
and right after we see  `Hello, from the main controller initialization!`.

