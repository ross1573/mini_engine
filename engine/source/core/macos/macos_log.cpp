module;

CORE_API void DarwinLog(char const* msg);

module mini.core;

void LogMessage(char const* msg)
{
    DarwinLog(msg);
}