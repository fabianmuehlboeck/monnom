
class Stream {
   first : any;
   rest : any;
   constructor(f : any, r : any) {
     this.first = f;
     this.rest = r;
   }
};

function stream_get(s : any, n : any) : any {
  while(n>0) {
    n=n-1;
    s=s.rest();
  }
  return s.first;
}
}
