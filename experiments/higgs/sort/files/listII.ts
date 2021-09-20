export module main
{
    interface IIterator
    {
	    current : () => number;
        clone : () => IIterator;
        moveNext : () => boolean;
        movePrev : () => boolean;
        setValue : (x : number) => void;
    }

    interface IList
    {
        getIterator : () => IIterator;
        getSize : () => number;
        add : (val : number) => void;
    }

