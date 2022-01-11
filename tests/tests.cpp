#include "test_constructors.h"
#include "test_comparisons.h"
#include "test_assignments.h"


int main()
{
   io::test_constructors();
   io::test_comparisons();
   io::test_assignments();

   return 0;
}
