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
