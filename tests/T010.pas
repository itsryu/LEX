program T_010;

var a, b, c: integer;

begin
    a := 10;
    b := 20;
    c := (a + b) * (a - b) / (a + 1) + (b * 2 - a / 2) * (a + 3)); // Syntax error: Unexpected token ')' at 8:63
end.