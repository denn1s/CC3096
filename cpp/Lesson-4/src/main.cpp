#include <iostream>	

void Print(int a)
{
  std::cout << a << std::endl;
}

void Print(std::string a)
{
  std::cout << a << std::endl;
}

template<typename T>
void PrintWithTemplate(T a)
{
  std::cout << a << std::endl;
}

template<int N>
class MyArray {
  private:
    int array[N];
  
  public:
    int len() { return N; }
};

int main(int argc, char* args[] )
{
  Print(5);
  Print("Hello");
  PrintWithTemplate<float>(5);
  PrintWithTemplate<std::string>("Hello");
  PrintWithTemplate<float>(5.1);
  MyArray<10> a;
  Print(a.len());
}

