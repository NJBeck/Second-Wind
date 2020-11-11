#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include "glad/glad.h"


namespace utility {
    // gives the absolute path to the asset from the relative path in /data/ folder
    std::string getDataPath(std::string);

    GLenum glCheckError_(const char* file, int line);

}
