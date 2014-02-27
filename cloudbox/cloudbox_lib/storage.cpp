#include "storage.h"
#include <boost/filesystem.hpp>


Storage::Storage(const std::string& root)
    : root_(boost::filesystem::canonical(root).generic_string())
    , settings_(boost::filesystem::canonical("settings.js", root).generic_string()) {
    // nothing
}

Storage::~Storage(void) {
    // nothing
}

const char* Storage::settings(void) const {
    return settings_.c_str();
}