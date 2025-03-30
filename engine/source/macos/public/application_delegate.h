struct ApplicationDelegate {
public:
    void Run();
    void Stop();

    virtual void ApplicationWillFinishLaunching(void*) = 0;
    virtual void ApplicationDidFinishLaunching(void*) = 0;
};