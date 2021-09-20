Higgs
=====

[![Build Status](https://travis-ci.org/higgsjs/Higgs.png?branch=master)](https://travis-ci.org/higgsjs/Higgs)

A JIT compiler for JavaScript targetting x86-64 platforms.

**Requirements:**

- D compiler (latest [DMD](http://dlang.org/download.html) recommended)
- POSIX compliant OS (Linux, Unix, MacOS X)
- Python 2.7 (if regenerating object layouts)
- x86 64-bit CPU
- 2 GB of RAM
- GNU make
- GNU time

**Quickstart:**

*Get the source:*

`git clone https://github.com/higgsjs/Higgs.git && cd Higgs/source`

*Compile a binary:*

NOTE: if you run a non-Linux OS like FreeBSD you may not have `GNU make` installed. You may need to install the "gmake" package or otherwise acquire `GNU make`.

NOTE: if your default python version is 3.x, just append `PYTHON=$PATH_TO_PYTHON2` to the `make` commands

`make all`
generates a binary `higgs` in the source directory.

*Compile a release binary:*

`make release`
generates a binary `higgs` in the source directoy.

*Install (optional):*

`sudo make install`
generates a release binary using the `release` target, then copies the `higgs` binary to `/usr/bin` and the runtime files to `/etc/higgs`. The installation directories for the binary and runtime files may be changed with `BIN_DIR` and `LIB_DIR` respectively:
```sh
make install BIN_DIR=/my/bin/dir LIB_DIR=/my/lib/dir
```

`sudo make install-dev`
is essentially the same as `make install`, but instead of copying the higgs binary and runtime files to your install directories, it creates symbolic links to your source folder.

*Cleanup:*

`make clean`
will remove any binaries in the source directory.

*You may wish to run the unit tests:*

`make test`
generates a binary `test-higgs` and tests its proper functioning.

For further info, see the [makefile](https://github.com/higgsjs/Higgs/blob/master/source/makefile).

**Usage:**

`higgs` will start Higgs and give you a REPL (read-eval-print loop).

NOTE: if you did not run `make install`, then you must run higgs directly from the source directory, as otherwise it will not know where to find its runtime files.

To execute one or more files, pass them to `higgs`:

`higgs file1.js file2.js`

The `--e` option accepts a code string to execute:

`higgs --e "var x = 4; x = x + 5; print(x)"`

The `--repl` option will start a REPL after evaluating a string and/or files:

`higgs --repl file1.js` will evaluate `file1.js` and then start a REPL.

`higgs file1.js` will evaluate `file1.js` and then exit.

The `--dumpasm` option will dump the assembler code generated by the JIT to the console.

Command-line arguments can be passed to a JS script using the `--` separator, as follows:

`higgs file1.js file2.js -- 0 1 2`

These arguments will be evaluated as JS code in the global scope and the resulting values inserted in a global `arguments` array.

**Notes:**
 - You may wish to use `rlwrap` for a better REPL experience.
 - You will need to install `libx11-dev` to use the [draw](https://github.com/higgsjs/Higgs/blob/master/source/lib/draw.js) library, or to run the [example programs](https://github.com/higgsjs/Higgs/blob/master/examples).

More
=====

Documentation for Higgs and included libraries can be found in the [Higgs Wiki](https://github.com/higgsjs/Higgs/wiki).

You can follow the development of Higgs on [Maxime's blog](http://pointersgonewild.com/category/higgs/).