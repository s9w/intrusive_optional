#include "test_comparisons.h"

#include "tests_common.h"


namespace
{

   constexpr auto test_1_to_6() -> bool
   {
      // 1-2
      {
         constexpr two_values_optional first(std::in_place, 2, 5);
         constexpr two_values_optional second(std::in_place, 2, 5);
         constexpr two_values_optional third(std::in_place, 3, 5);
         static_assert(first == first);
         static_assert(first == second);
         static_assert(first != third);
      }

      // 3-6
      {
         constexpr two_values_optional first(std::in_place, 2, 5);
         constexpr two_values_optional second(std::in_place, 3, 5);
         static_assert(first < second);
         static_assert(first < first == false);
         static_assert(first <= first);

         static_assert(second > first);
         static_assert(second > second == false);
         static_assert(second >= second);
      }

      // overload 7
      {
         io::intrusive_optional < -1 > a;
         io::intrusive_optional < -1.0 > b;
         io::assert(a<=>b == std::strong_ordering::equal);
         b.emplace(2.0);
         io::assert(a <=> b == std::strong_ordering::less);
         a.emplace(3);
         io::assert(a <=> b == std::strong_ordering::greater);
      }

      return true;
   }
   
} // namespace {}

auto io::test_comparisons() -> void
{
   test_1_to_6();
}
