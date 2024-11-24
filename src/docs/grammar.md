$$
\begin{align}
    [\text{Programa}] &\to \text{program} \ [\text{Identificador}] \ ; \ [\text{Bloco}] \ . \\
    \\
    [\text{Bloco}] &\to [\text{ParteDeclVar}] \ [\text{ComandoComposto}] \\
    \\
    [\text{ParteDeclVar}] &\to \{\ \text{var} \ [\text{DeclVar}] \ \{\ ; \ [\text{DeclVar}] \} \ ; \} \\
    \\
    [\text{DeclVar}] &\to [\text{ListaIdentificadores}] \ : \ [\text{Tipo}] \\
    \\
    [\text{ListaIdentificadores}] &\to [\text{Identificador}] \ \{\ , \ [\text{Identificador}] \} \\\
    \\
    [\text{Tipo}] &\to \text{integer} \ | \ \text{real} \ | \ \text{sfd} \\
    \\
    [\text{ComandoComposto}] &\to \text{begin} \ [\text{Comando}] \ ; \ \{\ [\text{Comando}] \ ; \} \ \text{end} \\
    \\
    [\text{Comando}] &\to
    \begin{cases}
        [\text{Atribuicao}] \\
        [\text{ComandoComposto}] \\
        [\text{ComandoCondicional}] \\
        [\text{ComandoRepetitivo}]
    \end{cases} \\
    \\
    [\text{Atribuicao}] &\to [\text{Variavel}] \ := \ [\text{Expressao}] \\
    \\
    [\text{ComandoCondicional}] &\to \text{if} \ [\text{Expressao}] \ \text{then} \ [\text{Comando}] \ [\text{ElseOpcional}] \\
    \\
    [\text{ElseOpcional}] &\to
    \begin{cases}
        \text{else} \ [\text{Comando}] \\
        \epsilon
    \end{cases} \\
    \\
    [\text{ComandoRepetitivo}] &\to \text{while} \ [\text{Expressao}] \ \text{do} \ [\text{Comando}] \\
    \\
    [\text{Expressao}] &\to [\text{ExpressaoSimples}] \ [\text{RelacaoOpcional}] \\
    \\
    [\text{RelacaoOpcional}] &\to
    \begin{cases}
        [\text{Relacao}] \ [\text{ExpressaoSimples}] \\
        \epsilon
    \end{cases} \\
    \\
    [\text{Relacao}] &\to = \ | \ <> \ | \ < \ | \ <= \ | \ >= \ | \ > \\
    \\
    [\text{ExpressaoSimples}] &\to [\text{SinalOpcional}] \ [\text{Termo}] \ \{\ [\text{OperadorSoma}] \ [\text{Termo}] \} \\
    \\
    [\text{SinalOpcional}] &\to + \ | \ - \ | \ \epsilon \\
    \\
    [\text{OperadorSoma}] &\to + \ | \ - \\ 
    \\
    [\text{Termo}] &\to [\text{Fator}] \ \{\ [\text{OperadorMultiplicacao}] \ [\text{Fator}] \} \\
    \\
    [\text{OperadorMultiplicacao}] &\to * \ | \ / \\
    \\
    [\text{Fator}] &\to
    \begin{cases}
        [\text{Variavel}] \\
        [\text{Numero}] \\
        ( \ [\text{Expressao}] \ )
    \end{cases} \\
    \\
    [\text{Variavel}] &\to [\text{Identificador}] \\
    \\
    [\text{Identificador}] &\to \text{ident} \\
    \\
    [\text{Numero}] &\to \text{int\_lit} \ | \ \text{real\_lit}
\end{align}
$$
