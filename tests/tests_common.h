#pragma once

#include <vector>

#include "../intrusive_optional.h"

namespace io
{

   struct one_value
   {
      int a{};
   };
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
using two_values_optional_safe = io::intrusive_optional < io::two_values{}, io::safety_mode::safe >;
