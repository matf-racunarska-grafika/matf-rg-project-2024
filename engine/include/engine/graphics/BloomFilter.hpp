//
// Created by matija on 10/28/25.
//

#ifndef MATF_RG_PROJECT_BLOOMFILTER_HPP
#define MATF_RG_PROJECT_BLOOMFILTER_HPP
#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/MyFrameBuffer.hpp>

namespace engine::graphics {

class BloomFilter {
public:
    BloomFilter( float intensity=1.0f, float threshold=1.0f, unsigned int num_of_passes=5);
    ~BloomFilter();

    float getTreshold() const;

    void initilizeBuffers(unsigned int scr_width, unsigned int scr_height);

    void setUpCanvas();

    void applyBloom();

    void clearBuffers();
    void destroyBuffers();
    void applyBlur();

    unsigned int get_framebuffer_id();

    void set_threshold(float threshold);

    void setIntensity(float brightness);

private:
    float m_bright_threshold;
    float m_intensity;
    unsigned int m_num_swaps;

    resources::MyFrameBuffer m_bloom_buffer;
    resources::MyFrameBuffer m_pong_buffer[2];

    float m_gauss_weights[5] = {0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162};

    resources::Shader* m_combine_shader;
    resources::Shader* m_blur_shader_h;
    resources::Shader* m_blur_shader_v;


};

}
#endif //MATF_RG_PROJECT_BLOOMFILTER_HPP