#include <iostream>
#include <vector>

int GetRValue()
{
  return 10;
}

int& GetLValue()
{
  static int myvar = 10;
  return myvar;
}

void setValue(const int& param)
{

}

class String
{
  public:
    std::string str;

    String(const char* s) : str(s) { std::cout << "String created" << std::endl; };
    String(const String& s) : str(s.str) { std::cout << "String copied" << std::endl; };
    ~String() { std::cout << "String deleted" << std::endl; };
};

std::ostream& operator<< (std::ostream& stream, const String& other) { 
  stream << "--> " << other.str;
  return stream; 
};

void printl(const std::string& value)
{
  std::cout << value << std::endl;
}


void printr(std::string&& value)
{
  std::cout << value << std::endl;
}

void print(String a)
{
  std::cout << a << std::endl;
}


void print2(String& a)
{
  std::cout << a << std::endl;
}


int main ()
{

  String a = String("Hello");
  print2(a);

  /*

  int a = 5;
  // a es un lvalue, 5 es un rvalue;

  int b = GetRValue();
  // GetRValue() = 3;
  GetLValue() = 3;

  setValue(a); // se llama con un lvalue;
  setValue(5); // se llama con un rvalue;

  // no podemos tener una referencia  de un rvalue

  std::string hello = "Hello";
  std::string world = "World";
  std::string helloWorld = hello + world;

  printl(helloWorld);
  printr(hello + world);
  */ 
  return 0;
}

