program exOverloading (input, output, error);
import * from io;
procedure typeof(v : integer);
begin
   writeln("It is a variable of integer");
end;
procedure typeof(v : real);
begin
   writeln("It is a variable of real");
end;

begin
   typeof(1);
   typeof(1.0);
end.
