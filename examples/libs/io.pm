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

JFUNCTION writeln(integer);
BEGIN`
.limit locals 2
.limit stack 2
getstatic java/lang/System/out Ljava/io/PrintStream;
iload 0
invokestatic java/lang/String/valueOf(I)Ljava/lang/String;
invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
return
`END;

JFUNCTION writeln(real);
BEGIN`
.limit locals 2
.limit stack 2
getstatic java/lang/System/out Ljava/io/PrintStream;
fload 0
invokestatic java/lang/String/valueOf(F)Ljava/lang/String;
invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
return
`END;

JFUNCTION writeln(string);
BEGIN`
.limit locals 2
.limit stack 2
getstatic java/lang/System/out Ljava/io/PrintStream;
aload 0
invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
return
`END;

JFUNCTION scan_integer(): integer;
BEGIN`
.limit locals 2
.limit stack 3
new           java/util/Scanner
dup
getstatic java/lang/System.in Ljava/io/InputStream;
invokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V
invokevirtual java/util/Scanner/nextInt()I
ireturn
`END;

JFUNCTION scan_real(): real;
BEGIN`
.limit locals 2
.limit stack 3
new           java/util/Scanner
dup
getstatic java/lang/System.in Ljava/io/InputStream;
invokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V
invokevirtual java/util/Scanner/nextDouble()D
d2f
freturn
`END;
.
