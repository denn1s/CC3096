#include <iostream>	


class Father {};

class Son : public Father {
  public:
    virtual void talk() = 0;
};

class Timmy : public Son {
  public:
    void talk() override {
      std::cout << "IM TIMMY!" << std::endl;
    } 
};

class Billy : public Son {
  public:
    void talk() override {
      std::cout << "IM BILLY!" << std::endl;
    } 
};

void speak(Son* who) {
  who->talk();
}

int main(int argc, char* args[] )
{
  Timmy* t = new Timmy();
  speak(t);

  Billy* b = new Billy();
  speak(b);
}

