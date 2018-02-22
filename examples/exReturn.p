program exReturn (input, output, error);
import * from io;
function foo(n: integer): integer;
begin
   if n > 5 then
      return n;
   n := n*2 + 1;
   foo := foo(n);
end;

begin
   writeln(foo(1));
end.
