export module mini.platform:handle;

import mini.graphics;
import :window;

namespace mini::platform {

export class PLATFORM_API Handle {
public:
    virtual ~Handle() noexcept = default;

    virtual bool IsValid() const noexcept = 0;

    virtual void PollEvents() = 0;
    virtual void AlertError(StringView const&) = 0;

protected:
    Handle() = default;
};

} // namespace mini::platform