#pragma once
/** WIN32 */
#define WIN32_MEAN_AND_LEAN
#define NO_MIN_MAX
#include <Windows.h>

/** Standard Library */
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <sstream>
#include <string_view>
#include <string>
#include <algorithm>
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

/** Thirdparty */
#include <taskflow/taskflow.hpp>

// Direct3D
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <D3D12MemoryAllocator/D3D12MemAlloc.h>

// DirectXMath : requires Windows 11 SDK(10.0.22000)+
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

// DirectX Shader Compiler
#include <dxcapi.h>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <imgui/imgui.h>

namespace anya
{
    namespace fs = std::filesystem;
    namespace chrono = std::chrono;
}