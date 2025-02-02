#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#include <string>

class Framebuffer {
public:
    unsigned int m_screenWidth, m_screenHeight;
    unsigned int m_hdrFBO;
    unsigned int m_pingpongFBO[2];
    unsigned int m_colorBuffers[2];
    unsigned int m_pingpongColorBuffers[2];

    Framebuffer(const unsigned int screenWidth, const unsigned int screenHeight) :
    m_screenWidth(screenWidth), m_screenHeight(screenHeight) {
    }

    void init();

    void bind(const unsigned int fbo);

    void bindTexture(const unsigned int t);

    void activeTexture(const unsigned int i);

    void renderQuad();

    void clear();
};

#endif //FRAMEBUFFER_HPP
