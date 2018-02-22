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
