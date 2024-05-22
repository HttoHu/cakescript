function abs(a) {
  if (a < 0) return -a;
  return a;
}
let N = 8;
let curAns = [], result = [];

for (let j = 0; j < N; ++j)
  curAns.push(0);

for (let i = 0; i < 1000; ++i) {
  console.log("round ", i);
  eightQueen(0);
}

function eightQueen(n) {
  if (n == N) {
    let newArry = [];
    for (let i = 0; i < N; ++i)
      newArry.push(curAns[i]);
    result.push(newArry)
    return
  }
  for (let i = 0; i < N; ++i) {
    curAns[n] = i;
    if (judge(n)) {
      eightQueen(n + 1)
    }
  }
}

function judge(n) {
  for (let i = 0; i < n; ++i) {
    if (curAns[i] == curAns[n] ||
      abs(n - i) == abs(curAns[n] - curAns[i])) {
      return 0
    }
  }
  return 1
}