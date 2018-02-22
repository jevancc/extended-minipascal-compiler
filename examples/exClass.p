program exClass (input, output, error);
use leSolver;
import * from io;
var solver : leSolver;
begin
    solver.m := 10.0;
    solver.b := 5;
    solver.y := 7;
    solver.solve;
    writeln("x equals to " + string(solver.x));
end.
