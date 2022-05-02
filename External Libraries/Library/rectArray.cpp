#include "graphics-internal.hpp"

gfx::RectArray::RectArray(gfx::Texture* image_) {
    image = image_;
    
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &color_buffer);
    glGenBuffers(1, &texture_pos_buffer);
}

gfx::RectArray::~RectArray() {
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &color_buffer);
    glDeleteBuffers(1, &texture_pos_buffer);
}

void gfx::RectArray::setVertex(int index, int x, int y) {
    vertex_array[index * 2] = x;
    vertex_array[index * 2 + 1] = y;
}

void gfx::RectArray::setVertexColor(int index, Color color) {
    color_array[index * 4] = color.r * (1 / 255.f);
    color_array[index * 4 + 1] = color.g * (1 / 255.f);
    color_array[index * 4 + 2] = color.b * (1 / 255.f);
    color_array[index * 4 + 3] = color.a * (1 / 255.f);
}

void gfx::RectArray::setVertexTextureCoord(int index, int x, int y) {
    texture_pos_array[index * 2] = x;
    texture_pos_array[index * 2 + 1] = y;
}

void gfx::RectArray::setRect(int index, RectShape rect) {
    if(index < 0 || index >= length)
        throw std::runtime_error("Rect index must be positive and in bounds.");
    
    int x1 = rect.x, y1 = rect.y, x2 = rect.x + rect.w, y2 = rect.y + rect.h;
    setVertex(index * 6, x1, y1);
    setVertex(index * 6 + 1, x2, y1);
    setVertex(index * 6 + 2, x1, y2);
    setVertex(index * 6 + 3, x2, y1);
    setVertex(index * 6 + 4, x1, y2);
    setVertex(index * 6 + 5, x2, y2);
    
    update_vertex = true;
}

void gfx::RectArray::setColor(int index, Color color) {
    setColor(index, color, color, color, color);
}

void gfx::RectArray::setColor(int index, Color color1, Color color2, Color color3, Color color4) {
    if(index < 0 || index >= length)
        throw std::runtime_error("Color index must be positive and in bounds.");
    
    setVertexColor(index * 6, color1);
    setVertexColor(index * 6 + 1, color2);
    setVertexColor(index * 6 + 2, color3);
    setVertexColor(index * 6 + 3, color2);
    setVertexColor(index * 6 + 4, color3);
    setVertexColor(index * 6 + 5, color4);
    
    update_color = true;
}

void gfx::RectArray::setTextureCoords(int index, RectShape texture_coordinates) {
    if(index < 0 || index >= length)
        throw std::runtime_error("Texture coord index must be positive and in bounds.");
    
    int x1 = texture_coordinates.x, y1 = texture_coordinates.y, x2 = texture_coordinates.x + texture_coordinates.w, y2 = texture_coordinates.y + texture_coordinates.h;
    setVertexTextureCoord(index * 6, x1, y1);
    setVertexTextureCoord(index * 6 + 1, x2, y1);
    setVertexTextureCoord(index * 6 + 2, x1, y2);
    setVertexTextureCoord(index * 6 + 3, x2, y1);
    setVertexTextureCoord(index * 6 + 4, x1, y2);
    setVertexTextureCoord(index * 6 + 5, x2, y2);
}

void gfx::RectArray::resize(int size) {
    if(size < 0)
        throw std::runtime_error("RectArray size must be positive.");
    
    vertex_array.resize(size * 6 * 2);
    color_array.resize(size * 6 * 4);
    texture_pos_array.resize(size * 6 * 2);
    length = size;
}

void gfx::RectArray::render(int x, int y, bool blend_multiply) {
    glEnableVertexAttribArray(1);
    
    glUniform1i(uniform_has_texture, 0);
    glUniform1i(uniform_has_color_buffer, 1);
    
    glUniformMatrix3fv(uniform_transform_matrix, 1, GL_FALSE, normalization_transform.getArray());
    
    if(update_vertex) {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, vertex_array.size() * sizeof(float), &vertex_array[0], GL_STATIC_DRAW);
        
        update_vertex = false;
    }
    
    if(update_color) {
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
        glBufferData(GL_ARRAY_BUFFER, color_array.size() * sizeof(float), &color_array[0], GL_STATIC_DRAW);
        
        update_color = false;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(SHADER_VERTEX_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glVertexAttribPointer(SHADER_COLOR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    glDrawArrays(GL_TRIANGLES, 0, (int)vertex_array.size() / 2);
    
    glDisableVertexAttribArray(1);
}
