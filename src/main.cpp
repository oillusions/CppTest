#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>
#include <map>
#include <typeindex>
using namespace std;
using namespace filesystem;

class BaseAssetLoader {
    public:
        virtual ~BaseAssetLoader() = default;
};

template<typename T>
class AssetLoader : public BaseAssetLoader {
    public:
        void virtual load(path, function<void(T)>) = 0;
};

class AssetManager {
    public:
        template<typename T>
        static void registerLoader(const shared_ptr<AssetLoader<T>>& loader) {
            loaders.emplace(typeid(T), static_pointer_cast<BaseAssetLoader>(loader));
        }

        template<typename T>
        static void request(const path& asset_path, const function<void(shared_ptr<T>)>& listener) {
            if (loaders.count(typeid(T))) {
                shared_ptr<AssetLoader<T>> loader = dynamic_pointer_cast<AssetLoader<T>>(loaders[typeid(T)]);
            }
        }
    private:
        static map<type_index, BaseAssetLoader> loaders;
};

class Text {
    public:
        explicit Text(string text) : text(std::move(text)) {}
        string getText() {return text;}
    private:
        string text;
};

class TextLoader : public AssetLoader<Text> {
    public:
        void load(path asset_path, function<void(Text)> listener) override {
            if (exists(asset_path)) {
                ifstream asset_file(asset_path);
                if (asset_file.is_open()) {
                    stringstream stream;
                    stream << asset_file.rdbuf();
                    listener(Text(stream.str()));
                }
            }
        }
};


map<type_index, BaseAssetLoader> AssetManager::loaders;

int main() {
    AssetManager::registerLoader<Text>(make_shared<TextLoader>());

    AssetManager::request<Text>("assets/text/text.txt", [](shared_ptr<Text> text) {
        cout << text->getText() << endl;
    });
}
