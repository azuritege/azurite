/**
 * A 2D/3D graphics math library used in Volcano Client. Unlike GLM,
 * it's rather simple and does not assume clip spaces that cause
 * problems.
 *
 * @copyright 2026 The Volcano Authors & Gabriel Jemail.
 * @license MIT
 */

#pragma once
#ifndef VOLCANO_GFX_MATH_H
#define VOLCANO_GFX_MATH_H

#include <bit>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
	#include <immintrin.h>
	#define GFXM_X86_SIMD 1
#endif

#if defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
	#define GFXM_SSE2 1
#endif

#if defined(__AVX__) || defined(_M_AVX)
	#define GFXM_AVX 1
#endif

namespace GFXM {

#pragma region Number Types

typedef uint8_t			u8;
typedef int8_t			i8;
typedef uint16_t		u16;
typedef int16_t			i16;
typedef uint32_t		u32;
typedef int32_t			i32;
typedef uint64_t		u64;
typedef int64_t			i64;
typedef float			f32;
typedef double			f64;

#pragma endregion

#pragma region Vector Types

template<typename T>
struct Vector2;

template<typename T>
struct Vector3;

template<typename T>
struct Vector4;

template<typename V>
struct VecInfo
{
	static constexpr size_t Size = 0;
};

template<typename T>
struct VecInfo<Vector2<T>>
{
	using value_type = T;
	static constexpr size_t Size = 2;
};

template<typename T>
struct VecInfo<Vector3<T>>
{
	using value_type = T;
	static constexpr size_t Size = 3;
};

template<typename T>
struct VecInfo<Vector4<T>>
{
	using value_type = T;
	static constexpr size_t Size = 4;
};

template<typename T>
concept Vec2_T = VecInfo<T>::Size == 2;

template<typename T>
concept Vec3_T = VecInfo<T>::Size == 3;

template<typename T>
concept Vec4_T = VecInfo<T>::Size == 4;

template<typename T>
concept Vec_T = VecInfo<T>::Size != 0;

template<typename T>
concept Numeric = std::is_arithmetic_v<T> && !std::is_same_v<T, bool>;

template<typename T>
using FloatType = std::conditional_t<std::is_floating_point_v<T>, T, f64>;

#pragma endregion

#pragma region SIMD Registers

#if defined(GFXM_SSE2)

struct F32x4
{
	__m128 value;
};

struct F64x2
{
	__m128d value;
};

struct I32x4
{
	__m128i value;
};

struct I64x2
{
	__m128i value;
};

#if defined(GFXM_AVX)
struct F64x4
{
	__m256d value;
};
#endif

#endif

#pragma endregion

#pragma region Vector Operations

namespace detail
{

struct AddOp
{
	template<Numeric T>
	constexpr T operator()(T a, T b) const
	{
		return a + b;
	}

#if defined(GFXM_SSE2)
	F32x4 operator()(F32x4 a, F32x4 b) const { return {_mm_add_ps(a.value, b.value)}; }
	F64x2 operator()(F64x2 a, F64x2 b) const { return {_mm_add_pd(a.value, b.value)}; }
#if defined(GFXM_AVX)
	F64x4 operator()(F64x4 a, F64x4 b) const { return {_mm256_add_pd(a.value, b.value)}; }
#endif
	I32x4 operator()(I32x4 a, I32x4 b) const { return {_mm_add_epi32(a.value, b.value)}; }
	I64x2 operator()(I64x2 a, I64x2 b) const { return {_mm_add_epi64(a.value, b.value)}; }
#endif
};

struct SubtractOp
{
	template<Numeric T>
	constexpr T operator()(T a, T b) const
	{
		return a - b;
	}

#if defined(GFXM_SSE2)
	F32x4 operator()(F32x4 a, F32x4 b) const { return {_mm_sub_ps(a.value, b.value)}; }
	F64x2 operator()(F64x2 a, F64x2 b) const { return {_mm_sub_pd(a.value, b.value)}; }
#if defined(GFXM_AVX)
	F64x4 operator()(F64x4 a, F64x4 b) const { return {_mm256_sub_pd(a.value, b.value)}; }
#endif
	I32x4 operator()(I32x4 a, I32x4 b) const { return {_mm_sub_epi32(a.value, b.value)}; }
	I64x2 operator()(I64x2 a, I64x2 b) const { return {_mm_sub_epi64(a.value, b.value)}; }
#endif
};

struct MultiplyOp
{
	template<Numeric T>
	constexpr T operator()(T a, T b) const
	{
		return a * b;
	}

#if defined(GFXM_SSE2)
	F32x4 operator()(F32x4 a, F32x4 b) const { return {_mm_mul_ps(a.value, b.value)}; }
	F64x2 operator()(F64x2 a, F64x2 b) const { return {_mm_mul_pd(a.value, b.value)}; }
#if defined(GFXM_AVX)
	F64x4 operator()(F64x4 a, F64x4 b) const { return {_mm256_mul_pd(a.value, b.value)}; }
#endif
#if defined(__SSE4_1__)
	I32x4 operator()(I32x4 a, I32x4 b) const { return {_mm_mullo_epi32(a.value, b.value)}; }
#endif
#endif
};

struct DivideOp
{
	template<Numeric T>
	T operator()(T a, T b) const
	{
		return a / b;
	}

#if defined(GFXM_SSE2)
	F32x4 operator()(F32x4 a, F32x4 b) const { return {_mm_div_ps(a.value, b.value)}; }
	F64x2 operator()(F64x2 a, F64x2 b) const { return {_mm_div_pd(a.value, b.value)}; }
#if defined(GFXM_AVX)
	F64x4 operator()(F64x4 a, F64x4 b) const { return {_mm256_div_pd(a.value, b.value)}; }
#endif
#endif
};

struct MinOp
{
	template<Numeric T>
	constexpr T operator()(T a, T b) const
	{
		return a < b ? a : b;
	}

#if defined(GFXM_SSE2)
	F32x4 operator()(F32x4 a, F32x4 b) const { return {_mm_min_ps(a.value, b.value)}; }
	F64x2 operator()(F64x2 a, F64x2 b) const { return {_mm_min_pd(a.value, b.value)}; }
#if defined(GFXM_AVX)
	F64x4 operator()(F64x4 a, F64x4 b) const { return {_mm256_min_pd(a.value, b.value)}; }
#endif
#endif
};

struct MaxOp
{
	template<Numeric T>
	constexpr T operator()(T a, T b) const
	{
		return a > b ? a : b;
	}

#if defined(GFXM_SSE2)
	F32x4 operator()(F32x4 a, F32x4 b) const { return {_mm_max_ps(a.value, b.value)}; }
	F64x2 operator()(F64x2 a, F64x2 b) const { return {_mm_max_pd(a.value, b.value)}; }
#if defined(GFXM_AVX)
	F64x4 operator()(F64x4 a, F64x4 b) const { return {_mm256_max_pd(a.value, b.value)}; }
#endif
#endif
};

struct NegateOp
{
	template<Numeric T>
	constexpr T operator()(T a) const
	{
		return -a;
	}

#if defined(GFXM_SSE2)
	F32x4 operator()(F32x4 a) const { return {_mm_sub_ps(_mm_setzero_ps(), a.value)}; }
	F64x2 operator()(F64x2 a) const { return {_mm_sub_pd(_mm_setzero_pd(), a.value)}; }
#if defined(GFXM_AVX)
	F64x4 operator()(F64x4 a) const { return {_mm256_sub_pd(_mm256_setzero_pd(), a.value)}; }
#endif
	I32x4 operator()(I32x4 a) const { return {_mm_sub_epi32(_mm_setzero_si128(), a.value)}; }
	I64x2 operator()(I64x2 a) const { return {_mm_sub_epi64(_mm_setzero_si128(), a.value)}; }
#endif
};

struct AbsOp
{
	template<Numeric T>
	T operator()(T a) const
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			return std::fabs(a);
		}
		else
		{
			return a < T{} ? -a : a;
		}
	}

#if defined(GFXM_SSE2)
	F32x4 operator()(F32x4 a) const
	{
		const __m128 mask = _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff));
		return {_mm_and_ps(a.value, mask)};
	}

	F64x2 operator()(F64x2 a) const
	{
		const __m128d mask = _mm_castsi128_pd(_mm_set1_epi64x(0x7fffffffffffffffLL));
		return {_mm_and_pd(a.value, mask)};
	}
