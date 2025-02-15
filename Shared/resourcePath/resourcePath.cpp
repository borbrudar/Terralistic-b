#include <set>
#include "resourcePath.hpp"

std::string removeEndUntilChars(std::string string, const std::set<char>& chars) {
    while(!string.empty() && chars.count(string.back()) == 0)
        string.pop_back();
    if(!string.empty())
        string.pop_back();
    return string;
}

std::string returnEndUntilChars(std::string string, const std::set<char>& chars) {
    int iter = (int)string.size() - 1;
    while(iter >= 0 && chars.count(string[iter]) == 0)
        iter--;
    return string.substr(iter + 1, string.size());
}

std::string getResourcePath(const std::string& executable_path) {
    std::string parent_directory = removeEndUntilChars(executable_path, {'/', '\\'});

    if(parent_directory.empty())
        parent_directory = ".";
    
    std::string parent_parent_directory = removeEndUntilChars(parent_directory, {'/', '\\'}) + "/";
    std::string parent_directory_name = returnEndUntilChars(parent_directory, {'/', '\\'});

    return parent_directory_name == "MacOS" ? parent_parent_directory + "Resources/" : parent_directory + "/Resources/";
}
