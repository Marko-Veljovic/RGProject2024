#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/graphics/Framebuffer.hpp>

namespace app {

    class MainController : public engine::core::Controller {
        void initialize() override;

        bool loop() override;

        void draw_lighthouse();

        void draw_reflector();

        void update_camera();

        void update() override;

        void begin_draw() override;

        void draw_skybox();

        void draw() override;

        void end_draw() override;

        Framebuffer *m_fbo;

    public:
        std::string_view name() const override {
            return "app::MainController";
        }
    };

} // app

#endif //MAINCONTROLLER_HPP
