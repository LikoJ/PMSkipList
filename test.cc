#include <iostream>
#include <string>
#include "skiplist.h"
int main() {
    PMSkiplist::Skiplist l;
    std::string key, value;

    std::cout << "--------Insert--------" << std::endl;
    for (int i = 0; i < 10; i++, i++) {
        key = "k";
        key += std::to_string(i);
        value = "v";
        value += std::to_string(i);
        l.Write(key, value);
        std::cout << key << ": " << value << std::endl;
    }
    for (int i = 1; i < 10; i++, i++) {
        key = "k";
        key += std::to_string(i);
        value = "v";
        value += std::to_string(i);
        l.Write(key, value);
        std::cout << key << ": " << value << std::endl;
    }

    std::cout << "---------Read---------" << std::endl;
    for (int i = 0; i < 10; i++) {
        key = "k";
        key += std::to_string(i);
        if (l.Read(key, &value)) {
            std::cout << key << ": " << value << std::endl;
        } else {
            std::cout << key << ": not found!" << std::endl;
        }
    }

    // not found
    key = "k";
    if (l.Read(key, &value)) {
        std::cout << key << ": " << value << std::endl;
    } else {
        std::cout << key << ": not found!" << std::endl;
    }

    std::cout << "--------Update--------" << std::endl;
    for (int i = 0; i < 10; i += 3) {
        key = "k";
        key += std::to_string(i);
        value = "u";
        value += std::to_string(i);
        l.Write(key, value);
        std::cout << key << ": " << value << std::endl;
    }
    
    std::cout << "---------Read---------" << std::endl;
    for (int i = 0; i < 10; i++) {
        key = "k";
        key += std::to_string(i);
        if (l.Read(key, &value)) {
            std::cout << key << ": " << value << std::endl;
        } else {
            std::cout << key << ": not found!" << std::endl;
        }
    }

    std::cout << "---------Scan---------" << std::endl;
    PMSkiplist::Iterator it(&l);
    key = "k";
    key += std::to_string(4);
    for (it.Seek(key); it.Valid(); it.Next()) {
        key = it.Key();
        value = it.Value();
        std::cout << key << ": " << value << std::endl;
    }
    return 0;
}