#if defined(GFXM_AVX)
	F64x4 operator()(F64x4 a) const
	{
		const __m256d mask = _mm256_castsi256_pd(_mm256_set1_epi64x(0x7fffffffffffffffLL));
		return {_mm256_and_pd(a.value, mask)};
	}
#endif
#endif
};

struct FloorOp
{
	template<Numeric T>
	T operator()(T a) const
	{
		return static_cast<T>(std::floor(a));
	}
};

struct CeilOp
{
	template<Numeric T>
	T operator()(T a) const
	{
		return static_cast<T>(std::ceil(a));
	}
};

struct RoundOp
{
	template<Numeric T>
	T operator()(T a) const
	{
		return static_cast<T>(std::round(a));
	}
};

struct TruncOp
{
	template<Numeric T>
	T operator()(T a) const
	{
		return static_cast<T>(std::trunc(a));
	}
};

struct SqrtOp
{
	template<Numeric T>
	T operator()(T a) const
	{
		return static_cast<T>(std::sqrt(a));
	}

#if defined(GFXM_SSE2)
	F32x4 operator()(F32x4 a) const { return {_mm_sqrt_ps(a.value)}; }
	F64x2 operator()(F64x2 a) const { return {_mm_sqrt_pd(a.value)}; }
#if defined(GFXM_AVX)
	F64x4 operator()(F64x4 a) const { return {_mm256_sqrt_pd(a.value)}; }
#endif
#endif
};

template<typename T>
constexpr T ClampValue(T value, T min, T max)
{
	return value < min ? min : (value > max ? max : value);
}

template<typename V, typename Op>
V BinaryOp(const V& a, const V& b, Op op);

template<typename V, typename Op>
V BinaryOpScalarRight(const V& a, typename VecInfo<V>::value_type scalar, Op op);

template<typename V, typename Op>
V BinaryOpScalarLeft(const V& a, typename VecInfo<V>::value_type scalar, Op op);

template<typename V, typename Op>
V UnaryOp(const V& a, Op op);

template<typename V>
typename VecInfo<V>::value_type HorizontalSum(const V& v);

template<typename V>
FloatType<typename VecInfo<V>::value_type> LengthSquared(const V& v);

template<typename V>
FloatType<typename VecInfo<V>::value_type> Length(const V& v);

template<typename V>
V Normalize(const V& v);

} // namespace detail

#pragma endregion

#pragma region Vector Base

template<typename Derived>
struct VecOps
{
	using Vec = Derived;
	using value_type = typename VecInfo<Derived>::value_type;
	using Float = FloatType<value_type>;
	static constexpr size_t Size = VecInfo<Derived>::Size;

	constexpr Vec& Self()
	{
		return static_cast<Vec&>(*this);
	}

	constexpr const Vec& Self() const
	{
		return static_cast<const Vec&>(*this);
	}

