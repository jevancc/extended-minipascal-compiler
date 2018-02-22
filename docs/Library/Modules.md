# Modules
As your program gets longer, you may want to split it into several files for easier maintenance. These files may contain definitions of different functional subprograms. You may also want to use these handy subprograms that you’ve written in several programs without copying its definition into each program.

To support this, Mini-Pascal has a way to put subprogram definitions in a file and use them in the main program. Such a file is called a module; definitions from a module can be imported into other the main Mini-Pascal program.

A module is a file containing Mini-Pascal function definitions and statements. The file name is the module name with the suffix `.pm` appended. Within a module, the module’s name should be the same as the file name.

## Syntax
To write a module, you need to create a new file with suffix `.pm`. The syntax of module file is as follows.
```
MODULE <module_name>;
<subprogram_definitions>
.
```
Notice that the `<module_name>` should be equal to the file name. A valid subprogram can be a `function`, `procedure`, or `jfunction`.

## Import Module in Main Program
In your Mini-Pascal main program, the syntax for you to import the module is as follows:
```
import <module_name>;
```
It will then import the module file from the search path.   
After import the module, you can access the subprograms in the following way
```
<module_name>.<subprogram_name>
```
To avoid function name confliction, you may want to import part of the subprogram definitions from a module. In that case, you can import subprograms you want in the following way:
```
import <subprogram_name1>, <subprogram_name2>, ... from <module_name>;
```
The compiler will import the subprograms you list in the import statement to the program scope, and all the overloaded subprograms with the matched subprogram name will be imported. Hence, the subprograms import in this way should be access without module name specified:
```
<subprogram_name>
```
Sometimes, you may import all the subprograms in the module but do not want to access the function with the module name. In that case, you can import the module in following way:
```
import * from <module_name>;
```

## The Module Search Path
When a module is imported in the main program, the compiler searches for a file named `<module_name>.pm` in a list of directories given with the compiler optional argument `--library,-l`. It will search the directories in the order.

> Note: On file systems which support symlinks, the directories containing the symlink or the module symlink is not added to the module search path.

## Example
The example is a simple version of `example/libs/io.pm`   
```
MODULE io;

JFUNCTION write(integer);
BEGIN`
.limit locals 2
.limit stack 2
getstatic java/lang/System/out Ljava/io/PrintStream;
iload 0
invokestatic java/lang/String/valueOf(I)Ljava/lang/String;
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
return
`END;

JFUNCTION write(real);
BEGIN`
.limit locals 2
.limit stack 2
getstatic java/lang/System/out Ljava/io/PrintStream;
fload 0
invokestatic java/lang/String/valueOf(F)Ljava/lang/String;
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
return
`END;

JFUNCTION write(string);
BEGIN`
.limit locals 2
.limit stack 2
getstatic java/lang/System/out Ljava/io/PrintStream;
aload 0
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
return
`END;
.
```
