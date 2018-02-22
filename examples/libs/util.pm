MODULE util;

JFUNCTION rand_int(integer):integer;
BEGIN`
.limit locals 100
.limit stack 100
new  java/util/Random
dup
invokespecial java/util/Random/<init>()V
astore_1
aload_1
iload_0
invokevirtual java/util/Random/nextInt(I)I
ireturn
`END;
.
