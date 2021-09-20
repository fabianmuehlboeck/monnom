export module main {
function CountFrom(n : any) : any
{
  return new Stream(n, function() : any { return CountFrom(n+1); });
}
