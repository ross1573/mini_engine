module;

#include <Metal/MTLLogState.hpp>
#include <Metal/MTLPixelFormat.hpp>
#include <cstddef>

export module mini.metal4:common;

import mini.core;
import mini.apple;
import mini.graphics;
import :device;

export namespace MTL {

using MTL::LogLevel;
using MTL::LogState;
using MTL::LogStateDescriptor;
using MTL::PixelFormat;

} // namespace MTL

namespace mini::metal4 {

export METAL4_API constexpr MTL::PixelFormat MTLPixelFormat(graphics::PixelFormat pixelFormat) noexcept
{
    switch (pixelFormat) {
        case graphics::PixelFormat::Null:           return MTL::PixelFormatInvalid;
        case graphics::PixelFormat::Unspecialized:  return MTL::PixelFormatUnspecialized;
        case graphics::PixelFormat::A8unorm:        return MTL::PixelFormatA8Unorm;
        case graphics::PixelFormat::R8unorm:        return MTL::PixelFormatR8Unorm;
        case graphics::PixelFormat::R8unormSRGB:    return MTL::PixelFormatR8Unorm_sRGB;
        case graphics::PixelFormat::R8norm:         return MTL::PixelFormatR8Snorm;
        case graphics::PixelFormat::R8uint:         return MTL::PixelFormatR8Uint;
        case graphics::PixelFormat::R8int:          return MTL::PixelFormatR8Sint;
        case graphics::PixelFormat::R16unorm:       return MTL::PixelFormatR16Unorm;
        case graphics::PixelFormat::R16norm:        return MTL::PixelFormatR16Snorm;
        case graphics::PixelFormat::R16uint:        return MTL::PixelFormatR16Uint;
        case graphics::PixelFormat::R16int:         return MTL::PixelFormatR16Sint;
        case graphics::PixelFormat::R16float:       return MTL::PixelFormatR16Float;
        case graphics::PixelFormat::RG8unorm:       return MTL::PixelFormatRG8Unorm;
        case graphics::PixelFormat::RG8unormSRGB:   return MTL::PixelFormatRG8Unorm_sRGB;
        case graphics::PixelFormat::RG8norm:        return MTL::PixelFormatRG8Snorm;
        case graphics::PixelFormat::RG8uint:        return MTL::PixelFormatRG8Uint;
        case graphics::PixelFormat::RG8int:         return MTL::PixelFormatRG8Sint;
        case graphics::PixelFormat::R32uint:        return MTL::PixelFormatR32Uint;
        case graphics::PixelFormat::R32int:         return MTL::PixelFormatR32Sint;
        case graphics::PixelFormat::R32float:       return MTL::PixelFormatR32Float;
        case graphics::PixelFormat::RG16unorm:      return MTL::PixelFormatRG16Unorm;
        case graphics::PixelFormat::RG16norm:       return MTL::PixelFormatRG16Snorm;
        case graphics::PixelFormat::RG16uint:       return MTL::PixelFormatRG16Uint;
        case graphics::PixelFormat::RG16int:        return MTL::PixelFormatRG16Sint;
        case graphics::PixelFormat::RG16float:      return MTL::PixelFormatRG16Float;
        case graphics::PixelFormat::RGBA8unorm:     return MTL::PixelFormatRGBA8Unorm;
        case graphics::PixelFormat::RGBA8unormSRGB: return MTL::PixelFormatRGBA8Unorm_sRGB;
        case graphics::PixelFormat::RGBA8norm:      return MTL::PixelFormatRGBA8Snorm;
        case graphics::PixelFormat::RGBA8uint:      return MTL::PixelFormatRGBA8Uint;
        case graphics::PixelFormat::RGBA8int:       return MTL::PixelFormatRGBA8Sint;
        case graphics::PixelFormat::BGRA8unorm:     return MTL::PixelFormatBGRA8Unorm;
        case graphics::PixelFormat::BGRA8unormSRGB: return MTL::PixelFormatBGRA8Unorm_sRGB;
        case graphics::PixelFormat::RG32uint:       return MTL::PixelFormatRG32Uint;
        case graphics::PixelFormat::RG32int:        return MTL::PixelFormatRG32Sint;
        case graphics::PixelFormat::RG32float:      return MTL::PixelFormatRG32Float;
        case graphics::PixelFormat::RGBA16unorm:    return MTL::PixelFormatRGBA16Unorm;
        case graphics::PixelFormat::RGBA16norm:     return MTL::PixelFormatRGBA16Snorm;
        case graphics::PixelFormat::RGBA16uint:     return MTL::PixelFormatRGBA16Uint;
        case graphics::PixelFormat::RGBA16int:      return MTL::PixelFormatRGBA16Sint;
        case graphics::PixelFormat::RGBA16float:    return MTL::PixelFormatRGBA16Float;
        case graphics::PixelFormat::RGBA32uint:     return MTL::PixelFormatRGBA32Uint;
        case graphics::PixelFormat::RGBA32int:      return MTL::PixelFormatRGBA32Sint;
        case graphics::PixelFormat::RGBA32float:    return MTL::PixelFormatRGBA32Float;
    };

    ASSERT(pixelFormat == graphics::PixelFormat::Null, "invalid pixel format {}.", static_cast<byte>(pixelFormat));
    return MTL::PixelFormat::PixelFormatInvalid;
}

export METAL4_API constexpr MTL::LogLevel MTLLogLevel(Logger::Level logLevel) noexcept
{
    switch (logLevel) {
        case Logger::Level::debug: return MTL::LogLevelDebug;
        case Logger::Level::info:  return MTL::LogLevelInfo;
        case Logger::Level::warn:  return MTL::LogLevelNotice;
        case Logger::Level::error: return MTL::LogLevelError;
        case Logger::Level::fatal: return MTL::LogLevelFault;
    }

    ASSERT(logLevel == Logger::Level::warn, "invalid log level {}.", static_cast<byte>(logLevel));
    return MTL::LogLevelNotice;
}

export METAL4_API constexpr Logger::Level LogLevel(MTL::LogLevel logLevel) noexcept
{
    switch (logLevel) {
        case MTL::LogLevelDebug:     return Logger::Level::debug;
        case MTL::LogLevelInfo:      return Logger::Level::info;
        case MTL::LogLevelUndefined:
        case MTL::LogLevelNotice:    return Logger::Level::warn;
        case MTL::LogLevelError:     return Logger::Level::error;
        case MTL::LogLevelFault:     return Logger::Level::fatal;
    }

    ASSERT(logLevel == MTL::LogLevelUndefined, "invalid log level {}.", static_cast<byte>(logLevel));
    return Logger::Level::warn;
}

export class METAL4_API LogState {
public:
    typedef void (*LogHandler)(NS::String* subSystem, NS::String* category, MTL::LogLevel logLevel, NS::String* mesage);

private:
    SharedPtr<MTL::LogState> m_logState;
    Logger::Level m_logLevel;

    static constexpr size_t defaultBufferSize = static_cast<size_t>(1024 * 1024) /*1MB*/;

public:
    LogState(Device* device, Logger::Level level, LogHandler logHandler, size_t bufferSize = defaultBufferSize);

    [[nodiscard]] bool Valid() const noexcept { return m_logState.Valid(); }
    [[nodiscard]] Logger::Level LogLevel() const noexcept { return m_logLevel; }

    [[nodiscard]] MTL::LogState* MTLLogState() const noexcept { return m_logState.Get(); }

private:
    void Init(Device* device, Logger::Level level, size_t bufferSize);
    static void HandleLog(NS::String* subSystem, NS::String* category, MTL::LogLevel logLevel, NS::String* mesage);
};

} // namespace mini::metal4