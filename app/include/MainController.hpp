#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/graphics/BloomEffect.hpp>
#include <engine/graphics/VolumetricLight.hpp>

namespace app {

    class MainController : public engine::core::Controller {
        std::unique_ptr<BloomEffect> m_bloom_effect;
        std::unique_ptr<VolumetricLight> m_volumetric_light;

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

    public:
        std::string_view name() const override {
            return "app::MainController";
        }
    };

} // app

#endif //MAINCONTROLLER_HPP
