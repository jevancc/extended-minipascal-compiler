# If-Then-Else Statement
An if-then statement can be followed by an optional else statement, which executes when the Boolean expression is false.

## Syntax
```
if <condition> then <S1> else <S2>
```
Where condition is a relational expression and S1, S2 are simple or compound statements. In the if-then-else statements, when the test condition is true, the statement S1 is executed and S2 is skipped; when the test condition is false, then S1 is bypassed and statement S2 is executed.

## Flow Diagram
// keep

## Example
```
program exIfThenElse (input, output, error);
import * from io;
var a : integer;
begin
   a := 100;
   if a < 20 then
      writeln("a is less than 20")
   else
      writeln("value of a is " + string(a));
end.
```
Output:
```
value of a is 100
```
