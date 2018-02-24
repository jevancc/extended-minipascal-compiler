# Hello World!
This page will take you exploring the world of Mini-Pascal through a simple helloworld example.

## Source Code
Open the `helloworld.p` file in the example directory. You can find out the following code:
```
program helloworld(input, output, error);
import * from io;
begin
    writeln("Hello World!");
end
.
```
The purpose of this code is to display `Hello World!` on your console.

## Compile
Make sure you install the compiler before it.   
To compile your program, execute the following command:
```
$ mini-pascal helloworld.p -o helloworld.jar -l ./libs
```
The option `-o` is to tell the compiler to generate the compiled java jar file into `helloworld.jar`. The option `-l` is to specified the library search path, which is the directory which our module file `io.pm` locates.

## Execution
The compiler will generate a java jar file. Run the java to execute our program:
```
$ java -jar helloworld.jar
```
Now you can see the output:
```
Hello World!
```
