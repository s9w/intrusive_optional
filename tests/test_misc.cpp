#include "test_misc.h"

#include "tests_common.h"


namespace
{

   auto test_hash() -> bool
   {
      using opt_type = io::intrusive_optional<-1>;
      {
         const opt_type a(2);
         const opt_type b(3);
         io::assert(std::hash<opt_type>{}(a) == std::hash<opt_type>{}(a));
         io::assert(std::hash<opt_type>{}(a) != std::hash<opt_type>{}(b));
      }

      return true;
   }
   
} // namespace {}

auto io::test_misc() -> void
{
   test_hash();
}
