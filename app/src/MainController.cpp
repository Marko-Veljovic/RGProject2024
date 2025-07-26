#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

#include <GUIController.hpp>
#include <MainController.hpp>
#include <ProgramStateController.hpp>
#include <json.hpp>
#include <engine/graphics/WaterEffect.hpp>
#include <spdlog/spdlog.h>

namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    void on_mouse_move(engine::platform::MousePosition position) override;

    void on_scroll(engine::platform::MousePosition position) override;

    void on_window_resize(int width, int height) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui_controller = engine::core::Controller::get<GuiController>();
    if (!gui_controller->is_enabled()) {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        camera->rotate_camera(position.dx, position.dy);
    }
}

void MainPlatformEventObserver::on_scroll(engine::platform::MousePosition position) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
    auto mouse = platform->mouse();

    camera->zoom(mouse.scroll);
    graphics->perspective_params().FOV = glm::radians(camera->Zoom);
}

void MainPlatformEventObserver::on_window_resize(int width, int height) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->viewport(width, height);

    auto main_controller = engine::core::Controller::get<app::MainController>();
    main_controller->resize_effects(width, height);
}

void MainController::initialize() {
    auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::graphics::OpenGL::enable_depth_testing();

    // bloom effect
    int screen_width = platform->window()->width();
    int screen_height = platform->window()->height();

    m_bloom_effect = std::make_unique<BloomEffect>();
    m_bloom_effect->init(screen_width, screen_height);

    auto blur_shader = resources->shader("blur");
    blur_shader->use();
    blur_shader->set_int("image", 0);

    auto bloom_shader = resources->shader("bloom");
    bloom_shader->use();
    bloom_shader->set_int("scene", 0);
    bloom_shader->set_int("bloomBlur", 1);

    // volumetric light
    m_volumetric_light = std::make_unique<VolumetricLight>();
    m_volumetric_light->init(screen_width, screen_height);

    auto volumetric_light_shader = resources->shader("volumetricLight");
    volumetric_light_shader->use();

    volumetric_light_shader->set_int("scene", 0);
    volumetric_light_shader->set_int("blurredLight", 1);
    volumetric_light_shader->set_int("coefficients.num_samples", 128);
    volumetric_light_shader->set_float("coefficients.density", 1.0f);
    volumetric_light_shader->set_float("coefficients.exposure", 1.0f);
    volumetric_light_shader->set_float("coefficients.decay", 1.0f);
    volumetric_light_shader->set_float("coefficients.weight", 0.05f);

    // water effect
    m_water_effect = std::make_unique<WaterEffect>();
    m_water_effect->init();
    auto water_shader = resources->shader("water");
    water_shader->use();
    water_shader->set_int("reflectionTexture", 0);
    water_shader->set_int("dudvMap", 1);
}

void MainController::resize_effects(int width, int height) {
    m_bloom_effect->resize(width, height);
    m_volumetric_light->resize(width, height);
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) { return false; }
    return true;
}

void MainController::draw_lighthouse() {
    // Model
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *lighthouse = resources->model("lighthouse");
    // Shader
    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, m_lighthouse_y, -3.0f));
    model = glm::scale(model, glm::vec3(0.3f));
    shader->set_mat4("model", model);

    // directional light
    shader->set_vec3("dirLight.direction", glm::vec3(0.57f, -0.15f, 0.8f));
    // correct dir light
    shader->set_vec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    shader->set_vec3("dirLight.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
    shader->set_vec3("dirLight.specular", glm::vec3(0.2f, 0.2f, 0.2f));

    shader->set_vec3("viewPos", graphics->camera()->Position);

    // spotlight
    auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
    float time = platform->frame_time().current;

    shader->set_vec3("spotLight.position", glm::vec3(0.0f, m_lighthouse_y + 2.67f, -3.0f));
    shader->set_vec3("spotLight.direction", glm::vec3(glm::sin(2 * time), 0.0f, glm::cos(2 * time)));
    shader->set_float("spotLight.cutOff", glm::cos(glm::radians(20.0f)));
    shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(22.0f)));
    shader->set_float("spotLight.constant", 1.0f);
    shader->set_float("spotLight.linear", 0.10f);
    shader->set_float("spotLight.quadratic", 0.044f);
    shader->set_vec3("spotLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
    shader->set_vec3("spotLight.diffuse", glm::vec3(10.0f, 10.0f, 10.0f));
    shader->set_vec3("spotLight.specular", glm::vec3(10.0f, 10.0f, 10.0f));

    shader->set_vec4("plane", glm::vec4(0.0f, 1.0f, 0.0f, -0.0f));

    shader->set_bool("lighthouse", true);// write to lighthouse light buffer
    lighthouse->draw(shader);
    shader->set_bool("lighthouse", false);// disable further writing to it
}

