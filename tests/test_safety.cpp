#include "test_safety.h"

#include "tests_common.h"


namespace
{

   template<typename lambda>
   auto has_thrown(const lambda& fun) -> bool
   {
      try
      {
         fun();
      }
      catch(io::unintentionally_null)
      {
         return true;
      }
      return false;
   }



   auto test_ctor_6() -> bool
   {
      using opt_type = io::intrusive_optional<-1, io::safety_mode::safe>;
      constexpr auto lambda = []() {opt_type value(std::in_place, -1); };
      if (has_thrown(lambda) == false)
      {
         std::terminate();
      }

      return true;
   }


   auto test_ctor_7() -> bool
   {
      constexpr auto lambda = []() {two_values_optional_safe value(std::in_place, {0}, 0); };
      if (has_thrown(lambda) == false)
      {
         std::terminate();
      }

      return true;
   }


   auto test_ctor_8() -> bool
   {
      using opt_type = io::intrusive_optional<-1, io::safety_mode::safe>;
      constexpr auto lambda = []() {opt_type value(-1); };
      if (has_thrown(lambda) == false)
      {
         std::terminate();
      }

      return true;
   }


   auto test_assignment_4() -> bool
   {
      using opt_type = io::intrusive_optional<0, io::safety_mode::safe>;
      auto lambda = []()
      {
         opt_type value;
         value = 0ui8;
      };
      if (has_thrown(lambda) == false)
      {
         std::terminate();
      }

      return true;
   }


   auto test_operator_star() -> bool
   {
      using opt_type = io::intrusive_optional<-1, io::safety_mode::safe>;
      opt_type default_constructed;

      // This won't compile
      //*default_constructed = 5;

      return true;
   }


   auto test_value() -> bool
   {
      using opt_type = io::intrusive_optional<-1, io::safety_mode::safe>;
      opt_type default_constructed;

      // This won't compile
      //default_constructed.value() = 5;

      return true;
   }


   auto test_emplace_1() -> bool
   {
      constexpr auto lambda = []()
      {
         two_values_optional_safe value;
         value.emplace(0, 0);
      };
      if (has_thrown(lambda) == false)
      {
         std::terminate();
      }

      return true;
   }


   auto test_emplace_2() -> bool
   {
      constexpr auto lambda = []()
      {
         two_values_optional_safe value;
         value.emplace({ 0 }, 0);
      };
      if (has_thrown(lambda) == false)
      {
         std::terminate();
      }

      return true;
   }
   
   
} // namespace {}


auto io::test_safety() -> void
{
   test_ctor_6();
   test_ctor_7();
   test_ctor_8();
   test_assignment_4();
   test_operator_star();
   test_value();
   test_emplace_1();
   test_emplace_2();
}
