/*@internal*/
namespace ts {
    export function transformGenerateContracts(context: TransformationContext) {
        var usedTypes: MapLike<Type> = {};
        var typeHashes: MapLike<string> = {};
        const emitResolver = context.getEmitResolver();

        const primitiveLike = TypeFlags.StringLike | TypeFlags.NumberLike | TypeFlags.BooleanLike;

        return transformSourceFile;

        // Serialize a type in a consistent(ish) way
        function serializeType(type: Type): string {
            var cache: MapLike<number> = {};
            var cacheCt = 0;

            function jSerializeType(type: Type): any {
                if (!(type.flags & TypeFlags.StructuredType)) {
                    // No structure, so just return the flags
                    return type.flags+"";
                }

                // Check if it's already in the cache
                if (cache[type.id])
                    return cache[type.id];

                // Make it
                var id = cacheCt++;
                cache[type.id] = id;
                var jsType: any[] = [];

                // With structured types, write out all the properties
                var properties = emitResolver.getPropertiesOfType(type);
                for (var pi = 0; pi < properties.length; pi++) {
                    var prop = properties[pi];
                    jsType.push({"k": prop.name,
                                 "v": jSerializeType(emitResolver.getTypeOfSymbol(prop))});
                }

                // And the index properties
                var indexInfo = emitResolver.getIndexInfoOfType(type, IndexKind.Number);
                if (indexInfo && indexInfo.type)
                    jsType.push({"k": "0",
                                 "v": jSerializeType(indexInfo.type)});

                // FIXME: Calls

                return jsType;
            }

            return JSON.stringify(jSerializeType(type));
        }

        function hashType(type: Type): string {
            if (type.id in typeHashes)
                return typeHashes[type.id];
            return (typeHashes[type.id] = sys.createHash(serializeType(type)));
        }

        /* True if a type is
         *  * any
         *  * void
         *  * a union or intersection including primitive types
         *  * a type parameter constrained by an any-like type */
        function isAnyLike(type: Type): boolean {
            if (type.flags & (TypeFlags.Any|TypeFlags.Void))
                return true;

            if (type.flags & TypeFlags.UnionOrIntersection) {
                // See if it has primitive members
                var uType = <UnionOrIntersectionType> type;
                for (var ti = 0; ti < uType.types.length; ti++) {
                    if (uType.types[ti].flags & TypeFlags.Primitive)
                        return true;
                }
            }

            if (type.flags & TypeFlags.TypeParameter) {
                var cType = <ResolvedType> (<TypeParameter> type).constraint;
                if (isAnyLike(cType))
                    return true;

                /* Bug: TypeScript reports an empty Object contraint for
                 * unconstrained, when it should report any */
                if (cType.flags & TypeFlags.Object &&
                    emitResolver.getPropertiesOfType(cType).length === 0 &&
                    (<ResolvedType> cType).callSignatures.length === 0 &&
                    (<ResolvedType> cType).constructSignatures.length === 0)
                    return true;
            }

            var rType: ResolvedType = <ResolvedType> type;
            if ((rType.callSignatures && rType.callSignatures.length) ||
                (rType.constructSignatures && rType.constructSignatures.length))
                return true;

            return false;
        }

        // Generate blame information for this node
        function createBlameFromNode(node: Node): Expression {
            return createStringLiteral(nodePosToString(node));
        }

        // Generate a contract checker or adder for a given type
        function createContractCheck(node: Expression, ctx: Node, type: Type): Expression {
            if (isAnyLike(type))
                return node;

            if (type.flags & primitiveLike) {
                var checkType: string = "object";
                if (type.flags & TypeFlags.StringLike) {
                    checkType = "string";
                } else if (type.flags & TypeFlags.NumberLike) {
                    checkType = "number";
                } else if (type.flags & TypeFlags.BooleanLike) {
                    checkType = "boolean";
                }

                return createCall(
                    createIdentifier("$rt_check"), [],
                    [
                        node,
                        createStringLiteral(checkType)
                    ]
                );

            } else if (type.flags & TypeFlags.StructuredType) {
                usedTypes[type.id] = type;
                return createCall(
                    createIdentifier("$rt_addContract"), [],
                    [
                        node,
                        createIdentifier("c" + hashType(type) + "c"),
                        createBlameFromNode(ctx)
                    ]
                );

            }

            return node;
        }
        
        function transformSourceFile(node: SourceFile) {
            var ret = visitEachChild(node, visitor, context);

            var contracts: Statement[] = [];
            var contractCache: MapLike<string> = {};

            function createIntrinsicContract(type: string): Expression {
                return createCall(
                    createIdentifier("$ir_contract_for"), [], [
                        createStringLiteral(type)
                    ]);
            }

            function contractForType(type: Type): string {
                if (type.id in contractCache)
                    return contractCache[type.id];

                var typeHash = hashType(type);
                var ret = "c" + typeHash;
                contractCache[type.id] = ret;

                var contract: Expression = null;

                if (type.flags & TypeFlags.StringLike) {
                    contract = createIntrinsicContract("string");

                } else if (type.flags & TypeFlags.NumberLike) {
                    contract = createIntrinsicContract("number");

                } else if (type.flags & TypeFlags.BooleanLike) {
                    contract = createIntrinsicContract("boolean");

                } else if (type.flags & TypeFlags.StructuredType) {
                    ret += "f()";
                    contractCache[type.id] = ret;

                    var properties = emitResolver.getPropertiesOfType(type);

                    // Assume it's been resolved by getting its properties
                    var rType = <ResolvedType> type;
                    var isFunction = false;
                    if ((rType.callSignatures && rType.callSignatures.length) ||
                        (rType.constructSignatures && rType.constructSignatures.length))
                        isFunction = true;

                    // Begin our contract
                    var wrContract: Statement[] = []
                    wrContract.push(
                        createStatement(
                            createStringLiteral(serializeType(type))
                        )
                    );
                    wrContract.push(
                        createIf(
                            createStrictInequality(
                                createTypeOf(createIdentifier("c" + typeHash)),
                                createStringLiteral("undefined")
                            ),
                            createReturn(
                                createIdentifier("c" + typeHash)
                            )
                        )
                    );
                    wrContract.push(
                        createVariableStatement([], [createVariableDeclaration(
                            "co", undefined,
                            createIntrinsicContract("object")
                        )])
                    );
                    if (isFunction)
                        wrContract.push(
                            createVariableStatement([], [createVariableDeclaration(
                                "cf", undefined,
                                createIntrinsicContract("function")
                            )])
                        );

                    wrContract.push(
                        createVariableStatement([], [createVariableDeclaration(
                            "c", undefined,
                            createIdentifier(isFunction?"cf":"co")
                        )])
                    );

                    // First add each property
                    for (var pi = 0; pi < properties.length; pi++) {
                        var prop = properties[pi];
                        var pType = emitResolver.getTypeOfSymbol(prop);
                        if (!isAnyLike(pType)) {
                            wrContract.push(
                                createStatement(createAssignment(
                                    createIdentifier("c"),
                                    createCall(
                                        createIdentifier("$ir_contract_oblige_member"), [],
                                        [
                                            createIdentifier("c"),
                                            createStringLiteral(prop.name),
                                            createIdentifier("co")
                                        ]
                                    )
                                ))
                            );
                        }
                    }

                    // Including indices if they exist
                    if (rType.numberIndexInfo) {
                        if (!isAnyLike(rType.numberIndexInfo.type)) {
                            wrContract.push(
                                createStatement(createAssignment(
                                    createIdentifier("c"),
                                    createCall(
                                        createIdentifier("$ir_contract_oblige_array"), [],
                                        [
                                            createIdentifier("c"),
                                            createIdentifier("co")
                                        ]
                                    )
                                ))
                            );
                        }
                    }

                    // And the return, if applicable
                    if (isFunction) {
                        wrContract.push(
                            createStatement(createAssignment(
                                createIdentifier("c"),
                                createCall(
                                    createIdentifier("$ir_contract_oblige_return"), [],
                                    [
                                        createIdentifier("c"),
                                        createIdentifier("co")
                                    ]
                                )
                            ))
                        );
                    }

                    // Now set the name, so recursive contracts can use it
                    wrContract.push(
                        createStatement(createAssignment(
                            createIdentifier("c" + typeHash),
                            createIdentifier("c")
                        ))
                    );

                    // Then set the actual property obligations
                    for (var pi = 0; pi < properties.length; pi++) {
                        var prop = properties[pi];
                        var pType = emitResolver.getTypeOfSymbol(prop);
                        if (!isAnyLike(pType)) {
                            wrContract.push(
                                createStatement(createCall(
                                    createIdentifier("$ir_contract_oblige_member"), [],
                                    [
                                        createIdentifier("c"),
                                        createStringLiteral(prop.name),
                                        createIdentifier(contractForType(pType))
                                    ]
                                ))
                            );
                        }
                    }

                    // The actual index
                    if (rType.numberIndexInfo) {
                        if (!isAnyLike(rType.numberIndexInfo.type)) {
                            wrContract.push(
                                createStatement(createAssignment(
                                    createIdentifier("c"),
                                    createCall(
                                        createIdentifier("$ir_contract_oblige_array"), [],
                                        [
                                            createIdentifier("c"),
                                            createIdentifier(contractForType(rType.numberIndexInfo.type))
                                        ]
                                    )
                                ))
                            );
                        }
                    }

                    // And the actual return, if applicable
                    if (isFunction) {
                        // For the time being, we can only make a contract if all return types are the same
                        var returnType: Type = null;
                        for (var ci = 0; ci < rType.callSignatures.length; ci++) {
                            var signature = rType.callSignatures[ci];
                            if (ci === 0) {
                                returnType = signature.resolvedReturnType;
                            } else if (returnType !== signature.resolvedReturnType) {
                                returnType = null;
                            }
                        }
                        for (var ci = 0; ci < rType.constructSignatures.length; ci++) {
                            var signature = rType.constructSignatures[ci];
                            if (ci === 0 && rType.callSignatures.length === 0) {
                                returnType = signature.resolvedReturnType;
                            } else if (returnType !== signature.resolvedReturnType) {
                                returnType = null;
                            }
                        }

                        if (returnType) {
                            // Everyone agrees on the return type
                            if (!isAnyLike(returnType)) {
                                wrContract.push(
                                    createStatement(createCall(
                                        createIdentifier("$ir_contract_oblige_return"), [],
                                        [
                                            createIdentifier("c"),
                                            createIdentifier(contractForType(returnType))
                                        ]
                                    ))
                                );
                            }
                        }
                    }

                    wrContract.push(
                        createReturn(
                            createIdentifier("c")
                        )
                    );

                    contracts.push(
                        createFunctionDeclaration([], [], undefined,
                            "c" + typeHash + "f", [], [], undefined,
                            createBlock(
                                wrContract, true
                            )
                        )
                    );

                    contract = createCall(createIdentifier("c" + typeHash + "f"), [], []);

                } else if (type.flags & TypeFlags.TypeParameter) {
                    /* All we know locally is that this is a type parameter, so
                     * we can only add a contract for the constraint type */
                    contract = createIdentifier(contractForType(
                        (<TypeParameter> type).constraint));

                } else {
                    sys.write("WARN: Unrecognized type flag " + type.flags + sys.newLine);

                }

                if (contract) {
                    contracts.push(
                        createVariableStatement([], [createVariableDeclaration(
                            createIdentifier("c" + typeHash), undefined, undefined
                        )])
                    );

                    contracts.push(
                        createIf(
                            createStrictEquality(
                                createTypeOf(createIdentifier("c" + typeHash)),
                                createStringLiteral("undefined")
                            ),
                            createStatement(createBinary(
                                createIdentifier("c" + typeHash),
                                SyntaxKind.EqualsToken,
                                contract
                            ))
                        )
                    );

                    /*
                    contracts.push(
                        createVariableStatement(
                            [createToken(SyntaxKind.ConstKeyword)],
                            [
                                createVariableDeclaration(
                                    createIdentifier("c" + typeHash + "c"), undefined,
                                    createIdentifier("c" + typeHash)
                                )
                            ]
                        )
                    );
                    */

                    contracts.push(
                        createStatement(createCall(
                            createIdentifier("$ir_obj_def_const"), [],
                            [
                                createThis(),
                                createStringLiteral("c" + typeHash + "c"),
                                createIdentifier("c" + typeHash),
                                createTrue()
                            ]
                        ))
                    );
                }

                return ret;
            }


            // Create our contracts
            for (var typeId in usedTypes) {
                var type = usedTypes[typeId];
                contractForType(type);
            }

            if (contracts.length) {
                contracts.push.apply(contracts, ret.statements);
                ret.statements = createNodeArray<Statement>(contracts);
            }

            return ret;
        }

        function visitor(node: Node): VisitResult<Node> {
            /* We need to generate checks anywhere where a value cannot be
             * trusted. Specifically:
             *
             *  * All function parameters
             *  * Casts, including implicit during assignment/initialization
             *    but not arguments
             *  * Access to globals (not yet implemented)
             */
            switch (node.kind) {
                case SyntaxKind.FunctionDeclaration:
                case SyntaxKind.MethodDeclaration:
                case SyntaxKind.Constructor:
                case SyntaxKind.GetAccessor:
                case SyntaxKind.SetAccessor:
                    return visitFunctionLikeDeclaration(<FunctionLikeDeclaration> node);

                case SyntaxKind.TypeAssertionExpression:
                    // ("cast")
                    return visitTypeAssertionExpression(<TypeAssertion> node);

                case SyntaxKind.BinaryExpression:
                    // Assignment, possible implicit cast
                    return visitBinaryExpression(<BinaryExpression> node);

                case SyntaxKind.VariableDeclaration:
                    // Initializer, possible implicit cast
                    return visitVariableDeclaration(<VariableDeclaration> node);

                case SyntaxKind.Identifier:
                    return visitIdentifier(<Identifier> node);

                default:
                    return visitEachChild(node, visitor, context);
            }
        }

        // Add contracts to all arguments
        function visitFunctionLikeDeclaration(node: FunctionLikeDeclaration): VisitResult<Node> {
            var contractAddStatements: Statement[] = [];

            // Check "this" if applicable
            if (node.kind !== SyntaxKind.FunctionDeclaration) {
                var thisType: Type = emitResolver.getDeclaredTypeOfSymbol(node.parent.symbol);
                if (thisType && (thisType.flags & TypeFlags.Object)) {
                    contractAddStatements.push(
                        createStatement(createContractCheck(
                            createThis(), node, thisType
                        ))
                    );
                }
            }

            // Collect the types of parameters
            for (var pi = 0; pi < node.parameters.length; pi++) {
                var parameter = node.parameters[pi];
                if (parameter.name.kind === SyntaxKind.Identifier /* FIXME */ &&
                    parameter.symbol) {
                    var name = (<Identifier> parameter.name).text;
                    var type = emitResolver.getTypeOfSymbol(parameter.symbol);
                    var paramNode = createIdentifier(name);
                    var checkNode = createContractCheck(paramNode, parameter, type);
                    if (checkNode !== paramNode)
                        contractAddStatements.push(createStatement(checkNode));
                }
            }

            // Add our contracts if necessary
            if (contractAddStatements.length && node.body) {
                if (node.body.kind !== SyntaxKind.Block)
                    contractAddStatements.push(
                        createReturn(<Expression> node.body)
                    );
                else
                    contractAddStatements.push.apply(contractAddStatements, (<Block> node.body).statements);

                node.body =
                    setOriginalNode(
                        createBlock(
                            createNodeArray<Statement>(contractAddStatements), true
                        ),
                        node.body
                    );
            }

            return visitEachChild(node, visitor, context);
        }

        // Add contracts through casts
        function visitTypeAssertionExpression(node: TypeAssertion): VisitResult<Node> {
            var type = emitResolver.getTypeFromTypeNode(node.type);
            visitEachChild(node, visitor, context);
            return createContractCheck(node, node, type);
        }

        // Add contracts through unsafe assignments
        function visitBinaryExpression(node: BinaryExpression): VisitResult<Node> {
            // Only assignment matters
            if (node.operatorToken.kind !== SyntaxKind.EqualsToken)
                return visitEachChild(node, visitor, context);

            // We don't care if there was no implicit cast
            var lType = emitResolver.getTypeOfExpression(node.left);
            var rType = emitResolver.getTypeOfExpression(node.right);
            if (isAnyLike(lType) ||
                !isAnyLike(rType))
                return visitEachChild(node, visitor, context);

            // Clamp it down
            visitEachChild(node, visitor, context);
            return createContractCheck(node, node, lType);
        }

        // Add contracts through unsafe variable initialization
        function visitVariableDeclaration(node: VariableDeclaration): VisitResult<Node> {
            // Don't care if there's no initializer
            if (!node.initializer)
                return visitEachChild(node, visitor, context);

            // Don't care if there was no implicit cast
            var lType = emitResolver.getTypeOfSymbol(node.symbol);
            var rType = emitResolver.getTypeOfExpression(node.initializer);
            if (isAnyLike(lType) ||
                !isAnyLike(rType))
                return visitEachChild(node, visitor, context);

            // Clamp it down
            visitEachChild(node, visitor, context);
            node.initializer = setOriginalNode(
                createContractCheck(node.initializer, node.initializer, lType),
                node.initializer
            );
            return node;
        }

        // Add contracts on identifiers if they're not local
        function visitIdentifier(node: Identifier): VisitResult<Node> {
            /* Identifiers are used in various places that aren't expressions,
             * but we only want to modify expressions */
            var pnode = node.parent;
            if (!pnode ||
                (!isExpression(pnode) && pnode.kind !== SyntaxKind.ExpressionStatement))
                return visitEachChild(node, visitor, context);

            // We don't want to modify assignment targets
            if (isAssignmentTarget(node))
                return visitEachChild(node, visitor, context);

            // And we don't want to do anything if we're a member
            if (pnode.kind === SyntaxKind.PropertyAccessExpression &&
                (<PropertyAccessExpression> pnode).name === node)
                return visitEachChild(node, visitor, context);

            // Figure out the correct type
            var dec = emitResolver.getReferencedValueDeclaration(node);
            if (!dec || !dec.symbol)
                return visitEachChild(node, visitor, context);

            var type = emitResolver.getTypeOfSymbol(dec.symbol);

            // Incredibly lazy/wrong way of figuring out if this is global
            var scope = getEnclosingBlockScopeContainer(dec);
            if (scope.kind !== SyntaxKind.SourceFile)
                return visitEachChild(node, visitor, context);

            if (isAnyLike(type))
                return visitEachChild(node, visitor, context);

            visitEachChild(node, visitor, context);
            return node;
            //return createContractCheck(node, node, type);
        }
    }
}
