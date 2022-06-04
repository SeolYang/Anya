#pragma once
#pragma warning(disable : 4127) /** https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4127?view=msvc-170 */
#pragma warning(disable : 4324) /** https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4324?view=msvc-170 */

/** WIN32 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

/** Standard Library */
#include <vector>
#include <array>
#include <queue>
#include <unordered_map>
#include <map>
#include <memory>
#include <sstream>
#include <string_view>
#include <string>
#include <algorithm>
#include <atomic>
#include <optional>
#include <variant>
#include <type_traits>
#include <random>
#include <chrono>
#include <numeric>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <filesystem>
#include <thread>
#include <utility>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <ranges>
#include <numbers>

/** Third-party */
// Direct3D
extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 700; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\"; }
#include <AgilitySDK/d3d12.h>
#include <AgilitySDK/d3d12sdklayers.h>
#include <AgilitySDK/d3d12shader.h>
#include <AgilitySDK/d3dcommon.h>
#include <AgilitySDK/dxgiformat.h>
#include <d3dx12.h>
#include <dxgi.h>
#include <dxgi1_6.h>

// DirectXMath : requires Windows 11 SDK(10.0.22000)+
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

// DirectX Shader Compiler
#include <dxcapi.h>

// DirectXTex
#pragma warning(push)
#pragma warning(disable:26812)
#include <WICTextureLoader/WICTextureLoader.h>
#include <DDSTextureLoader/DDSTextureLoader.h>
#pragma warning(pop)

// D3D12MemoryAllocator
/** https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator */
#pragma warning(push)
#pragma warning(disable:26812 26495 6387 6386 26110)
#include <D3D12MemoryAllocator/D3D12MemAlloc.h>
#pragma warning(pop)

// Taskflow
/** https://github.com/taskflow/taskflow */
#pragma warning(push)
#pragma warning(disable:6387 26439 26495)
#include <taskflow/taskflow.hpp>
#pragma warning(pop)

// Assimp
/** https://github.com/assimp/assimp */
#pragma warning(push)
#pragma warning(disable:26451 26495 26812)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma warning(pop)

// STB
/** https://github.com/nothings/stb */
#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable:6262 26451)
#include <stb/stb_image.h>
#pragma warning(pop)

// Dear IMGUI
/** https://github.com/ocornut/imgui */
#include <imgui/imgui.h>

// robin_hood
/** https://github.com/martinus/robin-hood-hashing */
#include <robin_hood/robin_hood.h>

// spdlog
/** https://github.com/gabime/spdlog */
#pragma warning(push)
#pragma warning(disable:4819 6285 26437 26451 26498 26800)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

// WinPixEventRuntime
/** https://devblogs.microsoft.com/pix/winpixeventruntime */
#define USE_PIX
#ifdef USE_PIX
#pragma warning(push)
#pragma warning(disable:6101)
#include <WinPixEventRuntime/pix3.h>
#pragma warning(pop)
#endif

// nlohmann's json lib (single-header verision)
/** https://github.com/nlohmann/json */
#include <nlohmann/json.hpp>

namespace sy
{
    namespace fs = std::filesystem;
    namespace chrono = std::chrono;
    namespace ranges = std::ranges;
    namespace views = std::views;
    using namespace nlohmann;

    using uint8 = uint8_t;
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    using uint64 = uint64_t;

    using int8 = int8_t;
    using int16 = int16_t;
    using int32 = int32_t;
    using int64 = int64_t;

    using float32 = float;
    using float64 = double;

    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    using Logger = spdlog::logger;

    template <typename T>
    using OptionalRef = std::optional<std::reference_wrapper<T>>;

    using Mutex = std::shared_timed_mutex;
    using ReadWriteLock = std::unique_lock<Mutex>;
    using ReadOnlyLock = std::shared_lock<Mutex>;
}