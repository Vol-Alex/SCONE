#include "TaggedPtr.h"

#include <algorithm>
#include <cassert>

namespace SCONE
{

TaggedPtr::TaggedPtr(void* ptr)
{
    *this = ptr;
}

TaggedPtr& TaggedPtr::operator=(void* ptr)
{
    _ptr = reinterpret_cast<uintptr_t>(ptr);
    assert(!(_ptr & 1));

    return *this;
}

TaggedPtr::operator bool() const
{
    return _ptr;
}

bool TaggedPtr::hasFlag() const
{
    return _ptr & 1;
}

void TaggedPtr::setFlag(const bool value)
{
    if (value)
    {
        _ptr |= 1;
    }
    else
    {
        _ptr &= ~uintptr_t(1U);
    }
}

void TaggedPtr::swap(TaggedPtr& other)
{
    std::swap(_ptr, other._ptr);
}

} // namespace SCONE
