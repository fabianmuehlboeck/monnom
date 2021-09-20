export module main {
function CountFrom(n : any) : any
{
  return new Stream(n, function() : Stream { return CountFrom(n+1); });
}
