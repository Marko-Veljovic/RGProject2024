#include <GUIController.hpp>
#include <MainController.hpp>
#include <MyApp.hpp>
#include <ProgramStateController.hpp>
#include <spdlog/spdlog.h>

namespace app {
    void MyApp::app_setup() {
        spdlog::info("App setup completed!");
        auto main_controller          = register_controller<app::MainController>();
        auto gui_controller           = register_controller<app::GuiController>();
        auto program_state_controller = register_controller<app::ProgramStateController>();
        program_state_controller->before(main_controller);
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
        main_controller->before(gui_controller);
    }
} // app
