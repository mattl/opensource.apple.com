//  PR c++/7910
// { dg-do compile { target i?86-*-cygwin* i?86-*-mingw*} }
// { dg-options { -Wall -W } }

class __attribute__((dllimport)) Foo
{
 public:
  virtual void dummy_foo_func(void)	// { dg-warning "inline function" }
    {}
  void Foo::dummy_foo_fun2();
  virtual ~Foo();  //  avoid warning  
};

void Foo::dummy_foo_fun2()	//  { dg-warning "defined" }
{
}

class Bar : public Foo
{
public:
  ~Bar();
  void dummy_bar_func();
};

Bar::~Bar()
{}

void Bar::dummy_bar_func()
{}

// { dg-final { scan-assembler-not "__imp___ZN3Foo14dummy_foo_fun" } }
