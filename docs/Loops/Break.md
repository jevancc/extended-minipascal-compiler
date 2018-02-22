# Break
The break statement in Mini-Pascal has the following two usages:
* When the break statement is encountered inside a loop, the loop is immediately terminated and program control resumes at the next statement following the loop.
* If you are using nested loops (i.e., one loop inside another loop), the break statement will stop the execution of the innermost loop and start executing the next line of code after the block.

## Flow diagram
// keep

## Example
```
program exBreak(input, output, error);
import * from io;
var a: integer;

begin
   a := 10;
   while  a < 20 do
   begin
      writeln("value of a: " + string(a));
      a := a +1;

      if a > 15 then
        break;
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
```

## Reference
* [Pascal - Break Statement, tutorialspoint](https://www.tutorialspoint.com/pascal/pascal_break_statement.htm)
