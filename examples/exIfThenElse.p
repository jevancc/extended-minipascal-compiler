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
