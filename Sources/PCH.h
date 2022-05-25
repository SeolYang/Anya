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
#include <condition_variable>

/** Thirdparty */
#pragma warning(push)
#pragma warning(disable:6387 26439 26495)
#include <taskflow/taskflow.hpp>
#pragma warning(pop)

// Direct3D
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi.h>
#include <dxgi1_6.h>

#pragma warning(push)
#pragma warning(disable:26812)
#include <D3D12MemoryAllocator/D3D12MemAlloc.h>
#pragma warning(pop)

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

// Assimp
#pragma warning(push)
#pragma warning(disable:26451 26495 26812)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma warning(pop)

// STB
#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable:6262 26451)
#include <stb/stb_image.h>
#pragma warning(pop)

// IMGUI
#include <imgui/imgui.h>

// robin_hood
#include <robin_hood/robin_hood.h>

// SPDLOG
#pragma warning(push)
#pragma warning(disable:4819 6285 26437 26451 26498 26800)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace sy
{
    namespace fs = std::filesystem;
    namespace chrono = std::chrono;

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

    template <typename T>
    using OptionalRef = std::optional<std::reference_wrapper<T>>;
}