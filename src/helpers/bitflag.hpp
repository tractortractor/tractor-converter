#ifndef TRACTOR_CONVERTER_BITFLAG_H
#define TRACTOR_CONVERTER_BITFLAG_H

#include <cstddef>
#include <type_traits>
#include <limits>
#include <bitset>



namespace tractor_converter{
namespace helpers{



// Modified version of https://softwareengineering.stackexchange.com/a/338472
template<typename Enum, bool IsEnum = std::is_enum<Enum>::value>
class bitflag;

template<typename Enum>
class bitflag<Enum, true>
{
public:
  const static int number_of_bits =
    std::numeric_limits<typename std::underlying_type<Enum>::type>::digits;

  bitflag() = default;
  bitflag(Enum value) : bits(1 << static_cast<std::size_t>(value)) {}
  bitflag(const bitflag &other) : bits(other.bits) {}

  bitflag operator|(bitflag other) const
  {
    bitflag result = *this;
    result.bits |= other.bits;
    return result;
  }
  bitflag operator|(Enum value) const
  {
    bitflag result = *this;
    result.bits |= 1 << static_cast<std::size_t>(value);
    return result;
  }

  bitflag operator&(bitflag other) const
  {
    bitflag result = *this;
    result.bits &= other.bits;
    return result;
  }
  bitflag operator&(Enum value) const
  {
    bitflag result = *this;
    result.bits &= 1 << static_cast<std::size_t>(value);
    return result;
  }

  bitflag operator^(bitflag other) const
  {
    bitflag result = *this;
    result.bits ^= other.bits;
    return result;
  }
  bitflag operator^(Enum value) const
  {
    bitflag result = *this;
    result.bits ^= 1 << static_cast<std::size_t>(value);
    return result;
  }

  bitflag operator~() const
  {
    bitflag result = *this;
    result.bits.flip();
    return result;
  }

  bitflag &operator|=(bitflag other)
  {
    bits |= other.bits;
    return *this;
  }
  bitflag &operator|=(Enum value)
  {
    bits |= 1 << static_cast<std::size_t>(value);
    return *this;
  }

  bitflag &operator&=(bitflag other)
  {
    bits &= other.bits;
    return *this;
  }
  bitflag &operator&=(Enum value)
  {
    bits &= 1 << static_cast<std::size_t>(value);
    return *this;
  }

  bitflag &operator^=(bitflag other)
  {
    bits ^= other.bits;
    return *this;
  }
  bitflag &operator^=(Enum value)
  {
    bits ^= 1 << static_cast<std::size_t>(value);
    return *this;
  }

  bool any() const { return bits.any(); }
  bool all() const { return bits.all(); }
  bool none() const { return bits.none(); }
  explicit operator bool() const { return any(); }

  bool test(Enum value) const
  {
    return bits.test(1 << static_cast<std::size_t>(value));
  }
  void set(Enum value)
  {
    bits.set(1 << static_cast<std::size_t>(value));
  }
  void unset(Enum value)
  {
    bits.reset(1 << static_cast<std::size_t>(value));
  }

private:
  std::bitset<number_of_bits> bits;
};

template<typename Enum>
typename std::enable_if<std::is_enum<Enum>::value, bitflag<Enum>>::type
  operator|(Enum left, Enum right)
{
  return bitflag<Enum>(left) | right;
}
template<typename Enum>
typename std::enable_if<std::is_enum<Enum>::value, bitflag<Enum>>::type
  operator&(Enum left, Enum right)
{
  return bitflag<Enum>(left) & right;
}
template<typename Enum>
typename std::enable_if<std::is_enum<Enum>::value, bitflag<Enum>>::type
  operator^(Enum left, Enum right)
{
  return bitflag<Enum>(left) ^ right;
}
template<typename Enum>
typename std::enable_if<std::is_enum<Enum>::value, bitflag<Enum>>::type
  operator~(Enum value)
{
  return ~bitflag<Enum>(value);
}



} // namespace helpers
} // namespace tractor_converter

#endif // TRACTOR_CONVERTER_BITFLAG_H