void MainController::draw_reflector() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("lightCube");
    auto volumetric_light_shader = resources->shader("volumetricLight");
    auto reflector = resources->model("cube");
    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, m_lighthouse_y + 2.67f, -3.0f));
    model = glm::scale(model, glm::vec3(0.1f));
    glm::mat4 projection = graphics->projection_matrix();
    glm::mat4 view = graphics->camera()->view_matrix();

    glm::vec4 clip_space = projection * view * model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 ndc_space = clip_space / clip_space.w;
    glm::vec4 screen_space = (ndc_space + 1.0f) * 0.5f;

    shader->set_mat4("projection", projection);
    shader->set_mat4("view", view);
    shader->set_mat4("model", model);

    volumetric_light_shader->use();
    volumetric_light_shader->set_vec4("screen_space_light_position", screen_space);

    reflector->draw(shader);
}

void MainController::prepare_reflection_texture() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
    auto camera = graphics->camera();

    float water_height = 0.0f;

    float distance = 2 * (camera->Position.y - water_height);
    camera->Position.y -= distance;
    camera->Pitch = -camera->Pitch;
    camera->update_camera_vectors();// HACK: made method public

    m_water_effect->enable_clip_distance();
    m_water_effect->bind_reflection_fbo();
    m_bloom_effect->clear_color_depth_buffers();

    draw_lighthouse();
    draw_reflector();
    draw_island();
    draw_skybox();

    camera->Position.y += distance;
    camera->Pitch = -camera->Pitch;
    camera->update_camera_vectors();// HACK: made method public

    int screen_width = platform->window()->width();
    int screen_height = platform->window()->height();
    m_water_effect->bind_default_fbo(screen_width, screen_height);
    m_water_effect->disable_clip_distance();
}

void MainController::draw_water() {
    auto resource = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    float water_height = 0.0f;

    engine::resources::Shader *shader = resource->shader("water");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", camera->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1000.0f));
    model = glm::translate(model, glm::vec3(0.0f, water_height, -3.0f));
    shader->set_mat4("model", model);

    // directional light
    shader->set_vec3("dirLight.direction", glm::vec3(0.57f, -0.15f, 0.8f));
    // correct dir light
    shader->set_vec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    shader->set_vec3("dirLight.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
    shader->set_vec3("dirLight.specular", glm::vec3(0.2f, 0.2f, 0.2f));

    shader->set_vec3("viewPos", camera->Position);

    // spotlight
    auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
    float time = platform->frame_time().current;

    shader->set_vec3("spotLight.position", glm::vec3(0.0f, m_lighthouse_y + 2.67f, -3.0f));
    shader->set_vec3("spotLight.direction", glm::vec3(glm::sin(2 * time), 0.0f, glm::cos(2 * time)));
    shader->set_float("spotLight.cutOff", glm::cos(glm::radians(20.0f)));
    shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(22.0f)));
    shader->set_float("spotLight.constant", 1.0f);
    shader->set_float("spotLight.linear", 0.10f);
    shader->set_float("spotLight.quadratic", 0.044f);
    shader->set_vec3("spotLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
    shader->set_vec3("spotLight.diffuse", glm::vec3(10.0f, 10.0f, 10.0f));
    shader->set_vec3("spotLight.specular", glm::vec3(10.0f, 10.0f, 10.0f));

    shader->set_vec4("plane", glm::vec4(0.0f, 1.0f, 0.0f, -water_height));
    float moveFactorSpeed = 0.03f;// tweak this to control speed
    float moveFactor = fmod(moveFactorSpeed * time, 1.0f);
    shader->set_float("moveFactor", moveFactor);

    engine::resources::Texture *dudv_map = resource->texture("dudvMap");
    unsigned int dudv_map_id = dudv_map->id();

    m_water_effect->active_reflection_texture();
    m_water_effect->active_dudv_map(dudv_map_id);
    m_water_effect->active_texture0();
    m_water_effect->draw_water();
}

