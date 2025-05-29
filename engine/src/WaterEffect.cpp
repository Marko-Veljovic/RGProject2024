#include "engine/graphics/WaterEffect.hpp"
#include <glad/glad.h>
#include <engine/graphics/OpenGL.hpp>

#include <spdlog/spdlog.h>

void WaterEffect::init(unsigned int screen_width, unsigned int screen_height) {
    CHECKED_GL_CALL(glGenFramebuffers, 1, &m_FBO);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_FBO);

    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_color_buffer);
    CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_buffer, 0);

    unsigned int rbo_depth;
    CHECKED_GL_CALL(glGenRenderbuffers, 1, &rbo_depth);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, rbo_depth);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
    CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

    if (CHECKED_GL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) spdlog::error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void WaterEffect::bind_reflection_fbo() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_FBO);
    CHECKED_GL_CALL(glViewport, 0, 0, m_REFLECTION_WIDTH, m_REFLECTION_HEIGHT);
}

void WaterEffect::bind_default_fbo(unsigned int screen_width, unsigned int screen_height) {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glViewport, 0, 0, screen_width, screen_height);
}

void WaterEffect::draw_water() {
    if (m_water_VAO == 0) {
        float water_vertices[] = {
                // positions          // normals           // texCoords
                5.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
                -5.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                -5.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,

                5.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
                -5.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
                5.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 2.0f
        };


        // setup water VAO
        CHECKED_GL_CALL(glGenVertexArrays, 1, &m_water_VAO);
        CHECKED_GL_CALL(glGenBuffers, 1, &m_water_VBO);
        CHECKED_GL_CALL(glBindVertexArray, m_water_VAO);
        CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_water_VBO);
        CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(water_vertices), &water_vertices, GL_STATIC_DRAW);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 0);
        CHECKED_GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 1);
        CHECKED_GL_CALL(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *) (3 * sizeof(float)));
        CHECKED_GL_CALL(glEnableVertexAttribArray, 2);
        CHECKED_GL_CALL(glVertexAttribPointer, 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *) (6 * sizeof(float)));
    }

    CHECKED_GL_CALL(glBindVertexArray, m_water_VAO);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 6);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}