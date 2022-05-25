#pragma once
#include <PCH.h>

namespace sy
{
	template <typename UINT_TYPE>
		requires std::unsigned_integral<UINT_TYPE>
	struct DimensionType
	{
	public:
		DimensionType(UINT_TYPE width = 1, UINT_TYPE height = 1, UINT_TYPE depth = 1) :
			Width(width), Height(height), Depth(depth)
		{
		}

		inline bool operator==(const DimensionType& rhs) const
		{
			return (Width == rhs.Width) && (Height == rhs.Height) && (Depth == rhs.Depth);
		}

		inline bool operator!=(const DimensionType& rhs) const
		{
			return (Width != rhs.Width) || (Height != rhs.Height) || (Depth != rhs.Depth);
		}

	public:
		UINT_TYPE Width = 1;
		UINT_TYPE Height = 1;
		UINT_TYPE Depth = 1;
	};

	using Dimension = DimensionType<uint32_t>;
	using Dimension64 = DimensionType<uint64_t>;

}