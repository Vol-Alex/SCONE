#pragma once

#include <cstdint>

namespace SCONE
{
namespace VectorDetails
{
template <typename T>
class MemoryOptimizedStorage;

template <typename T, uint32_t InlineSize>
class InlineStorage;

} // namespace VectorDetails

template <typename StorageType>
class Vector;

template <typename T>
using CompactVector = Vector<VectorDetails::MemoryOptimizedStorage<T>>;

template <typename T, uint32_t InlineSize = 1U>
using InlineVector = Vector<VectorDetails::InlineStorage<T, InlineSize>>;

} // namespace SCONE
