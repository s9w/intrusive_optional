#include "test_constructors.h"
#include "test_comparisons.h"
#include "test_assignments.h"
#include "test_safety.h"
#include "test_misc.h"

#include "tests_common.h"

#include "../swl_optional.hpp"


int main()
{
   io::test_constructors();
   io::test_comparisons();
   io::test_assignments();
   io::test_safety();
   io::test_misc();

   return 0;
}
