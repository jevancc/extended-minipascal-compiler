# Classes
Similar to the structure in C, class is another user-defined data type available in Mini-Pascal that allows combining data items of different kinds. Moreover, class in Mini-Pascal allow users to define methods, it acts similar to the methods in C++ class.

A class is a file containing Mini-Pascal variable declarations, function definitions, and statements. The file name is the class name with the suffix `.pc` appended. Within a class, the class' name should be the same as the file name.

## Syntax
To write a module, you need to create a new file with suffix `.pc`. The syntax of module file is as follows.
```
CLASS <class_name>;
<variable_declarations>
<subprogram_definitions>
.
```
Notice that the `<class_name>` should be equal to the file name.
The variables in `<variable_declarations>` will be the member of the class, it is same as the member in the C structure.

The subprograms defined in `<subprogram_definitions>` will be the methods of the class. It is similar to the methods of C++ class. A valid subprogram can be a `function`, `procedure`, or `jfunction`.

## Use Class in Main Program
In your Mini-Pascal main program, the syntax for you to import the module is as follows:
```
use <class_name>;
```
It will then import the module file from the search path.

## The Class Search Path
When a class is used in the main program, the compiler searches for a file named `<class_name>.pc` in a list of directories given with the compiler optional argument `--library,-l`. It will search the directories in the order.

> Note: On file systems which support symlinks, the directories containing the symlink or the class symlink is not added to the library search path.

## Class Type Variable
After the use statement in the main program, the used class will act as a type in the program. You can declare variables with class type, declare a function with class type argument, etc. You can access the member in the following way:
```
<class_name>.<member>
```

## Class Methods
The subprograms defined in the class file will are the methods of the class. In the methods, you can access the member variables by accessing their variable name. Unlike C++, you can not use the `this` to specify members of the class.
> If you define the method as a JFunction, you should notice that the class object will be stored in register 0, and the register number that store the arguments will begin from number 1

## Example
Following is a simple example of class in Mini-Pascal.   
leSolver is a class to solve the linear equation `y = mx+b`. By setting the `m`, `b`, and `y` of the member, you can call the method `solve` and the value of `x` will be stored in the member `x`. The codes can be found in the `example/` and `example/class`.    

**class/leSolver.pc**
```
class leSolver;
var m, x, y, b: real;

procedure solve;
begin
    x := (y - b)/m;
end;
.
```
**exClass.p**
```
program exClass (input, output, error);
use leSolver;
import * from io;
var solver : leSolver;
begin
    solver.m := 10.0;
    solver.b := 5;
    solver.y := 7;
    solver.solve;
    writeln("x equals to " + string(solver.x));
end.
```
Compile Command:
```
$ mini-pascal exClass.p -l libs class
```
Output:
```
x equals to 0.2
```

## Implementation List
Due to the time limit of the project, the spec of class in Mini-Pascal is not completely implemented. Here is the list of the implementation list of my compiler:

| Name | Status |
| ---- | ------ |
| Class file support | v |
| Use class in main program | v |
| Declare variables with class type | v |
| Array of class variables | |
| Class variable assignment | |
| Class variable as a subprogram argument | v |
| Subprograms return class type | |
| Class member methods | v |
| Class constructor | |
| Class operator overloaded | |
| Class inheritance | &nbsp; |
