#include "liquids.hpp"
#include "graphics.hpp"

LiquidType::LiquidType(std::string name, unsigned short flow_time, float speed_multiplier, gfx::Color color) : name(std::move(name)), flow_time(flow_time), speed_multiplier(speed_multiplier), color(color) {}

void Liquids::create() {
    liquids = new Liquid[blocks->getWidth() * blocks->getHeight()];
}

Liquids::Liquid* Liquids::getLiquid(int x, int y) {
    if(x >= blocks->getWidth() || y >= blocks->getHeight())
        throw LiquidOutOfBoundsException();
    return &liquids[y * blocks->getWidth() + x];
}

const LiquidInfoOld& Liquids::getLiquidInfo(int x, int y) {
    return ::getLiquidInfoOld(getLiquid(x, y)->type);
}

LiquidTypeOld Liquids::getLiquidType(int x, int y) {
    return getLiquid(x, y)->type;
}

void Liquids::setLiquidTypeSilently(int x, int y, LiquidTypeOld type) {
    if((int)type < 0 || type >= LiquidTypeOld::NUM_LIQUIDS)
        throw InvalidBlockTypeException();
    getLiquid(x, y)->type = type;
}

void Liquids::setLiquidType(int x, int y, LiquidTypeOld type) {
    if(type != getLiquidType(x, y)) {
        if(type == LiquidTypeOld::EMPTY)
            setLiquidLevel(x, y, 0);
        
        setLiquidTypeSilently(x, y, type);
        
        LiquidChangeEvent event(x, y);
        liquid_change_event.call(event);
    }
}

void Liquids::scheduleLiquidUpdate(int x, int y) {
    getLiquid(x, y)->when_to_update = gfx::getTicks() + getLiquidInfo(x, y).flow_time;
}

bool Liquids::canUpdateLiquid(int x, int y) {
    return getLiquid(x, y)->when_to_update && gfx::getTicks() > getLiquid(x, y)->when_to_update;
}

bool Liquids::isFlowable(int x, int y) {
    return blocks->getBlockInfo(x, y).ghost && getLiquidType(x, y) == LiquidTypeOld::EMPTY;
}

void Liquids::updateLiquid(int x, int y) {
    getLiquid(x, y)->when_to_update = 0;
    
    if(!blocks->getBlockInfo(x, y).ghost)
        setLiquidType(x, y, LiquidTypeOld::EMPTY);
    
    if(getLiquidLevel(x, y) == 0)
        return;
    
    bool under_exists = false, left_exists = false, right_exists = false;
    
    if(y < blocks->getHeight() && (isFlowable(x, y + 1) || (getLiquidType(x, y + 1) == getLiquidType(x, y) && getLiquidLevel(x, y + 1) != 127)))
        under_exists = true;
    
    if(x >= 0 && (isFlowable(x - 1, y) || (getLiquidType(x - 1, y) == getLiquidType(x, y) && getLiquidLevel(x - 1, y) < getLiquidLevel(x, y))))
        left_exists = true;
    
    if(x < blocks->getWidth() && (isFlowable(x + 1, y) || (getLiquidType(x + 1, y) == getLiquidType(x, y) && getLiquidLevel(x + 1, y) < getLiquidLevel(x, y))))
        right_exists = true;
    
    
    if(under_exists) {
        setLiquidType(x, y + 1, getLiquidType(x, y));
        
        short liquid_sum = getLiquidLevel(x, y + 1) + getLiquidLevel(x, y);
        if(liquid_sum > 127) {
            setLiquidLevel(x, y + 1, 127);
            setLiquidLevel(x, y, liquid_sum - 127);
        } else {
            setLiquidType(x, y, LiquidTypeOld::EMPTY);
            setLiquidLevel(x, y + 1, liquid_sum);
        }
        
        getLiquid(x, y + 1)->flow_direction = FlowDirection::NONE;
    }
    
    if(getLiquidLevel(x, y) == 0)
        return;
    
    if(right_exists)
        setLiquidType(x + 1, y, getLiquidType(x, y));
    if(left_exists)
        setLiquidType(x - 1, y, getLiquidType(x, y));
    
    if(left_exists && right_exists) {
        short avg = (getLiquidLevel(x, y) + getLiquidLevel(x + 1, y) + getLiquidLevel(x - 1, y)) / 3;
        short mod = (getLiquidLevel(x, y) + getLiquidLevel(x + 1, y) + getLiquidLevel(x - 1, y)) % 3;
        if(mod) {
            if(getLiquid(x, y)->flow_direction == FlowDirection::LEFT) {
                setLiquidLevel(x - 1, y, avg + mod);
                getLiquid(x - 1, y)->flow_direction = FlowDirection::LEFT;
                setLiquidLevel(x + 1, y, avg);
            } else {
                setLiquidLevel(x + 1, y, avg + mod);
                getLiquid(x + 1, y)->flow_direction = FlowDirection::RIGHT;
                setLiquidLevel(x - 1, y, avg);
            }
        } else {
            getLiquid(x - 1, y)->flow_direction = FlowDirection::NONE;
            setLiquidLevel(x - 1, y, avg);
            getLiquid(x + 1, y)->flow_direction = FlowDirection::NONE;
            setLiquidLevel(x + 1, y, avg);
        }
        
        setLiquidLevel(x, y, avg);
        getLiquid(x, y)->flow_direction = FlowDirection::NONE;
        
    } else if(right_exists) {
        short avg = (getLiquidLevel(x, y) + getLiquidLevel(x + 1, y)) / 2;
        short mod = (getLiquidLevel(x, y) + getLiquidLevel(x + 1, y)) % 2;
        setLiquidLevel(x + 1, y, avg + mod);
        getLiquid(x + 1, y)->flow_direction = FlowDirection::RIGHT;
        setLiquidLevel(x, y, avg);
        
    } else if(left_exists) {
        short avg = (getLiquidLevel(x, y) + getLiquidLevel(x - 1, y)) / 2;
        short mod = (getLiquidLevel(x, y) + getLiquidLevel(x - 1, y)) % 2;
        setLiquidLevel(x - 1, y, avg + mod);
        getLiquid(x - 1, y)->flow_direction = FlowDirection::LEFT;
        setLiquidLevel(x, y, avg);
    }
}

