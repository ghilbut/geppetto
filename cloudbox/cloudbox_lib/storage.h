#ifndef STORAGE_H_
#define STORAGE_H_

#include <string>


class Storage {
public:
    Storage(const std::string& root);
    ~Storage(void);

    const char* settings(void) const;

private:
    const std::string root_;
    std::string settings_;
};

#endif  // STORAGE_H_