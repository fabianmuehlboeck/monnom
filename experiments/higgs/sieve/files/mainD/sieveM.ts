
function Sieve(s : any) : any {
  var fst : any = s.first;
  return new Stream(fst, function () : any { return Sieve(Sift(fst, s.rest())); });
}
