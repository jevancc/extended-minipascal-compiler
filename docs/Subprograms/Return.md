# Return Statement
In Pascal function, the returned value is required to be stored in a variable with function's name and it will not be returned until the last line of the function. Mini-Pascal provides a C++-like return statement for programmers to end the function and return value of expression immediately .

## Syntax
```
return <expression>; // In Mini-Pascal function
return ;             // In Mini-Pascal procedure
```

## Example
```
program exReturn (input, output, error);
import * from io;
function foo(n: integer): integer;
begin
   if n > 5 then
      return n;
   n := n*2 + 1;
   foo := foo(n);
end;

begin
   writeln(foo(1));
end.
```
Output:
```
7
```
