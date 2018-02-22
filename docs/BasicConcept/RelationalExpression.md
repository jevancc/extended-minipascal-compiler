# Relational Expression
In Mini-Pascal, there is no Boolean value type. For the condition evaluation in decision making, Mini-Pascal introduces the relational expression. The relational expression is similar to the normal expression; it is composed of operands and relational operators and boolean operators. However, the value of a relational expression cannot be stored in a variable. It is used in the condition evaluation in loops and if statement.

## Relational Operators
Following table shows all the relational operators supported by Mini-Pascal.

| Operator | Description | Valid Operand Type |
| -------- | ----------- | --------|
| = | Checks if the values of two operands are equal or not, if yes, then condition becomes true. | `Integer`, `Real` |
| > | Checks if the value of left operand is greater than the value of right operand, if yes, then condition becomes true. | `Integer`, `Real` |
| < | Checks if the value of left operand is less than the value of right operand, if yes, then condition becomes true. | `Integer`, `Real` |
| >= | Checks if the value of left operand is greater than or equal to the value of right operand, if yes, then condition becomes true. | `Integer`, `Real` |
| <= | Checks if the value of left operand is less than or equal to the value of right operand, if yes, then condition becomes true. | `Integer`, `Real` |

## Boolean Operators
Following table shows all the Boolean operators supported by the Mini-Pascal language. All these operators work on relational expression operands and produce relational expression.

| Operator | Description |
| -------- | ------------|
| and | Called Boolean AND operator. If both the operands are true, then condition becomes true. |
| or | Called Boolean OR Operator. If any of the two operands is true, then condition becomes true. |
| not | Called Boolean NOT Operator. Used to reverse the logical state of its operand. If a condition is true, then Logical NOT operator will make it false. |

## Reference
* [Pascal - Operators, tutorialspoint](https://www.tutorialspoint.com/pascal/pascal_operators.htm)
