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



   auto test_6() -> bool
   {
      using opt_type = io::intrusive_optional<-1, io::safety_mode::safe>;
      const auto lambda = []() {opt_type value(std::in_place, -1); };
      if (has_thrown(lambda) == false)
      {
         std::terminate();
      }

      return true;
   }


   auto test_7() -> bool
   {
      const auto lambda = []() {two_values_optional_safe value(std::in_place, {0}, 0); };
      if (has_thrown(lambda) == false)
      {
         std::terminate();
      }

      return true;
   }


   auto test_8() -> bool
   {
      using opt_type = io::intrusive_optional < io::one_value{ 0 }, io::safety_mode::safe > ;
      const auto lambda = []() {opt_type value(0); };
      if (has_thrown(lambda) == false)
      {
         std::terminate();
      }

      return true;
   }

   
   
} // namespace {}


auto io::test_safety() -> void
{
   test_6();
   test_7();
   test_8();
}
