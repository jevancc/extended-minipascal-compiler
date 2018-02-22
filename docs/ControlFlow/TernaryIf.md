# Ternary If
The conditional (ternary) operator is the operator that takes three operands. This operator is frequently used as a shortcut for the if-then-else. However, ternary if can be used in an expression and it can be used to select the value that operated in an expression.

## Syntax
```
<condition> ? <E1> : <E2>
```
Where condition is a relational expression and E1, E2 are expressions. In the ternary if, when the test condition is true, the expression E1 is executed and E2 is skipped; when the test condition is false, then E1 is bypassed and E2 is executed.

## Type Check
For the type checking during the compile time, the value types of the E1 and E2 should be the same.

## Example
```
program exTernaryIf (input, output, error);
import * from io;
var a : integer;
begin
   a := 100;
   writeln(a < 20 ? "a is less than 20" : "value of a is " + string(a))
end.
```
Output:
```
value of a is 100
```
