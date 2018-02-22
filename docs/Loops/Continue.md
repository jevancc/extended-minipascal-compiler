# Continue
The continue statement in Mini-Pascal works somewhat like the break statement. Instead of forcing termination, however, continue forces the next iteration of the loop to take place, skipping any code in between.

For the for-do loop, continue statement causes the conditional test and increment portions of the loop to execute. For the while-do and repeat...until loops, continue statement causes the program control to pass to the conditional tests.

## Flow diagram
// keep

## Example
```
program exContinue(input, output, error);
import * from io;
var a: integer;
begin
   a := 10;

   repeat begin
      if a = 15 then begin
         a := a + 1;
         continue;
      end;

      writeln("value of a: " + string(a));
      a := a + 1;
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
value of a: 16
value of a: 17
value of a: 18
value of a: 19
```

## Reference
* [Pascal - Continue Statement, tutorialspoint](https://www.tutorialspoint.com/pascal/pascal_continue_statement.htm)
