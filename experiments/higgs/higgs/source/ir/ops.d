/*****************************************************************************
*
*                      Higgs JavaScript Virtual Machine
*
*  This file is part of the Higgs project. The project is distributed at:
*  https://github.com/maximecb/Higgs
*
*  Copyright (c) 2011-2015, Maxime Chevalier-Boisvert. All rights reserved.
*
*  This software is licensed under the following license (Modified BSD
*  License):
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are
*  met:
*   1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*   3. The name of the author may not be used to endorse or promote
*      products derived from this software without specific prior written
*      permission.
*
*  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
*  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
*  NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
*  NOT LIMITED TO PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
*  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

module ir.ops;

import ir.ir;
import jit.codeblock;
import jit.ops;

/**
Opcode argument type
*/
enum OpArg
{
    INT32,
    FLOAT64,
    STRING,
    LOCAL,
    FUN
}

/**
Opcode information
*/
struct OpInfo
{
    alias OpFlag = uint;
    enum : OpFlag
    {
        VAR_ARG     = 1 << 0,
        BRANCH      = 1 << 1,
        CALL        = 1 << 2,
        MAY_GC      = 1 << 3,
        BOOL_VAL    = 1 << 4,
        IMPURE      = 1 << 5
    }

    string mnem;
    bool output;
    OpArg[] argTypes;
    GenFn genFn;
    OpFlag opFlags = 0;

    bool isVarArg() const { return (opFlags & VAR_ARG) != 0; }
    bool isBranch() const { return (opFlags & BRANCH) != 0; }
    bool isCall  () const { return (opFlags & CALL) != 0; }
    bool mayGC   () const { return (opFlags & MAY_GC) != 0; }
    bool boolVal () const { return (opFlags & BOOL_VAL) != 0; }
    bool isImpure() const { return (opFlags & IMPURE) != 0; }

    OpArg getArgType(size_t i) immutable
    {
        if (i < argTypes.length)
            return argTypes[i];
        else if (isVarArg)
            return OpArg.LOCAL;
        else
            assert (false, "invalid arg index");
    }
}

/// Instruction type (opcode) alias
alias static immutable(OpInfo) Opcode;

// Access visible (non-hidden) function arguments by index
Opcode GET_ARG = { "get_arg", true, [OpArg.LOCAL], &gen_get_arg };

// Word/type manipulation primitives
Opcode MAKE_VALUE = { "make_value", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_make_value };
Opcode GET_WORD = { "get_word", true, [OpArg.LOCAL], &gen_get_word };
Opcode GET_TAG = { "get_tag", true, [OpArg.LOCAL], &gen_get_tag };

// Type tag test
Opcode IS_UNDEF = { "is_undef", true, [OpArg.LOCAL], &gen_is_undef, OpInfo.BOOL_VAL };
Opcode IS_NULL = { "is_null", true, [OpArg.LOCAL], &gen_is_null, OpInfo.BOOL_VAL };
Opcode IS_BOOL = { "is_bool", true, [OpArg.LOCAL], &gen_is_bool, OpInfo.BOOL_VAL };
Opcode IS_INT32 = { "is_int32", true, [OpArg.LOCAL], &gen_is_int32, OpInfo.BOOL_VAL };
Opcode IS_INT64 = { "is_int64", true, [OpArg.LOCAL], &gen_is_int64, OpInfo.BOOL_VAL };
Opcode IS_FLOAT64 = { "is_float64", true, [OpArg.LOCAL], &gen_is_float64, OpInfo.BOOL_VAL };
Opcode IS_RAWPTR = { "is_rawptr", true, [OpArg.LOCAL], &gen_is_rawptr, OpInfo.BOOL_VAL };
Opcode IS_REFPTR = { "is_refptr", true, [OpArg.LOCAL], &gen_is_refptr, OpInfo.BOOL_VAL };
Opcode IS_OBJECT = { "is_object", true, [OpArg.LOCAL], &gen_is_object, OpInfo.BOOL_VAL };
Opcode IS_ARRAY = { "is_array", true, [OpArg.LOCAL], &gen_is_array, OpInfo.BOOL_VAL };
Opcode IS_CLOSURE = { "is_closure", true, [OpArg.LOCAL], &gen_is_closure, OpInfo.BOOL_VAL };
Opcode IS_STRING = { "is_string", true, [OpArg.LOCAL], &gen_is_string, OpInfo.BOOL_VAL };
Opcode IS_ROPE = { "is_rope", true, [OpArg.LOCAL], &gen_is_rope, OpInfo.BOOL_VAL };

