#include "graphics-internal.hpp"

void gfx::init(unsigned short window_width_, unsigned short window_height_) {
    sf::RenderWindow sfml_window(sf::VideoMode(800, 600), "SFML window");

    while (sfml_window.isOpen()) {
        sf::Event event;
        while (sfml_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                sfml_window.close();
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                sfml_window.close();
        }

        sfml_window.clear();
        sfml_window.display();
    }
    
    window_width = window_width_;
    window_height = window_height_;
    
    // initialize basic sdl module
    int result = SDL_Init(SDL_INIT_EVERYTHING);
    SDL_assert(result >= 0);

    // initialize image loading part of sdl
    result = IMG_Init(IMG_INIT_PNG);
    SDL_assert(result & IMG_INIT_PNG);
    
    // initialize font rendering part of sdl
    result = TTF_Init();
    SDL_assert(result != -1);
    
    // create actual window
    window = SDL_CreateWindow("Terralistic", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_RESIZABLE);
    SDL_assert(window);

    // create renderer for GPU accelerated
    renderer = SDL_CreateRenderer(gfx::window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_assert(renderer);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_DisplayMode dm = {SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, nullptr};
    SDL_SetWindowDisplayMode(window, &dm);
}

void gfx::setWindowMinimumSize(unsigned short width, unsigned short height) {
    SDL_SetWindowMinimumSize(window, width, height);
}

void gfx::loadFont(const std::string& path, unsigned char size) {
    font = TTF_OpenFont((resource_path + path).c_str(), size);
    SDL_assert(font);
}

void gfx::quit() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

unsigned short gfx::getWindowWidth() {
    return window_width;
}

unsigned short gfx::getWindowHeight() {
    return window_height;
}

unsigned short gfx::getMouseX() {
    return mouse_x;
}

unsigned short gfx::getMouseY() {
    return mouse_y;
}

void gfx::setRenderTarget(Image& tex) {
    SDL_SetRenderTarget(renderer, (SDL_Texture*)tex.getTexture());
}

void gfx::resetRenderTarget() {
    SDL_SetRenderTarget(renderer, nullptr);
}

bool gfx::colliding(RectShape a, RectShape b) {
    return a.y + a.h > b.y && a.y < b.y + b.h && a.x + a.w > b.x && a.x < b.x + b.w;
}

unsigned int gfx::getTicks() {
    return SDL_GetTicks();
}

float gfx::getDeltaTime() {
    return frame_length;
}

void gfx::clearWindow() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}

void gfx::updateWindow() {
    SDL_RenderPresent(renderer);
}

void gfx::sleep(unsigned short ms) {
    SDL_Delay(ms);
}
