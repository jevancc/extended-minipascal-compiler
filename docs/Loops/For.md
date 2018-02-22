# For-Do Loop
A for-do loop is a repetition control structure that allows you to efficiently write a loop that needs to execute a specific number of times.

## Syntax
The syntax for the for-do loop in Mini-Pascal is as follows:
```
for <variable-name> := <initial_value> to [down to] <final_value> do
   S;
```
Where, the variable specifies a variable of `integer` type, called control variable or index variable; `conditional_expression` and `simple_expression` values are values that the control variable can take as initial value and final value. The value type of `initial_value` must be `integer`; and `S` is the body of the for-do loop that could be a simple statement or a group of statements.

For example:
```
for i:= 1 to 10 do writeln(i);
```
Here is the flow of control in a for-do loop:
* The initial step is executed first, and only once. This step allows you to initialize any loop control variables. **Notice that the variable is required to be declared before**
* Next, the condition is evaluated. If it is true, the body of the loop is executed. If it is false, the body of the loop does not execute and flow of control jumps to the next statement just after the for-do loop.
* After the body of the for-do loop executes, the value of the variable is either increased or decreased, depends on the statement in for-do loop is `to` or `down to`.
* The condition is now evaluated again. If it is true, the loop executes and the process repeats itself (body of loop, then increment step, and then again condition). After the condition becomes false, the for-do loop terminates.

## Flow diagram
// keep

## Example
```
program exFor (input, output, error);
import * from io;
var a: integer;
begin
   for a := 10 to 20 do
   begin
      writeln("value of a: " + string(a));
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
value of a: 20
```

## Reference
* [Pascal - For-do Loop, tutorialspoint](https://www.tutorialspoint.com/pascal/pascal_for_do_loop.htm)
