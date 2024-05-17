let str = [1, 2, 3, 4, 5];
let tmp = "length";
let muffin = {
  a: [1, 2, 3], b: 2
}
let t = 1;
function printMuffin(arr) {
  let i = 0;
  while (i < length(arr)) {
    print(arr[i])
    i = i + 1
  }
}
muffin.a = [4, 5, 6];
muffin.a[1] = 1
print(muffin.a[100])
printMuffin(muffin.a)
muffin.a = str
print(muffin)
t = muffin.b;
t = 34;
print(muffin.b)
muffin.b = { "k": 3, "b": 4, "e": 5 }
muffin.b["k"] = 4;
muffin.b["k1"] = 4;
let k3 = "k3"
muffin.b[k3] = 4;
print(muffin)