	Vec Add(const Vec& rhs) const { return detail::BinaryOp(Self(), rhs, detail::AddOp{}); }
	Vec Add(value_type scalar) const { return detail::BinaryOpScalarRight(Self(), scalar, detail::AddOp{}); }
	Vec Subtract(const Vec& rhs) const { return detail::BinaryOp(Self(), rhs, detail::SubtractOp{}); }
	Vec Subtract(value_type scalar) const { return detail::BinaryOpScalarRight(Self(), scalar, detail::SubtractOp{}); }
	Vec Multiply(const Vec& rhs) const { return detail::BinaryOp(Self(), rhs, detail::MultiplyOp{}); }
	Vec Multiply(value_type scalar) const { return detail::BinaryOpScalarRight(Self(), scalar, detail::MultiplyOp{}); }
	Vec Divide(const Vec& rhs) const { return detail::BinaryOp(Self(), rhs, detail::DivideOp{}); }
	Vec Divide(value_type scalar) const { return detail::BinaryOpScalarRight(Self(), scalar, detail::DivideOp{}); }
	Vec Min(const Vec& rhs) const { return detail::BinaryOp(Self(), rhs, detail::MinOp{}); }
	Vec Min(value_type scalar) const { return detail::BinaryOpScalarRight(Self(), scalar, detail::MinOp{}); }
	Vec Max(const Vec& rhs) const { return detail::BinaryOp(Self(), rhs, detail::MaxOp{}); }
	Vec Max(value_type scalar) const { return detail::BinaryOpScalarRight(Self(), scalar, detail::MaxOp{}); }

	Vec operator+(const Vec& rhs) const { return Add(rhs); }
	Vec operator+(value_type scalar) const { return Add(scalar); }
	Vec operator-(const Vec& rhs) const { return Subtract(rhs); }
	Vec operator-(value_type scalar) const { return Subtract(scalar); }
	Vec operator*(const Vec& rhs) const { return Multiply(rhs); }
	Vec operator*(value_type scalar) const { return Multiply(scalar); }
	Vec operator/(const Vec& rhs) const { return Divide(rhs); }
	Vec operator/(value_type scalar) const { return Divide(scalar); }
	Vec operator-() const { return detail::UnaryOp(Self(), detail::NegateOp{}); }

	Vec& operator+=(const Vec& rhs) { Self() = Add(rhs); return Self(); }
	Vec& operator+=(value_type scalar) { Self() = Add(scalar); return Self(); }
	Vec& operator-=(const Vec& rhs) { Self() = Subtract(rhs); return Self(); }
	Vec& operator-=(value_type scalar) { Self() = Subtract(scalar); return Self(); }
	Vec& operator*=(const Vec& rhs) { Self() = Multiply(rhs); return Self(); }
	Vec& operator*=(value_type scalar) { Self() = Multiply(scalar); return Self(); }
	Vec& operator/=(const Vec& rhs) { Self() = Divide(rhs); return Self(); }
	Vec& operator/=(value_type scalar) { Self() = Divide(scalar); return Self(); }

	value_type Sum() const { return detail::HorizontalSum(Self()); }
	Vec Diff(const Vec& rhs) const { return Subtract(rhs); }
	Vec Normalize() const { return detail::Normalize(Self()); }
	Vec Normalized() const { return Normalize(); }
	Vec Abs() const { return detail::UnaryOp(Self(), detail::AbsOp{}); }
	Vec Floor() const { return detail::UnaryOp(Self(), detail::FloorOp{}); }
	Vec Ceil() const { return detail::UnaryOp(Self(), detail::CeilOp{}); }
	Vec Round() const { return detail::UnaryOp(Self(), detail::RoundOp{}); }
	Vec Trunc() const { return detail::UnaryOp(Self(), detail::TruncOp{}); }
	Vec Sqrt() const { return detail::UnaryOp(Self(), detail::SqrtOp{}); }

	value_type Dot(const Vec& rhs) const { return Multiply(rhs).Sum(); }
	Float LengthSquared() const { return detail::LengthSquared(Self()); }
	Float Length() const { return detail::Length(Self()); }
	Float DistanceSquared(const Vec& rhs) const { return (Self() - rhs).LengthSquared(); }
	Float Distance(const Vec& rhs) const { return (Self() - rhs).Length(); }

	Vec Clamp(const Vec& min, const Vec& max) const { return Max(min).Min(max); }

	Vec Clamp(value_type min, value_type max) const
	{
		Vec result = Self();
		for (size_t i = 0; i < Size; ++i)
		{
			result[i] = detail::ClampValue(result[i], min, max);
		}
		return result;
	}

	Vec Lerp(const Vec& end, value_type t) const
	{
		return Self() + (end - Self()) * t;
	}

	static Vec Lerp(const Vec& start, const Vec& end, value_type t)
	{
		return start.Lerp(end, t);
	}

	Vec Frac() const
	{
		return Self() - Floor();
	}
};

#pragma endregion

#pragma region Vector Definitions

template<typename T>
struct Vector2 : VecOps<Vector2<T>>
{
	using value_type = T;
	static constexpr size_t Size = 2;

	T x{};
	T y{};

	constexpr Vector2() = default;
	constexpr Vector2(T x, T y) : x(x), y(y) {}
	constexpr explicit Vector2(T scalar) : x(scalar), y(scalar) {}

	constexpr T& operator[](size_t index)
	{
		return index == 0 ? x : y;
	}

	constexpr const T& operator[](size_t index) const
	{
		return index == 0 ? x : y;
	}

	constexpr T* Data() { return &x; }
	constexpr const T* Data() const { return &x; }
};

template<typename T>
struct Vector3 : VecOps<Vector3<T>>
{
	using value_type = T;
	static constexpr size_t Size = 3;

	T x{};
	T y{};
	T z{};

	constexpr Vector3() = default;
	constexpr Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
	constexpr explicit Vector3(T scalar) : x(scalar), y(scalar), z(scalar) {}

	constexpr T& operator[](size_t index)
	{
		return index == 0 ? x : (index == 1 ? y : z);
	}

	constexpr const T& operator[](size_t index) const
	{
		return index == 0 ? x : (index == 1 ? y : z);
	}

	constexpr T* Data() { return &x; }
	constexpr const T* Data() const { return &x; }

	constexpr Vector3 Cross(const Vector3& rhs) const
	{
		return {
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x
		};
	}
};

template<typename T>
struct Vector4 : VecOps<Vector4<T>>
{
	using value_type = T;
	static constexpr size_t Size = 4;

	T x{};
	T y{};
	T z{};
	T w{};

	constexpr Vector4() = default;
	constexpr Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
	constexpr explicit Vector4(T scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}

