# Repeat-Until Loop
Unlike for and while loops, which test the loop condition at the top of the loop, the repeat-until loop in Mini-Pascal checks its condition at the bottom of the loop.

A repeat-until loop is similar to a while loop, except that a repeat-until loop is guaranteed to execute at least one time.

## Syntax
```
repeat
   S1;
   S2;
   ...
   ...
   Sn;
until condition;
```

For example:
```
repeat
   sum := sum + number;
   number := number - 2;
until number = 0;
```
Notice that the conditional expression appears at the end of the loop, so the statement(s) in the loop execute once before the condition is tested.

If the condition is false, the flow of control jumps back up to repeat, and the statement(s) in the loop execute again. This process repeats until the given condition becomes true.

## Flow diagram
// keep

## Example
```
program exRepeat (input, output, error);
import * from io;
var a: integer;
begin
   a := 10;
   repeat begin
      writeln("value of a: " + string(a));
      a := a + 1
   end until a = 20;
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
* [Pascal - Repeat-Until Loop, tutorialspoint](https://www.tutorialspoint.com/pascal/pascal_repeat_until_loop.htm)
