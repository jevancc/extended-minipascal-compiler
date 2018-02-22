# Overloading
Mini-Pascal allows you to specify more than one definition for a function name in the same scope, which is called function overloading.

An overloaded declaration is a declaration that is declared with the same name as a previously declared declaration in the same scope, except that both declarations have different arguments and obviously different definition (implementation).

You can have multiple definitions for the same function name in the same scope. The definition of the function must differ from each other by the types and/or the number of arguments in the argument list. You cannot overload function declarations that differ only by return type.

Unlike overloading in C++ and other programming languages, overloading in Mini-Pascal do not do overload resolution, which means that number and types of all the arguments passed to function should perfectly match the argument list of any function definition.

## Example
```
program exOverloading (input, output, error);
import * from io;
procedure typeof(v : integer);
begin
   writeln("It is a variable of integer");
end;
procedure typeof(v : real);
begin
   writeln("It is a variable of real");
end;

begin
   typeof(1);
   typeof(1.0);
end.
```
Output:
```
It is a variable of integer
It is a variable of real
```
