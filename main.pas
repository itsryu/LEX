program Exemplo;

var 
   x, y: integer; 
   z: real;
   s: string;

begin
   x := 10;
   y := 20;
   z := x - y * 2.5;
   s := 'teste';

   if x > y then
      x := x - 1;
   else
      y := y + 1;

   while z <= 100 do

   begin
      z := z * 1.5;
      x := x + 2;
   end
end.