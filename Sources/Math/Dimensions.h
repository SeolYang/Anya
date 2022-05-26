#pragma once
#include <PCH.h>

namespace sy
{
	template <typename UINT_TYPE>
		requires std::unsigned_integral<UINT_TYPE>
	struct DimensionsType
	{
	public:
		DimensionsType(UINT_TYPE width = 1, UINT_TYPE height = 1, UINT_TYPE depth = 1) :
			Width(width), Height(height), Depth(depth)
		{
		}

		inline bool operator==(const DimensionsType& rhs) const
		{
			return (Width == rhs.Width) && (Height == rhs.Height) && (Depth == rhs.Depth);
		}

		inline bool operator!=(const DimensionsType& rhs) const
		{
			return (Width != rhs.Width) || (Height != rhs.Height) || (Depth != rhs.Depth);
		}

	public:
		UINT_TYPE Width = 1;
		UINT_TYPE Height = 1;
		UINT_TYPE Depth = 1;
	};

	using Dimensions = DimensionsType<uint32_t>;
	using Dimensions64 = DimensionsType<uint64_t>;

}