#include <memory>
#include <functional>
#include <map>
#include <iostream>

class Base {
protected:
    virtual ~Base()  = default;
    static constexpr std::string GetType() { return "Base"; }
};

class Store {
public:
    template<class T>
    void insert(std::shared_ptr<T> &object) {
        static_assert(std::is_base_of_v<Base, T>, "Error: T is not based on Base!");
        std::string type = T::GetType();
        this->store[type] = std::dynamic_pointer_cast<Base>(object);
    }

    template<class T>
    void on(std::function<void(std::shared_ptr<T> &)> callback) {
        static_assert(std::is_base_of_v<Base, T>, "Error: T is not based on Base!");
        std::string type = T::GetType();
        event_listeners[type] = [callback](std::shared_ptr<Base>  base) {
            std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(base);
            callback(ptr);
        };
    }

    template<class T>
    bool send() {
        static_assert(std::is_base_of_v<Base, T>, "Error: T is not based on Base!");
        std::string type = T::GetType();
        if(!store.count(type) || !event_listeners.count(type)){
            return false;
        }

        auto ptr = store[type];
        event_listeners[type]( ptr );
        return true;
    }

private:
    std::map<std::string, std::shared_ptr<Base>> store;
    std::map<std::string, std::function<void(std::shared_ptr<Base> )>> event_listeners;
};

class Asset : public Base{
public:
    virtual ~Asset() { }
    static constexpr std::string GetType() { return "Asset"; }
    std::string data = "fuck";
};

int main() {
    Store store;
    auto asset = std::make_shared<Asset>();
    store.insert(asset);
    store.on<Asset>([](std::shared_ptr<Asset> & p){
        std::cout << p->data << std::endl;
    });

    store.send<Asset>();
}