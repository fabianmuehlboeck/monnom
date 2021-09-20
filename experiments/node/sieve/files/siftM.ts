
function Sift(n : number, s : Stream) : Stream {
  var fst : number = s.first;
  if(fst % n == 0) {
	return Sift(n, s.rest());
  }
  return new Stream(fst, function () : Stream { return Sift(n, s.rest()); });
}
