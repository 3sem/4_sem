#include "lamport_clock.h"

void assert_eq(int retcode, int arg) {
  std::cout << retcode == arg ? "True":"False" 
    << "assertion on equivalence"<< retcode<< " and "<< arg << std::endl; 
}

using namespace std;

void test1() {
  LamportClock clock;

  clock.receive_event(3);
  assert_eq(clock.get_time(), 4);

  clock.receive_event(2);
  assert_eq(clock.get_time(), 5);

  clock.receive_event(5);
  assert_eq(clock.get_time(), 6);
}

int main() {
  test1();
  return 0;
}
