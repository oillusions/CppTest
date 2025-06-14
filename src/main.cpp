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
        void virtual load(path, function<void(shared_ptr<T>)>) = 0;
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

                loader->load(asset_path, listener);
            } else {
                cerr << "No Loader" << endl;
            }
        }
    private:
        static map<type_index, shared_ptr<BaseAssetLoader>> loaders;
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
        void load(path asset_path, function<void(shared_ptr<Text>)> listener) override {
            if (exists(asset_path)) {
                ifstream asset_file(asset_path);
                if (asset_file.is_open()) {
                    stringstream buffer;
                    buffer << asset_file.rdbuf();
                    listener(make_shared<Text>(buffer.str()));
                } else {
                    cerr << "could not open file" << endl;
                }
            } else {
                cerr << "NO File" << endl;
            }
        }
};


map<type_index, shared_ptr<BaseAssetLoader>> AssetManager::loaders;

int main() {
    AssetManager::registerLoader<Text>(make_shared<TextLoader>());

    AssetManager::request<Text>("assets/text/text.txt", [](shared_ptr<Text> text) {
        cout << text->getText() << endl;
    });
}
