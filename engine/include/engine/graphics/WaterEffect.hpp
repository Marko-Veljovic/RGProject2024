#ifndef WATEREFFECT_HPP
#define WATEREFFECT_HPP

class WaterEffect {
private:
    unsigned int m_FBO;
    unsigned int m_color_buffer;

    unsigned const int m_REFLECTION_WIDTH  = 320;
    unsigned const int m_REFLECTION_HEIGHT = 180;

    unsigned int m_water_VAO = 0;
    unsigned int m_water_VBO;

public:
    void init();

    void bind_reflection_fbo();

    void bind_default_fbo(unsigned int screen_width, unsigned int screen_height);

    void enable_clip_distance();

    void disable_clip_distance();

    void active_reflection_texture();

    void active_dudv_map(unsigned int texture_id);

    void draw_water();
};

#endif //WATEREFFECT_HPP
