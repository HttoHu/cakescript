function test(a, b) {
  let sum = 0;
  while (a < b) {
    sum = sum + a;
    a = a + 1;
  }
  return sum;
}

test(1, 100)

function fib(x) {
  if (x <= 2)
    return 1;
  return fib(x - 1) + fib(x - 2);
}
fib(8)+test(1,10)
