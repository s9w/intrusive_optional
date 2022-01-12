#include "test_assignments.h"

#include "tests_common.h"


namespace
{

   constexpr auto test_1() -> bool
   {
      {
         constexpr two_values_optional assigned = std::nullopt;
         static_assert(assigned.has_value() == false);
      }

      return true;
   }


   constexpr auto test_2() -> bool
   {
      {
         constexpr two_values_optional first(std::in_place, 2, 3);
         constexpr two_values_optional assigned = first;
         static_assert(*first == *assigned);
      }

      {
         constexpr two_values_optional first;
         constexpr two_values_optional assigned = first;
         static_assert(*first == *assigned);
      }

      return true;
   }


   constexpr auto test_3() -> bool
   {
      {
         constexpr two_values_optional first(std::in_place, 2, 3);
         constexpr two_values_optional assigned = std::move(first);
         static_assert(*first == *assigned);
      }

      {
         constexpr two_values_optional first;
         constexpr two_values_optional assigned = std::move(first);
         static_assert(*first == *assigned);
      }

      return true;
   }


   constexpr auto test_4() -> bool
   {
      using opt_type = io::intrusive_optional<5>;

      {
         constexpr auto generator = []()
         {
            opt_type value;
            value = 5ui8;
            return value;
         };
         static_assert(*generator() == 5);
      }

      return true;
   }


   constexpr auto test_5() -> bool
   {
      using opt_type_a = io::intrusive_optional<10>;
      using opt_type_b = io::intrusive_optional<10ui8>;
      {
         opt_type_b first;
         opt_type_a second;
         second = first;
         io::assert(second.has_value() == false);;
      }
      {
         opt_type_b first(5ui8);
         opt_type_a second;
         second = first;
         io::assert(*second == 5);;
      }

      return true;
   }


   constexpr auto test_6() -> bool
   {
      using opt_type_a = io::intrusive_optional<10>;
      using opt_type_b = io::intrusive_optional<10ui8>;
      {
         opt_type_b first;
         opt_type_a second;
         second = std::move(first);
         io::assert(second.has_value() == false);;
      }
      {
         opt_type_b first(5ui8);
         opt_type_a second;
         second = std::move(first);
         io::assert(*second == 5);;
         io::assert(first.has_value());;
      }

      return true;
   }
   
} // namespace {}


auto io::test_assignments() -> void
{
   test_1();
   test_2();
   test_3();
   test_4();
   test_5();
   test_6();
}
