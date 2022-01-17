#include "test_constructors.h"

#include "tests_common.h"


namespace
{

   constexpr auto test_1()-> void
   {
      {
         constexpr two_values_optional default_constructed;
         static_assert(default_constructed.has_value() == false);
      }

      {
         constexpr two_values_optional nullopt_constructed(std::nullopt);
         static_assert(nullopt_constructed.has_value() == false);
      }
   }


   constexpr auto test_2()-> void
   {
      {
         constexpr two_values_optional default_constructed;
         constexpr two_values_optional copy_target(default_constructed);
         static_assert(copy_target.has_value() == false);
      }

      {
         constexpr two_values_optional value(std::in_place, 2, 3);
         constexpr two_values_optional copy_target(value);
         static_assert(copy_target.has_value() == true);
         static_assert(copy_target == two_values_optional(std::in_place, 2, 3));
      }

      {
         using one_value_optional = io::intrusive_optional < io::one_value{} > ;
         const one_value_optional value(std::in_place, 2);
         one_value_optional copy_target(value);
         io::assert(*value == *copy_target);
      }
   }


   constexpr auto test_3()-> void
   {
      {
         constexpr auto f = []() {
            two_values_optional source(std::in_place, 2, 3);
            two_values_optional copy = source;
            two_values_optional target = std::move(source);
            if (copy != target)
               return false;
            if (source.has_value() == false) // "If other contains a value [...] and does not make other empty"
               return false;
            return true;
         };
         static_assert(f() == true);
      }
      {
         using one_value_optional = io::intrusive_optional < io::one_value{} > ;
         one_value_optional value(std::in_place, 2);
         one_value_optional copy_target(std::move(value));
         io::assert(*value == *copy_target);
         io::assert(value.has_value());
      }
   }


   constexpr auto test_4()-> void
   {
      {
         using type0 = io::intrusive_optional < io::one_value{ 0 } > ;
         using type1 = io::intrusive_optional < io::one_value{ -1 } > ;
         constexpr type0 first(7);
         constexpr type1 second(first);
         static_assert(*first == *second);
      }
      {
         using type0 = io::intrusive_optional < io::one_value{ 0 } > ;
         using type1 = io::intrusive_optional < io::one_value{ -1 } > ;
         constexpr type1 first(7);
         constexpr type0 second(first);
         static_assert(*first == *second);
      }
   }


   constexpr auto test_5()-> void
   {
      using type0 = io::intrusive_optional < io::one_value{ 0 } > ;
      using type1 = io::intrusive_optional < io::one_value{ -1 } > ;
      constexpr type0 first(7);
      constexpr type1 second(std::move(first));
      static_assert(*first == *second);
   }


   constexpr auto test_6()-> void
   {
      constexpr two_values_optional first(std::in_place, 2, 3);
      constexpr io::two_values expected(2, 3);
      static_assert(first.value() == expected);
   }


   constexpr auto test_7()-> void
   {
      constexpr two_values_optional first(std::in_place, {2}, 3);
      constexpr io::two_values expected(2, 3);
      static_assert(first.value() == expected);
   }


   constexpr auto test_8()-> void
   {
      constexpr io::intrusive_optional < io::one_value{} > first(5);
      constexpr io::one_value expected(5);
      static_assert(first.value() == expected);
   }
   
} // namespace {}


auto io::test_constructors() -> void
{
   test_1();
   test_2();
   test_3();
   test_4();
   test_5();
   test_6();
   test_7();
   test_8();
}
