program T_008;

var x: integer;

begin
    x := 10;

    if (x > 5 then // Syntax error: expected ')' at 8:18
        x := 5;
    else
        x := 0;
    end;
end.