
function Sieve(s : any) : any {
  var fst : any = s.first;
  return new Stream(fst, function () : Stream { return Sieve(Sift(fst, s.rest())); });
}
