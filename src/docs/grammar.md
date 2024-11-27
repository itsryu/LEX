$$
\begin{align}
    [\text{Program}] &\to \text{program} \ [\text{Identifier}] \ ; \ [\text{Block}] \ . \\
    \\
    [\text{Block}] &\to [\text{OptionalVarDeclPart}] \ [\text{CompoundCommand}] \\
    \\
    [\text{OptionalVarDeclPart}] &\to \epsilon \ | \ [\text{VarDeclPart}] \\
    \\
    [\text{VarDeclPart}] &\to \text{var} \ [\text{VarDecl}] \ \{\ ; \ [\text{VarDecl}] \} \ ; \\
    \\
    [\text{VarDecl}] &\to [\text{IdentifierList}] \ : \ [\text{Type}] \\
    \\
    [\text{IdentifierList}] &\to [\text{Identifier}] \ \{\ , \ [\text{Identifier}] \} \\
    \\
    [\text{Type}] &\to \text{integer} \ | \ \text{real} \ | \ \text{sfd} \\
    \\
    [\text{CompoundCommand}] &\to \text{begin} \ [\text{OptionalVarDeclPart}] \ [\text{Command}] \ ; \ \{\ [\text{Command}] \ ; \} \ \text{end} \\
    \\
    [\text{Command}] &\to
    \begin{cases}
        [\text{Assignment}] \\
        [\text{CompoundCommand}] \\
        [\text{ConditionalCommand}] \\
        [\text{RepetitiveCommand}]
    \end{cases} \\
    \\
    [\text{Assignment}] &\to [\text{Variable}] \ := \ [\text{Expression}] \\
    \\
    [\text{ConditionalCommand}] &\to \text{if} \ [\text{Expression}] \ \text{then} \ [\text{Command}] \ [\text{OptionalElse}] \\
    \\
    [\text{OptionalElse}] &\to
    \begin{cases}
        \text{else} \ [\text{Command}] \\
        \epsilon
    \end{cases} \\
    \\
    [\text{RepetitiveCommand}] &\to \text{while} \ [\text{Expression}] \ \text{do} \ [\text{Command}] \\
    \\
    [\text{Expression}] &\to [\text{SimpleExpression}] \ [\text{OptionalRelation}] \\
    \\
    [\text{OptionalRelation}] &\to
    \begin{cases}
        [\text{Relation}] \ [\text{SimpleExpression}] \\
        \epsilon
    \end{cases} \\
    \\
    [\text{Relation}] &\to = \ | \ <> \ | \ < \ | \ <= \ | \ >= \ | \ > \\
    \\
    [\text{SimpleExpression}] &\to [\text{OptionalSign}] \ [\text{Term}] \ \{\ [\text{AddOperator}] \ [\text{Term}] \} \\
    \\
    [\text{OptionalSign}] &\to + \ | \ - \ | \ \epsilon \\
    \\
    [\text{AddOperator}] &\to + \ | \ - \\
    \\
    [\text{Term}] &\to [\text{Factor}] \ \{\ [\text{MultiplicationOperator}] \ [\text{Factor}] \} \\
    \\
    [\text{MultiplicationOperator}] &\to * \ | \ / \\
    \\
    [\text{Factor}] &\to
    \begin{cases}
        [\text{Variable}] \\
        [\text{Number}] \\
        ( \ [\text{Expression}] \ )
    \end{cases} \\
    \\
    [\text{Variable}] &\to [\text{Identifier}] \\
    \\
    [\text{Identifier}] &\to \text{ident} \\
    \\
    [\text{Number}] &\to \text{int\_lit} \ | \ \text{real\_lit}
\end{align}
$$