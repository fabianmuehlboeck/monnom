
function getPrimes() : any {
  return Sieve(CountFrom(2));
}

export function main() {
  var nminusone : any = 2499;
  var result : any = stream_get(getPrimes(),nminusone);
}

export const runs = 1;
