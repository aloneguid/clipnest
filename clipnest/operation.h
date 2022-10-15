#include <string>
#include <map>
#include <memory>

namespace clipnest {
	class operation {
    public:
        operation(const std::string& id, const std::string& name) 
            : id{ id }, name{ name } {

        }

        virtual void compute(const std::string& input) = 0;

        const std::string id;
        const std::string name;

        std::string result;

        static std::map<std::string, std::shared_ptr<operation>> all;

        static void init();

        static void compute_all(const std::string& input);
        
    private:

        template<class TOp> static void add() {
            auto ptr = std::make_shared<TOp>();
            all[ptr->id] = ptr;
        }
	};
}