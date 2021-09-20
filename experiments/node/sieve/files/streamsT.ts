
class Stream {
   first : number;
   rest : () => Stream;
   constructor(f : number, r : () => Stream) {
     this.first = f;
     this.rest = r;
   }
};

function stream_get(s : Stream, n : number) : number {
  while(n>0) {
    n=n-1;
    s=s.rest();
  }
  return s.first;
}
}
