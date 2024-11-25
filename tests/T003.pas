program T_003;

// ignoring line comments;

var
    x, y, total: real;

begin
    x := 1.0;
    y := 5.0;
    total := 0.0;

    while x <= y do
    begin
        total := total + x;
        x := x + 1.0;
    end
end.