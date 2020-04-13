#include <stdio.h>
#include <stdlib.h>
#include "../src/app/trivial.h"

int main() {
   Trivial* trivial = new Trivial(0, "", 1);
   trivial->run_();
   return 0;
}
