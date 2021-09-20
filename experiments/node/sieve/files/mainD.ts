
function getPrimes() : Stream {
  return Sieve(CountFrom(2));
}

export function main() {
  var nminusone : number = 9999;
  var result : number = stream_get(getPrimes(),nminusone);
}

export const runs = 1;