	constexpr T& operator[](size_t index)
	{
		return index == 0 ? x : (index == 1 ? y : (index == 2 ? z : w));
	}

	constexpr const T& operator[](size_t index) const
	{
		return index == 0 ? x : (index == 1 ? y : (index == 2 ? z : w));
	}

	constexpr T* Data() { return &x; }
	constexpr const T* Data() const { return &x; }
};

using Vector2i  = Vector2<i32>;
using Vector2l  = Vector2<i64>;
using Vector2ui = Vector2<u32>;
using Vector2ul = Vector2<u64>;
using Vector2f  = Vector2<f32>;
using Vector2d  = Vector2<f64>;

using Vector3i  = Vector3<i32>;
using Vector3l  = Vector3<i64>;
using Vector3ui = Vector3<u32>;
using Vector3ul = Vector3<u64>;
using Vector3f  = Vector3<f32>;
using Vector3d  = Vector3<f64>;

using Vector4i  = Vector4<i32>;
using Vector4l  = Vector4<i64>;
using Vector4ui = Vector4<u32>;
using Vector4ul = Vector4<u64>;
using Vector4f  = Vector4<f32>;
using Vector4d  = Vector4<f64>;

#pragma endregion

#pragma region Matrix Types

template<typename T>
struct Matrix4
{
	using value_type = T;
	using Vector = Vector4<T>;
	using Float = FloatType<T>;
	static constexpr size_t Size = 16;
	static constexpr size_t ColumnCount = 4;

	Vector columns[ColumnCount];

	constexpr Matrix4() = default;

	constexpr Matrix4(const Vector& c0, const Vector& c1, const Vector& c2, const Vector& c3)
	{
		columns[0] = c0;
		columns[1] = c1;
		columns[2] = c2;
		columns[3] = c3;
	}

	static constexpr Matrix4 Zero()
	{
		return {};
	}

	static constexpr Matrix4 Identity()
	{
		Matrix4 result{};
		result.columns[0] = Vector{T{1}, T{0}, T{0}, T{0}};
		result.columns[1] = Vector{T{0}, T{1}, T{0}, T{0}};
		result.columns[2] = Vector{T{0}, T{0}, T{1}, T{0}};
		result.columns[3] = Vector{T{0}, T{0}, T{0}, T{1}};
		return result;
	}

	static constexpr Matrix4 Translation(const Vector3<T>& translation)
	{
		Matrix4 result = Identity();
		result.columns[3] = Vector{translation.x, translation.y, translation.z, T{1}};
		return result;
	}

	static constexpr Matrix4 Scaling(const Vector3<T>& scale)
	{
		Matrix4 result = Identity();
		result.columns[0] = Vector{scale.x, T{0}, T{0}, T{0}};
		result.columns[1] = Vector{T{0}, scale.y, T{0}, T{0}};
		result.columns[2] = Vector{T{0}, T{0}, scale.z, T{0}};
		return result;
	}

	static constexpr Matrix4 Scaling(T scale)
	{
		return Scaling(Vector3<T>{scale, scale, scale});
	}

	constexpr Vector& operator[](size_t index)
	{
		return columns[index];
	}

	constexpr const Vector& operator[](size_t index) const
	{
		return columns[index];
	}

	constexpr Vector& Column(size_t index)
	{
		return columns[index];
	}

	constexpr const Vector& Column(size_t index) const
	{
		return columns[index];
	}

	constexpr Vector Row(size_t index) const
	{
		return {
			columns[0][index],
			columns[1][index],
			columns[2][index],
			columns[3][index]
		};
	}

