# Nesting and Labels
It's possible to `break` or `continue` outer loops when dealing with nested loops. In these cases, the loops must be annotated with a label, and the label must be passed to the break/continue statement. A valid label is an identifier with prefix `'`, and there should not exist any conflictions of labels in the same scope.

## Token
```
label ::= '[a-zA-Z_][a-zA-Z0-9_]*
```

## Grammar
```
// labeled while
'label : while (condition) do S;
// labeled for
'label : for <variable-name> := <initial_value> to [down to] <final_value> do S;
// labeled repeat
'label : repeat S1; until condition;
```
Break and Continue with label:
```
break 'label;
continue 'label;
```

## Example
```
program exLabel (input, output, error);
import * from io;
var a: integer;
begin
   a := 1;
   'outer : repeat begin
      while a < 10 do begin
         writeln("value of a: " + string(a));
         a := a + 1;
         break 'outer;
      end;
   end until a > 20;
end.
```
Output:
```
value of a: 1
```
