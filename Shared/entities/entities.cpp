#include <cmath>
#include "entities.hpp"

void Entities::updateAllEntities() {
    for(auto & entity : entities) {
        float old_vel_x = entity->velocity_x, old_vel_y = entity->velocity_y;
        entity->updateEntity(blocks);
        if(entity->type == EntityType::PLAYER && (old_vel_x != entity->velocity_x || old_vel_y != entity->velocity_y)){
            EntityAbsoluteVelocityChangeEvent event(entity, old_vel_x, old_vel_y);
            entity_absolute_velocity_change_event.call(event);
        }
    }
}

const std::vector<Entity*>& Entities::getEntities() {
    return entities;
}

void Entities::registerEntity(Entity* entity) {
    entities.push_back(entity);
}

void Entities::removeEntity(Entity* entity) {
    auto pos = std::find(entities.begin(), entities.end(), entity);
    if(pos == entities.end())
        throw Exception("Removed non existing entity");
    entities.erase(pos);
    EntityDeletionEvent event(entity);
    entity_deletion_event.call(event);
    delete entity;
}

Entity* Entities::getEntityById(int id) {
    for(auto & entity : entities)
        if(entity->id == id)
            return entity;
    throw Exception("Entity not found by id");
}

void Entity::updateEntity(Blocks *blocks) {
    if(friction) {
        velocity_y *= 0.995f;
        velocity_x *= isTouchingGround(blocks) ? 0.99f : 0.9995f;
    }
    
    if(gravity)
        velocity_y += 0.2f;

    float prev_y = y;
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

    float prev_x = x;
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
    has_moved_x = prev_x != x;
    has_moved = (prev_y != y) || has_moved_x;
}

bool Entity::isTouchingGround(Blocks* blocks) {
    return isCollidingWithBlocks(blocks, x, y + 1) && velocity_y == 0;
}

bool Entity::isCollidingWithBlocks(Blocks* blocks) {
    return isCollidingWithBlocks(blocks, x, y);
}

bool Entity::isCollidingWithBlocks(Blocks* blocks, float colliding_x, float colliding_y) {
    if(colliding_x < 0 || colliding_y < 0 ||
       colliding_y >= blocks->getHeight() * BLOCK_WIDTH * 2 - getHeight() ||
       colliding_x >= blocks->getWidth() * BLOCK_WIDTH * 2 - getWidth())
        return true;

    int starting_x = colliding_x / (BLOCK_WIDTH * 2);
    int starting_y = colliding_y / (BLOCK_WIDTH * 2);
    int ending_x = (colliding_x + getWidth() - 1) / (BLOCK_WIDTH * 2);
    int ending_y = (colliding_y + getHeight() - 1) / (BLOCK_WIDTH * 2);
    
    for(int x_ = starting_x; x_ <= ending_x; x_++)
        for(int y_ = starting_y; y_ <= ending_y; y_++)
            if(!blocks->getBlockType(x_, y_)->ghost)
                return true;
    
    return false;
}

float Entity::getVelocityX() const {
    return velocity_x;
}

float Entity::getVelocityY() const {
    return velocity_y;
}

int Entity::getX() const {
    return x;
}

int Entity::getY() const {
    return y;
}

void Entities::setVelocityX(Entity* entity, float velocity_x) {
    if(entity->velocity_x != velocity_x) {
        entity->velocity_x = velocity_x;
        EntityVelocityChangeEvent event(entity);
        entity_velocity_change_event.call(event);
    }
}

void Entities::setVelocityY(Entity* entity, float velocity_y) {
    if(entity->velocity_y != velocity_y) {
        entity->velocity_y = velocity_y;
        EntityVelocityChangeEvent event(entity);
        entity_velocity_change_event.call(event);
    }
}

void Entities::addVelocityX(Entity* entity, float velocity_x) {
    setVelocityX(entity, entity->velocity_x + velocity_x);
}

void Entities::addVelocityY(Entity* entity, float velocity_y) {
    setVelocityY(entity, entity->velocity_y + velocity_y);
}

void Entities::setX(Entity* entity, float x, bool send_to_everyone) {
    if(entity->x != x) {
        entity->x = x;
        if(send_to_everyone) {
            EntityPositionChangeEvent event(entity);
            entity_position_change_event.call(event);
        }
    }
}

void Entities::setY(Entity* entity, float y, bool send_to_everyone) {
    if(entity->y != y) {
        entity->y = y;
        if(send_to_everyone) {
            EntityPositionChangeEvent event(entity);
            entity_position_change_event.call(event);
        }
    }
}

Entities::~Entities() {
    for(auto & entitie : entities)
        delete entitie;
}