	Matrix4 Add(const Matrix4& rhs) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i] + rhs.columns[i];
		}
		return result;
	}

	Matrix4 Add(value_type scalar) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i] + scalar;
		}
		return result;
	}

	Matrix4 Subtract(const Matrix4& rhs) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i] - rhs.columns[i];
		}
		return result;
	}

	Matrix4 Subtract(value_type scalar) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i] - scalar;
		}
		return result;
	}

	Matrix4 Multiply(const Matrix4& rhs) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = *this * rhs.columns[i];
		}
		return result;
	}

	Matrix4 Multiply(value_type scalar) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i] * scalar;
		}
		return result;
	}

	Matrix4 MultiplyComponent(const Matrix4& rhs) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i].Multiply(rhs.columns[i]);
		}
		return result;
	}

	Matrix4 Divide(value_type scalar) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i] / scalar;
		}
		return result;
	}

	Matrix4 DivideComponent(const Matrix4& rhs) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i].Divide(rhs.columns[i]);
		}
		return result;
	}

	Matrix4 operator+(const Matrix4& rhs) const { return Add(rhs); }
	Matrix4 operator+(value_type scalar) const { return Add(scalar); }
	Matrix4 operator-(const Matrix4& rhs) const { return Subtract(rhs); }
	Matrix4 operator-(value_type scalar) const { return Subtract(scalar); }
	Matrix4 operator*(const Matrix4& rhs) const { return Multiply(rhs); }
	Matrix4 operator*(value_type scalar) const { return Multiply(scalar); }
	Matrix4 operator/(value_type scalar) const { return Divide(scalar); }
	Matrix4 operator-() const { return Multiply(T{0} - T{1}); }

	Vector operator*(const Vector& v) const
	{
		return columns[0] * v.x + columns[1] * v.y + columns[2] * v.z + columns[3] * v.w;
	}

	Vector Transform(const Vector& v) const
	{
		return *this * v;
	}

	Matrix4& operator+=(const Matrix4& rhs) { *this = Add(rhs); return *this; }
	Matrix4& operator+=(value_type scalar) { *this = Add(scalar); return *this; }
	Matrix4& operator-=(const Matrix4& rhs) { *this = Subtract(rhs); return *this; }
	Matrix4& operator-=(value_type scalar) { *this = Subtract(scalar); return *this; }
	Matrix4& operator*=(const Matrix4& rhs) { *this = Multiply(rhs); return *this; }
	Matrix4& operator*=(value_type scalar) { *this = Multiply(scalar); return *this; }
	Matrix4& operator/=(value_type scalar) { *this = Divide(scalar); return *this; }

	value_type Sum() const
	{
		value_type sum{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			for (size_t j = 0; j < Vector::Size; ++j)
			{
				sum += columns[i][j];
			}
		}
		return sum;
	}

	Matrix4 Diff(const Matrix4& rhs) const { return Subtract(rhs); }
	value_type Trace() const { return columns[0][0] + columns[1][1] + columns[2][2] + columns[3][3]; }

	Matrix4 Min(const Matrix4& rhs) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i].Min(rhs.columns[i]);
		}
		return result;
	}

	Matrix4 Max(const Matrix4& rhs) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i].Max(rhs.columns[i]);
		}
		return result;
	}

	Matrix4 Min(value_type scalar) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i].Min(scalar);
		}
		return result;
	}

	Matrix4 Max(value_type scalar) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i].Max(scalar);
		}
		return result;
	}

	Matrix4 Clamp(const Matrix4& min, const Matrix4& max) const { return Max(min).Min(max); }

	Matrix4 Clamp(value_type min, value_type max) const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i].Clamp(min, max);
		}
		return result;
	}

	Matrix4 Lerp(const Matrix4& end, value_type t) const
	{
		return *this + (end - *this) * t;
	}

	static Matrix4 Lerp(const Matrix4& start, const Matrix4& end, value_type t)
	{
		return start.Lerp(end, t);
	}

	Matrix4 Abs() const
	{
		Matrix4 result{};
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			result.columns[i] = columns[i].Abs();
		}
		return result;
	}

	Matrix4 Transpose() const
	{
		Matrix4 result{};
		for (size_t col = 0; col < ColumnCount; ++col)
		{
			for (size_t row = 0; row < Vector::Size; ++row)
			{
				result.columns[col][row] = columns[row][col];
			}
		}
		return result;
	}

	Float Determinant() const
	{
		Float a[ColumnCount][Vector::Size]{};
		for (size_t col = 0; col < ColumnCount; ++col)
		{
			for (size_t row = 0; row < Vector::Size; ++row)
			{
				a[row][col] = static_cast<Float>(columns[col][row]);
			}
		}

		Float determinant = Float{1};
		for (size_t pivotIndex = 0; pivotIndex < ColumnCount - 1; ++pivotIndex)
		{
			size_t pivot = pivotIndex;
			for (size_t row = pivotIndex + 1; row < ColumnCount; ++row)
			{
				if (std::abs(a[row][pivotIndex]) > std::abs(a[pivot][pivotIndex]))
				{
					pivot = row;
				}
			}

			if (std::abs(a[pivot][pivotIndex]) <= std::numeric_limits<Float>::epsilon())
			{
				return Float{};
			}

			if (pivot != pivotIndex)
			{
				for (size_t col = 0; col < ColumnCount; ++col)
				{
					std::swap(a[pivotIndex][col], a[pivot][col]);
				}
				determinant = -determinant;
			}

			const Float pivotValue = a[pivotIndex][pivotIndex];
			determinant *= pivotValue;
			for (size_t row = pivotIndex + 1; row < ColumnCount; ++row)
			{
				const Float factor = a[row][pivotIndex] / pivotValue;
				for (size_t col = pivotIndex + 1; col < ColumnCount; ++col)
				{
					a[row][col] -= factor * a[pivotIndex][col];
				}
			}
		}

		return determinant * a[ColumnCount - 1][ColumnCount - 1];
	}

	Matrix4 Inverse() const
		requires std::is_floating_point_v<T>
	{
		Float a[ColumnCount][ColumnCount * 2]{};
		for (size_t row = 0; row < ColumnCount; ++row)
		{
			for (size_t col = 0; col < ColumnCount; ++col)
			{
				a[row][col] = static_cast<Float>(columns[col][row]);
				a[row][ColumnCount + col] = row == col ? Float{1} : Float{};
			}
		}

		for (size_t pivotIndex = 0; pivotIndex < ColumnCount; ++pivotIndex)
		{
			size_t pivot = pivotIndex;
			for (size_t row = pivotIndex + 1; row < ColumnCount; ++row)
			{
				if (std::abs(a[row][pivotIndex]) > std::abs(a[pivot][pivotIndex]))
				{
					pivot = row;
				}
			}

			if (std::abs(a[pivot][pivotIndex]) <= std::numeric_limits<Float>::epsilon())
			{
				return Zero();
			}

			if (pivot != pivotIndex)
			{
				for (size_t col = 0; col < ColumnCount * 2; ++col)
				{
					std::swap(a[pivotIndex][col], a[pivot][col]);
				}
			}

			const Float pivotValue = a[pivotIndex][pivotIndex];
			for (size_t col = 0; col < ColumnCount * 2; ++col)
			{
				a[pivotIndex][col] /= pivotValue;
			}

			for (size_t row = 0; row < ColumnCount; ++row)
			{
				if (row == pivotIndex) continue;
				const Float factor = a[row][pivotIndex];
				for (size_t col = 0; col < ColumnCount * 2; ++col)
				{
					a[row][col] -= factor * a[pivotIndex][col];
				}
			}
		}

		Matrix4 result{};
		for (size_t row = 0; row < ColumnCount; ++row)
		{
			for (size_t col = 0; col < ColumnCount; ++col)
			{
				result.columns[col][row] = static_cast<T>(a[row][ColumnCount + col]);
			}
		}
		return result;
	}

	bool IsIdentity() const
	{
		const Matrix4 identity = Identity();
		for (size_t i = 0; i < ColumnCount; ++i)
		{
			for (size_t j = 0; j < Vector::Size; ++j)
			{
				if (columns[i][j] != identity.columns[i][j]) return false;
			}
		}
		return true;
	}
};

template<typename T>
struct MatInfo
{
	static constexpr size_t Size = 0;
};

template<typename T>
struct MatInfo<Matrix4<T>>
{
	using value_type = T;
	using vector_type = Vector4<T>;
	static constexpr size_t Size = 16;
};

