# Directory Listing Module Tutorial

Let's implement a simple directory listing module.

## Purpose

This module will handle incoming http requests and serve the file contained in the request's target field.

For example, for the following request:
```
GET /index.html HTTP/1.1
```

Our module will open the `index.html` and serve it back to the user.

## Tutorial

First, let's implement the `IHandlerModule` interface. We give it a basic priority and we specify that it only handles GET requests.

```c++
#include <fstream>

#include "ziapi/Module.hpp"

class DirectoryListingModule : public ziapi::IHandlerModule {
public:
    void Init(const ziapi::config::Node &cfg) override {}

    [[nodiscard]] ziapi::Version GetVersion() const noexcept override { return {3, 1, 0}; }

    [[nodiscard]] ziapi::Version GetCompatibleApiVersion() const noexcept override { return {3, 1, 0}; }

    [[nodiscard]] const char *GetName() const noexcept override { return "DirectoryListing"; }

    [[nodiscard]] const char *GetDescription() const noexcept override { return "Give access to a filesystem over HTTP"; }

    [[nodiscard]] double GetHandlerPriority() const noexcept override
    {
        /// Our module doesn't have any specific priority requirements.
        return 0.5f;
    }

    [[nodiscard]] bool ShouldHandle(const ziapi::http::Context &ctx, const ziapi::http::Request &req) const override
    {
        /// We only want to handle GET requests.
        return req.method == ziapi::http::method::kGet;
    }

    void Handle(ziapi::http::Context &ctx, const ziapi::http::Request &req, ziapi::http::Response &res) override {}
};
```

First thing we want to do is make our module configurable. Imagine we want to serve a specific directory on our computer. Like let's say we want to make the contents of the `/var/www` directory available.

Well, we can add the path to this directory as a variable of our config file and fetch it in the `Init()` function. We will store it in a `root_` member variable of our class.

```c++
...

void Init(const ziapi::config::Node &cfg) override
{
    /// In our config, we can specify which folder our module serves.
    root_ = cfg["modules"]["directoryListing"]["root"].AsString();
}

...
```

Let's now code the `Handle()` method. We first obtain the path of the file we want to serve, then we check if it's a directory or a file and we either send back the contents of the file or the contents of the directory using `std::filesystem`.

```c++
...

void Handle(ziapi::http::Context &ctx, const ziapi::http::Request &req, ziapi::http::Response &res) override
{
        auto filepath = std::filesystem::path(root_) / std::filesystem::path(req.target);
        std::error_code ec;

        res.status_code = ziapi::http::code::OK;
        res.reason = ziapi::http::reason::OK;
        if (std::filesystem::is_directory(filepath, ec)) {
            std::ostringstream ss;
            for (const auto &entry : std::filesystem::directory_iterator(filepath)) {
                ss << entry.path().string() << "\n";
            }
            res.body = ss.str();
            return;
        } else {
            std::ifstream file_stream(filepath.filename());
            std::ostringstream ss;
            ss << file_stream.rdbuf();
            res.body = ss.str();
            return;
        }
        res.status_code = ziapi::http::code::NOT_FOUND;
        res.reason = ziapi::http::reason::NOT_FOUND;
}

...
```

You can check the full source code for this example [here](/examples/modules/directory-listing/DirectoryListingModule.hpp).
