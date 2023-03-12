#include "../siaudio/build.hpp"
#include "ecow.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  auto m = unit::create<mod>("nessa");
  m->add_part("gen");

  auto poc = unit::create<app>("poc");
  poc->add_ref(m);
  poc->add_wsdep("siaudio", siaudio());
  poc->add_unit<>("poc");
  return run_main(poc, argc, argv);
}
