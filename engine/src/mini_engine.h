#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <cstring>
#include <cmath>
#include <type_traits>
#include <concepts>
#include <iterator>
#include <memory>
#include <utility>
#include <atomic>
#include <string>
#include <string_view>
#include <format>
#include <source_location>

#include "core/define.h"

#ifdef PLATFORM_WINDOWS
#   include <SDKDDKVer.h>
#   include <initguid.h>
#   include <windows.h>
#   include <winuser.h>
#   include <wrl.h>
#   include <d3d11.h>
#   include <d3d12.h>
#   include <dxgi1_6.h>
#   include <d3dx12.h>
#   include <d3dcompiler.h>
#undef DELETE
#ifdef DEBUG
#   include <crtdbg.h>
#   include <dxgidebug.h>
#endif
#   include <malloc.h>
#   include <memory.h>
#   include <tchar.h>
#endif

#include "core/type.h"
#include "core/assert.h"
#include "core/utility.h"
#include "core/option.h"
#include "core/log.h"

#include "container/array.h"
#include "container/static_array.h"
#include "container/static_queue.h"
#include "container/string.h"

#include "math/common.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/vector2_int.h"
#include "math/vector3_int.h"
#include "math/rect.h"
#include "math/rect_int.h"
#include "math/vertex.h"
#include "math/color.h"

#include "platform/platform.h"
#include "platform/handle.h"
#include "platform/window.h"

#include "graphics/graphics.h"
#include "graphics/device.h"
#include "graphics/swap_chain.h"
#include "graphics/render_context.h"

#include "engine/engine.h"
