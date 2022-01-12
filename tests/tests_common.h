#pragma once

#include <vector>

#include "../intrusive_optional.h"

namespace io
{

   constexpr auto assert(const bool value) -> void
   {
      if(value == false)
      {
         std::terminate();
      }
   }

   struct one_value
   {
      int a{};
      constexpr one_value() = default;
      constexpr one_value(const one_value& other)
         : a(other.a)
      {
         
      }
      constexpr one_value(int p) : a(p) {}
      constexpr one_value(const std::initializer_list<int>& ilist)
         : a(*ilist.begin())
      {}
   };
   static_assert(std::is_trivially_copy_constructible_v<one_value> == false);
   static_assert(std::is_trivially_move_constructible_v<one_value> == false);
   constexpr auto operator==(const one_value& first, const one_value& second) -> bool
   {
      return first.a == second.a;
   }

   struct two_values
   {
      int m_a{};
      int m_b{};
      two_values& operator=(const two_values& other)
      {
         m_a = other.m_a;
         m_b = other.m_b;
         return *this;
      }
      constexpr two_values() = default;

      constexpr two_values(const std::initializer_list<int>& ilist)
         : m_a(*ilist.begin())
         , m_b(*(ilist.begin()+1))
      {

      }

      constexpr two_values(const two_values& other)
         : m_a(other.m_a)
         , m_b(other.m_b)
      {
         
      }

      template<typename ... Args>
      constexpr two_values(const std::initializer_list<int>& ilist, Args&&... args)
         : m_a(*ilist.begin())
         , m_b(args...)
      {

      }

      constexpr two_values(const int a, const int b)
         : m_a(a)
         , m_b(b)
      {

      }
   };
   static_assert(std::is_trivially_copy_constructible_v<two_values> == false);
   static_assert(std::is_trivially_move_constructible_v<two_values> == false);

   [[nodiscard]] constexpr auto operator==(const two_values& first, const two_values& second) -> bool
   {
      return first.m_a == second.m_a && first.m_b == second.m_b;
   }
   [[nodiscard]] constexpr auto operator!=(const two_values& first, const two_values& second) -> bool
   {
      return (first == second) == false;
   }

   // All other comparisons are limited to first value
   [[nodiscard]] constexpr auto operator<(const two_values& first, const two_values& second) -> bool
   {
      return first.m_a < second.m_a;
   }
   [[nodiscard]] constexpr auto operator>(const two_values& first, const two_values& second) -> bool
   {
      return first.m_a > second.m_a;
   }
   [[nodiscard]] constexpr auto operator<=(const two_values& first, const two_values& second) -> bool
   {
      return first.m_a <= second.m_a;
   }
   [[nodiscard]] constexpr auto operator>=(const two_values& first, const two_values& second) -> bool
   {
      return first.m_a >= second.m_a;
   }
   
}


using two_values_optional = io::intrusive_optional < io::two_values{} >;
using two_values_optional_safe = io::intrusive_optional < io::two_values{}, io::safety_mode_t::safe >;
