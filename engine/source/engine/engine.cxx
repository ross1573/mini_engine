export module mini.engine;

import mini.core;
import mini.platform;
import mini.graphics;

namespace mini {

export class ENGINE_API Engine final : public ModuleInterface {
private:
    bool m_running;
    uint64 m_frameCount;

    Module<Platform> m_platform;
    Module<Graphics> m_graphics;

public:
    Engine();
    ~Engine() noexcept override;

    void launch();
    void shutdown();

    static void quit();
    static void abort(String const& = "");

    static bool running() noexcept;
    static uint64 frameCount() noexcept;
};

ENGINE_API Engine* instance = nullptr;

} // namespace mini