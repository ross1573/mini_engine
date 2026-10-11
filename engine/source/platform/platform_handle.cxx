export module mini.platform:handle;

import mini.graphics;
import :window;

namespace mini::platform {

export class PLATFORM_API Handle {
public:
    virtual ~Handle() noexcept = default;

    virtual void pollEvents() = 0;
    virtual void alertError(StringView const&) = 0;

    [[nodiscard]] virtual bool valid() const noexcept = 0;

protected:
    Handle() noexcept = default;
};

} // namespace mini::platform