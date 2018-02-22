# While-Do Loop
A while-do loop statement in Mini-Pascal allows repetitive computations till some test condition is satisfied. In other words, it repeatedly executes a target statement as long as a given condition is true.

## Syntax
```
while (condition) do S;
```

## Control Flow
// keep

## Example
```
program exWhile (input, output, error);
import * from io;
var a: integer;
begin
   a := 10;
   while  a < 20  do

   begin
      writeln("value of a: " + string(a));
      a := a + 1;
   end;
end.
```
Output:
```
value of a: 10
value of a: 11
value of a: 12
value of a: 13
value of a: 14
value of a: 15
value of a: 16
value of a: 17
value of a: 18
value of a: 19
```

## Reference
* [Pascal - While-do Loop, tutorialspoint](https://www.tutorialspoint.com/pascal/pascal_while_do_loop.htm)
