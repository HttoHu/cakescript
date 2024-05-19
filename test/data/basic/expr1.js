let no = 0;
function my_print(s){
  console.log(s);
  // console.log(++no,":",s)
}

my_print(1 << 2) // # 1
my_print(32 >> 4) // #2 
my_print(1234 ^ 34455) //# 3
let t = 324234;
let a = 2913;
let c = 324234;
my_print(t & a - 4) // #4
my_print(t && a) // #5
my_print(t || a);
t = 0;
my_print(t && a)
my_print(t || a)
my_print(t ^ a + 1)
my_print(5<4 && t==0)

my_print(pop_count(12345));
my_print(pop_count(0));
my_print(pop_count(2222222));
function pop_count(x){
  let i = 0;
  let res = 0 ;
  while(i< 32){
    if(x >> i & 1)
      ++res;
    ++i;
  }
  return res;
}