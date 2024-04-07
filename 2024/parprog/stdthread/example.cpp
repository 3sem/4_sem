#include <iostream>
#include <thread>

using namespace std;

//function
void foo(int x) {
  for (int i = 0; i < x; i++) {
    cout << "Thread uses this function as callable\n";
  }
}

// functor
class thread_obj {
public:
  void operator()(int x) {
    for (int i = 0; i < x; i++)
      cout << "Thread uses this object as callable\n";
  }
}; 

int main() {
  // function 'foo' will be called
  thread th1(foo, 5);
  // functor will be called
  thread th2(thread_obj(), 5);
  // threads should be joined or detached. UB otherwise.
  th1.join();
  th2.join();
  return 0;
} 
