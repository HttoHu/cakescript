let str = "damn it "
let arr = [1, 2, 3, str, 4];
let i = 0;
arr[0] = 0 - 1;
while (i < length(arr)) {
  print(arr[i]);
  i = i + 1;
}
let arr2 = arr;
arr[0] = [1, 2, [3, "hell"]];
print("---", arr2[0][2])