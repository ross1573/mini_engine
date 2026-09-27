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

    void Launch();
    void Shutdown();

    static void Quit();
    static void Abort(String const& = "");

    static bool Running() noexcept;
    static uint64 FrameCount() noexcept;
};

ENGINE_API Engine* g_engine = nullptr;

} // namespace mini