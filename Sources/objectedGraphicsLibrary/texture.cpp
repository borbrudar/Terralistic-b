//
//  centeredTexture.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 04/07/2020.
//
#include "singleWindowLibrary.hpp"
#include "objectedGraphicsLibrary.hpp"

ogl::texture::texture(SDL_Texture* initial_texture) {
    setTexture(initial_texture);
}

void ogl::texture::setTexture(SDL_Texture* input_texture) {
    freeTexture();
    texture_ = input_texture;
}

void ogl::texture::render() {
    SDL_Rect render_rect = {
        getX(getWidth()),
        getY(getHeight()),
        getWidth(),
        getHeight(),
    };
    swl::render(texture_, render_rect);
}

void ogl::texture::freeTexture() {
    if(texture_)
        SDL_DestroyTexture(texture_);
}

ogl::texture::~texture() {
    freeTexture();
}

void ogl::texture::loadFromText(std::string text, SDL_Color text_color) {
    int temp_width, temp_height;
    setTexture(swl::loadTextureFromText(text, text_color, &temp_width, &temp_height));
    texture_width = temp_width;
    texture_height = temp_height;
}

void ogl::texture::loadFromFile(std::string path) {
    int temp_width, temp_height;
    setTexture(swl::loadTextureFromFile(path, &temp_width, &temp_height));
    texture_width = temp_width;
    texture_height = temp_height;
}
