# Installation
The Mini-Pascal is a JVM based language, the compiler written in C++, with Flex and Bison as the token and parser generator, and Jamin to translate the Jasmin code into java class.     
The source code can be downloaded from the [repository](https://github.com/jevan0307/extended-minipascal-compiler).

## Project Structure
The following is the directory tree of this project:
```
|-- inc/
    |-- codegen.h
    |-- symtab.h
    |-- node.h
    |-- logger.h
|-- src/
    |-- lex.l
    |-- yacc.y
    |-- codegen.cc
    |-- symtab.cc
    |-- symtab_type.cc
    |-- node.cc
    |-- logger.cc
|-- util/
    |-- jasmin-2.4/
|-- example/
|-- snap/
    |-- snapcraft.yaml
|--makefile
```

## Snaps
The mini-pascal compiler is released in [snaps](https://snapcraft.io/). If you are using Linux system, you can install snaps and download the mini-pascal package [here](https://github.com/jevan0307/extended-minipascal-compiler/blob/master/release/mini-pascal_1.0.0_amd64.snap).  
Since the package is released in development mode, use the following command to install it:
```
$ snap install mini-pascal_1.0.0_amd64.snap --devmode --dangerous
```

## Dependencies
The develop environment is Ubuntu 16.04. You can install the following dependencies by executing `apt install` in Debian and Ubuntu.     
```
g++
make
flex
bison
libboost-all-dev
default-jdk
default-jre
```

## Install
Run the following command in the root directory of the project:
```
$ make install
```
Make sure you install all the dependencies before the installation. It will install the Mini-Pascal compiler into `/usr/bin/mini-pascal`.

## Uninstall
Run the following command in the root directory of the project:
```
$ make uninstall
```

## Usage
Run the following command for the help
```
$ mini-pascal --help
```

## Option Description
* `-h, --help`: See the help
* `-v, --version`: See the version of the compiler
* `-o <file>, --output <file>`: The compiler will generate a java jar file. Use this option to specify the file name you want to generate. By default, it is `Main.jar`
* `-l <path1> <path2> ..., --library <path1> <path2> ...`: The search path for the libraries.
* `-b <path>, --build <path>`: The compiler use the Jasmin instruction code as intermediate representation. By default, it will not output the Jasmin instruction files but only java jar file. If you want it to output Jasmin instruction file, use this option to specify the output path.
* `--ast`: If this option is provided, the compiler will display the Abstract Syntax Tree during the compile time.
* `--symtab`: If this option is provided, the compiler will display the Symbol Table during the compile time.
* `--verbose`: If this option is provided, the compiler will display the compile information during the compile time.
