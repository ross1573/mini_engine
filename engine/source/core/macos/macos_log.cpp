module;

void DarwinLog(char const* msg);

module mini.core;

void LogMessage(char const* msg)
{
    DarwinLog(msg);
}