void Liquids::setLiquidLevelSilently(int x, int y, unsigned char level) {
    getLiquid(x, y)->level = level;
}

void Liquids::setLiquidLevel(int x, int y, unsigned char level) {
    if(level != getLiquidLevel(x, y)) {
        setLiquidLevelSilently(x, y, level);
        if(level == 0)
            setLiquidType(x, y, LiquidTypeOld::EMPTY);
        
        LiquidChangeEvent event(x, y);
        liquid_change_event.call(event);
    }
}

unsigned char Liquids::getLiquidLevel(int x, int y) {
    return getLiquid(x, y)->level;
}

void Liquids::serialize(std::vector<char>& serial) {
    serial.reserve(serial.size() + blocks->getWidth() * blocks->getHeight() * 2);
    Liquid* liquid = liquids;
    for(int y = 0; y < blocks->getHeight(); y++)
        for(int x = 0; x < blocks->getWidth(); x++) {
            if(blocks->getBlockInfo(x, y).ghost) {
                serial.push_back((char)liquid->type);
                if(getLiquidType(x, y) != LiquidTypeOld::EMPTY)
                    serial.push_back((char)liquid->level);
            }
            liquid++;
        }
}

char* Liquids::loadFromSerial(char* iter) {
    create();
    Liquid* liquid = liquids;
    for(int y = 0; y < blocks->getHeight(); y++)
        for(int x = 0; x < blocks->getWidth(); x++) {
            if(blocks->getBlockInfo(x, y).ghost) {
                liquid->type = (LiquidTypeOld)*iter++;
                if(liquid->type != LiquidTypeOld::EMPTY)
                    liquid->level = *iter++;
            }
            liquid++;
        }
    return iter;
}

int Liquids::getWidth() const {
    return blocks->getWidth();
}

int Liquids::getHeight() const {
    return blocks->getHeight();
}

void Liquids::registerNewLiquidType(LiquidType* liquid_type) {
    liquid_type->id = liquid_types.size();
    liquid_types.push_back(liquid_type);
}

LiquidType* Liquids::getLiquidTypeById(unsigned char liquid_id) {
    return liquid_types[liquid_id];
}

unsigned char Liquids::getNumLiquidTypes() {
    return liquid_types.size();
}


Liquids::~Liquids() {
    delete[] liquids;
}
