#include "clientInventory.hpp"

const ItemInfo& ClientInventoryItem::getUniqueItem() const {
    return ::getItemInfo(item_id);
}

void ClientInventoryItem::setStack(unsigned short stack_) {
    if(stack != stack_) {
        stack = stack_;
        stack_changed = true;
    }
}

unsigned short ClientInventoryItem::getStack() const {
    return stack;
}

unsigned short ClientInventoryItem::increaseStack(unsigned short stack_) {
    int stack_to_be = stack + stack_, result;
    if(stack_to_be > getUniqueItem().stack_size)
        stack_to_be = getUniqueItem().stack_size;
    result = stack_to_be - stack;
    setStack((unsigned short)stack_to_be);
    return (unsigned short)result;
}