template<typename T>
concept Matrix4_T = MatInfo<T>::Size == 16;

using Matrix4i  = Matrix4<i32>;
using Matrix4l  = Matrix4<i64>;
using Matrix4ui = Matrix4<u32>;
using Matrix4ul = Matrix4<u64>;
using Matrix4f  = Matrix4<f32>;
using Matrix4d  = Matrix4<f64>;

#pragma endregion

#pragma region SIMD Vector Adapters

template<typename V>
struct SimdOps
{
	static constexpr bool available = false;
	using register_type = int;
};

#if defined(GFXM_SSE2)

template<>
struct SimdOps<Vector2<f32>>
{
	using register_type = F32x4;
	static constexpr bool available = true;

	static register_type Load(const Vector2<f32>& v)
	{
		return {_mm_setr_ps(v.x, v.y, 0.0f, 0.0f)};
	}

	static Vector2<f32> Store(register_type r)
	{
		alignas(16) f32 values[4];
		_mm_store_ps(values, r.value);
		return {values[0], values[1]};
	}

	static register_type Broadcast(f32 scalar)
	{
		return {_mm_set1_ps(scalar)};
	}
};

template<>
struct SimdOps<Vector3<f32>>
{
	using register_type = F32x4;
	static constexpr bool available = true;

	static register_type Load(const Vector3<f32>& v)
	{
		return {_mm_setr_ps(v.x, v.y, v.z, 0.0f)};
	}

	static Vector3<f32> Store(register_type r)
	{
		alignas(16) f32 values[4];
		_mm_store_ps(values, r.value);
		return {values[0], values[1], values[2]};
	}

	static register_type Broadcast(f32 scalar)
	{
		return {_mm_set1_ps(scalar)};
	}
};

template<>
struct SimdOps<Vector4<f32>>
{
	using register_type = F32x4;
	static constexpr bool available = true;

	static register_type Load(const Vector4<f32>& v)
	{
		return {_mm_loadu_ps(v.Data())};
	}

	static Vector4<f32> Store(register_type r)
	{
		Vector4<f32> result{};
		_mm_storeu_ps(result.Data(), r.value);
		return result;
	}

	static register_type Broadcast(f32 scalar)
	{
		return {_mm_set1_ps(scalar)};
	}
};

template<>
struct SimdOps<Vector2<f64>>
{
	using register_type = F64x2;
	static constexpr bool available = true;

	static register_type Load(const Vector2<f64>& v)
	{
		return {_mm_loadu_pd(v.Data())};
	}

	static Vector2<f64> Store(register_type r)
	{
		Vector2<f64> result{};
		_mm_storeu_pd(result.Data(), r.value);
		return result;
	}

	static register_type Broadcast(f64 scalar)
	{
		return {_mm_set1_pd(scalar)};
	}
};

#if defined(GFXM_AVX)
template<>
struct SimdOps<Vector4<f64>>
{
	using register_type = F64x4;
	static constexpr bool available = true;

	static register_type Load(const Vector4<f64>& v)
	{
		return {_mm256_loadu_pd(v.Data())};
	}

	static Vector4<f64> Store(register_type r)
	{
		Vector4<f64> result{};
		_mm256_storeu_pd(result.Data(), r.value);
		return result;
	}

	static register_type Broadcast(f64 scalar)
	{
		return {_mm256_set1_pd(scalar)};
	}
};
#endif

template<>
struct SimdOps<Vector2<i32>>
{
	using register_type = I32x4;
	static constexpr bool available = true;

	static register_type Load(const Vector2<i32>& v)
	{
		return {_mm_setr_epi32(v.x, v.y, 0, 0)};
	}

	static Vector2<i32> Store(register_type r)
	{
		alignas(16) i32 values[4];
		_mm_store_si128(reinterpret_cast<__m128i*>(values), r.value);
		return {values[0], values[1]};
	}

	static register_type Broadcast(i32 scalar)
	{
		return {_mm_set1_epi32(scalar)};
	}
};

template<>
struct SimdOps<Vector3<i32>>
{
	using register_type = I32x4;
	static constexpr bool available = true;

	static register_type Load(const Vector3<i32>& v)
	{
		return {_mm_setr_epi32(v.x, v.y, v.z, 0)};
	}

	static Vector3<i32> Store(register_type r)
	{
		alignas(16) i32 values[4];
		_mm_store_si128(reinterpret_cast<__m128i*>(values), r.value);
		return {values[0], values[1], values[2]};
	}

	static register_type Broadcast(i32 scalar)
	{
		return {_mm_set1_epi32(scalar)};
	}
};

template<>
struct SimdOps<Vector4<i32>>
{
	using register_type = I32x4;
	static constexpr bool available = true;

	static register_type Load(const Vector4<i32>& v)
	{
		return {_mm_loadu_si128(reinterpret_cast<const __m128i*>(v.Data()))};
	}

	static Vector4<i32> Store(register_type r)
	{
		Vector4<i32> result{};
		_mm_storeu_si128(reinterpret_cast<__m128i*>(result.Data()), r.value);
		return result;
	}

	static register_type Broadcast(i32 scalar)
	{
		return {_mm_set1_epi32(scalar)};
	}
};

template<>
struct SimdOps<Vector2<u32>>
{
	using register_type = I32x4;
	static constexpr bool available = true;

	static register_type Load(const Vector2<u32>& v)
	{
		return {_mm_setr_epi32(std::bit_cast<i32>(v.x), std::bit_cast<i32>(v.y), 0, 0)};
	}

	static Vector2<u32> Store(register_type r)
	{
		alignas(16) i32 values[4];
		_mm_store_si128(reinterpret_cast<__m128i*>(values), r.value);
		return {static_cast<u32>(values[0]), static_cast<u32>(values[1])};
	}

	static register_type Broadcast(u32 scalar)
	{
		return {_mm_set1_epi32(std::bit_cast<i32>(scalar))};
	}
};

template<>
struct SimdOps<Vector3<u32>>
{
	using register_type = I32x4;
	static constexpr bool available = true;

