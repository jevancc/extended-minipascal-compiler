# Type Conversions

## Type Casting
Mini-Pascal is designed as a strong type language. Same as C++ and other strong type language, many conversions in Mini-Pascal, specially those that imply a different interpretation of the value, require an explicit conversion. The grammar for type casting is as below:
```
type_cast ::= standard_type ( conditional_expression )
```
The followings are the example of type_casting;
```
a := 1 + integer(2.0) + integer("3"); // a = 6       : integer
b := real(1) + 2.0 + real("3.0");     // b = 6.0     : real
c := "1" + string(2) + string(3.0);   // c = "123.0" : string
```
The functionality of these generic forms of type-casting is enough for most needs with fundamental data types, such as `integer`, `real`, and `string`.
Unlike C++, type-casting cannot be defined as a method and applied to classes.

If the conversion is from `real` to `integer`, the value is truncated (the decimal part is removed).   
If the conversion is from `string` to `real`, the compiled program will call
`Float.parseFloat(String)Float` in Java to parse the string. Similarly, if the conversion is from `string` to `integer`, the program will call `Double.parseDouble(String)Double` in Java to parse the string to double and then convert the double value to int.

When converting the value from `integer` and `real` to `string`, the program will call `String.valueOf(Integer/Float)String` in Java to do the conversion.

## Implicit Type Conversion
Implicit conversions are automatically performed when a value is copied to a compatible type. For example:
```
iv : integer;
rv : real;
sv : string;
begin
    rv := iv + 3.0; // iv is promoted to real
    sv := iv + rv;  // iv is promoted to real, (iv+rv) is promoted to string
end.
```
As what example shows, the value of `iv` in the first statement is promoted from `integer` to `real`, and the value of `iv` in the second example is promoted from `integer` to `real` and the operation result is promoted from `real` to `string`. This is known as a standard conversion. Standard conversions affect fundamental data types and allow the conversions between `integer`, `real`, and `string`.

## Type Precedence
Implicit conversions only happen when converting values from low precedence type to high precedence type. The following is the table of type precedence:

| Type | precedence |
| ---- | -------- |
| Integer | 1   |
| Real    | 2   |
| String  | 3   |
| Array   | NA  |
| Function| NA  |
| Class   | NA  |
| Relational Expression | NA |

If the type of two operand values in operation is different, the implicit type conversion will convert the value with lower type precedence to the type of other operands.    

Implicit conversion also works on assignment statements. However, if the type precedence of left-hand-side is lower than right-hand-side, the implicit conversion will not be performed, and the compiler will throw an error during the type checking.

To avoid ambiguity on function overloading, the implicit type conversions do not work on arguments of function calls. In this case, please use type casting when passing arguments.
