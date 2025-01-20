#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {
    class MainController : public engine::core::Controller {
        void initialize() override;

        bool loop() override;

        void draw_backpack();

        void draw() override;
    public:
        std::string_view name() const override {
            return "app::MainController";
        }
    };
} // app

#endif //MAINCONTROLLER_HPP