	static register_type Load(const Vector3<u32>& v)
	{
		return {_mm_setr_epi32(std::bit_cast<i32>(v.x), std::bit_cast<i32>(v.y), std::bit_cast<i32>(v.z), 0)};
	}

	static Vector3<u32> Store(register_type r)
	{
		alignas(16) i32 values[4];
		_mm_store_si128(reinterpret_cast<__m128i*>(values), r.value);
		return {static_cast<u32>(values[0]), static_cast<u32>(values[1]), static_cast<u32>(values[2])};
	}

	static register_type Broadcast(u32 scalar)
	{
		return {_mm_set1_epi32(std::bit_cast<i32>(scalar))};
	}
};

template<>
struct SimdOps<Vector4<u32>>
{
	using register_type = I32x4;
	static constexpr bool available = true;

	static register_type Load(const Vector4<u32>& v)
	{
		return {_mm_loadu_si128(reinterpret_cast<const __m128i*>(v.Data()))};
	}

	static Vector4<u32> Store(register_type r)
	{
		Vector4<u32> result{};
		_mm_storeu_si128(reinterpret_cast<__m128i*>(result.Data()), r.value);
		return result;
	}

	static register_type Broadcast(u32 scalar)
	{
		return {_mm_set1_epi32(std::bit_cast<i32>(scalar))};
	}
};

template<>
struct SimdOps<Vector2<i64>>
{
	using register_type = I64x2;
	static constexpr bool available = true;

	static register_type Load(const Vector2<i64>& v)
	{
		return {_mm_set_epi64x(v.y, v.x)};
	}

	static Vector2<i64> Store(register_type r)
	{
		alignas(16) i64 values[2];
		_mm_store_si128(reinterpret_cast<__m128i*>(values), r.value);
		return {values[0], values[1]};
	}

	static register_type Broadcast(i64 scalar)
	{
		return {_mm_set1_epi64x(scalar)};
	}
};

template<>
struct SimdOps<Vector2<u64>>
{
	using register_type = I64x2;
	static constexpr bool available = true;

	static register_type Load(const Vector2<u64>& v)
	{
		return {_mm_set_epi64x(std::bit_cast<i64>(v.y), std::bit_cast<i64>(v.x))};
	}

	static Vector2<u64> Store(register_type r)
	{
		alignas(16) i64 values[2];
		_mm_store_si128(reinterpret_cast<__m128i*>(values), r.value);
		return {static_cast<u64>(values[0]), static_cast<u64>(values[1])};
	}

	static register_type Broadcast(u64 scalar)
	{
		return {_mm_set1_epi64x(std::bit_cast<i64>(scalar))};
	}
};

#endif

#pragma endregion

#pragma region Vector Function Implementations

namespace detail
{

template<typename V, typename Op>
concept SimdBinaryOp = requires(Op op, typename SimdOps<V>::register_type a, typename SimdOps<V>::register_type b)
{
	{ op(a, b) } -> std::same_as<typename SimdOps<V>::register_type>;
};

template<typename V, typename Op>
concept SimdUnaryOp = requires(Op op, typename SimdOps<V>::register_type a)
{
	{ op(a) } -> std::same_as<typename SimdOps<V>::register_type>;
};

template<typename V, typename Op>
V BinaryOpFallback(const V& a, const V& b, Op op)
{
	V result{};
	for (size_t i = 0; i < VecInfo<V>::Size; ++i)
	{
		result[i] = op(a[i], b[i]);
	}
	return result;
}

template<typename V, typename Op>
V BinaryOp(const V& a, const V& b, Op op)
{
	if constexpr (SimdOps<V>::available && SimdBinaryOp<V, Op>)
	{
		return SimdOps<V>::Store(op(SimdOps<V>::Load(a), SimdOps<V>::Load(b)));
	}
	else
	{
		return BinaryOpFallback(a, b, op);
	}
}

template<typename V, typename Op>
V BinaryOpScalarRight(const V& a, typename VecInfo<V>::value_type scalar, Op op)
{
	if constexpr (SimdOps<V>::available && SimdBinaryOp<V, Op>)
	{
		return SimdOps<V>::Store(op(SimdOps<V>::Load(a), SimdOps<V>::Broadcast(scalar)));
	}
	else
	{
		V result{};
		for (size_t i = 0; i < VecInfo<V>::Size; ++i)
		{
			result[i] = op(a[i], scalar);
		}
		return result;
	}
}

template<typename V, typename Op>
V BinaryOpScalarLeft(const V& a, typename VecInfo<V>::value_type scalar, Op op)
{
	if constexpr (SimdOps<V>::available && SimdBinaryOp<V, Op>)
	{
		return SimdOps<V>::Store(op(SimdOps<V>::Broadcast(scalar), SimdOps<V>::Load(a)));
	}
	else
	{
		V result{};
		for (size_t i = 0; i < VecInfo<V>::Size; ++i)
		{
			result[i] = op(scalar, a[i]);
		}
		return result;
	}
}

template<typename V, typename Op>
V UnaryOpFallback(const V& a, Op op)
{
	V result{};
	for (size_t i = 0; i < VecInfo<V>::Size; ++i)
	{
		result[i] = op(a[i]);
	}
	return result;
}

template<typename V, typename Op>
V UnaryOp(const V& a, Op op)
{
	if constexpr (SimdOps<V>::available && SimdUnaryOp<V, Op>)
	{
		return SimdOps<V>::Store(op(SimdOps<V>::Load(a)));
	}
	else
	{
		return UnaryOpFallback(a, op);
	}
}

template<typename V>
typename VecInfo<V>::value_type HorizontalSum(const V& v)
{
	using T = typename VecInfo<V>::value_type;
	T sum{};
	for (size_t i = 0; i < VecInfo<V>::Size; ++i)
	{
		sum += v[i];
	}
	return sum;
}

template<typename V>
FloatType<typename VecInfo<V>::value_type> LengthSquared(const V& v)
{
	using T = typename VecInfo<V>::value_type;
	using F = FloatType<T>;
	F sum{};
	for (size_t i = 0; i < VecInfo<V>::Size; ++i)
	{
		const F component = static_cast<F>(v[i]);
		sum += component * component;
	}
	return sum;
}

template<typename V>
FloatType<typename VecInfo<V>::value_type> Length(const V& v)
{
	using F = FloatType<typename VecInfo<V>::value_type>;
	return static_cast<F>(std::sqrt(LengthSquared(v)));
}

template<typename V>
V Normalize(const V& v)
{
	using T = typename VecInfo<V>::value_type;
	using F = FloatType<T>;
	const F length = Length(v);
	if (length == F{})
	{
		return v;
	}

	V result{};
	for (size_t i = 0; i < VecInfo<V>::Size; ++i)
	{
		result[i] = static_cast<T>(static_cast<F>(v[i]) / length);
	}
	return result;
}

} // namespace detail

