#include "engine/graphics/BloomEffect.hpp"
#include <glad/glad.h>
#include <engine/graphics/OpenGL.hpp>

#include <iostream>
#include <spdlog/spdlog.h>

void BloomEffect::init(unsigned int screen_width, unsigned int screen_height) {
    // hdr_FBO
    CHECKED_GL_CALL(glGenFramebuffers, 1, &m_hdr_FBO);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_hdr_FBO);

    CHECKED_GL_CALL(glGenTextures, 2, m_color_buffers);
    for (unsigned int i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_color_buffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT,
                        nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                        m_color_buffers[i], 0);
    }

    unsigned int rbo_depth;
    CHECKED_GL_CALL(glGenRenderbuffers, 1, &rbo_depth);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, rbo_depth);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
    CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    CHECKED_GL_CALL(glDrawBuffers, 2, attachments);

    if (CHECKED_GL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        spdlog::error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

    // ping_pong_FBO
    CHECKED_GL_CALL(glGenFramebuffers, 2, m_ping_pong_FBO);
    CHECKED_GL_CALL(glGenTextures, 2, m_ping_pong_color_buffers);
    for (unsigned int i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_ping_pong_FBO[i]);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_ping_pong_color_buffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT,
                        nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                        m_ping_pong_color_buffers[i], 0);

        if (CHECKED_GL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            spdlog::error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void BloomEffect::bind_hdr_fbo() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_hdr_FBO);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomEffect::bind_ping_pong_fbo(bool horizontal) {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_ping_pong_FBO[horizontal]);
}

void BloomEffect::bind_ping_pong_texture(bool first_iteration, bool horizontal) {
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D,
                    first_iteration ? m_color_buffers[1] : m_ping_pong_color_buffers[!horizontal]);
}

void BloomEffect::bind_default_fbo() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void BloomEffect::clear_color_depth_buffers() {
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomEffect::finalize(bool horizontal) {
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_color_buffers[0]);
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE1);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_ping_pong_color_buffers[!horizontal]);
}

void BloomEffect::render_quad() {
    if (m_quad_VAO == 0) {
        float quad_vertices[] = {
                // positions        // texture Coords
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup quad VAO
        CHECKED_GL_CALL(glGenVertexArrays, 1, &m_quad_VAO);
        CHECKED_GL_CALL(glGenBuffers, 1, &m_quad_VBO);
        CHECKED_GL_CALL(glBindVertexArray, m_quad_VAO);
        CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_quad_VBO);
        CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 0);
        CHECKED_GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        CHECKED_GL_CALL(glEnableVertexAttribArray, 1);
        CHECKED_GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *) (3 * sizeof(float)));
    }

    CHECKED_GL_CALL(glBindVertexArray, m_quad_VAO);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}
