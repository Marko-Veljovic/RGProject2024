#ifndef BLOOMEFFECT_HPP
#define BLOOMEFFECT_HPP

class BloomEffect {
private:
    unsigned int m_hdr_FBO;
    unsigned int m_color_buffers[2];

    unsigned int m_quad_VAO = 0;
    unsigned int m_quad_VBO;

public:
    void init(unsigned int screen_width, unsigned int screen_height);

    void begin_render();

    void end_render();

    void blur();

    void finalize();

    void render_quad();
};

#endif //BLOOMEFFECT_HPP
