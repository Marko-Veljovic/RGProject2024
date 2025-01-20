#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

#include <MainController.hpp>
#include <spdlog/spdlog.h>

namespace app {
    void MainController::initialize() {
        spdlog::info("MainController initialized");
    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    void MainController::draw_backpack() {
        // Model
        auto resource                   = engine::core::Controller::get<engine::resources::ResourcesController>();
        engine::resources::Model* model = resource->model("backpack");
        // Shader
        engine::resources::Shader* shader = resource->shader("basic");
        model->draw(shader);
    }

    void MainController::draw() {
        draw_backpack();
    }
} // app
