#ifndef fileManager_hpp
#define fileManager_hpp

#include <string>

namespace fileManager {

    void init();

    std::string getWorldsPath();
    std::string getModsPath();
    std::string getDataPath();

}

#endif
