#pragma once

#include <cstdint>

namespace SCONE
{

class TaggedPtr final
{
public:
    TaggedPtr() = default;

    TaggedPtr(void* ptr);
    TaggedPtr& operator=(void* ptr);

    template <typename PtrType>
    PtrType* getAs() const
    {
        return reinterpret_cast<PtrType*>(_ptr & (~uintptr_t(1)));
    }

    explicit operator bool() const;

    bool hasFlag() const;
    void setFlag(const bool value);

    void swap(TaggedPtr& other);

private:
    uintptr_t _ptr = 0;
};

} // namespace SCONE
