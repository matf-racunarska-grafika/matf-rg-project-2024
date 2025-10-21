#include <engine/core/App.hpp>

class MyApp : public engine::core::App {
protected:
    void app_setup() override {}
};

int main(int argc, char **argv) {
    MyApp app;
    return app.run(argc, argv);
}