let str = " hell wolrdwo\n\n";
console.log(str.length);
str.set(1, "g")
console.log(str.trim());
console.log(str.indexOf("wo"));
console.log(str.indexOf("wo", 8));
str=str.trim();
str+="kkk"
console.log(str)

let arr = [];
arr.resize(10);
arr[9] = 1;
console.log(arr);