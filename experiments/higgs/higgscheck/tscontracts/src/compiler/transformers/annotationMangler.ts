/*@internal*/
namespace ts {
    export function transformAnnotationMangler(context: TransformationContext) {
        const compilerOptions = context.getCompilerOptions();
        const annotationMangler = compilerOptions.annotationMangler;
        const emitResolver = context.getEmitResolver();
        var annotationCt = 0;

        return transformSourceFile;

        function transformSourceFile(node: SourceFile) {
            visitEachChild(node, visitor, context);
            if (annotationMangler === "00")
                sys.write(`${annotationCt} annotations.${sys.newLine}`);
            return node;
        }

        function shouldMangleAnnotation(num: number) {
            var hi = ~~(num/16);
            var lo = num%16;
            if (hi >= annotationMangler.length)
                return false;
            if (parseInt(annotationMangler[annotationMangler.length-1-hi], 16) & (1<<lo))
                return true;
            return false;
        }

        /* The annotations to be considered are variables/parameters, function
         * returns and casts */
        function visitor(node: Node): VisitResult<Node> {
            switch (node.kind) {
                case SyntaxKind.VariableDeclaration:
                case SyntaxKind.Parameter:
                case SyntaxKind.FunctionDeclaration:
                case SyntaxKind.MethodDeclaration:
                case SyntaxKind.GetAccessor:
                    return visitDeclaration(<any> node);

                case SyntaxKind.TypeAssertionExpression:
                    return visitTypeAssertionExpression(<TypeAssertion> node);

                default:
                    return visitEachChild(node, visitor, context);
            }
        }

        function visitDeclaration(node:
            VariableDeclaration|ParameterDeclaration|FunctionLikeDeclaration): VisitResult<Node> {
            var type = emitResolver.getTypeOfSymbol(node.symbol);
            if (type.flags & TypeFlags.Any)
                return visitEachChild(node, visitor, context);

            var annotationNum = annotationCt++;

            visitEachChild(node, visitor, context);
            if (!shouldMangleAnnotation(annotationNum))
                return node;
            node.type = <KeywordTypeNode> {kind: SyntaxKind.AnyKeyword};
            return node;
        }

        function visitTypeAssertionExpression(node: TypeAssertion): VisitResult<Node> {
            var type = emitResolver.getTypeFromTypeNode(node.type);
            if (type.flags & TypeFlags.Any)
                return visitEachChild(node, visitor, context);

            var annotationNum = annotationCt++;

            visitEachChild(node, visitor, context);
            if (!shouldMangleAnnotation(annotationNum))
                return node;
            node.type = <KeywordTypeNode> {kind: SyntaxKind.AnyKeyword};
            return node;
        }
    }
}
