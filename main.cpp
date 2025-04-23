#include <iostream>
#include <unordered_map>
#include <map>
#include <memory>
#include <stdexcept>

class DuplicateItemException : public std::runtime_error {
public:
    DuplicateItemException(const std::string& msg) : std::runtime_error(msg) {}
};

class ItemNotFoundException : public std::runtime_error {
public:
    ItemNotFoundException(const std::string& msg) : std::runtime_error(msg) {}
};

class StoredItem {
private:
    std::string id;
    std::string description;
    std::string location;

public:
    StoredItem(std::string id, std::string desc, std::string loc)
        : id(id), description(desc), location(loc) {}

    std::string getId() const { return id; }
    std::string getDescription() const { return description; }
    std::string getLocation() const { return location; }
};

class StorageManager {
private:
    std::unordered_map<std::string, std::shared_ptr<StoredItem>> itemById;
    std::map<std::string, std::shared_ptr<StoredItem>> itemByDescription;

public:
    void addItem(const std::shared_ptr<StoredItem>& item) {
        if (itemById.find(item->getId()) != itemById.end()) {
            throw DuplicateItemException("Item with ID " + item->getId() + " already exists!");
        }
        itemById[item->getId()] = item;
        itemByDescription[item->getDescription()] = item;
        std::cout << "Added: " << item->getId() << " - " << item->getDescription() << std::endl;
    }

    std::shared_ptr<StoredItem> findById(const std::string& id) const {
        auto it = itemById.find(id);
        if (it == itemById.end()) {
            throw ItemNotFoundException("Item with ID " + id + " not found!");
        }
        return it->second;
    }

    void removeItem(const std::string& id) {
        auto it = itemById.find(id);
        if (it == itemById.end()) {
            throw ItemNotFoundException("Item with ID " + id + " not found!");
        }
        std::string description = it->second->getDescription();
        itemById.erase(id);
        itemByDescription.erase(description);
        std::cout << "Removed: " << id << std::endl;
    }

    void listItemsByDescription() const {
        std::cout << "Items in Description Order:\n";
        for (const auto& pair : itemByDescription) {
            std::cout << "- " << pair.second->getDescription()
                      << ": " << pair.second->getLocation() << std::endl;
        }
    }
};

void testDuplicateAddition(StorageManager& manager) {
    try {
        auto item = std::make_shared<StoredItem>("ITEM001", "LED Light", "Aisle 3, Shelf 1");
        manager.addItem(item);
        manager.addItem(item);  // Attempt to add duplicate
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void testItemNotFound(StorageManager& manager) {
    try {
        manager.removeItem("ITEM999");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    StorageManager manager;

    auto item1 = std::make_shared<StoredItem>("ITEM001", "LED Light", "Aisle 3, Shelf 1");
    auto item2 = std::make_shared<StoredItem>("ITEM002", "Fan Motor", "Aisle 2, Shelf 5");

    manager.addItem(item1);
    manager.addItem(item2);

    std::cout << "\nRetrieving ITEM002...\n";
    try {
        auto found = manager.findById("ITEM002");
        std::cout << "Found: " << found->getDescription() << " at " << found->getLocation() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "\nRemoving ITEM003...\n";
    testItemNotFound(manager);

    std::cout << "\nTesting duplicate addition...\n";
    testDuplicateAddition(manager);

    std::cout << "\nListing all items by description:\n";
    manager.listItemsByDescription();

    return 0;
}
