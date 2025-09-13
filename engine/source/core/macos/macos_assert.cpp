import mini.core;

constexpr int bufSize = DEBUG * 1023 + 1;

char assertMsg[bufSize] = { 0 };
char funcInfo[bufSize] = { 0 };

namespace mini::detail {

int ConcatStrings(char* dest, int destLen, char const** src, int srcCount);
int IntegerToASCII(char* dest, int destLen, int src);
int SourceLocationToString(char* dest, int destLen, std::source_location const& loc);

char* AssertMsg(char const* expr, char const* msg)
{
    char const* str[3] = { expr, msg == nullptr ? nullptr : "', message: '", msg };
    ConcatStrings(assertMsg, sizeof(assertMsg), str, 3);
    return assertMsg;
}

char* AssertLoc(std::source_location const& loc)
{
    int len = SourceLocationToString(funcInfo, sizeof(funcInfo), loc);
    memory::MemCopy(funcInfo + len, "\n\0", 3);
    return funcInfo;
}

void EnsureHelper(char const* expr, char const* msg, std::source_location const& loc)
{
    char locBuffer[512];
    int len = SourceLocationToString(locBuffer, sizeof(locBuffer) - 2, loc);

    memory::MemCopy(locBuffer + len, "\n\n\0", 3);
    log::Message(Format("\nEnsure failed!\n"
                        "  Expression: {0}\n"
                        "  Message: {1}\n"
                        "  Function: {2}",
                        expr, (msg == nullptr ? "" : msg), locBuffer));
}

} // namespace mini::detail