#pragma endregion

#pragma region Vector Operators

template<Vec_T V>
V operator+(const V& a, const V& b)
{
	return a.Add(b);
}

template<Vec_T V>
V operator-(const V& a, const V& b)
{
	return a.Subtract(b);
}

template<Vec_T V>
V operator*(const V& a, const V& b)
{
	return a.Multiply(b);
}

template<Vec_T V>
V operator/(const V& a, const V& b)
{
	return a.Divide(b);
}

template<Vec_T V>
V operator+(const V& v, typename VecInfo<V>::value_type scalar)
{
	return v.Add(scalar);
}

template<Vec_T V>
V operator+(typename VecInfo<V>::value_type scalar, const V& v)
{
	return detail::BinaryOpScalarLeft(v, scalar, detail::AddOp{});
}

template<Vec_T V>
V operator-(const V& v, typename VecInfo<V>::value_type scalar)
{
	return v.Subtract(scalar);
}

template<Vec_T V>
V operator-(typename VecInfo<V>::value_type scalar, const V& v)
{
	return detail::BinaryOpScalarLeft(v, scalar, detail::SubtractOp{});
}

template<Vec_T V>
V operator*(const V& v, typename VecInfo<V>::value_type scalar)
{
	return v.Multiply(scalar);
}

template<Vec_T V>
V operator*(typename VecInfo<V>::value_type scalar, const V& v)
{
	return v.Multiply(scalar);
}

template<Vec_T V>
V operator/(const V& v, typename VecInfo<V>::value_type scalar)
{
	return v.Divide(scalar);
}

template<Vec_T V>
V operator/(typename VecInfo<V>::value_type scalar, const V& v)
{
	return detail::BinaryOpScalarLeft(v, scalar, detail::DivideOp{});
}

template<Vec_T V>
bool operator==(const V& a, const V& b)
{
	for (size_t i = 0; i < VecInfo<V>::Size; ++i)
	{
		if (a[i] != b[i]) return false;
	}
	return true;
}

template<Vec_T V>
bool operator!=(const V& a, const V& b)
{
	return !(a == b);
}

template<Vec_T V>
bool operator<(const V& a, const V& b)
{
	for (size_t i = 0; i < VecInfo<V>::Size; ++i)
	{
		if (a[i] >= b[i]) return false;
	}
	return true;
}

template<Vec_T V>
bool operator<=(const V& a, const V& b)
{
	for (size_t i = 0; i < VecInfo<V>::Size; ++i)
	{
		if (a[i] > b[i]) return false;
	}
	return true;
}

template<Vec_T V>
bool operator>(const V& a, const V& b)
{
	for (size_t i = 0; i < VecInfo<V>::Size; ++i)
	{
		if (a[i] <= b[i]) return false;
	}
	return true;
}

template<Vec_T V>
bool operator>=(const V& a, const V& b)
{
	for (size_t i = 0; i < VecInfo<V>::Size; ++i)
	{
		if (a[i] < b[i]) return false;
	}
	return true;
}

#pragma endregion

#pragma region Matrix Operators

template<Matrix4_T M>
M operator+(typename MatInfo<M>::value_type scalar, const M& m)
{
	return m.Add(scalar);
}

template<Matrix4_T M>
M operator-(typename MatInfo<M>::value_type scalar, const M& m)
{
	using Vector = typename MatInfo<M>::vector_type;
	M result{};
	for (size_t i = 0; i < MatInfo<M>::Size / Vector::Size; ++i)
	{
		Vector column{};
		for (size_t j = 0; j < Vector::Size; ++j)
		{
			column[j] = scalar - m.columns[i][j];
		}
		result.columns[i] = column;
	}
	return result;
}

template<Matrix4_T M>
M operator*(typename MatInfo<M>::value_type scalar, const M& m)
{
	return m.Multiply(scalar);
}

template<Matrix4_T M>
M operator/(typename MatInfo<M>::value_type scalar, const M& m)
{
	using Vector = typename MatInfo<M>::vector_type;
	M result{};
	for (size_t i = 0; i < MatInfo<M>::Size / Vector::Size; ++i)
	{
		Vector column{};
		for (size_t j = 0; j < Vector::Size; ++j)
		{
			column[j] = scalar / m.columns[i][j];
		}
		result.columns[i] = column;
	}
	return result;
}

template<Matrix4_T M>
bool operator==(const M& a, const M& b)
{
	using Vector = typename MatInfo<M>::vector_type;
	for (size_t i = 0; i < MatInfo<M>::Size / Vector::Size; ++i)
	{
		for (size_t j = 0; j < Vector::Size; ++j)
		{
			if (a[i][j] != b[i][j]) return false;
		}
	}
	return true;
}

template<Matrix4_T M>
bool operator!=(const M& a, const M& b)
{
	return !(a == b);
}

#pragma endregion

#pragma region Constants

inline constexpr f32 kPi = 3.14159265358979323846f;
inline constexpr f32 kTwoPi = kPi * 2.0f;
inline constexpr f32 kHalfPi = kPi * 0.5f;

#pragma endregion

} // namespace GFXM

#endif