// Type conversion
Opcode I32_TO_F64 = { "i32_to_f64", true, [OpArg.LOCAL], &gen_i32_to_f64 };
Opcode F64_TO_I32 = { "f64_to_i32", true, [OpArg.LOCAL], &gen_f64_to_i32 };

// Integer arithmetic
Opcode ADD_I32 = { "add_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_add_i32 };
Opcode SUB_I32 = { "sub_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_sub_i32 };
Opcode MUL_I32 = { "mul_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_mul_i32 };
Opcode DIV_I32 = { "div_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_div_i32 };
Opcode MOD_I32 = { "mod_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_mod_i32 };

// Integer arithmetic with overflow handling
Opcode ADD_I32_OVF = { "add_i32_ovf", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_add_i32_ovf, OpInfo.BRANCH };
Opcode SUB_I32_OVF = { "sub_i32_ovf", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_sub_i32_ovf, OpInfo.BRANCH };
Opcode MUL_I32_OVF = { "mul_i32_ovf", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_mul_i32_ovf, OpInfo.BRANCH };

// Pointer arithmetic
Opcode ADD_PTR_I32 = { "add_ptr_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_add_ptr_i32 };

// Bitwise operations
Opcode AND_I32 = { "and_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_and_i32 };
Opcode OR_I32 = { "or_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_or_i32 };
Opcode XOR_I32 = { "xor_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_xor_i32 };
Opcode LSFT_I32 = { "lsft_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_lsft_i32 };
Opcode RSFT_I32 = { "rsft_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_rsft_i32 };
Opcode URSFT_I32 = { "ursft_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_ursft_i32 };
Opcode NOT_I32 = { "not_i32", true, [OpArg.LOCAL], &gen_not_i32 };

// Floating-point arithmetic
Opcode ADD_F64 = { "add_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_add_f64 };
Opcode SUB_F64 = { "sub_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_sub_f64 };
Opcode MUL_F64 = { "mul_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_mul_f64 };
Opcode DIV_F64 = { "div_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_div_f64 };
Opcode MOD_F64 = { "mod_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_mod_f64 };

// Higher-level floating-point functions
Opcode SIN_F64 = { "sin_f64", true, [OpArg.LOCAL], &gen_sin_f64 };
Opcode COS_F64 = { "cos_f64", true, [OpArg.LOCAL], &gen_cos_f64 };
Opcode SQRT_F64 = { "sqrt_f64", true, [OpArg.LOCAL], &gen_sqrt_f64 };
Opcode CEIL_F64 = { "ceil_f64", true, [OpArg.LOCAL], &gen_ceil_f64 };
Opcode FLOOR_F64 = { "floor_f64", true, [OpArg.LOCAL], &gen_floor_f64 };
Opcode LOG_F64 = { "log_f64", true, [OpArg.LOCAL], &gen_log_f64 };
Opcode EXP_F64 = { "exp_f64", true, [OpArg.LOCAL], &gen_exp_f64 };
Opcode POW_F64 = { "pow_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_pow_f64 };

// Integer comparison instructions
Opcode EQ_I8 = { "eq_i8", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_eq_i8, OpInfo.BOOL_VAL };
Opcode EQ_I32 = { "eq_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_eq_i32, OpInfo.BOOL_VAL };
Opcode NE_I32 = { "ne_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_ne_i32, OpInfo.BOOL_VAL };
Opcode LT_I32 = { "lt_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_lt_i32, OpInfo.BOOL_VAL };
Opcode GT_I32 = { "gt_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_gt_i32, OpInfo.BOOL_VAL };
Opcode LE_I32 = { "le_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_le_i32, OpInfo.BOOL_VAL };
Opcode GE_I32 = { "ge_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_ge_i32, OpInfo.BOOL_VAL };
Opcode EQ_I64 = { "eq_i64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_eq_i64, OpInfo.BOOL_VAL };

// Pointer comparison instructions
Opcode EQ_REFPTR = { "eq_refptr", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_eq_refptr, OpInfo.BOOL_VAL };
Opcode NE_REFPTR = { "ne_refptr", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_ne_refptr, OpInfo.BOOL_VAL };
Opcode EQ_RAWPTR = { "eq_rawptr", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_eq_rawptr, OpInfo.BOOL_VAL };
Opcode NE_RAWPTR = { "ne_rawptr", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_ne_rawptr, OpInfo.BOOL_VAL };

// Boolean comparison instructions
Opcode EQ_BOOL = { "eq_bool", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_eq_bool, OpInfo.BOOL_VAL };
Opcode NE_BOOL = { "ne_bool", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_ne_bool, OpInfo.BOOL_VAL };

// Floating-point comparison instructions
Opcode EQ_F64 = { "eq_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_eq_f64, OpInfo.BOOL_VAL };
Opcode NE_F64 = { "ne_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_ne_f64, OpInfo.BOOL_VAL };
Opcode LT_F64 = { "lt_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_lt_f64, OpInfo.BOOL_VAL };
Opcode GT_F64 = { "gt_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_gt_f64, OpInfo.BOOL_VAL };
Opcode LE_F64 = { "le_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_le_f64, OpInfo.BOOL_VAL };
Opcode GE_F64 = { "ge_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_ge_f64, OpInfo.BOOL_VAL };

// Load instructions
Opcode LOAD_U8 = { "load_u8", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_u8 };
Opcode LOAD_U16 = { "load_u16", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_u16 };
Opcode LOAD_U32 = { "load_u32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_u32 };
Opcode LOAD_U64 = { "load_u64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_u64 };
Opcode LOAD_I8 = { "load_i8", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_i8 };
Opcode LOAD_I16 = { "load_i16", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_i16 };
Opcode LOAD_I32 = { "load_i32", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_i32 };
Opcode LOAD_I64 = { "load_64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_i64 };
Opcode LOAD_F64 = { "load_f64", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_f64 };
Opcode LOAD_REFPTR = { "load_refptr", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_refptr };
Opcode LOAD_STRING = { "load_string", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_string };
Opcode LOAD_RAWPTR = { "load_rawptr", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_rawptr };
Opcode LOAD_FUNPTR = { "load_funptr", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_load_funptr };

// Store instructions
Opcode STORE_U8 = { "store_u8", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_u8, OpInfo.IMPURE };
Opcode STORE_U16 = { "store_u16", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_u16, OpInfo.IMPURE };
Opcode STORE_U32 = { "store_u32", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_u32, OpInfo.IMPURE };
Opcode STORE_U64 = { "store_u64", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_u64, OpInfo.IMPURE };
Opcode STORE_I8 = { "store_i8", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_i8, OpInfo.IMPURE };
Opcode STORE_I16 = { "store_i16", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_i16, OpInfo.IMPURE };
Opcode STORE_I32 = { "store_i32", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_i32, OpInfo.IMPURE };
Opcode STORE_I64 = { "store_i64", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_i64, OpInfo.IMPURE };
Opcode STORE_F64 = { "store_f64", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_f64, OpInfo.IMPURE };
Opcode STORE_REFPTR = { "store_refptr", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_refptr, OpInfo.IMPURE };
Opcode STORE_RAWPTR = { "store_rawptr", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_rawptr, OpInfo.IMPURE };
Opcode STORE_FUNPTR = { "store_funptr", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_store_funptr, OpInfo.IMPURE };

// Unconditional jump
Opcode JUMP = { "jump", false, [], &gen_jump, OpInfo.BRANCH };

// Branch based on a boolean value
Opcode IF_TRUE = { "if_true", false, [OpArg.LOCAL], &gen_if_true, OpInfo.BRANCH };

// <dstLocal> = CALL_PRIM <primName> <primFun> ...
// Call a primitive function by name (compile-time lookup)
// Note: the second argument is a cached function reference
Opcode CALL_PRIM = { "call_prim", true, [OpArg.STRING], &gen_call_prim, OpInfo.VAR_ARG | OpInfo.BRANCH | OpInfo.CALL };

// <dstLocal> = CALL <closLocal> <thisArg> ...
// Makes the execution go to the callee entry
// Sets the frame pointer to the new frame's base
// Pushes the return address word
Opcode CALL = { "call", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_call, OpInfo.VAR_ARG | OpInfo.BRANCH | OpInfo.CALL };

// <dstLocal> = CALL_APPLY <closArg> <thisArg> <argTable> <numArgs>
// Call with an array of arguments
Opcode CALL_APPLY = { "call_apply", true, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_call_apply, OpInfo.BRANCH | OpInfo.CALL };

/// Load a source code unit from a file
Opcode LOAD_FILE = { "load_file", true, [OpArg.LOCAL], &gen_load_file, OpInfo.BRANCH | OpInfo.CALL | OpInfo.MAY_GC | OpInfo.IMPURE };

/// Evaluate a source string in the global scope
Opcode EVAL_STR = { "eval_str", true, [OpArg.LOCAL], &gen_eval_str, OpInfo.BRANCH | OpInfo.CALL | OpInfo.MAY_GC | OpInfo.IMPURE };

// RET <retLocal>
// Pops the callee frame (size known by context)
Opcode RET = { "ret", false, [OpArg.LOCAL], &gen_ret, OpInfo.BRANCH };

// THROW <excLocal>
// Throws an exception, unwinds the stack
Opcode THROW = { "throw", false, [OpArg.LOCAL], &gen_throw, OpInfo.BRANCH };

// Catch the exception value from a function call
Opcode CATCH = { "catch", true, [OpArg.LOCAL], &gen_catch };

// Special implementation object/value access instructions
Opcode GET_OBJ_PROTO = { "get_obj_proto", true, [], &gen_get_obj_proto };
Opcode GET_ARR_PROTO = { "get_arr_proto", true, [], &gen_get_arr_proto };
Opcode GET_FUN_PROTO = { "get_fun_proto", true, [], &gen_get_fun_proto };
Opcode GET_STR_PROTO = { "get_str_proto", true, [], &gen_get_str_proto };
Opcode GET_GLOBAL_OBJ = { "get_global_obj", true, [], &gen_get_global_obj };
Opcode GET_HEAP_SIZE = { "get_heap_size", true, [], &gen_get_heap_size };
Opcode GET_HEAP_FREE = { "get_heap_free", true, [], &gen_get_heap_free };
Opcode GET_GC_COUNT = { "get_gc_count", true, [], &gen_get_gc_count };

/// Heap memory block allocation instructions
/// Note: each of these assigns a different type tag to the output pointer
Opcode ALLOC_REFPTR = { "alloc_refptr", true, [OpArg.LOCAL], &gen_alloc_refptr, OpInfo.MAY_GC };
Opcode ALLOC_OBJECT = { "alloc_object", true, [OpArg.LOCAL], &gen_alloc_object, OpInfo.MAY_GC };
Opcode ALLOC_ARRAY = { "alloc_array", true, [OpArg.LOCAL], &gen_alloc_array, OpInfo.MAY_GC };
Opcode ALLOC_CLOSURE = { "alloc_closure", true, [OpArg.LOCAL], &gen_alloc_closure, OpInfo.MAY_GC };
Opcode ALLOC_STRING = { "alloc_string", true, [OpArg.LOCAL], &gen_alloc_string, OpInfo.MAY_GC };
Opcode ALLOC_ROPE = { "alloc_rope", true, [OpArg.LOCAL], &gen_alloc_rope, OpInfo.MAY_GC };

/// Trigger a garbage collection
Opcode GC_COLLECT = { "gc_collect", false, [OpArg.LOCAL], &gen_gc_collect, OpInfo.MAY_GC | OpInfo.IMPURE };

/// Compute the hash code for a string and
/// try to find the string in the string table
Opcode GET_STR = { "get_str", true, [OpArg.LOCAL], &gen_get_str, OpInfo.MAY_GC };

/// Dummy branch instruction to stop compilation and resumes execution
Opcode BREAK = { "break", false, [], &gen_break, OpInfo.BRANCH };

/// Capture the type tag of a given value
Opcode CAPTURE_TAG = { "capture_tag", false, [OpArg.LOCAL], &gen_capture_tag, OpInfo.BRANCH };

/// Capture a given int32 value
Opcode CAPTURE_INT32 = { "capture_int32", false, [OpArg.LOCAL], &gen_capture_int32, OpInfo.BRANCH };

/// Capture the shape of a given object
Opcode CAPTURE_SHAPE = { "capture_shape", false, [OpArg.LOCAL, OpArg.LOCAL], &gen_capture_shape, OpInfo.BRANCH };

/// Clear the shape of a given object
Opcode CLEAR_SHAPE = { "clear_shape", false, [OpArg.LOCAL], &gen_clear_shape, OpInfo.IMPURE };

/// Get the shape index of an object
Opcode READ_SHAPE_IDX = { "read_shape_idx", true, [OpArg.LOCAL], &gen_read_shape_idx };

/// Initialize the shape of an object to the empty shape
Opcode OBJ_INIT_SHAPE = { "obj_init_shape", false, [OpArg.LOCAL, OpArg.LOCAL], &gen_obj_init_shape, OpInfo.IMPURE };

/// Initialize the shape of an array
Opcode ARR_INIT_SHAPE = { "arr_init_shape", false, [OpArg.LOCAL], &gen_arr_init_shape, OpInfo.IMPURE };

/// Set the value of an object property based on its shape
Opcode OBJ_SET_PROP = { "obj_set_prop", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_obj_set_prop, OpInfo.MAY_GC | OpInfo.IMPURE | OpInfo.BRANCH };

/// Get the value of an object property based on its shape
Opcode OBJ_GET_PROP = { "obj_get_prop", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_obj_get_prop, OpInfo.BRANCH };

/// Add a contract to an object
Opcode OBJ_ADD_CONTRACT = { "obj_add_contract", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_obj_add_contract, OpInfo.IMPURE };

/// Get the prototype of an object
Opcode OBJ_GET_PROTO = { "obj_get_proto", true, [OpArg.LOCAL], &gen_obj_get_proto };

/// Define a constant property on an object
Opcode OBJ_DEF_CONST = { "obj_def_const", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_obj_def_const, OpInfo.MAY_GC | OpInfo.IMPURE };

/// Get the attributes for a given property
Opcode OBJ_GET_ATTRS = { "obj_get_attrs", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_obj_get_attrs };

/// Set the attributes for a given property
Opcode OBJ_SET_ATTRS = { "obj_set_attrs", false, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_obj_set_attrs, OpInfo.IMPURE };

/// Get a table of enumerable property names for an object
Opcode OBJ_ENUM_TBL = { "obj_enum_tbl", true, [OpArg.LOCAL], &gen_obj_enum_tbl, OpInfo.MAY_GC };

/// Set the value of a global property
Opcode SET_GLOBAL = { "set_global", false, [OpArg.STRING, OpArg.LOCAL], &gen_set_global, OpInfo.MAY_GC | OpInfo.IMPURE };

/// <dstLocal> = NEW_CLOS <funExpr>
/// Create a new closure from a function's AST node
Opcode NEW_CLOS = { "new_clos", true, [OpArg.FUN], &gen_new_clos, OpInfo.MAY_GC };

/// Get a primitive contract from a type name (which must be statically known)
Opcode CONTRACT_FOR = { "contract_for", true, [OpArg.LOCAL], &gen_contract_for };

/// Oblige a contract to have a member with a given contract type
Opcode CONTRACT_OBLIGE_MEMBER = { "contract_oblige_member", true, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_contract_oblige_member, OpInfo.IMPURE };

/// Oblige a contract over arrays to have the given contract type as array members
Opcode CONTRACT_OBLIGE_ARRAY = { "contract_oblige_array", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_contract_oblige_array };

/// Oblige a function contract to have a given return type
Opcode CONTRACT_OBLIGE_RETURN = { "contract_oblige_return", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_contract_oblige_return };

/// Freeze a contract
Opcode CONTRACT_FREEZE = { "contract_freeze", false, [OpArg.LOCAL], &gen_contract_freeze };

/// Can a contractual obligation fail for this member access (given an object)?
Opcode CONTRACT_CAN_FAIL = { "contract_can_fail", true, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_contract_can_fail, OpInfo.BRANCH };

/// Can a contractual obligation fail for array-ish access (given an object and read/written value)?
Opcode CONTRACT_CAN_FAIL_ARRAY = { "contract_can_fail_array", true, [OpArg.LOCAL, OpArg.LOCAL], &gen_contract_can_fail_array, OpInfo.BRANCH };

/// Check this value against a given contractual obligation
Opcode CONTRACT_CHECK = { "contract_check", true, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_contract_check, OpInfo.BRANCH };

/// Get the blame associated with this contract
Opcode CONTRACT_BLAME = { "contract_blame", true, [OpArg.LOCAL, OpArg.LOCAL, OpArg.LOCAL], &gen_contract_blame, OpInfo.MAY_GC };

/// Print a string to standard output
Opcode PRINT_STR = { "print_str", false, [OpArg.LOCAL], &gen_print_str, OpInfo.IMPURE };

/// Print a pointer value (in hex notation) to standard output
Opcode PRINT_PTR = { "print_ptr", false, [OpArg.LOCAL], &gen_print_ptr, OpInfo.IMPURE };

/// Get the time in milliseconds since process start
Opcode GET_TIME_MS = { "get_time_ms", true, [], &gen_get_time_ms };

/// Format a floating-point value as a string
Opcode F64_TO_STR = { "f64_to_str", true, [OpArg.LOCAL], &gen_f64_to_str, OpInfo.MAY_GC };

/// Format a floating-point value as a string (long)
Opcode F64_TO_STR_LNG = { "f64_to_str_lng", true, [OpArg.LOCAL], &gen_f64_to_str_lng, OpInfo.MAY_GC };

/// Get a string representation of a function's AST
Opcode GET_AST_STR = { "get_ast_str", true, [OpArg.LOCAL], &gen_get_ast_str, OpInfo.MAY_GC };

/// Get a string representation of a function's IR
Opcode GET_IR_STR = { "get_ir_str", true, [OpArg.LOCAL], &gen_get_ir_str, OpInfo.MAY_GC };

/// Get a string representation of a function's machine code
Opcode GET_ASM_STR = { "get_asm_str", true, [OpArg.LOCAL], &gen_get_asm_str, OpInfo.MAY_GC };

/// Load a shared lib
Opcode LOAD_LIB = { "load_lib", true, [OpArg.LOCAL], &gen_load_lib };

/// Close shared lib
Opcode CLOSE_LIB = { "close_lib", false, [OpArg.LOCAL], &gen_close_lib, OpInfo.IMPURE };

/// Lookup symbol in shared lib
Opcode GET_SYM = { "get_sym", true, [OpArg.LOCAL, OpArg.STRING], &gen_get_sym };

/// Call function in shared lib
Opcode CALL_FFI = { "call_ffi", true, [OpArg.LOCAL, OpArg.STRING], &gen_call_ffi, OpInfo.BRANCH | OpInfo.CALL | OpInfo.VAR_ARG };

/// Get a C function pointer (entry point) for a JS function
Opcode GET_C_FPTR = { "get_c_fptr", true, [OpArg.LOCAL, OpArg.STRING], &gen_get_c_fptr, OpInfo.MAY_GC};

