module;

#include "cmath.h"

export module mini.math:type;

import mini.type;

namespace mini {

template <typename T>
concept Int8ConvertibleT = ConvertibleToT<T, int8>;

template <typename T>
concept Int16ConvertibleT = ConvertibleToT<T, int16>;

template <typename T>
concept Int32ConvertibleT = ConvertibleToT<T, int32>;

template <typename T>
concept Int64ConvertibleT = ConvertibleToT<T, int64>;

template <typename T>
concept Uint8ConvertibleT = ConvertibleToT<T, uint8>;

template <typename T>
concept Uint16ConvertibleT = ConvertibleToT<T, uint16>;

template <typename T>
concept Uint32ConvertibleT = ConvertibleToT<T, uint32>;

template <typename T>
concept Uint64ConvertibleT = ConvertibleToT<T, uint64>;

template <typename T>
concept Float32ConvertibleT = ConvertibleToT<T, float32>;

template <typename T>
concept Float64ConvertibleT = ConvertibleToT<T, float64>;

} // namespace mini