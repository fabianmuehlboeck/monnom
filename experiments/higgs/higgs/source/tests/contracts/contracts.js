function contractTests() {
    $ir_obj_def_const(this, "VectorContract", (function () {
        var c = $ir_contract_for("object");
        var cn = $ir_contract_for("number");
        c = $ir_contract_oblige_member(c, "x", cn);
        $ir_contract_freeze(c);
        return c;
    })(), true);

    $ir_obj_def_const(this, "VectorPairContract", (function () {
        var c = $ir_contract_for("object");
        c = $ir_contract_oblige_member(c, "first", VectorContract);
        c = $ir_contract_oblige_member(c, "second", VectorContract);
        $ir_contract_freeze(c);
        return c;
    })(), true);

    $ir_obj_def_const(this, "VectorArrayContract", (function() {
        var c = $ir_contract_for("object");
        c = $ir_contract_oblige_array(c, VectorContract);
        $ir_contract_freeze(c);
        return c;
    })(), true);

    $ir_obj_def_const(this, "NArrayContract", (function() {
        var c = $ir_contract_for("object");
        var cn = $ir_contract_for("number");
        c = $ir_contract_oblige_array(c, cn);
        $ir_contract_freeze(c);
        return c;
    })(), true);

    $ir_obj_def_const(this, "NArrayArrayContract", (function() {
        var c = $ir_contract_for("object");
        c = $ir_contract_oblige_array(c, NArrayContract);
        $ir_contract_freeze(c);
        return c;
    })(), true);

    $ir_obj_def_const(this, "TreeNodeContract", (function() {
        var c = $ir_contract_for("object");
        var cn = $ir_contract_for("number");
        c = $ir_contract_oblige_member(c, "left", c);
        c = $ir_contract_oblige_member(c, "right", c);
        c = $ir_contract_oblige_member(c, "item", cn);
        $ir_contract_oblige_member(c, "left", c);
        $ir_contract_oblige_member(c, "right", c);
        $ir_contract_freeze(c);
        return c;
    })(), true);

    $ir_obj_def_const(this, "StringFunction", (function() {
        var c = $ir_contract_for("function");
        var cf = $ir_contract_for("string");
        c = $ir_contract_oblige_return(c, cf);
        return c;
    })(), true);

    function TreeNode(left, right, item){
        if (typeof item !== "number") {
            print(typeof item);
            throw new TypeError;
        }
        $rt_addContract(left, TreeNodeContract);
        $rt_addContract(right, TreeNodeContract);

        this.left = left;
        this.right = right;
        this.item = item;
    }

    function runTest(test) {
        test();
    }

    function runTestExpectContractError(test) {
        try {
            test();
            assert(false, "Expected ContractError");
        } catch (err) {
            assert(err.name == "ContractError", "Expected ContractError");
        }
    }

    /*
     * Check that contracts freeze after use
     */

    /* FIXME
    $ir_obj_def_const(this, "UnfrozenContract", (function() {
        var c = $ir_contract_for("object");
        c = $ir_contract_oblige_member(c, "next", c);
        $ir_contract_oblige_member(c, "next", c);
        return c;
    })(), true);

    // This should still work.
    $ir_contract_oblige_member(UnfrozenContract, "next", UnfrozenContract);

    var lst = { next: null };
    $rt_addContract(lst, UnfrozenContract);

    // This should fail
    try {
        $ir_contract_oblige_member(UnfrozenContract, "next", UnfrozenContract);
        assert(false, "Expected error");
    } catch (err) {
        assert(err.name == "ContractFrozenError", "Expected ContractFrozenError");
    }
    */


    /*
     * Primitive contract checks
     */

    runTest(function assignCorrectPrimitive() {
        var v = { x: 10 };
        $rt_addContract(v, VectorContract);
        v.x = 15;
    });

    runTestExpectContractError(function assignWrongPrimitive() {
        var v = { x: 10 };
        $rt_addContract(v, VectorContract);
        v.x = "bla";
    });

    runTestExpectContractError(function assignWrongPrimitive() {
        var v = { x: "bla" };
        $rt_addContract(v, VectorContract);
        v.x = "la";
    });

    runTest(function assignWrongPrimitive() {
        var v = { x: "bla" };
        $rt_addContract(v, VectorContract);
        v.x = 1;
    });

    runTest(function getCorrectPrimitive() {
        var v = { x: 10 };
        $rt_addContract(v, VectorContract);
        return v.x;
    });

    runTestExpectContractError(function getWrongPrimitive() {
        var v = { x: "bla" };
        $rt_addContract(v, VectorContract);
        return v.x;
    });

    /*
     * Higher order contract checks
     */

    runTest(function nestedObjects() {
        var v = { x: 1 };
        $rt_addContract(v, VectorContract);
        var vpair = { first: null, second: null };
        $rt_addContract(vpair, VectorPairContract);

        vpair.first = v;

        return (vpair.first, vpair.second);
    });

    runTest(function treeNodes() {
        var left = new TreeNode(null, null, 1);
        var right = new TreeNode(null, null, 3);
        var middle = new TreeNode(left, right, 2);
        $rt_addContract(middle, TreeNodeContract);

        x = middle.left;
        y = middle.right;

        return (x.item, y.item);
    });

    runTest(function treeNodes() {
        var right = new TreeNode(null, null, 1);
        var middle = new TreeNode(null, right, 2);
        $rt_addContract(middle, TreeNodeContract);

        x = middle.left;
        y = middle.right;

        return y.item;
    });

    runTestExpectContractError(function objectAddingContract() {
        var v = { x: 1 };
        var vpair = { first: null, second: null };
        $rt_addContract(vpair, VectorPairContract);

        // Creates a contract on v
        vpair.first = v;

        v.x = "bla";
    });

    /*
     * Array contract checks
     */

    runTest(function correctNestedArrays() {
        var nested = [[1, 2, 3], [4, 5, 6]];
        $rt_addContract(nested, NArrayArrayContract);
        return nested[1][1];
    });

    runTest(function correctNestedArraysAssign() {
        var nested = [[1, 2, 3], [4, 5, 6]];
        $rt_addContract(nested, NArrayArrayContract);
        nested[1][1] = 10;
    });

    runTest(function correctNestedArraysAssign() {
        var nested = [[1, 2, 3], [4, 5, 6]];
        $rt_addContract(nested, NArrayArrayContract);
        nested[1] = [];
    });

    runTest(function correctNestedArraysAssignWithoutContract() {
        var arr = [9, 8, 7];
        var nested = [[1, 2, 3], [4, 5, 6]];
        $rt_addContract(nested, NArrayArrayContract);
        nested[1] = arr;
    });

    runTest(function correctNestedArraysAssignWithContract() {
        var arr = [9, 8, 7];
        $rt_addContract(arr, NArrayContract);
        var nested = [[1, 2, 3], [4, 5, 6]];
        $rt_addContract(nested, NArrayArrayContract);
        nested[1] = arr;
    });

    runTestExpectContractError(function wrongNestedArrays() {
        var nested = [[1, 2, 3], [4, "bla", 6]];
        $rt_addContract(nested, NArrayArrayContract);
        return nested[1][1];
    });

    runTestExpectContractError(function wrongNestedArrays() {
        var nested = [[1, 2, 3], 4];
        $rt_addContract(nested, NArrayArrayContract);
        return nested[1][1];
    });

    /* FIXME
    runTestExpectContractError(function wrongNestedArrays() {
        var nested = [1, 2, 3];
        $rt_addContract(nested, NArrayContract);
        nested[1] = "bla";
    });

    runTestExpectContractError(function wrongNestedArrays() {
        var nested = [[1, 2, 3], [4, 5, 6]];
        $rt_addContract(nested, NArrayArrayContract);
        nested[1][1] = "bla";
    });
    */

    runTest(function vectorArray() {
        var nested = [{ x : 1 }, { x : 2 }];
        $rt_addContract(nested, VectorArrayContract);
        return nested[1].x;
    });

    runTestExpectContractError(function vectorArray() {
        var nested = [{ x : 1 }, { x : "bla" }];
        $rt_addContract(nested, VectorArrayContract);
        return nested[1].x;
    });

    runTest(function vectorArrayAddingContractGet() {
        var nested = [{ x : 1 }, { x : 2 }];
        $rt_addContract(nested, VectorArrayContract);

        // Creates a contract on v
        var v = nested[1];

        v.x = 3;
    });

    runTestExpectContractError(function vectorArrayAddingContractGet() {
        var nested = [{ x : 1 }, { x : 2 }];
        $rt_addContract(nested, VectorArrayContract);

        // Creates a contract on v
        var v = nested[1];

        v.x = "wrong";
    });

    runTest(function vectorArrayAddingContractSet() {
        var v = { x : 2 };
        var nested = [{ x : 1 }, { x : "wrong" }];
        $rt_addContract(nested, VectorArrayContract);

        // Creates a contract on v
        nested[1] = v;

        v.x = 4;
    });

    runTestExpectContractError(function vectorArrayAddingContractSet() {
        var v = { x : 2 };
        var nested = [{ x : 1 }, { x : 3 }];
        $rt_addContract(nested, VectorArrayContract);

        // Creates a contract on v
        nested[1] = v;

        v.x = "bla";
    });

    /*
     * Function contract checks
     */

    function helloWorldString() { return "Hello, World!"; }
    function helloWorldNumber() { return 1; }

    runTest(function functionReturnPrimitive() {
        $rt_addContract(helloWorldString, StringFunction);

        return helloWorldString();
    });

    /* FIXME
    runTestExpectContractError(function functionReturnPrimitiveFail() {
        $rt_addContract(helloWorldNumber, StringFunction);

        return helloWorldNumber();
    });
    */

    /* FIXME
     * What happens if we hit the shape version limit on an object
     * in $ir_capture_shape?
     */
}

contractTests()
