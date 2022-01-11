#include "test_comparisons.h"

#include "tests_common.h"


namespace
{

   constexpr auto test_1_to_6() -> bool
   {
      // operator ==, operator !=
      {
         constexpr two_values_optional first(std::in_place, 2, 5);
         constexpr two_values_optional second(std::in_place, 2, 5);
         constexpr two_values_optional third(std::in_place, 3, 5);
         static_assert(first == first);
         static_assert(first == second);
         static_assert(first != third);
      }

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

      return true;
   }
   
} // namespace {}

auto io::test_comparisons() -> void
{
   test_1_to_6();
}
