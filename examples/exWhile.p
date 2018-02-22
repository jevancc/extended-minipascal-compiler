program exWhile (input, output, error);
import * from io;
var a: integer;
begin
   a := 10;
   while  a < 20  do

   begin
      writeln("value of a: " + string(a));
      a := a + 1;
   end;
end.
