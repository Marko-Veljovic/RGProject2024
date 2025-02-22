//#shader vertex

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

//#shader fragment

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct PostprocessingCoefficients
{
    int num_samples;
    float density;
    float exposure;
    float decay;
    float weight;
};

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;
uniform vec4 screen_space_light_position;
uniform PostprocessingCoefficients coefficients;

vec3 radial_blur(PostprocessingCoefficients coefficients, vec2 screen_space_position);

void main()
{
    vec3 hdrColor = texture(scene, TexCoords).rgb;

    vec3 volumetric = radial_blur(coefficients, screen_space_light_position.xy);
    hdrColor += volumetric;

    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction
    const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));

    FragColor = vec4(result, 1.0);
}

vec3 radial_blur(PostprocessingCoefficients coefficients, vec2 screen_space_position)
{
    vec2 delta_tex_coord = (TexCoords - screen_space_position) * coefficients.density * (1.0 / float(coefficients.num_samples));
    vec2 tex_coordinates = TexCoords;
    float alpha = texture(bloomBlur, tex_coordinates).a;
    vec3 color;
    if (alpha != 0.9) {
        color = texture(bloomBlur, tex_coordinates).rgb;
    }
    else {
        color = vec3(0.0);
    }
    float decay = 1.0;
    for (int i = 0; i < coefficients.num_samples; ++i)
    {
        tex_coordinates -= delta_tex_coord;
        vec3 current_sample = texture(bloomBlur, tex_coordinates).rgb;
        current_sample *= decay * coefficients.weight;
        alpha = texture(bloomBlur, tex_coordinates).a;
        if (alpha != 0.9) {
            color += current_sample;
        }
        decay *= coefficients.decay;
    }

    return color * coefficients.exposure;
}