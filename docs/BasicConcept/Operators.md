# Operators
An operator is a symbol that tells the compiler to perform specific mathematical or logical manipulations. Mini-Pascal allows the following types of operators −
* Arithmetic operators
* Relational operators
* Boolean operators

## Arithmetic Operators
Following table shows all the arithmetic operators supported by Mini-Pascal.

| Operator | Description | Valid Operand Type |
| -------  | ----------  | ------------------ |
| + | Adds two operands. If the operands are strings, concatenate the operands. | `integer`, `real`, `string` |
| - | Subtracts second operand from the first | `integer`, `real`|
| * | Multiplies both operands | `integer`, `real`|
| / | Divides numerator by denominator | `integer`, `real`|

## Arithmetic Assignment Operators
Following table shows all the arithmetic operators supported by Mini-Pascal.

| Operator | Description | Valid Operand Type |
| -------  | ----------  | ------------------ |
| += | Adds two operands. If the operands are strings, concat the operands. After the operation, store the value to the left-hand-side variable. | `integer`, `real`, `string` |
| -= | Subtracts second operand from the first. After the operation, store the value to the left-hand-side variable. | `integer`, `real`|
| *= | Multiplies both operands. After the operation, store the value to the left-hand-side variable. | `integer`, `real`|
| /= | Divides numerator by denominator. After the operation, store the value to the left-hand-side variable. | `integer`, `real`|

## Relational Operators
See [Relational Expression](/BasicConcept/RelationalExpression.md#relational-operators)

## Boolean Operators
See [Relational Expression](/BasicConcept/RelationalExpression.md#boolean-operators)

## Operator Precedence
Operator precedence determines the grouping of terms in expression. This affects how an expression is evaluated.   
The following table lists the precedence and associativity of Mini-Pascal  operators.

| Precedence | Operator |
| ---------- | -------- |
| 1 | +, - (Sign) |
| 2 | *, / |
| 3 | +, - |
| 4 | =, <>, <, <=, >, >= |
| 5 | not |
| 6 | or |
| 7 | and |
| 8 | ?: (ternary if) |
| 9 | :=, +=, -=, *=, /= |

## Reference
* [Pascal - Operators](https://www.tutorialspoint.com/pascal/pascal_operators.htm)
