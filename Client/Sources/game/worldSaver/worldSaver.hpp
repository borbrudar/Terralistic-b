//
//  worldSaver.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 04/12/2020.
//

#ifndef worldSaver_hpp
#define worldSaver_hpp

namespace worldSaver {

void saveWorld(const std::string& world_name, map& world_map);
void loadWorld(const std::string& world_name, map& world_map);

}

#endif /* worldSaver_hpp */
