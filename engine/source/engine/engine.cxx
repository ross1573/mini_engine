export module mini.engine;

import mini.core;
import mini.platform;
import mini.graphics;

namespace mini {

export class ENGINE_API Engine final : public ModuleInterface {
private:
    bool m_Running;

public:
    Engine();
    ~Engine() noexcept;

    void Launch();
    void Shutdown();

    static void Quit();
    static void Abort(String const& = "");

    static bool Running() noexcept;
};

ENGINE_API Engine* engine = nullptr;

} // namespace mini