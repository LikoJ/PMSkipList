#include <iostream>
#include <string>
#include "skiplist.h"
int main() {
    pmskiplist::Skiplist l;
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

    std::cout << "--Scan-from-k4-to-k9--" << std::endl;
    pmskiplist::Iterator it(&l);
    key = "k";
    key += std::to_string(4);
    for (it.Seek(key); it.Valid(); it.Next()) {
        key = it.Key();
        value = it.Value();
        if (value > "k9") {
            break;
        }
        std::cout << key << ": " << value << std::endl;
    }

    std::cout << "--------Delete--------" << std::endl;
    for (int i = 0; i < 10; i++) {
        key = "k";
        key += std::to_string(i);
        l.Delete(key);
        std::cout << key << ": " << "delete" << std::endl;
    }

    std::cout << "---------Scan---------" << std::endl;
    pmskiplist::Iterator itr(&l);
    for (; itr.Valid(); itr.Next()) {
        key = itr.Key();
        value = itr.Value();
        std::cout << key << ": " << value << std::endl;
    }
    return 0;
}