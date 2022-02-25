#include "ziapi/Module.hpp"

#include "dylib/dylib.hpp"

class Module : public ziapi::IModule {
public:
    Module() = default;

    ~Module() override = default;

    void Init(const ziapi::config::Node &) override {}

    ziapi::Version GetVersion() const noexcept override { return {4, 0, 0}; }

    ziapi::Version GetCompatibleApiVersion() const noexcept override { return {4, 0, 0}; }

    [[nodiscard]] virtual const char *GetName() const noexcept override { return "module_name"; }

    [[nodiscard]] virtual const char *GetDescription() const noexcept override
    {
        return "A module implementation example";
    }
};

DYLIB_API ziapi::IModule *LoadZiaModule() { return new Module; }
