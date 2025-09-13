module;

void DarwinLog(char const* msg);

module mini.core;

void PlatformLog(char const* msg)
{
    DarwinLog(msg);
}