# JFunction
JFunction is subprograms that programmed in Jasmin instructions, which are similar to Java VM instructions.

## Define a JFunction
In Mini-Pascal, a JFunction is defined using the JFunction keyword. The general form of a JFunction definition is as follows:
```
JFunction name(argument_type1, argument_type2, ... ) : return_type;
begin`
   < JFunction body >
`end;
```
JFunction can also be defined without return value:
```
JFunction name(argument_type1, argument_type2, ... );
begin`
   < JFunction body >
`end;
```
A JFunction definition in Mini-Pascal consists of a header and a body. The JFunction header consists of the keyword jfunction and a name given to the JFunction. Here are all the parts of a JFunction:

* Arguments − The argument(s) establish the linkage between the calling program and the procedure identifiers and also called the formal parameters. Unlike `function` and `procedure`, the arguments are defined without names but only types in the argument list. The arguments can be accessed through JVM register 0 to n-1, where n is the number of arguments.
* JFunction Body − The JFunction body contains a collection of instructions that define what the JFunction does. It should always be enclosed between the reserved words ```begin` ``` and ``` `end```. It is the part of a procedure where all computations are done.

When compiling JFunction, the complier will not check the instructions in the JFunction body but just copy-and-paste. Hence, the error of JFunction can only be detected during the runtime. However, compiler will check the arguments when doing the function call, and the function overloading can be applied on JFunctions.

## Example
Following is the source code for a JFunction called `writeln`. This JFunction is defined in module `IO` and called the `System.out.Println` in Java to print the variable to stdout. This JFunction takes 1 parameters with `string` type and does not return any value.
```
JFUNCTION writeln(string);
BEGIN`
.limit locals 2
.limit stack 2
getstatic java/lang/System/out Ljava/io/PrintStream;
aload 0
invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
return
`END;
```

## Reference
The programmer should be familiar with the Jamin instructions and the workflow of Java Virtual Machine to write JFunctions. For more details, please refer to the documents of Jasmin
* [Jasmin User Guide](http://jasmin.sourceforge.net/guide.html)
* [Jasmin Instructions](http://jasmin.sourceforge.net/instructions.html)
