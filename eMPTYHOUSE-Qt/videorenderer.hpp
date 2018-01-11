#pragma once

#include <easyar/vector.hpp>
#include <easyar/matrix.hpp>

class VideoRenderer
{
public:
    void init();
    void render(const easyar::Matrix44F& projectionMatrix, const easyar::Matrix44F& cameraview, easyar::Vec2F size);
    int texId();
private:
    unsigned int program_box;
    int pos_coord_box;
    int pos_tex_box;
    int pos_trans_box;
    int pos_proj_box;
    unsigned int vbo_coord_box;
    unsigned int vbo_tex_box;
    unsigned int vbo_faces_box;
    unsigned int texture_id;
};
