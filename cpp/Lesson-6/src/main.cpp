#include <iostream>
#include <vector>

class MyClass
{
  public:
    MyClass() {
        std::cout << "My class constructor " << std::endl;
    }

    MyClass(const MyClass& copy) {
        std::cout << "My copy constructor?" << std::endl;
    }

    ~MyClass() {
    }
};

int main ()
{
  std::vector<MyClass> myvector;

  // set some content in the vector:
  for (int i=0; i<5; i++) {
    myvector.push_back(MyClass());

    std::cout << "size: " << (int) myvector.size() << '\n';
    std::cout << "capacity: " << (int) myvector.capacity() << '\n';
  }

  std::cout << "\n\nbetter: " << std::endl;

  std::vector<MyClass> mybettervector;

  mybettervector.reserve(5);
  for (int i=0; i<5; i++) {
    mybettervector.emplace_back();

    std::cout << "size: " << (int) myvector.size() << '\n';
    std::cout << "capacity: " << (int) myvector.capacity() << '\n';
  }


  return 0;
}

