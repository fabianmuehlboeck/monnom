// TEST START
var console : Console;
if (typeof console === "undefined")
    console = /* contract */ <any> {log: (x) => { (<any> print)(x); }};

// Simple interfaces with subtyping
interface SimpleList {
    next: SimpleList;
    val: number;
}

interface ColorfulList extends SimpleList {
    color: string;
}

function useless1(/* contract */ l: ColorfulList) {}

function sum(/* contract */ l: SimpleList): number {
    if (l.next)
        return /* contract */ sum(l.next) + l.val;
    else
        return l.val;
}

var sl: SimpleList = {
    "next": {
        "next": null,
        "val": 2
    },
    "val": 1
};
console.log(/* contract */ sum(sl));


// Classes with functions, constructors and this-typing
class ClassistList {
    constructor(public /* contract */ next: ClassistList, public /* contract */ val: number) {
        /* contract on 'this' */
    }
    sum(): number {
        /* contract on 'this' */
        if (this.next)
            return this.next.sum() + this.val;
        else
            return this.val;
    }
}

console.log(new /* contract */ ClassistList(new /* contract */ ClassistList(null, 2), 1).sum());


// Union and intersection types
interface A {
    a: number,
    b: number
}

interface B {
    b: string,
    c: string
}

interface C {
    c: boolean,
    d: boolean
}

function hm(/* contract */ x: A|B, /* contract */ y: A&C) {
    console.log(x);
    console.log(x.b);
    console.log(y);
    console.log(y.b);
    console.log(y.c);
}
hm({a: 42, b: 3}, {a: 42, b: 3, c: true, d: false});


// Generic types
interface GenericList<T> {
    next: GenericList<T>;
    val: T;
}

function gsum<T>(/* contract */ l: GenericList<T>) {
    if (l.next)
        return gsum<T>(l.next) + l.val;
    else
        return l.val;
}
console.log(gsum(
    <GenericList<string>> /* contract */ {next: {next: null, val: "Hello, "}, val: "world!"}
));
