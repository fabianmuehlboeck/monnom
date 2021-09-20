
function Sieve(s : Stream) : Stream {
  var fst : number = s.first;
  return new Stream(fst, function () : Stream { return Sieve(Sift(fst, s.rest())); });
}
