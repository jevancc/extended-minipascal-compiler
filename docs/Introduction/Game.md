# Simple Guessing Game
Following is a simple number guessing game example. The source can be found in the example directory with filename `guessGame.p`.

```
PROGRAM game (input, output, error);
import io;
import rand_int from util;
var max, min, ans, guess : integer;

BEGIN
    io.writeln("Welcome to number guess game");
    io.write("Please input the maximum number:");
    max := io.scan_integer;
    min := 0;
    ans := rand_int(max) + 1;
    max += 1;

    while guess != ans do begin
        min := guess < ans and ans-min > ans-guess ? guess : min;
        max := guess > ans and max-ans > guess-ans ? guess : max;

        io.writeln("Greater than "+min+", less than "+max+":");
        guess := io.scan_integer;
        if guess <= min or guess >= max then begin
            io.write("Invallid input number, not in the range: ");
            io.writeln(guess <= min ? "too small" : "too large");
        end;
    end;

    io.writeln("Congratulations, the answer is "+guess);
END.
```
