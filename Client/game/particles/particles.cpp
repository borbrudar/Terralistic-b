#include "particles.hpp"

const ParticleInfo* Particle::getInfo() {
    return info;
}

int Particle::getX() const {
    return x;
}

int Particle::getY() const {
    return y;
}

int Particle::getTimeSpawned() {
    return timer.getTimeElapsed();
}

bool Particle::isColliding(Blocks* blocks) const {
    return isColliding(blocks, x, y);
}

bool Particle::isColliding(Blocks* blocks, float colliding_x, float colliding_y) {
    if(colliding_x < 0 || colliding_y < 0 ||
       colliding_y >= blocks->getHeight() * BLOCK_WIDTH * 2 ||
       colliding_x >= blocks->getWidth() * BLOCK_WIDTH * 2)
        return true;

    int starting_x = colliding_x / (BLOCK_WIDTH * 2);
    int starting_y = colliding_y / (BLOCK_WIDTH * 2);
    int ending_x = colliding_x / (BLOCK_WIDTH * 2);
    int ending_y = colliding_y / (BLOCK_WIDTH * 2);
    
    for(int x_ = starting_x; x_ <= ending_x; x_++)
        for(int y_ = starting_y; y_ <= ending_y; y_++)
            if(!blocks->getBlockType(x_, y_)->ghost)
                return true;
    
    return false;
}

bool Particle::isGrounded(Blocks* blocks) const {
    return isColliding(blocks, x, y + 1);
}

void Particle::update(Blocks* blocks) {
    if(info->colliding) {
        velocity_x *= 0.98;
        velocity_y *= 0.98;
        
        if(isGrounded(blocks))
            velocity_x *= 0.8;
    }
    
    if(info->gravity)
        velocity_y += 0.2f;
    
    float y_to_be = y + velocity_y / 100;
    float move_y = y_to_be - y;
    int y_factor = move_y > 0 ? 1 : -1;
    for(int i = 0; i < std::abs(move_y); i++) {
        y += y_factor;
        if(isColliding(blocks)) {
            y -= y_factor;
            velocity_y = 0;
            break;
        }
    }
    if(velocity_y)
        y = y_to_be;
    
    float x_to_be = x + velocity_x / 100;
    float move_x = x_to_be - x;
    int x_factor = move_x > 0 ? 1 : -1;
    bool has_collided_x = false;
    for(int i = 0; i < std::abs(move_x); i++) {
        x += x_factor;
        if(isColliding(blocks)) {
            x -= x_factor;
            has_collided_x = true;
            break;
        }
    }
    if(!has_collided_x)
        x = x_to_be;
}

void Particles::init() {
    settings->addSetting(&particle_enable_setting);
}

void Particles::stop() {
    settings->removeSetting(&particle_enable_setting);
}

void Particles::update(float frame_length) {
    enabled = particle_enable_setting.getValue();
}

void Particles::updatePerMs() {
    for(int i = 0; i < particles.size(); i++) {
        particles[i].update(blocks);
        if(particles[i].getTimeSpawned() > particles[i].getInfo()->lifetime)
            particles.erase(particles.begin() + i);
    }
}

void Particles::render() {
    for(auto & particle : particles)
        particle.getInfo()->render(particle.getX() - camera->getX() + gfx::getWindowWidth() / 2, particle.getY() - camera->getY() + gfx::getWindowHeight() / 2);
}

void WalkParticle::render(int x, int y) const {
    gfx::RectShape(x - 1, y - 1, 2, 2).render({100, 100, 100});
}

void Particles::spawnParticle(Particle particle) {
    particles.push_back(particle);
}
