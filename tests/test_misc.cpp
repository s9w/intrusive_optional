#include "test_misc.h"

#include "tests_common.h"


namespace
{

   auto test_hash()-> void
   {
      using opt_type = io::intrusive_optional<-1>;
      {
         const opt_type a(2);
         const opt_type b(3);
         io::assert(std::hash<opt_type>{}(a) == std::hash<opt_type>{}(a));
         io::assert(std::hash<opt_type>{}(a) != std::hash<opt_type>{}(b));
      }
   }


   auto test_assignment_from_std()-> void
   {
      using opt_type = io::intrusive_optional<-1>;
      {
         std::optional<int> std_opt(5);
         opt_type io;
         io = std_opt;
         io::assert(*std_opt == *io);
      }
      {
         std::optional<int> std_opt;
         opt_type io;
         io = std_opt;
         io::assert(io.has_value() == false);
      }
      {
         std::optional<int> std_opt(5);
         opt_type io;
         io = std::move(std_opt);
         io::assert(*std_opt == *io);
      }
      {
         std::optional<int> std_opt;
         opt_type io;
         io = std::move(std_opt);
         io::assert(io.has_value() == false);
      }
   }


   auto test_conversion_to_std()-> void
   {
      using opt_type = io::intrusive_optional<-1>;
      {
         opt_type default_constructed;
         auto std_opt = default_constructed.get_std();
         io::assert(std_opt.has_value() == false);
      }
      {
         opt_type opt(5);
         auto std_opt = opt.get_std();
         io::assert(*std_opt == 5);
      }
   }


   auto test_construction_from_std()-> void
   {
      using opt_type = io::intrusive_optional<-1>;
      {
         std::optional<int> std;
         opt_type opt(std);
         io::assert(opt.has_value() == false);
      }
      {
         std::optional<int> std(5);
         opt_type opt(std);
         io::assert(*opt == *std);
      }

      {
         std::optional<int> std;
         opt_type opt(std::move(std));
         io::assert(opt.has_value() == false);
      }
      {
         std::optional<int> std(5);
         opt_type opt(std::move(std));
         io::assert(*opt == *std);
      }
   }
   
} // namespace {}


auto io::test_misc() -> void
{
   test_hash();
   test_assignment_from_std();
   test_conversion_to_std();
   test_construction_from_std();
}
