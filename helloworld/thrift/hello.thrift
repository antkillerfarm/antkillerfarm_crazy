typedef i32 MyInteger

//Hey look, you can create a struct
struct Hello_struct {
	1: i32 num1,
	2: i64 num2,
	3: string string1
}

//Service is for defining functions to be used for client and server
//You(the programmer) have to implement them. Just like header files in c++
service HelloSvc {
  //this is a function that returns a string with the name "hello_func" that doesn't have any args
	string hello_func();

  //this is a function that doesn't return a value(void) with the name "hello_func2" that has 2 args
  void hello_func2(1:string var1, 2:i64 var2);

  //this is a function that is oneway which means that client doesn't have to wait for a return value
  //and then can continue executing the next line of code. 
  oneway void hello_func3(1:string var1, 2:Hello_struct struct1, 3:MyInteger var3); 
}
