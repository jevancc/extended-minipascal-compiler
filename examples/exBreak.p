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
