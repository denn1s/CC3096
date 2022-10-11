#include <iostream>
#include <cstring>
#include <vector>
#include <memory>

class String
{
  public:
    char* data;
    int size;

    String(const char* s) { 
      std::cout << "String created" << std::endl;
      size = strlen(s);
      data = new char[size];
      memcpy(data, s, size);
    };

    String(const String& other) { 
      std::cout << "String copied" << std::endl;
      size = other.size;
      data = new char[size];
      memcpy(data, other.data, size);
    };

    String(String&& other) { 
      std::cout << "String moved" << std::endl;
      size = other.size;
      data = other.data;

      // hollow object
      other.data = nullptr;
      size = 0;
    };

    String& operator=(String&& other)
    {
      std::cout << "String moved via assignment" << std::endl;
      size = other.size;
      // we must delete or own old data
      delete data;
      data = other.data;

      // hollow object
      other.data = nullptr;
      size = 0;
      
      return *this;
    }

    // r value reference
    // String(String&& s) : str(s.str) { std::cout << "String moved" << std::endl; };

    ~String() {
      std::cout << "String deleted" << std::endl; 
      delete data;
    };
};

class Entity
{
  public:
    Entity(String&& n) : name((String&&)n) { std::cout << "Entity created" << std::endl; } ;
    ~Entity() { std::cout << "Entity destroyed" << std::endl; } ;

  private:
    String name;
};

class SmartPtr {
  private:
    int* ptr;
  public:
    SmartPtr(int* p) { ptr = p; }
     ~SmartPtr() { delete (ptr); }
    int& operator*() { return *ptr; }
    int* operator->() { return ptr; }
};

template <class T>
class SmartPtr2 {
  private:
    T* ptr;
  public:
    SmartPtr2(T* p) { ptr = p; }
     ~SmartPtr2() { delete (ptr); }
    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }
};


int main ()
{
  {
    // Entity e = Entity(String("PLAYER"));
    String a = String("FIRST");
    // String b = a;
    // String b = (String&&)a;
    // String b = std::move(a);

    String c = "SECOND";
    c = std::move(a);


    exit(1);
  } 

  while(1)
  {
    // SmartPtr a(new int(5));
    // SmartPtr2<int> a(new int(5));

    // std::unique_ptr<int> a = std::make_unique<int>(5);
    // std::unique_ptr<int> b = a;

    // std::shared_ptr<int> a = std::make_shared<int>(5);
    // std::shared_ptr<int> b = a;
    // std::cout << a.use_count() << std::endl;

    // std::shared_ptr<int> a = std::make_shared<int>(5);
    // std::weak_ptr<int> b = a;
    // std::cout << a.use_count() << std::endl;


    std::cout << "loop!" << std::endl;
  }
  return 0;
}

