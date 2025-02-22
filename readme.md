# Goblang
A very simple interpreted programming language inspired by c, lua and jvm. The goal behind this project was to create a simple programming language which could potentially be expanded upon in the future, with simple and user friendly function binding interface.

# Features
While goblang lacks many of the features present in most other languages there are still enough basic language features to make it usable for small programms
## Variables
There is currently support for local and global variables.
### Global variables
Global variables are created by using assignment operation without `let`, as long as the name used for the variable is not used by any local variable in the scope. 
```
    # will create global variable
    a = 8;
```
If the variable is in use it will perform simple assignment 
```
    let a = 9;
    a = 8;
```
Global variables are also a way to interface with c++ code, because they can be set to any value from the c++ code that runs the interpreter.
If variable name is used but no local variable is created prior to this call, the interpreter will assume that it is a call to global variable
```
    # assumes that 'a' is a variable that will be created from native code
    let b = a;
```
Although global variables can be created inside the goblang code, it is recommended to avoid this, using local variables instead.
Local variables are only usable inside the block that they were created in and do not use strings for identification under the hood
```
    let local_var = some_value;
    if(local_var){
        # this variable can only be accessed inside this block
        let exclusive = !local_var;
    }
    # because this is outside the if block, interpreter assumes that this is call to global variable
    let local2 = exclusive;
    if(!local_var){
        # this local variable is not related to the previous one
        let exclusive = local_var;
    }
```

## Loops

There is currently only support for `while` loops, although this does include operations like `break` and `continue`
Example of a loop is 
```
while(condition){
    # do stuff 
}
```
These loops will run for as long as the condition is true. While there are no for loops added explicitly it would be fairly trivial to replicate them 
```
    let i = 0;
    while(i < some_val){
        # ... actions
        i = i + 1;
    }
```
Same can be done for `do{}while()` loops present in c and c++ using breaks.
`break` and `continue` will always operate on the block that they are currently placed in
```
let i = 0;
while(true){
    # ... actions
    if(i < some_val){
        break;
    }
}
```

## Branches

Branches can be created by using `if` operator and can have as many `elif`(else if) branches as needed

```
if(cond1){

}
elif(cond2){

}
else{

}
```
## Arrays and strings

Arrays are special objects that represent a sequence of values. All arrays in goblang are typeless so any way can be added inside the array.
to create an array a call to `array` function is required. To get size of the array of the array `sizeof` can be called. 
All new values in the array will be set to `null`
```
    let arr = array(10);
    # will print 10
    print(sizeof(arr));
```
Values in the array can be accessed or changed by using `[]` operator.
```
    let arr = array(3);
    arr[0] = 9;
    arr[1] = array[2];
    arr[1][0] = "hello";
```
Same access operation and sizeof function can be used on strings. 

Unlike all other types strings and arrays are not passed around by value and instead are passed around by reference.

```
    # in this example both a and b point to the same string
    let a = "hello";
    let b = a;
    a[0] = 'j';
    # at this point both b and a will return "jello"
```

### Array literals
An alternative way to create an array without using `array` function would be to use array literals. By writing `[val1, val2,val3,..., valn]` user can create a "constant" array of size n. Both `array` function and array literal create same types of array object, however array literals can be used for quickly creating short arrays with known values. 
Although they are somewhat constant they are not the same type of array as a c-array. Each time an array literal is referenced all the values are pushed onto the stack and then popped by the `create_array n` operation. This means that any value can be an array value, including function calls, which will execute their logic once pushed.
For example 
```
let inputs = [input(), input(), input()];
```
Would be a valid way to create an array of size 3, where each element will be an input requested from the standard input. 

## Functions
As of right now only functions exposed to goblang using `addFunction` method can be called.

Native c++ functions can be called from goblang language by simply using the call operation `func()`. All native functions must receive pointer to the interpreter as the only argument and use operation stack for managing arguments.

Example of adding a native function:
```cpp
    // simple function that just multiplies the argument by 2
    void example(GobLang::Machine *m){
        using namespace GobLang;
        MemoryValue * v = m->getStackTopAndPop();
        m->pushToStack(MemoryValue{.type = Type::Int, .value = std::get<int32_t>(v->value) * 2});
        // dont forget to delete the memory value!
        delete v;
    }

    // inside the function that has instance of the Machine
    {
        // ...
        machine.addFunction(example, "example");
        // .. 
    }
```
and now this function can be called from goblang like this

```
    let a = example(9);
```
## Custom functions

Custom functions can be written using the `func` keyword. Functions can not be defined inside of other functions and can be called from any point at code.
Calling a function before it is defined will result in interpreter assuming that a call to global function was requested.
Creating functions

```
# A simple function that adds values of all arguments
func example(a,c,d){
    return a + c + d;
}
print(example(1,2,3));
```
Functions can have any amount of arguments or have zero

```
func hello_world(){
    print_line("hello world");
}
```

Functions can access global variables the same way as any other part of the code, however they have their own local variables and stack array meaning that they can not directly affect the state of the local code that called it

# Interpreter

Interpreter operates using a stack for all operations so anything that needs to be used needs to be put onto the stack first. There is are no registers of any kind.
For data storage there is dictionary of global variables `std::map<std::string, MemoryValue>` and local variable array `std::vector<MemoryValue>`
Each value is stored using a c++ alternative to union that being
```cpp
using FunctionValue = std::function<void(Machine *)>;
using Value = std::variant<bool, char, float, int32_t, void *, MemoryNode *, FunctionValue>;

struct MemoryValue
{
    Type type;
    Value value;
};
```

## Garbage collection

There is a very basic garbage collector implemented into the interpreter that uses reference counting to know when to delete objects. 
Every object is created with ref count being set to 0 and on every assignment operation(which includes operations like setting a global value, local value and array value) it increases the ref count by 1. On the same operations it checks if object that is being replaced by set operation is object and if so, decreases ref counter for that object.

Similar operation occurs when shrinking the local variable array, although it only performs ref count decrease.

# Using the interpreter

To execute the code call `goblang -i <code_with_file>` in the terminal
Options: 
* -v or --version    : Display version of the interpreter
* -h or --help       : View help about the interpreter
* -i or --input      : Run code from file in a given location
* -s or --showbytes  : Show bytecode before running code

# Possible future features

## Custom types
Custom types would be similar to `struct` in c and would just be plain data types, with possibly rust style syntax for adding methods

Example of possible syntax

```
struct Type{
    a : int,
    h : float,
    d : array
}


impl Type{
    func method1() {}
    func method2() {}
}
```

## Strong typing

While right now there are no 'compile' time type checks, i do want to add them to the project. As well as ability to disallow globals, requiring to tell the parser all functions in advance instead of assuming that they will be added after generating bytecode

