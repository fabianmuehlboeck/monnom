/// <reference types="node" />
if (process.argv.length < 3) {
    process.exit(1);
}
var directoryName = process.argv[2];
var fs = require("fs");
// main reason why we need separate process to check if it is safe to watch some path
// is to guard against crashes that cannot be intercepted with protected blocks and
// code in tsserver already can handle normal cases, like non-existing folders.
// This means that here we treat any result (success or exception) from fs.watch as success since it does not tear down the process.
// The only case that should be considered as failure - when watchGuard process crashes.
try {
    var watcher = fs.watch(directoryName, { recursive: true }, function () { return ({}); });
    watcher.close();
}
catch (_e) {
}
process.exit(0);
//# sourceMappingURL=file:////home/user/vm-gradual-typing/typescript-contracts/built/local/watchGuard.js.map