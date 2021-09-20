export module main {
function CountFrom(n : number) : Stream
{
  return new Stream(n, function() : Stream { return CountFrom(n+1); });
}
