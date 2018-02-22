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
