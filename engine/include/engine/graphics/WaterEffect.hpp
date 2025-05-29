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
    void init(unsigned int screen_width, unsigned int screen_height);

    void bind_reflection_fbo();

    void bind_default_fbo(unsigned int screen_width, unsigned int screen_height);

    void draw_water();
};

#endif //WATEREFFECT_HPP
