#pragma once
#include <PCH.h>

namespace sy
{
    template <typename SlotDataType = void>
    class Pool
    {
    public:
        template <typename DataType>
        struct SlotType
        {
            size_t Offset;
            DataType Data;
        };

        template <>
        struct SlotType<void>
        {
            size_t Offset;
        };

        using Slot = SlotType<SlotDataType>;

        Pool(const size_t sizePerSlot = sizeof(SlotDataType), const size_t numOfGrowSlots = 64) :
            sizePerSlot(sizePerSlot),
            numOfGrowSlots(numOfGrowSlots),
            allocatedSize(0)
        {
        }

        Slot Allocate()
        {
            if (freeSlots.empty())
            {
                Grow();
            }

            Slot slot = std::move(freeSlots.front());
            freeSlots.pop();
            return slot;
        }

        void Deallocate(Slot&& slot)
        {
            freeSlots.emplace(std::move(slot));
        }

    private:
        void Grow()
        {
            for (size_t idx = 0; idx < numOfGrowSlots; ++idx)
            {
                freeSlots.emplace(Slot{allocatedSize});
                allocatedSize += sizePerSlot;
            }
        }

    private:
        std::queue<Slot> freeSlots;
        const size_t sizePerSlot;
        const size_t numOfGrowSlots;
        size_t allocatedSize;

    };

    using OffsetPool = Pool<void>;
}