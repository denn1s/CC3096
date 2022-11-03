#include <iostream>
#include <sol/sol.hpp>

/*
inline void panic(sol::optional<std::string> maybe_msg)
{
  std::cout << "Lua is in panic mode! aaaaaa" << std::endl;
  if (maybe_msg)
  {
    std::cout << "This is causing the panic: " << maybe_msg.value() << std::endl;
  }
}
*/


void some_function() {
	std::cout << "some function!" << std::endl;
}

void some_other_function() {
	std::cout << "some other function!" << std::endl;
}

struct some_class {
	int variable = 30;

	double member_function() {
		return 24.5;
	}
};

struct Doge {
	int tailwag = 50;

	Doge() {
	}

	Doge(int wags)
	: tailwag(wags) {
	}

	~Doge() {
		std::cout << "Dog at " << this << " is being destroyed..." << std::endl;
	}
};

int main ()
{

	sol::state lua;
	//sol::state lua(sol::c_call<decltype(&panic), &panic>);

	// open some common libraries
	// lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.script("a = 5 + 5");
	lua.script("b = a + 10");
  int value = lua.script("return b");

	std::cout << "a in cpp: " << value << std::endl;

	lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.script("print('bark bark bark!')");
	std::cout << std::endl;



  auto result = lua.script("1", [](lua_State*, sol::protected_function_result pfr) {
		// pfr will contain things that went wrong, for either loading or executing the script
		// Can throw your own custom error
		// You can also just return it, and let the call-site handle the error if necessary.
		return pfr;
	});

  std::cout << "is it valid: " << result.valid() << " why? " << ((sol::error)result).what() << std::endl;

	// load file without execute
	sol::load_result script1 = lua.load_file("scripts/a_lua_script.lua");
	//execute
	script1();


	// load string without execute
	sol::load_result script2 = lua.load("a = 'test'");
	//execute
	sol::protected_function_result script2result = script2();
	// optionally, check if it worked
	if (script2result.valid()) {
		std::cout << "Script executed successfully" << std::endl;
	}
	else {
    sol::error err = script2result;
		std::cout << "Script failed successfully" << std::endl;
		std::cout << err.what() << std::endl;
	}

	sol::load_result script3 = lua.load("return 24");
	// execute, get return value
	int value2 = script3();

  std::cout << "Value: " << value2 << std::endl;

  // Pass parameters

	const auto& my_script = R"(
    local a,b,c = ...
    print(a,b,c)
  )";

  sol::load_result fx = lua.load(my_script);
  if (!fx.valid()) {
    sol::error err = fx;
    std::cerr << "failed to load string-based script into the program" << err.what() << std::endl;
  }
      
  // prints "your arguments here"
  fx("your", "arguments", "here");


  // Set and get

  // integer types
	lua.set("number", 24);
	// floating point numbers
	lua["number2"] = 24.5;
	// string types
	lua["important_string"] = "woof woof";
	// is callable, therefore gets stored as a function that can be called
	lua["a_function"] = []() { return 100; };
	// make a table
	lua["some_table"] = lua.create_table_with("value", 24);

  // get

  lua.script(R"(
		abc = { [0] = 24 }
		def = { 
			ghi = { 
				bark = 50, 
				woof = abc 
			} 
		}
	)");

	sol::table abc = lua["abc"];
	sol::table def = lua["def"];
	sol::table ghi = lua["def"]["ghi"];

	int bark1 = def["ghi"]["bark"];
	int bark2 = lua["def"]["ghi"]["bark"];
	int abcval1 = abc[0];
	int abcval2 = ghi["woof"][0];

  std::cout << "bark1: " << bark1 << std::endl;
  std::cout << "bark2: " << bark2 << std::endl;

  sol::optional<int> will_not_error = lua["abc"]["DOESNOTEXIST"]["ghi"];
	int also_will_not_error = lua["abc"]["def"]["ghi"]["jklm"].get_or(25);

  std::cout << "errors: " << also_will_not_error << std::endl;

  // functions 

	lua.script("function f (a, b, c, d) return 1 end");
	lua.script("function g (a, b) return a + b end");

	// sol::function is often easier: 
	// takes a variable number/types of arguments...
	sol::function fx2 = lua["f"];
	// fixed signature std::function<...>
	// can be used to tie a sol::function down
	std::function<int(int, double, int, std::string)> stdfx = fx2;

	int is_one = stdfx(1, 34.5, 3, "bark");
	int is_also_one = fx2(1, "boop", 3, "bark");

	// call through operator[]
	int is_three = lua["g"](1, 2);
	double is_4_8 = lua["g"](2.4, 2.4);

  std::cout << "4.8: " << is_4_8 << std::endl;

  // classes and functions 

  // put an instance of "some_class" into lua
	// (we'll go into more detail about this later
	// just know here that it works and is
	// put into lua as a userdata
	lua.set("sc", some_class());

	// binds a plain function
	lua["f1"] = some_function;
	lua.set_function("f2", &some_other_function);

	// binds just the member function
	lua["m1"] = &some_class::member_function;

	// binds the class to the type
	lua.set_function("m2", &some_class::member_function, some_class{});

	// binds just the member variable as a function
	lua["v1"] = &some_class::variable;

	// binds class with member variable as function
	lua.set_function("v2", &some_class::variable, some_class{});

  lua.script(R"(
	f1() -- some function!
	f2() -- some other function!
	
	-- need class instance if you don't bind it with the function
	print(m1(sc)) -- 24.5
	-- does not need class instance: was bound to lua with one 
	print(m2()) -- 24.5
	
	-- need class instance if you 
	-- don't bind it with the function
	print(v1(sc)) -- 30
	-- does not need class instance: 
	-- it was bound with one 
	print(v2()) -- 30

	-- can set, still 
	-- requires instance
	v1(sc, 212)
	-- can set, does not need 
	-- class instance: was bound with one 
	v2(254)

	print(v1(sc)) -- 212
	print(v2()) -- 254
	)");

  // multiple results

  lua.script("function f (a, b, c) return a, b, c end");

  std::tuple<int, int, int> resultTuple;
  resultTuple = lua["f"](100, 200, 300);
  // resultTuple == { 100, 200, 300 }
  int a;
  int b;
  std::string c;
  sol::tie(a, b, c) = lua["f"](100, 200, "bark");

	std::cout << std::endl;

  // using classes

  Doge dog{ 30 };

	// fresh one put into Lua
	lua["dog"] = Doge{};
	// Copy into lua: destroyed by Lua VM during garbage collection
	lua["dog_copy"] = dog;
	// OR: move semantics - will call move constructor if present instead
	// Again, owned by Lua
	lua["dog_move"] = std::move(dog);
	lua["dog_unique_ptr"] = std::make_unique<Doge>(25);
	lua["dog_shared_ptr"] = std::make_shared<Doge>(31);

	// Identical to above
	Doge dog2{ 30 };
	lua.set("dog2", Doge{});
	lua.set("dog2_copy", dog2);
	lua.set("dog2_move", std::move(dog2));
	lua.set("dog2_unique_ptr", std::unique_ptr<Doge>(new Doge(25)));
	lua.set("dog2_shared_ptr", std::shared_ptr<Doge>(new Doge(31)));

	// Note all of them can be retrieved the same way:
	Doge& lua_dog = lua["dog"];
	Doge& lua_dog_copy = lua["dog_copy"];
	Doge& lua_dog_move = lua["dog_move"];
	Doge& lua_dog_unique_ptr = lua["dog_unique_ptr"];
	Doge& lua_dog_shared_ptr = lua["dog_shared_ptr"];
	std::cout << std::endl;

  // if it wont die:

  lua["dog"] = &dog;
	// Same as above: respects std::reference_wrapper
	lua["dog"] = std::ref(dog);
	// These two are identical to above
	lua.set( "dog", &dog );
	lua.set( "dog", std::ref( dog ) );


	Doge& dog_ref = lua["dog"]; // References Lua memory
	Doge* dog_pointer = lua["dog"]; // References Lua memory
	Doge dog_copy = lua["dog"]; // Copies, will not affect lua

  	lua.new_usertype<Doge>("Doge",
		"tailwag", &Doge::tailwag
	);

	dog_copy.tailwag = 525;
	// Still 50

	dog_ref.tailwag = 100;
	// Now 100

	dog_pointer->tailwag = 345;
	// Now 345

	std::cout << std::endl;

  return 0;
}