void MainController::draw_island() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("basic");
    auto island = engine::core::Controller::get<engine::resources::ResourcesController>()->model("island");
    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.73f, -0.68f, -3.15f));
    model = glm::scale(model, glm::vec3(0.05f));
    glm::mat4 projection = graphics->projection_matrix();
    glm::mat4 view = graphics->camera()->view_matrix();

    shader->set_mat4("projection", projection);
    shader->set_mat4("view", view);
    shader->set_mat4("model", model);

    island->draw(shader);
}

void MainController::update_camera() {
    auto gui_controller = engine::core::Controller::get<GuiController>();
    if (gui_controller->is_enabled()) { return; }

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();
    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt); }
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt); }
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt); }
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt); }
}

void MainController::update() { update_camera(); }

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("sunset_skybox");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto program_state = engine::core::Controller::get<ProgramStateController>();
    auto blur_shader = resources->shader("blur");
    auto bloom_shader = resources->shader("bloom");
    auto volumetric_light_shader = resources->shader("volumetricLight");

    prepare_reflection_texture();

    // 1. render scene into floating point framebuffer
    m_bloom_effect->bind_hdr_fbo();
    m_bloom_effect->clear_color_depth_buffers();

    draw_lighthouse();
    draw_reflector();
    draw_island();
    draw_skybox();
    draw_water();

    m_bloom_effect->bind_default_fbo();

    // 2. blur bright fragments with two-pass Gaussian Blur
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;
    blur_shader->use();
    for (unsigned int i = 0; i < amount; i++) {
        // for bloom
        m_bloom_effect->bind_ping_pong_fbo(horizontal);
        blur_shader->set_bool("horizontal", horizontal);
        m_bloom_effect->bind_ping_pong_texture(first_iteration, horizontal);
        m_bloom_effect->render_quad();

        // for volumetric
        m_bloom_effect->bind_ping_pong_fbo2(horizontal);
        blur_shader->set_bool("horizontal", horizontal);
        m_bloom_effect->bind_ping_pong_texture2(first_iteration, horizontal);
        m_bloom_effect->render_quad();

        // end
        horizontal = !horizontal;
        if (first_iteration) first_iteration = false;
    }

    // 3. now render floating point color buffer to 2D quad and tonemap HDR colors to volumetric light fbo
    m_volumetric_light->bind_hdr_fbo();
    m_volumetric_light->clear_color_depth_buffers();
    bloom_shader->use();
    m_bloom_effect->finalize(horizontal);
    // exposure: higher -> darker pixels get more details, lower -> brighter pixels get more details
    bloom_shader->set_bool("bloom", program_state->m_bloom_enabled);

    m_bloom_effect->render_quad();

    // Volumetric light
    m_volumetric_light->bind_default_fbo();
    m_volumetric_light->clear_color_depth_buffers();
    volumetric_light_shader->use();
    volumetric_light_shader->set_float("exposure", program_state->m_exposure);
    m_volumetric_light->finalize();
    m_bloom_effect->active_volumetric_texture(horizontal);

    m_bloom_effect->render_quad();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

}// app
