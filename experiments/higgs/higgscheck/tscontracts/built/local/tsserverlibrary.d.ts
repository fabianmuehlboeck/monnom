/*! *****************************************************************************
Copyright (c) Microsoft Corporation. All rights reserved. 
Licensed under the Apache License, Version 2.0 (the "License"); you may not use
this file except in compliance with the License. You may obtain a copy of the
License at http://www.apache.org/licenses/LICENSE-2.0  
 
THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, 
MERCHANTABLITY OR NON-INFRINGEMENT. 
 
See the Apache Version 2.0 License for specific language governing permissions
and limitations under the License.
***************************************************************************** */

declare namespace ts {
    /**
     * Type of objects whose values are all of the same type.
     * The `in` and `for-in` operators can *not* be safely used,
     * since `Object.prototype` may be modified by outside code.
     */
    interface MapLike<T> {
        [index: string]: T;
    }
    /** ES6 Map interface. */
    interface Map<T> {
        get(key: string): T;
        has(key: string): boolean;
        set(key: string, value: T): this;
        delete(key: string): boolean;
        clear(): void;
        forEach(action: (value: T, key: string) => void): void;
        readonly size: number;
        keys(): Iterator<string>;
        values(): Iterator<T>;
        entries(): Iterator<[string, T]>;
    }
    /** ES6 Iterator type. */
    interface Iterator<T> {
        next(): {
            value: T;
            done: false;
        } | {
            value: never;
            done: true;
        };
    }
    type Path = string & {
        __pathBrand: any;
    };
    interface FileMap<T> {
        get(fileName: Path): T;
        set(fileName: Path, value: T): void;
        contains(fileName: Path): boolean;
        remove(fileName: Path): void;
        forEachValue(f: (key: Path, v: T) => void): void;
        getKeys(): Path[];
        clear(): void;
    }
    interface TextRange {
        pos: number;
        end: number;
    }
    const enum SyntaxKind {
        Unknown = 0,
        EndOfFileToken = 1,
        SingleLineCommentTrivia = 2,
        MultiLineCommentTrivia = 3,
        NewLineTrivia = 4,
        WhitespaceTrivia = 5,
        ShebangTrivia = 6,
        ConflictMarkerTrivia = 7,
        NumericLiteral = 8,
        StringLiteral = 9,
        JsxText = 10,
        RegularExpressionLiteral = 11,
        NoSubstitutionTemplateLiteral = 12,
        TemplateHead = 13,
        TemplateMiddle = 14,
        TemplateTail = 15,
        OpenBraceToken = 16,
        CloseBraceToken = 17,
        OpenParenToken = 18,
        CloseParenToken = 19,
        OpenBracketToken = 20,
        CloseBracketToken = 21,
        DotToken = 22,
        DotDotDotToken = 23,
        SemicolonToken = 24,
        CommaToken = 25,
        LessThanToken = 26,
        LessThanSlashToken = 27,
        GreaterThanToken = 28,
        LessThanEqualsToken = 29,
        GreaterThanEqualsToken = 30,
        EqualsEqualsToken = 31,
        ExclamationEqualsToken = 32,
        EqualsEqualsEqualsToken = 33,
        ExclamationEqualsEqualsToken = 34,
        EqualsGreaterThanToken = 35,
        PlusToken = 36,
        MinusToken = 37,
        AsteriskToken = 38,
        AsteriskAsteriskToken = 39,
        SlashToken = 40,
        PercentToken = 41,
        PlusPlusToken = 42,
        MinusMinusToken = 43,
        LessThanLessThanToken = 44,
        GreaterThanGreaterThanToken = 45,
        GreaterThanGreaterThanGreaterThanToken = 46,
        AmpersandToken = 47,
        BarToken = 48,
        CaretToken = 49,
        ExclamationToken = 50,
        TildeToken = 51,
        AmpersandAmpersandToken = 52,
        BarBarToken = 53,
        QuestionToken = 54,
        ColonToken = 55,
        AtToken = 56,
        EqualsToken = 57,
        PlusEqualsToken = 58,
        MinusEqualsToken = 59,
        AsteriskEqualsToken = 60,
        AsteriskAsteriskEqualsToken = 61,
        SlashEqualsToken = 62,
        PercentEqualsToken = 63,
        LessThanLessThanEqualsToken = 64,
        GreaterThanGreaterThanEqualsToken = 65,
        GreaterThanGreaterThanGreaterThanEqualsToken = 66,
        AmpersandEqualsToken = 67,
        BarEqualsToken = 68,
        CaretEqualsToken = 69,
        Identifier = 70,
        BreakKeyword = 71,
        CaseKeyword = 72,
        CatchKeyword = 73,
        ClassKeyword = 74,
        ConstKeyword = 75,
        ContinueKeyword = 76,
        DebuggerKeyword = 77,
        DefaultKeyword = 78,
        DeleteKeyword = 79,
        DoKeyword = 80,
        ElseKeyword = 81,
        EnumKeyword = 82,
        ExportKeyword = 83,
        ExtendsKeyword = 84,
        FalseKeyword = 85,
        FinallyKeyword = 86,
        ForKeyword = 87,
        FunctionKeyword = 88,
        IfKeyword = 89,
        ImportKeyword = 90,
        InKeyword = 91,
        InstanceOfKeyword = 92,
        NewKeyword = 93,
        NullKeyword = 94,
        ReturnKeyword = 95,
        SuperKeyword = 96,
        SwitchKeyword = 97,
        ThisKeyword = 98,
        ThrowKeyword = 99,
        TrueKeyword = 100,
        TryKeyword = 101,
        TypeOfKeyword = 102,
        VarKeyword = 103,
        VoidKeyword = 104,
        WhileKeyword = 105,
        WithKeyword = 106,
        ImplementsKeyword = 107,
        InterfaceKeyword = 108,
        LetKeyword = 109,
        PackageKeyword = 110,
        PrivateKeyword = 111,
        ProtectedKeyword = 112,
        PublicKeyword = 113,
        StaticKeyword = 114,
        YieldKeyword = 115,
        AbstractKeyword = 116,
        AsKeyword = 117,
        AnyKeyword = 118,
        AsyncKeyword = 119,
        AwaitKeyword = 120,
        BooleanKeyword = 121,
        ConstructorKeyword = 122,
        DeclareKeyword = 123,
        GetKeyword = 124,
        IsKeyword = 125,
        KeyOfKeyword = 126,
        ModuleKeyword = 127,
        NamespaceKeyword = 128,
        NeverKeyword = 129,
        ReadonlyKeyword = 130,
        RequireKeyword = 131,
        NumberKeyword = 132,
        ObjectKeyword = 133,
        SetKeyword = 134,
        StringKeyword = 135,
        SymbolKeyword = 136,
        TypeKeyword = 137,
        UndefinedKeyword = 138,
        FromKeyword = 139,
        GlobalKeyword = 140,
        OfKeyword = 141,
        QualifiedName = 142,
        ComputedPropertyName = 143,
        TypeParameter = 144,
        Parameter = 145,
        Decorator = 146,
        PropertySignature = 147,
        PropertyDeclaration = 148,
        MethodSignature = 149,
        MethodDeclaration = 150,
        Constructor = 151,
        GetAccessor = 152,
        SetAccessor = 153,
        CallSignature = 154,
        ConstructSignature = 155,
        IndexSignature = 156,
        TypePredicate = 157,
        TypeReference = 158,
        FunctionType = 159,
        ConstructorType = 160,
        TypeQuery = 161,
        TypeLiteral = 162,
        ArrayType = 163,
        TupleType = 164,
        UnionType = 165,
        IntersectionType = 166,
        ParenthesizedType = 167,
        ThisType = 168,
        TypeOperator = 169,
        IndexedAccessType = 170,
        MappedType = 171,
        LiteralType = 172,
        ObjectBindingPattern = 173,
        ArrayBindingPattern = 174,
        BindingElement = 175,
        ArrayLiteralExpression = 176,
        ObjectLiteralExpression = 177,
        PropertyAccessExpression = 178,
        ElementAccessExpression = 179,
        CallExpression = 180,
        NewExpression = 181,
        TaggedTemplateExpression = 182,
        TypeAssertionExpression = 183,
        ParenthesizedExpression = 184,
        FunctionExpression = 185,
        ArrowFunction = 186,
        DeleteExpression = 187,
        TypeOfExpression = 188,
        VoidExpression = 189,
        AwaitExpression = 190,
        PrefixUnaryExpression = 191,
        PostfixUnaryExpression = 192,
        BinaryExpression = 193,
        ConditionalExpression = 194,
        TemplateExpression = 195,
        YieldExpression = 196,
        SpreadElement = 197,
        ClassExpression = 198,
        OmittedExpression = 199,
        ExpressionWithTypeArguments = 200,
        AsExpression = 201,
        NonNullExpression = 202,
        MetaProperty = 203,
        TemplateSpan = 204,
        SemicolonClassElement = 205,
        Block = 206,
        VariableStatement = 207,
        EmptyStatement = 208,
        ExpressionStatement = 209,
        IfStatement = 210,
        DoStatement = 211,
        WhileStatement = 212,
        ForStatement = 213,
        ForInStatement = 214,
        ForOfStatement = 215,
        ContinueStatement = 216,
        BreakStatement = 217,
        ReturnStatement = 218,
        WithStatement = 219,
        SwitchStatement = 220,
        LabeledStatement = 221,
        ThrowStatement = 222,
        TryStatement = 223,
        DebuggerStatement = 224,
        VariableDeclaration = 225,
        VariableDeclarationList = 226,
        FunctionDeclaration = 227,
        ClassDeclaration = 228,
        InterfaceDeclaration = 229,
        TypeAliasDeclaration = 230,
        EnumDeclaration = 231,
        ModuleDeclaration = 232,
        ModuleBlock = 233,
        CaseBlock = 234,
        NamespaceExportDeclaration = 235,
        ImportEqualsDeclaration = 236,
        ImportDeclaration = 237,
        ImportClause = 238,
        NamespaceImport = 239,
        NamedImports = 240,
        ImportSpecifier = 241,
        ExportAssignment = 242,
        ExportDeclaration = 243,
        NamedExports = 244,
        ExportSpecifier = 245,
        MissingDeclaration = 246,
        ExternalModuleReference = 247,
        JsxElement = 248,
        JsxSelfClosingElement = 249,
        JsxOpeningElement = 250,
        JsxClosingElement = 251,
        JsxAttribute = 252,
        JsxSpreadAttribute = 253,
        JsxExpression = 254,
        CaseClause = 255,
        DefaultClause = 256,
        HeritageClause = 257,
        CatchClause = 258,
        PropertyAssignment = 259,
        ShorthandPropertyAssignment = 260,
        SpreadAssignment = 261,
        EnumMember = 262,
        SourceFile = 263,
        Bundle = 264,
        JSDocTypeExpression = 265,
        JSDocAllType = 266,
        JSDocUnknownType = 267,
        JSDocArrayType = 268,
        JSDocUnionType = 269,
        JSDocTupleType = 270,
        JSDocNullableType = 271,
        JSDocNonNullableType = 272,
        JSDocRecordType = 273,
        JSDocRecordMember = 274,
        JSDocTypeReference = 275,
        JSDocOptionalType = 276,
        JSDocFunctionType = 277,
        JSDocVariadicType = 278,
        JSDocConstructorType = 279,
        JSDocThisType = 280,
        JSDocComment = 281,
        JSDocTag = 282,
        JSDocAugmentsTag = 283,
        JSDocParameterTag = 284,
        JSDocReturnTag = 285,
        JSDocTypeTag = 286,
        JSDocTemplateTag = 287,
        JSDocTypedefTag = 288,
        JSDocPropertyTag = 289,
        JSDocTypeLiteral = 290,
        JSDocLiteralType = 291,
        JSDocNullKeyword = 292,
        JSDocUndefinedKeyword = 293,
        JSDocNeverKeyword = 294,
        SyntaxList = 295,
        NotEmittedStatement = 296,
        PartiallyEmittedExpression = 297,
        MergeDeclarationMarker = 298,
        EndOfDeclarationMarker = 299,
        Count = 300,
        FirstAssignment = 57,
        LastAssignment = 69,
        FirstCompoundAssignment = 58,
        LastCompoundAssignment = 69,
        FirstReservedWord = 71,
        LastReservedWord = 106,
        FirstKeyword = 71,
        LastKeyword = 141,
        FirstFutureReservedWord = 107,
        LastFutureReservedWord = 115,
        FirstTypeNode = 157,
        LastTypeNode = 172,
        FirstPunctuation = 16,
        LastPunctuation = 69,
        FirstToken = 0,
        LastToken = 141,
        FirstTriviaToken = 2,
        LastTriviaToken = 7,
        FirstLiteralToken = 8,
        LastLiteralToken = 12,
        FirstTemplateToken = 12,
        LastTemplateToken = 15,
        FirstBinaryOperator = 26,
        LastBinaryOperator = 69,
        FirstNode = 142,
        FirstJSDocNode = 265,
        LastJSDocNode = 294,
        FirstJSDocTagNode = 281,
        LastJSDocTagNode = 294,
    }
    const enum NodeFlags {
        None = 0,
        Let = 1,
        Const = 2,
        NestedNamespace = 4,
        Synthesized = 8,
        Namespace = 16,
        ExportContext = 32,
        ContainsThis = 64,
        HasImplicitReturn = 128,
        HasExplicitReturn = 256,
        GlobalAugmentation = 512,
        HasAsyncFunctions = 1024,
        DisallowInContext = 2048,
        YieldContext = 4096,
        DecoratorContext = 8192,
        AwaitContext = 16384,
        ThisNodeHasError = 32768,
        JavaScriptFile = 65536,
        ThisNodeOrAnySubNodesHasError = 131072,
        HasAggregatedChildData = 262144,
        BlockScoped = 3,
        ReachabilityCheckFlags = 384,
        ReachabilityAndEmitFlags = 1408,
        ContextFlags = 96256,
        TypeExcludesFlags = 20480,
    }
    const enum ModifierFlags {
        None = 0,
        Export = 1,
        Ambient = 2,
        Public = 4,
        Private = 8,
        Protected = 16,
        Static = 32,
        Readonly = 64,
        Abstract = 128,
        Async = 256,
        Default = 512,
        Const = 2048,
        HasComputedFlags = 536870912,
        AccessibilityModifier = 28,
        ParameterPropertyModifier = 92,
        NonPublicAccessibilityModifier = 24,
        TypeScriptModifier = 2270,
        ExportDefault = 513,
    }
    const enum JsxFlags {
        None = 0,
        /** An element from a named property of the JSX.IntrinsicElements interface */
        IntrinsicNamedElement = 1,
        /** An element inferred from the string index signature of the JSX.IntrinsicElements interface */
        IntrinsicIndexedElement = 2,
        IntrinsicElement = 3,
    }
    interface Node extends TextRange {
        kind: SyntaxKind;
        flags: NodeFlags;
        decorators?: NodeArray<Decorator>;
        modifiers?: ModifiersArray;
        parent?: Node;
    }
    interface NodeArray<T extends Node> extends Array<T>, TextRange {
        hasTrailingComma?: boolean;
    }
    interface Token<TKind extends SyntaxKind> extends Node {
        kind: TKind;
    }
    type DotDotDotToken = Token<SyntaxKind.DotDotDotToken>;
    type QuestionToken = Token<SyntaxKind.QuestionToken>;
    type ColonToken = Token<SyntaxKind.ColonToken>;
    type EqualsToken = Token<SyntaxKind.EqualsToken>;
    type AsteriskToken = Token<SyntaxKind.AsteriskToken>;
    type EqualsGreaterThanToken = Token<SyntaxKind.EqualsGreaterThanToken>;
    type EndOfFileToken = Token<SyntaxKind.EndOfFileToken>;
    type AtToken = Token<SyntaxKind.AtToken>;
    type ReadonlyToken = Token<SyntaxKind.ReadonlyKeyword>;
    type Modifier = Token<SyntaxKind.AbstractKeyword> | Token<SyntaxKind.AsyncKeyword> | Token<SyntaxKind.ConstKeyword> | Token<SyntaxKind.DeclareKeyword> | Token<SyntaxKind.DefaultKeyword> | Token<SyntaxKind.ExportKeyword> | Token<SyntaxKind.PublicKeyword> | Token<SyntaxKind.PrivateKeyword> | Token<SyntaxKind.ProtectedKeyword> | Token<SyntaxKind.ReadonlyKeyword> | Token<SyntaxKind.StaticKeyword>;
    type ModifiersArray = NodeArray<Modifier>;
    interface Identifier extends PrimaryExpression {
        kind: SyntaxKind.Identifier;
        text: string;
        originalKeywordKind?: SyntaxKind;
        isInJSDocNamespace?: boolean;
    }
    interface TransientIdentifier extends Identifier {
        resolvedSymbol: Symbol;
    }
    interface QualifiedName extends Node {
        kind: SyntaxKind.QualifiedName;
        left: EntityName;
        right: Identifier;
    }
    type EntityName = Identifier | QualifiedName;
    type PropertyName = Identifier | StringLiteral | NumericLiteral | ComputedPropertyName;
    type DeclarationName = Identifier | StringLiteral | NumericLiteral | ComputedPropertyName | BindingPattern;
    interface Declaration extends Node {
        _declarationBrand: any;
        name?: DeclarationName;
    }
    interface DeclarationStatement extends Declaration, Statement {
        name?: Identifier | StringLiteral | NumericLiteral;
    }
    interface ComputedPropertyName extends Node {
        kind: SyntaxKind.ComputedPropertyName;
        expression: Expression;
    }
    interface Decorator extends Node {
        kind: SyntaxKind.Decorator;
        expression: LeftHandSideExpression;
    }
    interface TypeParameterDeclaration extends Declaration {
        kind: SyntaxKind.TypeParameter;
        name: Identifier;
        constraint?: TypeNode;
        expression?: Expression;
    }
    interface SignatureDeclaration extends Declaration {
        name?: PropertyName;
        typeParameters?: NodeArray<TypeParameterDeclaration>;
        parameters: NodeArray<ParameterDeclaration>;
        type?: TypeNode;
    }
    interface CallSignatureDeclaration extends SignatureDeclaration, TypeElement {
        kind: SyntaxKind.CallSignature;
    }
    interface ConstructSignatureDeclaration extends SignatureDeclaration, TypeElement {
        kind: SyntaxKind.ConstructSignature;
    }
    type BindingName = Identifier | BindingPattern;
    interface VariableDeclaration extends Declaration {
        kind: SyntaxKind.VariableDeclaration;
        parent?: VariableDeclarationList;
        name: BindingName;
        type?: TypeNode;
        initializer?: Expression;
    }
    interface VariableDeclarationList extends Node {
        kind: SyntaxKind.VariableDeclarationList;
        declarations: NodeArray<VariableDeclaration>;
    }
    interface ParameterDeclaration extends Declaration {
        kind: SyntaxKind.Parameter;
        dotDotDotToken?: DotDotDotToken;
        name: BindingName;
        questionToken?: QuestionToken;
        type?: TypeNode;
        initializer?: Expression;
    }
    interface BindingElement extends Declaration {
        kind: SyntaxKind.BindingElement;
        propertyName?: PropertyName;
        dotDotDotToken?: DotDotDotToken;
        name: BindingName;
        initializer?: Expression;
    }
    interface PropertySignature extends TypeElement {
        kind: SyntaxKind.PropertySignature | SyntaxKind.JSDocRecordMember;
        name: PropertyName;
        questionToken?: QuestionToken;
        type?: TypeNode;
        initializer?: Expression;
    }
    interface PropertyDeclaration extends ClassElement {
        kind: SyntaxKind.PropertyDeclaration;
        questionToken?: QuestionToken;
        name: PropertyName;
        type?: TypeNode;
        initializer?: Expression;
    }
    interface ObjectLiteralElement extends Declaration {
        _objectLiteralBrandBrand: any;
        name?: PropertyName;
    }
    type ObjectLiteralElementLike = PropertyAssignment | ShorthandPropertyAssignment | MethodDeclaration | AccessorDeclaration | SpreadAssignment;
    interface PropertyAssignment extends ObjectLiteralElement {
        kind: SyntaxKind.PropertyAssignment;
        name: PropertyName;
        questionToken?: QuestionToken;
        initializer: Expression;
    }
    interface ShorthandPropertyAssignment extends ObjectLiteralElement {
        kind: SyntaxKind.ShorthandPropertyAssignment;
        name: Identifier;
        questionToken?: QuestionToken;
        equalsToken?: Token<SyntaxKind.EqualsToken>;
        objectAssignmentInitializer?: Expression;
    }
    interface SpreadAssignment extends ObjectLiteralElement {
        kind: SyntaxKind.SpreadAssignment;
        expression: Expression;
    }
    interface VariableLikeDeclaration extends Declaration {
        propertyName?: PropertyName;
        dotDotDotToken?: DotDotDotToken;
        name: DeclarationName;
        questionToken?: QuestionToken;
        type?: TypeNode;
        initializer?: Expression;
    }
    interface PropertyLikeDeclaration extends Declaration {
        name: PropertyName;
    }
    interface ObjectBindingPattern extends Node {
        kind: SyntaxKind.ObjectBindingPattern;
        elements: NodeArray<BindingElement>;
    }
    interface ArrayBindingPattern extends Node {
        kind: SyntaxKind.ArrayBindingPattern;
        elements: NodeArray<ArrayBindingElement>;
    }
    type BindingPattern = ObjectBindingPattern | ArrayBindingPattern;
    type ArrayBindingElement = BindingElement | OmittedExpression;
    /**
     * Several node kinds share function-like features such as a signature,
     * a name, and a body. These nodes should extend FunctionLikeDeclaration.
     * Examples:
     * - FunctionDeclaration
     * - MethodDeclaration
     * - AccessorDeclaration
     */
    interface FunctionLikeDeclaration extends SignatureDeclaration {
        _functionLikeDeclarationBrand: any;
        asteriskToken?: AsteriskToken;
        questionToken?: QuestionToken;
        body?: Block | Expression;
    }
    interface FunctionDeclaration extends FunctionLikeDeclaration, DeclarationStatement {
        kind: SyntaxKind.FunctionDeclaration;
        name?: Identifier;
        body?: FunctionBody;
    }
    interface MethodSignature extends SignatureDeclaration, TypeElement {
        kind: SyntaxKind.MethodSignature;
        name: PropertyName;
    }
    interface MethodDeclaration extends FunctionLikeDeclaration, ClassElement, ObjectLiteralElement {
        kind: SyntaxKind.MethodDeclaration;
        name: PropertyName;
        body?: FunctionBody;
    }
    interface ConstructorDeclaration extends FunctionLikeDeclaration, ClassElement {
        kind: SyntaxKind.Constructor;
        body?: FunctionBody;
    }
    interface SemicolonClassElement extends ClassElement {
        kind: SyntaxKind.SemicolonClassElement;
    }
    interface GetAccessorDeclaration extends FunctionLikeDeclaration, ClassElement, ObjectLiteralElement {
        kind: SyntaxKind.GetAccessor;
        name: PropertyName;
        body: FunctionBody;
    }
    interface SetAccessorDeclaration extends FunctionLikeDeclaration, ClassElement, ObjectLiteralElement {
        kind: SyntaxKind.SetAccessor;
        name: PropertyName;
        body: FunctionBody;
    }
    type AccessorDeclaration = GetAccessorDeclaration | SetAccessorDeclaration;
    interface IndexSignatureDeclaration extends SignatureDeclaration, ClassElement, TypeElement {
        kind: SyntaxKind.IndexSignature;
    }
    interface TypeNode extends Node {
        _typeNodeBrand: any;
    }
    interface KeywordTypeNode extends TypeNode {
        kind: SyntaxKind.AnyKeyword | SyntaxKind.NumberKeyword | SyntaxKind.ObjectKeyword | SyntaxKind.BooleanKeyword | SyntaxKind.StringKeyword | SyntaxKind.SymbolKeyword | SyntaxKind.VoidKeyword;
    }
    interface ThisTypeNode extends TypeNode {
        kind: SyntaxKind.ThisType;
    }
    interface FunctionOrConstructorTypeNode extends TypeNode, SignatureDeclaration {
        kind: SyntaxKind.FunctionType | SyntaxKind.ConstructorType;
    }
    interface FunctionTypeNode extends FunctionOrConstructorTypeNode {
        kind: SyntaxKind.FunctionType;
    }
    interface ConstructorTypeNode extends FunctionOrConstructorTypeNode {
        kind: SyntaxKind.ConstructorType;
    }
    interface TypeReferenceNode extends TypeNode {
        kind: SyntaxKind.TypeReference;
        typeName: EntityName;
        typeArguments?: NodeArray<TypeNode>;
    }
    interface TypePredicateNode extends TypeNode {
        kind: SyntaxKind.TypePredicate;
        parameterName: Identifier | ThisTypeNode;
        type: TypeNode;
    }
    interface TypeQueryNode extends TypeNode {
        kind: SyntaxKind.TypeQuery;
        exprName: EntityName;
    }
    interface TypeLiteralNode extends TypeNode, Declaration {
        kind: SyntaxKind.TypeLiteral;
        members: NodeArray<TypeElement>;
    }
    interface ArrayTypeNode extends TypeNode {
        kind: SyntaxKind.ArrayType;
        elementType: TypeNode;
    }
    interface TupleTypeNode extends TypeNode {
        kind: SyntaxKind.TupleType;
        elementTypes: NodeArray<TypeNode>;
    }
    interface UnionOrIntersectionTypeNode extends TypeNode {
        kind: SyntaxKind.UnionType | SyntaxKind.IntersectionType;
        types: NodeArray<TypeNode>;
    }
    interface UnionTypeNode extends UnionOrIntersectionTypeNode {
        kind: SyntaxKind.UnionType;
    }
    interface IntersectionTypeNode extends UnionOrIntersectionTypeNode {
        kind: SyntaxKind.IntersectionType;
    }
    interface ParenthesizedTypeNode extends TypeNode {
        kind: SyntaxKind.ParenthesizedType;
        type: TypeNode;
    }
    interface TypeOperatorNode extends TypeNode {
        kind: SyntaxKind.TypeOperator;
        operator: SyntaxKind.KeyOfKeyword;
        type: TypeNode;
    }
    interface IndexedAccessTypeNode extends TypeNode {
        kind: SyntaxKind.IndexedAccessType;
        objectType: TypeNode;
        indexType: TypeNode;
    }
    interface MappedTypeNode extends TypeNode, Declaration {
        kind: SyntaxKind.MappedType;
        readonlyToken?: ReadonlyToken;
        typeParameter: TypeParameterDeclaration;
        questionToken?: QuestionToken;
        type?: TypeNode;
    }
    interface LiteralTypeNode extends TypeNode {
        kind: SyntaxKind.LiteralType;
        literal: Expression;
    }
    interface StringLiteral extends LiteralExpression {
        kind: SyntaxKind.StringLiteral;
    }
    interface Expression extends Node {
        _expressionBrand: any;
        contextualType?: Type;
    }
    interface OmittedExpression extends Expression {
        kind: SyntaxKind.OmittedExpression;
    }
    interface PartiallyEmittedExpression extends LeftHandSideExpression {
        kind: SyntaxKind.PartiallyEmittedExpression;
        expression: Expression;
    }
    interface UnaryExpression extends Expression {
        _unaryExpressionBrand: any;
    }
    interface IncrementExpression extends UnaryExpression {
        _incrementExpressionBrand: any;
    }
    type PrefixUnaryOperator = SyntaxKind.PlusPlusToken | SyntaxKind.MinusMinusToken | SyntaxKind.PlusToken | SyntaxKind.MinusToken | SyntaxKind.TildeToken | SyntaxKind.ExclamationToken;
    interface PrefixUnaryExpression extends IncrementExpression {
        kind: SyntaxKind.PrefixUnaryExpression;
        operator: PrefixUnaryOperator;
        operand: UnaryExpression;
    }
    type PostfixUnaryOperator = SyntaxKind.PlusPlusToken | SyntaxKind.MinusMinusToken;
    interface PostfixUnaryExpression extends IncrementExpression {
        kind: SyntaxKind.PostfixUnaryExpression;
        operand: LeftHandSideExpression;
        operator: PostfixUnaryOperator;
    }
    interface LeftHandSideExpression extends IncrementExpression {
        _leftHandSideExpressionBrand: any;
    }
    interface MemberExpression extends LeftHandSideExpression {
        _memberExpressionBrand: any;
    }
    interface PrimaryExpression extends MemberExpression {
        _primaryExpressionBrand: any;
    }
    interface NullLiteral extends PrimaryExpression {
        kind: SyntaxKind.NullKeyword;
    }
    interface BooleanLiteral extends PrimaryExpression {
        kind: SyntaxKind.TrueKeyword | SyntaxKind.FalseKeyword;
    }
    interface ThisExpression extends PrimaryExpression {
        kind: SyntaxKind.ThisKeyword;
    }
    interface SuperExpression extends PrimaryExpression {
        kind: SyntaxKind.SuperKeyword;
    }
    interface DeleteExpression extends UnaryExpression {
        kind: SyntaxKind.DeleteExpression;
        expression: UnaryExpression;
    }
    interface TypeOfExpression extends UnaryExpression {
        kind: SyntaxKind.TypeOfExpression;
        expression: UnaryExpression;
    }
    interface VoidExpression extends UnaryExpression {
        kind: SyntaxKind.VoidExpression;
        expression: UnaryExpression;
    }
    interface AwaitExpression extends UnaryExpression {
        kind: SyntaxKind.AwaitExpression;
        expression: UnaryExpression;
    }
    interface YieldExpression extends Expression {
        kind: SyntaxKind.YieldExpression;
        asteriskToken?: AsteriskToken;
        expression?: Expression;
    }
    type ExponentiationOperator = SyntaxKind.AsteriskAsteriskToken;
    type MultiplicativeOperator = SyntaxKind.AsteriskToken | SyntaxKind.SlashToken | SyntaxKind.PercentToken;
    type MultiplicativeOperatorOrHigher = ExponentiationOperator | MultiplicativeOperator;
    type AdditiveOperator = SyntaxKind.PlusToken | SyntaxKind.MinusToken;
    type AdditiveOperatorOrHigher = MultiplicativeOperatorOrHigher | AdditiveOperator;
    type ShiftOperator = SyntaxKind.LessThanLessThanToken | SyntaxKind.GreaterThanGreaterThanToken | SyntaxKind.GreaterThanGreaterThanGreaterThanToken;
    type ShiftOperatorOrHigher = AdditiveOperatorOrHigher | ShiftOperator;
    type RelationalOperator = SyntaxKind.LessThanToken | SyntaxKind.LessThanEqualsToken | SyntaxKind.GreaterThanToken | SyntaxKind.GreaterThanEqualsToken | SyntaxKind.InstanceOfKeyword | SyntaxKind.InKeyword;
    type RelationalOperatorOrHigher = ShiftOperatorOrHigher | RelationalOperator;
    type EqualityOperator = SyntaxKind.EqualsEqualsToken | SyntaxKind.EqualsEqualsEqualsToken | SyntaxKind.ExclamationEqualsEqualsToken | SyntaxKind.ExclamationEqualsToken;
    type EqualityOperatorOrHigher = RelationalOperatorOrHigher | EqualityOperator;
    type BitwiseOperator = SyntaxKind.AmpersandToken | SyntaxKind.BarToken | SyntaxKind.CaretToken;
    type BitwiseOperatorOrHigher = EqualityOperatorOrHigher | BitwiseOperator;
    type LogicalOperator = SyntaxKind.AmpersandAmpersandToken | SyntaxKind.BarBarToken;
    type LogicalOperatorOrHigher = BitwiseOperatorOrHigher | LogicalOperator;
    type CompoundAssignmentOperator = SyntaxKind.PlusEqualsToken | SyntaxKind.MinusEqualsToken | SyntaxKind.AsteriskAsteriskEqualsToken | SyntaxKind.AsteriskEqualsToken | SyntaxKind.SlashEqualsToken | SyntaxKind.PercentEqualsToken | SyntaxKind.AmpersandEqualsToken | SyntaxKind.BarEqualsToken | SyntaxKind.CaretEqualsToken | SyntaxKind.LessThanLessThanEqualsToken | SyntaxKind.GreaterThanGreaterThanGreaterThanEqualsToken | SyntaxKind.GreaterThanGreaterThanEqualsToken;
    type AssignmentOperator = SyntaxKind.EqualsToken | CompoundAssignmentOperator;
    type AssignmentOperatorOrHigher = LogicalOperatorOrHigher | AssignmentOperator;
    type BinaryOperator = AssignmentOperatorOrHigher | SyntaxKind.CommaToken;
    type BinaryOperatorToken = Token<BinaryOperator>;
    interface BinaryExpression extends Expression, Declaration {
        kind: SyntaxKind.BinaryExpression;
        left: Expression;
        operatorToken: BinaryOperatorToken;
        right: Expression;
    }
    type AssignmentOperatorToken = Token<AssignmentOperator>;
    interface AssignmentExpression<TOperator extends AssignmentOperatorToken> extends BinaryExpression {
        left: LeftHandSideExpression;
        operatorToken: TOperator;
    }
    interface ObjectDestructuringAssignment extends AssignmentExpression<EqualsToken> {
        left: ObjectLiteralExpression;
    }
    interface ArrayDestructuringAssignment extends AssignmentExpression<EqualsToken> {
        left: ArrayLiteralExpression;
    }
    type DestructuringAssignment = ObjectDestructuringAssignment | ArrayDestructuringAssignment;
    type BindingOrAssignmentElement = VariableDeclaration | ParameterDeclaration | BindingElement | PropertyAssignment | ShorthandPropertyAssignment | SpreadAssignment | OmittedExpression | SpreadElement | ArrayLiteralExpression | ObjectLiteralExpression | AssignmentExpression<EqualsToken> | Identifier | PropertyAccessExpression | ElementAccessExpression;
    type BindingOrAssignmentElementRestIndicator = DotDotDotToken | SpreadElement | SpreadAssignment;
    type BindingOrAssignmentElementTarget = BindingOrAssignmentPattern | Expression;
    type ObjectBindingOrAssignmentPattern = ObjectBindingPattern | ObjectLiteralExpression;
    type ArrayBindingOrAssignmentPattern = ArrayBindingPattern | ArrayLiteralExpression;
    type AssignmentPattern = ObjectLiteralExpression | ArrayLiteralExpression;
    type BindingOrAssignmentPattern = ObjectBindingOrAssignmentPattern | ArrayBindingOrAssignmentPattern;
    interface ConditionalExpression extends Expression {
        kind: SyntaxKind.ConditionalExpression;
        condition: Expression;
        questionToken: QuestionToken;
        whenTrue: Expression;
        colonToken: ColonToken;
        whenFalse: Expression;
    }
    type FunctionBody = Block;
    type ConciseBody = FunctionBody | Expression;
    interface FunctionExpression extends PrimaryExpression, FunctionLikeDeclaration {
        kind: SyntaxKind.FunctionExpression;
        name?: Identifier;
        body: FunctionBody;
    }
    interface ArrowFunction extends Expression, FunctionLikeDeclaration {
        kind: SyntaxKind.ArrowFunction;
        equalsGreaterThanToken: EqualsGreaterThanToken;
        body: ConciseBody;
    }
    interface LiteralLikeNode extends Node {
        text: string;
        isUnterminated?: boolean;
        hasExtendedUnicodeEscape?: boolean;
    }
    interface LiteralExpression extends LiteralLikeNode, PrimaryExpression {
        _literalExpressionBrand: any;
    }
    interface RegularExpressionLiteral extends LiteralExpression {
        kind: SyntaxKind.RegularExpressionLiteral;
    }
    interface NoSubstitutionTemplateLiteral extends LiteralExpression {
        kind: SyntaxKind.NoSubstitutionTemplateLiteral;
    }
    interface NumericLiteral extends LiteralExpression {
        kind: SyntaxKind.NumericLiteral;
        trailingComment?: string;
    }
    interface TemplateHead extends LiteralLikeNode {
        kind: SyntaxKind.TemplateHead;
    }
    interface TemplateMiddle extends LiteralLikeNode {
        kind: SyntaxKind.TemplateMiddle;
    }
    interface TemplateTail extends LiteralLikeNode {
        kind: SyntaxKind.TemplateTail;
    }
    type TemplateLiteral = TemplateExpression | NoSubstitutionTemplateLiteral;
    interface TemplateExpression extends PrimaryExpression {
        kind: SyntaxKind.TemplateExpression;
        head: TemplateHead;
        templateSpans: NodeArray<TemplateSpan>;
    }
    interface TemplateSpan extends Node {
        kind: SyntaxKind.TemplateSpan;
        expression: Expression;
        literal: TemplateMiddle | TemplateTail;
    }
    interface ParenthesizedExpression extends PrimaryExpression {
        kind: SyntaxKind.ParenthesizedExpression;
        expression: Expression;
    }
    interface ArrayLiteralExpression extends PrimaryExpression {
        kind: SyntaxKind.ArrayLiteralExpression;
        elements: NodeArray<Expression>;
    }
    interface SpreadElement extends Expression {
        kind: SyntaxKind.SpreadElement;
        expression: Expression;
    }
    /**
      * This interface is a base interface for ObjectLiteralExpression and JSXAttributes to extend from. JSXAttributes is similar to
      * ObjectLiteralExpression in that it contains array of properties; however, JSXAttributes' properties can only be
      * JSXAttribute or JSXSpreadAttribute. ObjectLiteralExpression, on the other hand, can only have properties of type
      * ObjectLiteralElement (e.g. PropertyAssignment, ShorthandPropertyAssignment etc.)
     **/
    interface ObjectLiteralExpressionBase<T extends ObjectLiteralElement> extends PrimaryExpression, Declaration {
        properties: NodeArray<T>;
    }
    interface ObjectLiteralExpression extends ObjectLiteralExpressionBase<ObjectLiteralElementLike> {
        kind: SyntaxKind.ObjectLiteralExpression;
    }
    type EntityNameExpression = Identifier | PropertyAccessEntityNameExpression;
    type EntityNameOrEntityNameExpression = EntityName | EntityNameExpression;
    interface PropertyAccessExpression extends MemberExpression, Declaration {
        kind: SyntaxKind.PropertyAccessExpression;
        expression: LeftHandSideExpression;
        name: Identifier;
    }
    interface SuperPropertyAccessExpression extends PropertyAccessExpression {
        expression: SuperExpression;
    }
    /** Brand for a PropertyAccessExpression which, like a QualifiedName, consists of a sequence of identifiers separated by dots. */
    interface PropertyAccessEntityNameExpression extends PropertyAccessExpression {
        _propertyAccessExpressionLikeQualifiedNameBrand?: any;
        expression: EntityNameExpression;
    }
    interface ElementAccessExpression extends MemberExpression {
        kind: SyntaxKind.ElementAccessExpression;
        expression: LeftHandSideExpression;
        argumentExpression?: Expression;
    }
    interface SuperElementAccessExpression extends ElementAccessExpression {
        expression: SuperExpression;
    }
    type SuperProperty = SuperPropertyAccessExpression | SuperElementAccessExpression;
    interface CallExpression extends LeftHandSideExpression, Declaration {
        kind: SyntaxKind.CallExpression;
        expression: LeftHandSideExpression;
        typeArguments?: NodeArray<TypeNode>;
        arguments: NodeArray<Expression>;
    }
    interface SuperCall extends CallExpression {
        expression: SuperExpression;
    }
    interface ExpressionWithTypeArguments extends TypeNode {
        kind: SyntaxKind.ExpressionWithTypeArguments;
        expression: LeftHandSideExpression;
        typeArguments?: NodeArray<TypeNode>;
    }
    interface NewExpression extends PrimaryExpression, Declaration {
        kind: SyntaxKind.NewExpression;
        expression: LeftHandSideExpression;
        typeArguments?: NodeArray<TypeNode>;
        arguments: NodeArray<Expression>;
    }
    interface TaggedTemplateExpression extends MemberExpression {
        kind: SyntaxKind.TaggedTemplateExpression;
        tag: LeftHandSideExpression;
        template: TemplateLiteral;
    }
    type CallLikeExpression = CallExpression | NewExpression | TaggedTemplateExpression | Decorator;
    interface AsExpression extends Expression {
        kind: SyntaxKind.AsExpression;
        expression: Expression;
        type: TypeNode;
    }
    interface TypeAssertion extends UnaryExpression {
        kind: SyntaxKind.TypeAssertionExpression;
        type: TypeNode;
        expression: UnaryExpression;
    }
    type AssertionExpression = TypeAssertion | AsExpression;
    interface NonNullExpression extends LeftHandSideExpression {
        kind: SyntaxKind.NonNullExpression;
        expression: Expression;
    }
    interface MetaProperty extends PrimaryExpression {
        kind: SyntaxKind.MetaProperty;
        keywordToken: SyntaxKind;
        name: Identifier;
    }
    interface JsxElement extends PrimaryExpression {
        kind: SyntaxKind.JsxElement;
        openingElement: JsxOpeningElement;
        children: NodeArray<JsxChild>;
        closingElement: JsxClosingElement;
    }
    type JsxTagNameExpression = PrimaryExpression | PropertyAccessExpression;
    interface JsxOpeningElement extends Expression {
        kind: SyntaxKind.JsxOpeningElement;
        tagName: JsxTagNameExpression;
        attributes: NodeArray<JsxAttribute | JsxSpreadAttribute>;
    }
    interface JsxSelfClosingElement extends PrimaryExpression {
        kind: SyntaxKind.JsxSelfClosingElement;
        tagName: JsxTagNameExpression;
        attributes: NodeArray<JsxAttribute | JsxSpreadAttribute>;
    }
    type JsxOpeningLikeElement = JsxSelfClosingElement | JsxOpeningElement;
    type JsxAttributeLike = JsxAttribute | JsxSpreadAttribute;
    interface JsxAttribute extends Node {
        kind: SyntaxKind.JsxAttribute;
        name: Identifier;
        initializer?: StringLiteral | JsxExpression;
    }
    interface JsxSpreadAttribute extends Node {
        kind: SyntaxKind.JsxSpreadAttribute;
        expression: Expression;
    }
    interface JsxClosingElement extends Node {
        kind: SyntaxKind.JsxClosingElement;
        tagName: JsxTagNameExpression;
    }
    interface JsxExpression extends Expression {
        kind: SyntaxKind.JsxExpression;
        dotDotDotToken?: Token<SyntaxKind.DotDotDotToken>;
        expression?: Expression;
    }
    interface JsxText extends Node {
        kind: SyntaxKind.JsxText;
    }
    type JsxChild = JsxText | JsxExpression | JsxElement | JsxSelfClosingElement;
    interface Statement extends Node {
        _statementBrand: any;
    }
    interface NotEmittedStatement extends Statement {
        kind: SyntaxKind.NotEmittedStatement;
    }
    interface EmptyStatement extends Statement {
        kind: SyntaxKind.EmptyStatement;
    }
    interface DebuggerStatement extends Statement {
        kind: SyntaxKind.DebuggerStatement;
    }
    interface MissingDeclaration extends DeclarationStatement, ClassElement, ObjectLiteralElement, TypeElement {
        kind: SyntaxKind.MissingDeclaration;
        name?: Identifier;
    }
    type BlockLike = SourceFile | Block | ModuleBlock | CaseOrDefaultClause;
    interface Block extends Statement {
        kind: SyntaxKind.Block;
        statements: NodeArray<Statement>;
    }
    interface VariableStatement extends Statement {
        kind: SyntaxKind.VariableStatement;
        declarationList: VariableDeclarationList;
    }
    interface ExpressionStatement extends Statement {
        kind: SyntaxKind.ExpressionStatement;
        expression: Expression;
    }
    interface IfStatement extends Statement {
        kind: SyntaxKind.IfStatement;
        expression: Expression;
        thenStatement: Statement;
        elseStatement?: Statement;
    }
    interface IterationStatement extends Statement {
        statement: Statement;
    }
    interface DoStatement extends IterationStatement {
        kind: SyntaxKind.DoStatement;
        expression: Expression;
    }
    interface WhileStatement extends IterationStatement {
        kind: SyntaxKind.WhileStatement;
        expression: Expression;
    }
    type ForInitializer = VariableDeclarationList | Expression;
    interface ForStatement extends IterationStatement {
        kind: SyntaxKind.ForStatement;
        initializer?: ForInitializer;
        condition?: Expression;
        incrementor?: Expression;
    }
    interface ForInStatement extends IterationStatement {
        kind: SyntaxKind.ForInStatement;
        initializer: ForInitializer;
        expression: Expression;
    }
    interface ForOfStatement extends IterationStatement {
        kind: SyntaxKind.ForOfStatement;
        initializer: ForInitializer;
        expression: Expression;
    }
    interface BreakStatement extends Statement {
        kind: SyntaxKind.BreakStatement;
        label?: Identifier;
    }
    interface ContinueStatement extends Statement {
        kind: SyntaxKind.ContinueStatement;
        label?: Identifier;
    }
    type BreakOrContinueStatement = BreakStatement | ContinueStatement;
    interface ReturnStatement extends Statement {
        kind: SyntaxKind.ReturnStatement;
        expression?: Expression;
    }
    interface WithStatement extends Statement {
        kind: SyntaxKind.WithStatement;
        expression: Expression;
        statement: Statement;
    }
    interface SwitchStatement extends Statement {
        kind: SyntaxKind.SwitchStatement;
        expression: Expression;
        caseBlock: CaseBlock;
        possiblyExhaustive?: boolean;
    }
    interface CaseBlock extends Node {
        kind: SyntaxKind.CaseBlock;
        clauses: NodeArray<CaseOrDefaultClause>;
    }
    interface CaseClause extends Node {
        kind: SyntaxKind.CaseClause;
        expression: Expression;
        statements: NodeArray<Statement>;
    }
    interface DefaultClause extends Node {
        kind: SyntaxKind.DefaultClause;
        statements: NodeArray<Statement>;
    }
    type CaseOrDefaultClause = CaseClause | DefaultClause;
    interface LabeledStatement extends Statement {
        kind: SyntaxKind.LabeledStatement;
        label: Identifier;
        statement: Statement;
    }
    interface ThrowStatement extends Statement {
        kind: SyntaxKind.ThrowStatement;
        expression: Expression;
    }
    interface TryStatement extends Statement {
        kind: SyntaxKind.TryStatement;
        tryBlock: Block;
        catchClause?: CatchClause;
        finallyBlock?: Block;
    }
    interface CatchClause extends Node {
        kind: SyntaxKind.CatchClause;
        variableDeclaration: VariableDeclaration;
        block: Block;
    }
    type DeclarationWithTypeParameters = SignatureDeclaration | ClassLikeDeclaration | InterfaceDeclaration | TypeAliasDeclaration;
    interface ClassLikeDeclaration extends Declaration {
        name?: Identifier;
        typeParameters?: NodeArray<TypeParameterDeclaration>;
        heritageClauses?: NodeArray<HeritageClause>;
        members: NodeArray<ClassElement>;
    }
    interface ClassDeclaration extends ClassLikeDeclaration, DeclarationStatement {
        kind: SyntaxKind.ClassDeclaration;
        name?: Identifier;
    }
    interface ClassExpression extends ClassLikeDeclaration, PrimaryExpression {
        kind: SyntaxKind.ClassExpression;
    }
    interface ClassElement extends Declaration {
        _classElementBrand: any;
        name?: PropertyName;
    }
    interface TypeElement extends Declaration {
        _typeElementBrand: any;
        name?: PropertyName;
        questionToken?: QuestionToken;
    }
    interface InterfaceDeclaration extends DeclarationStatement {
        kind: SyntaxKind.InterfaceDeclaration;
        name: Identifier;
        typeParameters?: NodeArray<TypeParameterDeclaration>;
        heritageClauses?: NodeArray<HeritageClause>;
        members: NodeArray<TypeElement>;
    }
    interface HeritageClause extends Node {
        kind: SyntaxKind.HeritageClause;
        token: SyntaxKind;
        types?: NodeArray<ExpressionWithTypeArguments>;
    }
    interface TypeAliasDeclaration extends DeclarationStatement {
        kind: SyntaxKind.TypeAliasDeclaration;
        name: Identifier;
        typeParameters?: NodeArray<TypeParameterDeclaration>;
        type: TypeNode;
    }
    interface EnumMember extends Declaration {
        kind: SyntaxKind.EnumMember;
        name: PropertyName;
        initializer?: Expression;
    }
    interface EnumDeclaration extends DeclarationStatement {
        kind: SyntaxKind.EnumDeclaration;
        name: Identifier;
        members: NodeArray<EnumMember>;
    }
    type ModuleName = Identifier | StringLiteral;
    type ModuleBody = NamespaceBody | JSDocNamespaceBody;
    interface ModuleDeclaration extends DeclarationStatement {
        kind: SyntaxKind.ModuleDeclaration;
        name: Identifier | StringLiteral;
        body?: ModuleBody | JSDocNamespaceDeclaration | Identifier;
    }
    type NamespaceBody = ModuleBlock | NamespaceDeclaration;
    interface NamespaceDeclaration extends ModuleDeclaration {
        name: Identifier;
        body: NamespaceBody;
    }
    type JSDocNamespaceBody = Identifier | JSDocNamespaceDeclaration;
    interface JSDocNamespaceDeclaration extends ModuleDeclaration {
        name: Identifier;
        body: JSDocNamespaceBody;
    }
    interface ModuleBlock extends Node, Statement {
        kind: SyntaxKind.ModuleBlock;
        statements: NodeArray<Statement>;
    }
    type ModuleReference = EntityName | ExternalModuleReference;
    interface ImportEqualsDeclaration extends DeclarationStatement {
        kind: SyntaxKind.ImportEqualsDeclaration;
        name: Identifier;
        moduleReference: ModuleReference;
    }
    interface ExternalModuleReference extends Node {
        kind: SyntaxKind.ExternalModuleReference;
        expression?: Expression;
    }
    interface ImportDeclaration extends Statement {
        kind: SyntaxKind.ImportDeclaration;
        importClause?: ImportClause;
        moduleSpecifier: Expression;
    }
    type NamedImportBindings = NamespaceImport | NamedImports;
    interface ImportClause extends Declaration {
        kind: SyntaxKind.ImportClause;
        name?: Identifier;
        namedBindings?: NamedImportBindings;
    }
    interface NamespaceImport extends Declaration {
        kind: SyntaxKind.NamespaceImport;
        name: Identifier;
    }
    interface NamespaceExportDeclaration extends DeclarationStatement {
        kind: SyntaxKind.NamespaceExportDeclaration;
        name: Identifier;
        moduleReference: LiteralLikeNode;
    }
    interface ExportDeclaration extends DeclarationStatement {
        kind: SyntaxKind.ExportDeclaration;
        exportClause?: NamedExports;
        moduleSpecifier?: Expression;
    }
    interface NamedImports extends Node {
        kind: SyntaxKind.NamedImports;
        elements: NodeArray<ImportSpecifier>;
    }
    interface NamedExports extends Node {
        kind: SyntaxKind.NamedExports;
        elements: NodeArray<ExportSpecifier>;
    }
    type NamedImportsOrExports = NamedImports | NamedExports;
    interface ImportSpecifier extends Declaration {
        kind: SyntaxKind.ImportSpecifier;
        propertyName?: Identifier;
        name: Identifier;
    }
    interface ExportSpecifier extends Declaration {
        kind: SyntaxKind.ExportSpecifier;
        propertyName?: Identifier;
        name: Identifier;
    }
    type ImportOrExportSpecifier = ImportSpecifier | ExportSpecifier;
    interface ExportAssignment extends DeclarationStatement {
        kind: SyntaxKind.ExportAssignment;
        isExportEquals?: boolean;
        expression: Expression;
    }
    interface FileReference extends TextRange {
        fileName: string;
    }
    interface CommentRange extends TextRange {
        hasTrailingNewLine?: boolean;
        kind: SyntaxKind;
    }
    interface JSDocTypeExpression extends Node {
        kind: SyntaxKind.JSDocTypeExpression;
        type: JSDocType;
    }
    interface JSDocType extends TypeNode {
        _jsDocTypeBrand: any;
    }
    interface JSDocAllType extends JSDocType {
        kind: SyntaxKind.JSDocAllType;
    }
    interface JSDocUnknownType extends JSDocType {
        kind: SyntaxKind.JSDocUnknownType;
    }
    interface JSDocArrayType extends JSDocType {
        kind: SyntaxKind.JSDocArrayType;
        elementType: JSDocType;
    }
    interface JSDocUnionType extends JSDocType {
        kind: SyntaxKind.JSDocUnionType;
        types: NodeArray<JSDocType>;
    }
    interface JSDocTupleType extends JSDocType {
        kind: SyntaxKind.JSDocTupleType;
        types: NodeArray<JSDocType>;
    }
    interface JSDocNonNullableType extends JSDocType {
        kind: SyntaxKind.JSDocNonNullableType;
        type: JSDocType;
    }
    interface JSDocNullableType extends JSDocType {
        kind: SyntaxKind.JSDocNullableType;
        type: JSDocType;
    }
    interface JSDocRecordType extends JSDocType {
        kind: SyntaxKind.JSDocRecordType;
        literal: TypeLiteralNode;
    }
    interface JSDocTypeReference extends JSDocType {
        kind: SyntaxKind.JSDocTypeReference;
        name: EntityName;
        typeArguments: NodeArray<JSDocType>;
    }
    interface JSDocOptionalType extends JSDocType {
        kind: SyntaxKind.JSDocOptionalType;
        type: JSDocType;
    }
    interface JSDocFunctionType extends JSDocType, SignatureDeclaration {
        kind: SyntaxKind.JSDocFunctionType;
        parameters: NodeArray<ParameterDeclaration>;
        type: JSDocType;
    }
    interface JSDocVariadicType extends JSDocType {
        kind: SyntaxKind.JSDocVariadicType;
        type: JSDocType;
    }
    interface JSDocConstructorType extends JSDocType {
        kind: SyntaxKind.JSDocConstructorType;
        type: JSDocType;
    }
    interface JSDocThisType extends JSDocType {
        kind: SyntaxKind.JSDocThisType;
        type: JSDocType;
    }
    interface JSDocLiteralType extends JSDocType {
        kind: SyntaxKind.JSDocLiteralType;
        literal: LiteralTypeNode;
    }
    type JSDocTypeReferencingNode = JSDocThisType | JSDocConstructorType | JSDocVariadicType | JSDocOptionalType | JSDocNullableType | JSDocNonNullableType;
    interface JSDocRecordMember extends PropertySignature {
        kind: SyntaxKind.JSDocRecordMember;
        name: Identifier | StringLiteral | NumericLiteral;
        type?: JSDocType;
    }
    interface JSDoc extends Node {
        kind: SyntaxKind.JSDocComment;
        tags: NodeArray<JSDocTag> | undefined;
        comment: string | undefined;
    }
    interface JSDocTag extends Node {
        atToken: AtToken;
        tagName: Identifier;
        comment: string | undefined;
    }
    interface JSDocUnknownTag extends JSDocTag {
        kind: SyntaxKind.JSDocTag;
    }
    interface JSDocAugmentsTag extends JSDocTag {
        kind: SyntaxKind.JSDocAugmentsTag;
        typeExpression: JSDocTypeExpression;
    }
    interface JSDocTemplateTag extends JSDocTag {
        kind: SyntaxKind.JSDocTemplateTag;
        typeParameters: NodeArray<TypeParameterDeclaration>;
    }
    interface JSDocReturnTag extends JSDocTag {
        kind: SyntaxKind.JSDocReturnTag;
        typeExpression: JSDocTypeExpression;
    }
    interface JSDocTypeTag extends JSDocTag {
        kind: SyntaxKind.JSDocTypeTag;
        typeExpression: JSDocTypeExpression;
    }
    interface JSDocTypedefTag extends JSDocTag, Declaration {
        kind: SyntaxKind.JSDocTypedefTag;
        fullName?: JSDocNamespaceDeclaration | Identifier;
        name?: Identifier;
        typeExpression?: JSDocTypeExpression;
        jsDocTypeLiteral?: JSDocTypeLiteral;
    }
    interface JSDocPropertyTag extends JSDocTag, TypeElement {
        kind: SyntaxKind.JSDocPropertyTag;
        name: Identifier;
        typeExpression: JSDocTypeExpression;
    }
    interface JSDocTypeLiteral extends JSDocType {
        kind: SyntaxKind.JSDocTypeLiteral;
        jsDocPropertyTags?: NodeArray<JSDocPropertyTag>;
        jsDocTypeTag?: JSDocTypeTag;
    }
    interface JSDocParameterTag extends JSDocTag {
        kind: SyntaxKind.JSDocParameterTag;
        /** the parameter name, if provided *before* the type (TypeScript-style) */
        preParameterName?: Identifier;
        typeExpression?: JSDocTypeExpression;
        /** the parameter name, if provided *after* the type (JSDoc-standard) */
        postParameterName?: Identifier;
        /** the parameter name, regardless of the location it was provided */
        parameterName: Identifier;
        isBracketed: boolean;
    }
    const enum FlowFlags {
        Unreachable = 1,
        Start = 2,
        BranchLabel = 4,
        LoopLabel = 8,
        Assignment = 16,
        TrueCondition = 32,
        FalseCondition = 64,
        SwitchClause = 128,
        ArrayMutation = 256,
        Referenced = 512,
        Shared = 1024,
        PreFinally = 2048,
        AfterFinally = 4096,
        Label = 12,
        Condition = 96,
    }
    interface FlowLock {
        locked?: boolean;
    }
    interface AfterFinallyFlow extends FlowNode, FlowLock {
        antecedent: FlowNode;
    }
    interface PreFinallyFlow extends FlowNode {
        antecedent: FlowNode;
        lock: FlowLock;
    }
    interface FlowNode {
        flags: FlowFlags;
        id?: number;
    }
    interface FlowStart extends FlowNode {
        container?: FunctionExpression | ArrowFunction | MethodDeclaration;
    }
    interface FlowLabel extends FlowNode {
        antecedents: FlowNode[];
    }
    interface FlowAssignment extends FlowNode {
        node: Expression | VariableDeclaration | BindingElement;
        antecedent: FlowNode;
    }
    interface FlowCondition extends FlowNode {
        expression: Expression;
        antecedent: FlowNode;
    }
    interface FlowSwitchClause extends FlowNode {
        switchStatement: SwitchStatement;
        clauseStart: number;
        clauseEnd: number;
        antecedent: FlowNode;
    }
    interface FlowArrayMutation extends FlowNode {
        node: CallExpression | BinaryExpression;
        antecedent: FlowNode;
    }
    type FlowType = Type | IncompleteType;
    interface IncompleteType {
        flags: TypeFlags;
        type: Type;
    }
    interface AmdDependency {
        path: string;
        name: string;
    }
    interface SourceFile extends Declaration {
        kind: SyntaxKind.SourceFile;
        statements: NodeArray<Statement>;
        endOfFileToken: Token<SyntaxKind.EndOfFileToken>;
        fileName: string;
        path: Path;
        text: string;
        amdDependencies: AmdDependency[];
        moduleName: string;
        referencedFiles: FileReference[];
        typeReferenceDirectives: FileReference[];
        languageVariant: LanguageVariant;
        isDeclarationFile: boolean;
        /**
         * lib.d.ts should have a reference comment like
         *
         *  /// <reference no-default-lib="true"/>
         *
         * If any other file has this comment, it signals not to include lib.d.ts
         * because this containing file is intended to act as a default library.
         */
        hasNoDefaultLib: boolean;
        languageVersion: ScriptTarget;
    }
    interface Bundle extends Node {
        kind: SyntaxKind.Bundle;
        sourceFiles: SourceFile[];
    }
    interface ScriptReferenceHost {
        getCompilerOptions(): CompilerOptions;
        getSourceFile(fileName: string): SourceFile;
        getSourceFileByPath(path: Path): SourceFile;
        getCurrentDirectory(): string;
    }
    interface ParseConfigHost {
        useCaseSensitiveFileNames: boolean;
        readDirectory(rootDir: string, extensions: string[], excludes: string[], includes: string[]): string[];
        /**
          * Gets a value indicating whether the specified path exists and is a file.
          * @param path The path to test.
          */
        fileExists(path: string): boolean;
        readFile(path: string): string;
    }
    interface WriteFileCallback {
        (fileName: string, data: string, writeByteOrderMark: boolean, onError?: (message: string) => void, sourceFiles?: SourceFile[]): void;
    }
    class OperationCanceledException {
    }
    interface CancellationToken {
        isCancellationRequested(): boolean;
        /** @throws OperationCanceledException if isCancellationRequested is true */
        throwIfCancellationRequested(): void;
    }
    interface Program extends ScriptReferenceHost {
        /**
         * Get a list of root file names that were passed to a 'createProgram'
         */
        getRootFileNames(): string[];
        /**
         * Get a list of files in the program
         */
        getSourceFiles(): SourceFile[];
        /**
         * Emits the JavaScript and declaration files.  If targetSourceFile is not specified, then
         * the JavaScript and declaration files will be produced for all the files in this program.
         * If targetSourceFile is specified, then only the JavaScript and declaration for that
         * specific file will be generated.
         *
         * If writeFile is not specified then the writeFile callback from the compiler host will be
         * used for writing the JavaScript and declaration files.  Otherwise, the writeFile parameter
         * will be invoked when writing the JavaScript and declaration files.
         */
        emit(targetSourceFile?: SourceFile, writeFile?: WriteFileCallback, cancellationToken?: CancellationToken, emitOnlyDtsFiles?: boolean): EmitResult;
        getOptionsDiagnostics(cancellationToken?: CancellationToken): Diagnostic[];
        getGlobalDiagnostics(cancellationToken?: CancellationToken): Diagnostic[];
        getSyntacticDiagnostics(sourceFile?: SourceFile, cancellationToken?: CancellationToken): Diagnostic[];
        getSemanticDiagnostics(sourceFile?: SourceFile, cancellationToken?: CancellationToken): Diagnostic[];
        getDeclarationDiagnostics(sourceFile?: SourceFile, cancellationToken?: CancellationToken): Diagnostic[];
        /**
         * Gets a type checker that can be used to semantically analyze source fils in the program.
         */
        getTypeChecker(): TypeChecker;
    }
    interface SourceMapSpan {
        /** Line number in the .js file. */
        emittedLine: number;
        /** Column number in the .js file. */
        emittedColumn: number;
        /** Line number in the .ts file. */
        sourceLine: number;
        /** Column number in the .ts file. */
        sourceColumn: number;
        /** Optional name (index into names array) associated with this span. */
        nameIndex?: number;
        /** .ts file (index into sources array) associated with this span */
        sourceIndex: number;
    }
    interface SourceMapData {
        sourceMapFilePath: string;
        jsSourceMappingURL: string;
        sourceMapFile: string;
        sourceMapSourceRoot: string;
        sourceMapSources: string[];
        sourceMapSourcesContent?: string[];
        inputSourceFileNames: string[];
        sourceMapNames?: string[];
        sourceMapMappings: string;
        sourceMapDecodedMappings: SourceMapSpan[];
    }
    /** Return code used by getEmitOutput function to indicate status of the function */
    enum ExitStatus {
        Success = 0,
        DiagnosticsPresent_OutputsSkipped = 1,
        DiagnosticsPresent_OutputsGenerated = 2,
    }
    interface EmitResult {
        emitSkipped: boolean;
        /** Contains declaration emit diagnostics */
        diagnostics: Diagnostic[];
        emittedFiles: string[];
    }
    interface TypeChecker {
        getTypeOfSymbolAtLocation(symbol: Symbol, node: Node): Type;
        getDeclaredTypeOfSymbol(symbol: Symbol): Type;
        getPropertiesOfType(type: Type): Symbol[];
        getPropertyOfType(type: Type, propertyName: string): Symbol;
        getIndexInfoOfType(type: Type, kind: IndexKind): IndexInfo;
        getSignaturesOfType(type: Type, kind: SignatureKind): Signature[];
        getIndexTypeOfType(type: Type, kind: IndexKind): Type;
        getBaseTypes(type: InterfaceType): BaseType[];
        getBaseTypeOfLiteralType(type: Type): Type;
        getWidenedType(type: Type): Type;
        getReturnTypeOfSignature(signature: Signature): Type;
        getNonNullableType(type: Type): Type;
        getSymbolsInScope(location: Node, meaning: SymbolFlags): Symbol[];
        getSymbolAtLocation(node: Node): Symbol;
        getSymbolsOfParameterPropertyDeclaration(parameter: ParameterDeclaration, parameterName: string): Symbol[];
        getShorthandAssignmentValueSymbol(location: Node): Symbol;
        getExportSpecifierLocalTargetSymbol(location: ExportSpecifier): Symbol;
        getPropertySymbolOfDestructuringAssignment(location: Identifier): Symbol;
        getTypeAtLocation(node: Node): Type;
        getTypeFromTypeNode(node: TypeNode): Type;
        signatureToString(signature: Signature, enclosingDeclaration?: Node, flags?: TypeFormatFlags, kind?: SignatureKind): string;
        typeToString(type: Type, enclosingDeclaration?: Node, flags?: TypeFormatFlags): string;
        symbolToString(symbol: Symbol, enclosingDeclaration?: Node, meaning?: SymbolFlags): string;
        getSymbolDisplayBuilder(): SymbolDisplayBuilder;
        getFullyQualifiedName(symbol: Symbol): string;
        getAugmentedPropertiesOfType(type: Type): Symbol[];
        getRootSymbols(symbol: Symbol): Symbol[];
        getContextualType(node: Expression): Type;
        getResolvedSignature(node: CallLikeExpression, candidatesOutArray?: Signature[]): Signature;
        getSignatureFromDeclaration(declaration: SignatureDeclaration): Signature;
        isImplementationOfOverload(node: FunctionLikeDeclaration): boolean;
        isUndefinedSymbol(symbol: Symbol): boolean;
        isArgumentsSymbol(symbol: Symbol): boolean;
        isUnknownSymbol(symbol: Symbol): boolean;
        getConstantValue(node: EnumMember | PropertyAccessExpression | ElementAccessExpression): number;
        isValidPropertyAccess(node: PropertyAccessExpression | QualifiedName, propertyName: string): boolean;
        getAliasedSymbol(symbol: Symbol): Symbol;
        getExportsOfModule(moduleSymbol: Symbol): Symbol[];
        getJsxElementAttributesType(elementNode: JsxOpeningLikeElement): Type;
        getJsxIntrinsicTagNames(): Symbol[];
        isOptionalParameter(node: ParameterDeclaration): boolean;
        getAmbientModules(): Symbol[];
        tryGetMemberInModuleExports(memberName: string, moduleSymbol: Symbol): Symbol | undefined;
        getApparentType(type: Type): Type;
    }
    interface SymbolDisplayBuilder {
        buildTypeDisplay(type: Type, writer: SymbolWriter, enclosingDeclaration?: Node, flags?: TypeFormatFlags): void;
        buildSymbolDisplay(symbol: Symbol, writer: SymbolWriter, enclosingDeclaration?: Node, meaning?: SymbolFlags, flags?: SymbolFormatFlags): void;
        buildSignatureDisplay(signatures: Signature, writer: SymbolWriter, enclosingDeclaration?: Node, flags?: TypeFormatFlags, kind?: SignatureKind): void;
        buildIndexSignatureDisplay(info: IndexInfo, writer: SymbolWriter, kind: IndexKind, enclosingDeclaration?: Node, globalFlags?: TypeFormatFlags, symbolStack?: Symbol[]): void;
        buildParameterDisplay(parameter: Symbol, writer: SymbolWriter, enclosingDeclaration?: Node, flags?: TypeFormatFlags): void;
        buildTypeParameterDisplay(tp: TypeParameter, writer: SymbolWriter, enclosingDeclaration?: Node, flags?: TypeFormatFlags): void;
        buildTypePredicateDisplay(predicate: TypePredicate, writer: SymbolWriter, enclosingDeclaration?: Node, flags?: TypeFormatFlags): void;
        buildTypeParameterDisplayFromSymbol(symbol: Symbol, writer: SymbolWriter, enclosingDeclaration?: Node, flags?: TypeFormatFlags): void;
        buildDisplayForParametersAndDelimiters(thisParameter: Symbol, parameters: Symbol[], writer: SymbolWriter, enclosingDeclaration?: Node, flags?: TypeFormatFlags): void;
        buildDisplayForTypeParametersAndDelimiters(typeParameters: TypeParameter[], writer: SymbolWriter, enclosingDeclaration?: Node, flags?: TypeFormatFlags): void;
        buildReturnTypeDisplay(signature: Signature, writer: SymbolWriter, enclosingDeclaration?: Node, flags?: TypeFormatFlags): void;
    }
    interface SymbolWriter {
        writeKeyword(text: string): void;
        writeOperator(text: string): void;
        writePunctuation(text: string): void;
        writeSpace(text: string): void;
        writeStringLiteral(text: string): void;
        writeParameter(text: string): void;
        writeProperty(text: string): void;
        writeSymbol(text: string, symbol: Symbol): void;
        writeLine(): void;
        increaseIndent(): void;
        decreaseIndent(): void;
        clear(): void;
        trackSymbol(symbol: Symbol, enclosingDeclaration?: Node, meaning?: SymbolFlags): void;
        reportInaccessibleThisError(): void;
        reportIllegalExtends(): void;
    }
    const enum TypeFormatFlags {
        None = 0,
        WriteArrayAsGenericType = 1,
        UseTypeOfFunction = 2,
        NoTruncation = 4,
        WriteArrowStyleSignature = 8,
        WriteOwnNameForAnyLike = 16,
        WriteTypeArgumentsOfSignature = 32,
        InElementType = 64,
        UseFullyQualifiedType = 128,
        InFirstTypeArgument = 256,
        InTypeAlias = 512,
        UseTypeAliasValue = 1024,
        SuppressAnyReturnType = 2048,
        AddUndefined = 4096,
    }
    const enum SymbolFormatFlags {
        None = 0,
        WriteTypeParametersOrArguments = 1,
        UseOnlyExternalAliasing = 2,
    }
    const enum TypePredicateKind {
        This = 0,
        Identifier = 1,
    }
    interface TypePredicateBase {
        kind: TypePredicateKind;
        type: Type;
    }
    interface ThisTypePredicate extends TypePredicateBase {
        kind: TypePredicateKind.This;
    }
    interface IdentifierTypePredicate extends TypePredicateBase {
        kind: TypePredicateKind.Identifier;
        parameterName: string;
        parameterIndex: number;
    }
    type TypePredicate = IdentifierTypePredicate | ThisTypePredicate;
    const enum SymbolFlags {
        None = 0,
        FunctionScopedVariable = 1,
        BlockScopedVariable = 2,
        Property = 4,
        EnumMember = 8,
        Function = 16,
        Class = 32,
        Interface = 64,
        ConstEnum = 128,
        RegularEnum = 256,
        ValueModule = 512,
        NamespaceModule = 1024,
        TypeLiteral = 2048,
        ObjectLiteral = 4096,
        Method = 8192,
        Constructor = 16384,
        GetAccessor = 32768,
        SetAccessor = 65536,
        Signature = 131072,
        TypeParameter = 262144,
        TypeAlias = 524288,
        ExportValue = 1048576,
        ExportType = 2097152,
        ExportNamespace = 4194304,
        Alias = 8388608,
        Prototype = 16777216,
        ExportStar = 33554432,
        Optional = 67108864,
        Transient = 134217728,
        Enum = 384,
        Variable = 3,
        Value = 107455,
        Type = 793064,
        Namespace = 1920,
        Module = 1536,
        Accessor = 98304,
        FunctionScopedVariableExcludes = 107454,
        BlockScopedVariableExcludes = 107455,
        ParameterExcludes = 107455,
        PropertyExcludes = 0,
        EnumMemberExcludes = 900095,
        FunctionExcludes = 106927,
        ClassExcludes = 899519,
        InterfaceExcludes = 792968,
        RegularEnumExcludes = 899327,
        ConstEnumExcludes = 899967,
        ValueModuleExcludes = 106639,
        NamespaceModuleExcludes = 0,
        MethodExcludes = 99263,
        GetAccessorExcludes = 41919,
        SetAccessorExcludes = 74687,
        TypeParameterExcludes = 530920,
        TypeAliasExcludes = 793064,
        AliasExcludes = 8388608,
        ModuleMember = 8914931,
        ExportHasLocal = 944,
        HasExports = 1952,
        HasMembers = 6240,
        BlockScoped = 418,
        PropertyOrAccessor = 98308,
        Export = 7340032,
        ClassMember = 106500,
    }
    interface Symbol {
        flags: SymbolFlags;
        name: string;
        declarations?: Declaration[];
        valueDeclaration?: Declaration;
        members?: SymbolTable;
        exports?: SymbolTable;
        globalExports?: SymbolTable;
    }
    type SymbolTable = Map<Symbol>;
    const enum TypeFlags {
        Any = 1,
        String = 2,
        Number = 4,
        Boolean = 8,
        Enum = 16,
        StringLiteral = 32,
        NumberLiteral = 64,
        BooleanLiteral = 128,
        EnumLiteral = 256,
        ESSymbol = 512,
        Void = 1024,
        Undefined = 2048,
        Null = 4096,
        Never = 8192,
        TypeParameter = 16384,
        Object = 32768,
        Union = 65536,
        Intersection = 131072,
        Index = 262144,
        IndexedAccess = 524288,
        NonPrimitive = 16777216,
        Literal = 480,
        StringOrNumberLiteral = 96,
        PossiblyFalsy = 7406,
        StringLike = 262178,
        NumberLike = 340,
        BooleanLike = 136,
        EnumLike = 272,
        UnionOrIntersection = 196608,
        StructuredType = 229376,
        StructuredOrTypeVariable = 1032192,
        TypeVariable = 540672,
        Narrowable = 17810431,
        NotUnionOrUnit = 16810497,
    }
    type DestructuringPattern = BindingPattern | ObjectLiteralExpression | ArrayLiteralExpression;
    interface Type {
        flags: TypeFlags;
        symbol?: Symbol;
        pattern?: DestructuringPattern;
        aliasSymbol?: Symbol;
        aliasTypeArguments?: Type[];
    }
    interface LiteralType extends Type {
        text: string;
        freshType?: LiteralType;
        regularType?: LiteralType;
    }
    interface EnumType extends Type {
        memberTypes: EnumLiteralType[];
    }
    interface EnumLiteralType extends LiteralType {
        baseType: EnumType & UnionType;
    }
    const enum ObjectFlags {
        Class = 1,
        Interface = 2,
        Reference = 4,
        Tuple = 8,
        Anonymous = 16,
        Mapped = 32,
        Instantiated = 64,
        ObjectLiteral = 128,
        EvolvingArray = 256,
        ObjectLiteralPatternWithComputedProperties = 512,
        NonPrimitive = 1024,
        ClassOrInterface = 3,
    }
    interface ObjectType extends Type {
        objectFlags: ObjectFlags;
    }
    /** Class and interface types (TypeFlags.Class and TypeFlags.Interface). */
    interface InterfaceType extends ObjectType {
        typeParameters: TypeParameter[];
        outerTypeParameters: TypeParameter[];
        localTypeParameters: TypeParameter[];
        thisType: TypeParameter;
    }
    type BaseType = ObjectType | IntersectionType;
    interface InterfaceTypeWithDeclaredMembers extends InterfaceType {
        declaredProperties: Symbol[];
        declaredCallSignatures: Signature[];
        declaredConstructSignatures: Signature[];
        declaredStringIndexInfo: IndexInfo;
        declaredNumberIndexInfo: IndexInfo;
    }
    /**
     * Type references (TypeFlags.Reference). When a class or interface has type parameters or
     * a "this" type, references to the class or interface are made using type references. The
     * typeArguments property specifies the types to substitute for the type parameters of the
     * class or interface and optionally includes an extra element that specifies the type to
     * substitute for "this" in the resulting instantiation. When no extra argument is present,
     * the type reference itself is substituted for "this". The typeArguments property is undefined
     * if the class or interface has no type parameters and the reference isn't specifying an
     * explicit "this" argument.
     */
    interface TypeReference extends ObjectType {
        target: GenericType;
        typeArguments: Type[];
    }
    interface GenericType extends InterfaceType, TypeReference {
    }
    interface UnionOrIntersectionType extends Type {
        types: Type[];
    }
    interface UnionType extends UnionOrIntersectionType {
    }
    interface IntersectionType extends UnionOrIntersectionType {
    }
    type StructuredType = ObjectType | UnionType | IntersectionType;
    interface EvolvingArrayType extends ObjectType {
        elementType: Type;
        finalArrayType?: Type;
    }
    interface TypeVariable extends Type {
    }
    interface TypeParameter extends TypeVariable {
        constraint: Type;
    }
    interface IndexedAccessType extends TypeVariable {
        objectType: Type;
        indexType: Type;
        constraint?: Type;
    }
    interface IndexType extends Type {
        type: TypeVariable | UnionOrIntersectionType;
    }
    const enum SignatureKind {
        Call = 0,
        Construct = 1,
    }
    interface Signature {
        declaration: SignatureDeclaration;
        typeParameters: TypeParameter[];
        parameters: Symbol[];
    }
    const enum IndexKind {
        String = 0,
        Number = 1,
    }
    interface IndexInfo {
        type: Type;
        isReadonly: boolean;
        declaration?: SignatureDeclaration;
    }
    interface JsFileExtensionInfo {
        extension: string;
        isMixedContent: boolean;
    }
    interface DiagnosticMessage {
        key: string;
        category: DiagnosticCategory;
        code: number;
        message: string;
    }
    /**
     * A linked list of formatted diagnostic messages to be used as part of a multiline message.
     * It is built from the bottom up, leaving the head to be the "main" diagnostic.
     * While it seems that DiagnosticMessageChain is structurally similar to DiagnosticMessage,
     * the difference is that messages are all preformatted in DMC.
     */
    interface DiagnosticMessageChain {
        messageText: string;
        category: DiagnosticCategory;
        code: number;
        next?: DiagnosticMessageChain;
    }
    interface Diagnostic {
        file: SourceFile;
        start: number;
        length: number;
        messageText: string | DiagnosticMessageChain;
        category: DiagnosticCategory;
        code: number;
    }
    enum DiagnosticCategory {
        Warning = 0,
        Error = 1,
        Message = 2,
    }
    enum ModuleResolutionKind {
        Classic = 1,
        NodeJs = 2,
    }
    interface PluginImport {
        name: string;
    }
    type CompilerOptionsValue = string | number | boolean | (string | number)[] | string[] | MapLike<string[]> | PluginImport[];
    interface CompilerOptions {
        allowJs?: boolean;
        allowSyntheticDefaultImports?: boolean;
        allowUnreachableCode?: boolean;
        allowUnusedLabels?: boolean;
        alwaysStrict?: boolean;
        baseUrl?: string;
        charset?: string;
        declaration?: boolean;
        declarationDir?: string;
        disableSizeLimit?: boolean;
        emitBOM?: boolean;
        emitDecoratorMetadata?: boolean;
        experimentalDecorators?: boolean;
        forceConsistentCasingInFileNames?: boolean;
        importHelpers?: boolean;
        inlineSourceMap?: boolean;
        inlineSources?: boolean;
        isolatedModules?: boolean;
        jsx?: JsxEmit;
        lib?: string[];
        locale?: string;
        mapRoot?: string;
        maxNodeModuleJsDepth?: number;
        module?: ModuleKind;
        moduleResolution?: ModuleResolutionKind;
        newLine?: NewLineKind;
        noEmit?: boolean;
        noEmitHelpers?: boolean;
        noEmitOnError?: boolean;
        noErrorTruncation?: boolean;
        noFallthroughCasesInSwitch?: boolean;
        noImplicitAny?: boolean;
        noImplicitReturns?: boolean;
        noImplicitThis?: boolean;
        noUnusedLocals?: boolean;
        noUnusedParameters?: boolean;
        noImplicitUseStrict?: boolean;
        noLib?: boolean;
        noResolve?: boolean;
        out?: string;
        outDir?: string;
        outFile?: string;
        paths?: MapLike<string[]>;
        preserveConstEnums?: boolean;
        project?: string;
        reactNamespace?: string;
        jsxFactory?: string;
        removeComments?: boolean;
        rootDir?: string;
        rootDirs?: string[];
        skipLibCheck?: boolean;
        skipDefaultLibCheck?: boolean;
        sourceMap?: boolean;
        sourceRoot?: string;
        strictNullChecks?: boolean;
        suppressExcessPropertyErrors?: boolean;
        suppressImplicitAnyIndexErrors?: boolean;
        target?: ScriptTarget;
        traceResolution?: boolean;
        types?: string[];
        /** Paths used to compute primary types search locations */
        typeRoots?: string[];
        generateContracts?: boolean;
        annotationMangler?: string;
        [option: string]: CompilerOptionsValue | undefined;
    }
    interface TypeAcquisition {
        enableAutoDiscovery?: boolean;
        enable?: boolean;
        include?: string[];
        exclude?: string[];
        [option: string]: string[] | boolean | undefined;
    }
    interface DiscoverTypingsInfo {
        fileNames: string[];
        projectRootPath: string;
        safeListPath: string;
        packageNameToTypingLocation: Map<string>;
        typeAcquisition: TypeAcquisition;
        compilerOptions: CompilerOptions;
        unresolvedImports: ReadonlyArray<string>;
    }
    enum ModuleKind {
        None = 0,
        CommonJS = 1,
        AMD = 2,
        UMD = 3,
        System = 4,
        ES2015 = 5,
    }
    const enum JsxEmit {
        None = 0,
        Preserve = 1,
        React = 2,
        ReactNative = 3,
    }
    const enum NewLineKind {
        CarriageReturnLineFeed = 0,
        LineFeed = 1,
    }
    interface LineAndCharacter {
        line: number;
        character: number;
    }
    const enum ScriptKind {
        Unknown = 0,
        JS = 1,
        JSX = 2,
        TS = 3,
        TSX = 4,
        External = 5,
    }
    const enum ScriptTarget {
        ES3 = 0,
        ES5 = 1,
        ES2015 = 2,
        ES2016 = 3,
        ES2017 = 4,
        ESNext = 5,
        Latest = 5,
    }
    const enum LanguageVariant {
        Standard = 0,
        JSX = 1,
    }
    /** Either a parsed command line or a parsed tsconfig.json */
    interface ParsedCommandLine {
        options: CompilerOptions;
        typeAcquisition?: TypeAcquisition;
        fileNames: string[];
        raw?: any;
        errors: Diagnostic[];
        wildcardDirectories?: MapLike<WatchDirectoryFlags>;
        compileOnSave?: boolean;
    }
    const enum WatchDirectoryFlags {
        None = 0,
        Recursive = 1,
    }
    interface ExpandResult {
        fileNames: string[];
        wildcardDirectories: MapLike<WatchDirectoryFlags>;
    }
    interface ModuleResolutionHost {
        fileExists(fileName: string): boolean;
        readFile(fileName: string): string;
        trace?(s: string): void;
        directoryExists?(directoryName: string): boolean;
        realpath?(path: string): string;
        getCurrentDirectory?(): string;
        getDirectories?(path: string): string[];
    }
    /**
     * Represents the result of module resolution.
     * Module resolution will pick up tsx/jsx/js files even if '--jsx' and '--allowJs' are turned off.
     * The Program will then filter results based on these flags.
     *
     * Prefer to return a `ResolvedModuleFull` so that the file type does not have to be inferred.
     */
    interface ResolvedModule {
        /** Path of the file the module was resolved to. */
        resolvedFileName: string;
        /**
         * Denotes if 'resolvedFileName' is isExternalLibraryImport and thus should be a proper external module:
         * - be a .d.ts file
         * - use top level imports\exports
         * - don't use tripleslash references
         */
        isExternalLibraryImport?: boolean;
    }
    /**
     * ResolvedModule with an explicitly provided `extension` property.
     * Prefer this over `ResolvedModule`.
     */
    interface ResolvedModuleFull extends ResolvedModule {
        /**
         * Extension of resolvedFileName. This must match what's at the end of resolvedFileName.
         * This is optional for backwards-compatibility, but will be added if not provided.
         */
        extension: Extension;
    }
    enum Extension {
        Ts = 0,
        Tsx = 1,
        Dts = 2,
        Js = 3,
        Jsx = 4,
        LastTypeScriptExtension = 2,
    }
    interface ResolvedModuleWithFailedLookupLocations {
        resolvedModule: ResolvedModuleFull | undefined;
    }
    interface ResolvedTypeReferenceDirective {
        primary: boolean;
        resolvedFileName?: string;
    }
    interface ResolvedTypeReferenceDirectiveWithFailedLookupLocations {
        resolvedTypeReferenceDirective: ResolvedTypeReferenceDirective;
        failedLookupLocations: string[];
    }
    interface CompilerHost extends ModuleResolutionHost {
        getSourceFile(fileName: string, languageVersion: ScriptTarget, onError?: (message: string) => void): SourceFile;
        getSourceFileByPath?(fileName: string, path: Path, languageVersion: ScriptTarget, onError?: (message: string) => void): SourceFile;
        getCancellationToken?(): CancellationToken;
        getDefaultLibFileName(options: CompilerOptions): string;
        getDefaultLibLocation?(): string;
        writeFile: WriteFileCallback;
        getCurrentDirectory(): string;
        getDirectories(path: string): string[];
        getCanonicalFileName(fileName: string): string;
        useCaseSensitiveFileNames(): boolean;
        getNewLine(): string;
        resolveModuleNames?(moduleNames: string[], containingFile: string): ResolvedModule[];
        /**
         * This method is a companion for 'resolveModuleNames' and is used to resolve 'types' references to actual type declaration files
         */
        resolveTypeReferenceDirectives?(typeReferenceDirectiveNames: string[], containingFile: string): ResolvedTypeReferenceDirective[];
        getEnvironmentVariable?(name: string): string;
    }
    const enum EmitFlags {
        SingleLine = 1,
        AdviseOnEmitNode = 2,
        NoSubstitution = 4,
        CapturesThis = 8,
        NoLeadingSourceMap = 16,
        NoTrailingSourceMap = 32,
        NoSourceMap = 48,
        NoNestedSourceMaps = 64,
        NoTokenLeadingSourceMaps = 128,
        NoTokenTrailingSourceMaps = 256,
        NoTokenSourceMaps = 384,
        NoLeadingComments = 512,
        NoTrailingComments = 1024,
        NoComments = 1536,
        NoNestedComments = 2048,
        HelperName = 4096,
        ExportName = 8192,
        LocalName = 16384,
        Indented = 32768,
        NoIndentation = 65536,
        AsyncFunctionBody = 131072,
        ReuseTempVariableScope = 262144,
        CustomPrologue = 524288,
        NoHoisting = 1048576,
        HasEndOfDeclarationMarker = 2097152,
    }
    interface EmitHelper {
        readonly name: string;
        readonly scoped: boolean;
        readonly text: string;
        readonly priority?: number;
    }
    const enum EmitHint {
        SourceFile = 0,
        Expression = 1,
        IdentifierName = 2,
        Unspecified = 3,
    }
    interface Printer {
        /**
         * Print a node and its subtree as-is, without any emit transformations.
         * @param hint A value indicating the purpose of a node. This is primarily used to
         * distinguish between an `Identifier` used in an expression position, versus an
         * `Identifier` used as an `IdentifierName` as part of a declaration. For most nodes you
         * should just pass `Unspecified`.
         * @param node The node to print. The node and its subtree are printed as-is, without any
         * emit transformations.
         * @param sourceFile A source file that provides context for the node. The source text of
         * the file is used to emit the original source content for literals and identifiers, while
         * the identifiers of the source file are used when generating unique names to avoid
         * collisions.
         */
        printNode(hint: EmitHint, node: Node, sourceFile: SourceFile): string;
        /**
         * Prints a source file as-is, without any emit transformations.
         */
        printFile(sourceFile: SourceFile): string;
        /**
         * Prints a bundle of source files as-is, without any emit transformations.
         */
        printBundle(bundle: Bundle): string;
    }
    interface PrintHandlers {
        /**
         * A hook used by the Printer when generating unique names to avoid collisions with
         * globally defined names that exist outside of the current source file.
         */
        hasGlobalName?(name: string): boolean;
        /**
         * A hook used by the Printer to provide notifications prior to emitting a node. A
         * compatible implementation **must** invoke `emitCallback` with the provided `hint` and
         * `node` values.
         * @param hint A hint indicating the intended purpose of the node.
         * @param node The node to emit.
         * @param emitCallback A callback that, when invoked, will emit the node.
         * @example
         * ```ts
         * var printer = createPrinter(printerOptions, {
         *   onEmitNode(hint, node, emitCallback) {
         *     // set up or track state prior to emitting the node...
         *     emitCallback(hint, node);
         *     // restore state after emitting the node...
         *   }
         * });
         * ```
         */
        onEmitNode?(hint: EmitHint, node: Node, emitCallback: (hint: EmitHint, node: Node) => void): void;
        /**
         * A hook used by the Printer to perform just-in-time substitution of a node. This is
         * primarily used by node transformations that need to substitute one node for another,
         * such as replacing `myExportedVar` with `exports.myExportedVar`. A compatible
         * implementation **must** invoke `emitCallback` eith the provided `hint` and either
         * the provided `node`, or its substitute.
         * @param hint A hint indicating the intended purpose of the node.
         * @param node The node to emit.
         * @param emitCallback A callback that, when invoked, will emit the node.
         * @example
         * ```ts
         * var printer = createPrinter(printerOptions, {
         *   onSubstituteNode(hint, node, emitCallback) {
         *     // perform substitution if necessary...
         *     emitCallback(hint, node);
         *   }
         * });
         * ```
         */
        onSubstituteNode?(hint: EmitHint, node: Node, emitCallback: (hint: EmitHint, node: Node) => void): void;
    }
    interface PrinterOptions {
        target?: ScriptTarget;
        removeComments?: boolean;
        newLine?: NewLineKind;
    }
    interface TextSpan {
        start: number;
        length: number;
    }
    interface TextChangeRange {
        span: TextSpan;
        newLength: number;
    }
    interface SyntaxList extends Node {
        _children: Node[];
    }
}
declare namespace ts {
    /** The version of the TypeScript compiler release */
    const version = "2.2.1";
}
declare function setTimeout(handler: (...args: any[]) => void, timeout: number): any;
declare function clearTimeout(handle: any): void;
declare var arguments: any;
declare var higgsArguments: any;
declare namespace ts {
    type FileWatcherCallback = (fileName: string, removed?: boolean) => void;
    type DirectoryWatcherCallback = (fileName: string) => void;
    interface WatchedFile {
        fileName: string;
        callback: FileWatcherCallback;
        mtime?: Date;
    }
    interface System {
        args: string[];
        newLine: string;
        useCaseSensitiveFileNames: boolean;
        write(s: string): void;
        readFile(path: string, encoding?: string): string;
        getFileSize?(path: string): number;
        writeFile(path: string, data: string, writeByteOrderMark?: boolean): void;
        /**
         * @pollingInterval - this parameter is used in polling-based watchers and ignored in watchers that
         * use native OS file watching
         */
        watchFile?(path: string, callback: FileWatcherCallback, pollingInterval?: number): FileWatcher;
        watchDirectory?(path: string, callback: DirectoryWatcherCallback, recursive?: boolean): FileWatcher;
        resolvePath(path: string): string;
        fileExists(path: string): boolean;
        directoryExists(path: string): boolean;
        createDirectory(path: string): void;
        getExecutingFilePath(): string;
        getCurrentDirectory(): string;
        getDirectories(path: string): string[];
        readDirectory(path: string, extensions?: string[], exclude?: string[], include?: string[]): string[];
        getModifiedTime?(path: string): Date;
        createHash?(data: string): string;
        getMemoryUsage?(): number;
        exit(exitCode?: number): void;
        realpath?(path: string): string;
        setTimeout?(callback: (...args: any[]) => void, ms: number, ...args: any[]): any;
        clearTimeout?(timeoutId: any): void;
    }
    interface FileWatcher {
        close(): void;
    }
    interface DirectoryWatcher extends FileWatcher {
        directoryName: string;
        referenceCount: number;
    }
    function getNodeMajorVersion(): number;
    let sys: System;
}
declare namespace ts {
    interface ErrorCallback {
        (message: DiagnosticMessage, length: number): void;
    }
    interface Scanner {
        getStartPos(): number;
        getToken(): SyntaxKind;
        getTextPos(): number;
        getTokenPos(): number;
        getTokenText(): string;
        getTokenValue(): string;
        hasExtendedUnicodeEscape(): boolean;
        hasPrecedingLineBreak(): boolean;
        isIdentifier(): boolean;
        isReservedWord(): boolean;
        isUnterminated(): boolean;
        reScanGreaterToken(): SyntaxKind;
        reScanSlashToken(): SyntaxKind;
        reScanTemplateToken(): SyntaxKind;
        scanJsxIdentifier(): SyntaxKind;
        scanJsxAttributeValue(): SyntaxKind;
        reScanJsxToken(): SyntaxKind;
        scanJsxToken(): SyntaxKind;
        scanJSDocToken(): SyntaxKind;
        scan(): SyntaxKind;
        getText(): string;
        setText(text: string, start?: number, length?: number): void;
        setOnError(onError: ErrorCallback): void;
        setScriptTarget(scriptTarget: ScriptTarget): void;
        setLanguageVariant(variant: LanguageVariant): void;
        setTextPos(textPos: number): void;
        lookAhead<T>(callback: () => T): T;
        scanRange<T>(start: number, length: number, callback: () => T): T;
        tryScan<T>(callback: () => T): T;
    }
    function tokenToString(t: SyntaxKind): string;
    function getPositionOfLineAndCharacter(sourceFile: SourceFile, line: number, character: number): number;
    function getLineAndCharacterOfPosition(sourceFile: SourceFile, position: number): LineAndCharacter;
    function isWhiteSpace(ch: number): boolean;
    /** Does not include line breaks. For that, see isWhiteSpaceLike. */
    function isWhiteSpaceSingleLine(ch: number): boolean;
    function isLineBreak(ch: number): boolean;
    function couldStartTrivia(text: string, pos: number): boolean;
    function forEachLeadingCommentRange<T, U>(text: string, pos: number, cb: (pos: number, end: number, kind: SyntaxKind, hasTrailingNewLine: boolean, state: T) => U, state?: T): U;
    function forEachTrailingCommentRange<T, U>(text: string, pos: number, cb: (pos: number, end: number, kind: SyntaxKind, hasTrailingNewLine: boolean, state: T) => U, state?: T): U;
    function reduceEachLeadingCommentRange<T, U>(text: string, pos: number, cb: (pos: number, end: number, kind: SyntaxKind, hasTrailingNewLine: boolean, state: T, memo: U) => U, state: T, initial: U): U;
    function reduceEachTrailingCommentRange<T, U>(text: string, pos: number, cb: (pos: number, end: number, kind: SyntaxKind, hasTrailingNewLine: boolean, state: T, memo: U) => U, state: T, initial: U): U;
    function getLeadingCommentRanges(text: string, pos: number): CommentRange[] | undefined;
    function getTrailingCommentRanges(text: string, pos: number): CommentRange[] | undefined;
    /** Optionally, get the shebang */
    function getShebang(text: string): string;
    function isIdentifierStart(ch: number, languageVersion: ScriptTarget): boolean;
    function isIdentifierPart(ch: number, languageVersion: ScriptTarget): boolean;
    function createScanner(languageVersion: ScriptTarget, skipTrivia: boolean, languageVariant?: LanguageVariant, text?: string, onError?: ErrorCallback, start?: number, length?: number): Scanner;
}
declare namespace ts {
    function parseCommandLine(commandLine: string[], readFile?: (path: string) => string): ParsedCommandLine;
    /**
      * Read tsconfig.json file
      * @param fileName The path to the config file
      */
    function readConfigFile(fileName: string, readFile: (path: string) => string): {
        config?: any;
        error?: Diagnostic;
    };
    /**
      * Parse the text of the tsconfig.json file
      * @param fileName The path to the config file
      * @param jsonText The text of the config file
      */
    function parseConfigFileTextToJson(fileName: string, jsonText: string, stripComments?: boolean): {
        config?: any;
        error?: Diagnostic;
    };
    /**
      * Parse the contents of a config file (tsconfig.json).
      * @param json The contents of the config file to parse
      * @param host Instance of ParseConfigHost used to enumerate files in folder.
      * @param basePath A root directory to resolve relative path entries in the config
      *    file to. e.g. outDir
      */
    function parseJsonConfigFileContent(json: any, host: ParseConfigHost, basePath: string, existingOptions?: CompilerOptions, configFileName?: string, resolutionStack?: Path[], extraFileExtensions?: JsFileExtensionInfo[]): ParsedCommandLine;
    function convertCompileOnSaveOptionFromJson(jsonOption: any, basePath: string, errors: Diagnostic[]): boolean;
    function convertCompilerOptionsFromJson(jsonOptions: any, basePath: string, configFileName?: string): {
        options: CompilerOptions;
        errors: Diagnostic[];
    };
    function convertTypeAcquisitionFromJson(jsonOptions: any, basePath: string, configFileName?: string): {
        options: TypeAcquisition;
        errors: Diagnostic[];
    };
}
declare namespace ts {
    /** Array that is only intended to be pushed to, never read. */
    interface Push<T> {
        push(value: T): void;
    }
    function moduleHasNonRelativeName(moduleName: string): boolean;
    function getEffectiveTypeRoots(options: CompilerOptions, host: {
        directoryExists?: (directoryName: string) => boolean;
        getCurrentDirectory?: () => string;
    }): string[] | undefined;
    /**
     * @param {string | undefined} containingFile - file that contains type reference directive, can be undefined if containing file is unknown.
     * This is possible in case if resolution is performed for directives specified via 'types' parameter. In this case initial path for secondary lookups
     * is assumed to be the same as root directory of the project.
     */
    function resolveTypeReferenceDirective(typeReferenceDirectiveName: string, containingFile: string | undefined, options: CompilerOptions, host: ModuleResolutionHost): ResolvedTypeReferenceDirectiveWithFailedLookupLocations;
    /**
      * Given a set of options, returns the set of type directive names
      *   that should be included for this program automatically.
      * This list could either come from the config file,
      *   or from enumerating the types root + initial secondary types lookup location.
      * More type directives might appear in the program later as a result of loading actual source files;
      *   this list is only the set of defaults that are implicitly included.
      */
    function getAutomaticTypeDirectiveNames(options: CompilerOptions, host: ModuleResolutionHost): string[];
    /**
     * Cached module resolutions per containing directory.
     * This assumes that any module id will have the same resolution for sibling files located in the same folder.
     */
    interface ModuleResolutionCache extends NonRelativeModuleNameResolutionCache {
        getOrCreateCacheForDirectory(directoryName: string): Map<ResolvedModuleWithFailedLookupLocations>;
    }
    /**
     * Stored map from non-relative module name to a table: directory -> result of module lookup in this directory
     * We support only non-relative module names because resolution of relative module names is usually more deterministic and thus less expensive.
     */
    interface NonRelativeModuleNameResolutionCache {
        getOrCreateCacheForModuleName(nonRelativeModuleName: string): PerModuleNameCache;
    }
    interface PerModuleNameCache {
        get(directory: string): ResolvedModuleWithFailedLookupLocations;
        set(directory: string, result: ResolvedModuleWithFailedLookupLocations): void;
    }
    function createModuleResolutionCache(currentDirectory: string, getCanonicalFileName: (s: string) => string): ModuleResolutionCache;
    function resolveModuleName(moduleName: string, containingFile: string, compilerOptions: CompilerOptions, host: ModuleResolutionHost, cache?: ModuleResolutionCache): ResolvedModuleWithFailedLookupLocations;
    function nodeModuleNameResolver(moduleName: string, containingFile: string, compilerOptions: CompilerOptions, host: ModuleResolutionHost, cache?: ModuleResolutionCache): ResolvedModuleWithFailedLookupLocations;
    function classicNameResolver(moduleName: string, containingFile: string, compilerOptions: CompilerOptions, host: ModuleResolutionHost, cache?: NonRelativeModuleNameResolutionCache): ResolvedModuleWithFailedLookupLocations;
}
declare namespace ts {
    function getDefaultLibFileName(options: CompilerOptions): string;
    function textSpanEnd(span: TextSpan): number;
    function textSpanIsEmpty(span: TextSpan): boolean;
    function textSpanContainsPosition(span: TextSpan, position: number): boolean;
    function textSpanContainsTextSpan(span: TextSpan, other: TextSpan): boolean;
    function textSpanOverlapsWith(span: TextSpan, other: TextSpan): boolean;
    function textSpanOverlap(span1: TextSpan, span2: TextSpan): TextSpan;
    function textSpanIntersectsWithTextSpan(span: TextSpan, other: TextSpan): boolean;
    function textSpanIntersectsWith(span: TextSpan, start: number, length: number): boolean;
    function decodedTextSpanIntersectsWith(start1: number, length1: number, start2: number, length2: number): boolean;
    function textSpanIntersectsWithPosition(span: TextSpan, position: number): boolean;
    function textSpanIntersection(span1: TextSpan, span2: TextSpan): TextSpan;
    function createTextSpan(start: number, length: number): TextSpan;
    function createTextSpanFromBounds(start: number, end: number): TextSpan;
    function textChangeRangeNewSpan(range: TextChangeRange): TextSpan;
    function textChangeRangeIsUnchanged(range: TextChangeRange): boolean;
    function createTextChangeRange(span: TextSpan, newLength: number): TextChangeRange;
    let unchangedTextChangeRange: TextChangeRange;
    /**
     * Called to merge all the changes that occurred across several versions of a script snapshot
     * into a single change.  i.e. if a user keeps making successive edits to a script we will
     * have a text change from V1 to V2, V2 to V3, ..., Vn.
     *
     * This function will then merge those changes into a single change range valid between V1 and
     * Vn.
     */
    function collapseTextChangeRangesAcrossMultipleVersions(changes: TextChangeRange[]): TextChangeRange;
    function getTypeParameterOwner(d: Declaration): Declaration;
    function isParameterPropertyDeclaration(node: Node): boolean;
    function getCombinedModifierFlags(node: Node): ModifierFlags;
    function getCombinedNodeFlags(node: Node): NodeFlags;
    /**
      * Checks to see if the locale is in the appropriate format,
      * and if it is, attempts to set the appropriate language.
      */
    function validateLocaleAndSetLanguage(locale: string, sys: {
        getExecutingFilePath(): string;
        resolvePath(path: string): string;
        fileExists(fileName: string): boolean;
        readFile(fileName: string): string;
    }, errors?: Diagnostic[]): void;
    function getOriginalNode(node: Node): Node;
    function getOriginalNode<T extends Node>(node: Node, nodeTest: (node: Node) => node is T): T;
    /**
     * Gets a value indicating whether a node originated in the parse tree.
     *
     * @param node The node to test.
     */
    function isParseTreeNode(node: Node): boolean;
    /**
     * Gets the original parse tree node for a node.
     *
     * @param node The original node.
     * @returns The original parse tree node if found; otherwise, undefined.
     */
    function getParseTreeNode(node: Node): Node;
    /**
     * Gets the original parse tree node for a node.
     *
     * @param node The original node.
     * @param nodeTest A callback used to ensure the correct type of parse tree node is returned.
     * @returns The original parse tree node if found; otherwise, undefined.
     */
    function getParseTreeNode<T extends Node>(node: Node, nodeTest?: (node: Node) => node is T): T;
}
declare namespace ts {
    /**
     * Make `elements` into a `NodeArray<T>`. If `elements` is `undefined`, returns an empty `NodeArray<T>`.
     */
    function createNodeArray<T extends Node>(elements?: T[], hasTrailingComma?: boolean): NodeArray<T>;
    function createLiteral(value: string): StringLiteral;
    function createLiteral(value: number): NumericLiteral;
    function createLiteral(value: boolean): BooleanLiteral;
    /** Create a string literal whose source text is read from a source node during emit. */
    function createLiteral(sourceNode: StringLiteral | NumericLiteral | Identifier): StringLiteral;
    function createLiteral(value: string | number | boolean): PrimaryExpression;
    function createNumericLiteral(value: string): NumericLiteral;
    function createStringLiteral(text: string): StringLiteral;
    function createIdentifier(text: string): Identifier;
    /** Create a unique temporary variable. */
    function createTempVariable(recordTempVariable: ((node: Identifier) => void) | undefined): Identifier;
    /** Create a unique temporary variable for use in a loop. */
    function createLoopVariable(): Identifier;
    /** Create a unique name based on the supplied text. */
    function createUniqueName(text: string): Identifier;
    /** Create a unique name generated for a node. */
    function getGeneratedNameForNode(node: Node): Identifier;
    function createToken<TKind extends SyntaxKind>(token: TKind): Token<TKind>;
    function createSuper(): PrimaryExpression;
    function createThis(): PrimaryExpression;
    function createNull(): PrimaryExpression;
    function createTrue(): BooleanLiteral;
    function createFalse(): BooleanLiteral;
    function createQualifiedName(left: EntityName, right: string | Identifier): QualifiedName;
    function updateQualifiedName(node: QualifiedName, left: EntityName, right: Identifier): QualifiedName;
    function createComputedPropertyName(expression: Expression): ComputedPropertyName;
    function updateComputedPropertyName(node: ComputedPropertyName, expression: Expression): ComputedPropertyName;
    function createParameter(decorators: Decorator[], modifiers: Modifier[], dotDotDotToken: DotDotDotToken, name: string | Identifier | BindingPattern, questionToken?: QuestionToken, type?: TypeNode, initializer?: Expression): ParameterDeclaration;
    function updateParameter(node: ParameterDeclaration, decorators: Decorator[], modifiers: Modifier[], dotDotDotToken: DotDotDotToken, name: BindingName, type: TypeNode, initializer: Expression): ParameterDeclaration;
    function createDecorator(expression: Expression): Decorator;
    function updateDecorator(node: Decorator, expression: Expression): Decorator;
    function createProperty(decorators: Decorator[], modifiers: Modifier[], name: string | PropertyName, questionToken: QuestionToken, type: TypeNode, initializer: Expression): PropertyDeclaration;
    function updateProperty(node: PropertyDeclaration, decorators: Decorator[], modifiers: Modifier[], name: PropertyName, type: TypeNode, initializer: Expression): PropertyDeclaration;
    function createMethod(decorators: Decorator[], modifiers: Modifier[], asteriskToken: AsteriskToken, name: string | PropertyName, typeParameters: TypeParameterDeclaration[], parameters: ParameterDeclaration[], type: TypeNode, body: Block): MethodDeclaration;
    function updateMethod(node: MethodDeclaration, decorators: Decorator[], modifiers: Modifier[], name: PropertyName, typeParameters: TypeParameterDeclaration[], parameters: ParameterDeclaration[], type: TypeNode, body: Block): MethodDeclaration;
    function createConstructor(decorators: Decorator[], modifiers: Modifier[], parameters: ParameterDeclaration[], body: Block): ConstructorDeclaration;
    function updateConstructor(node: ConstructorDeclaration, decorators: Decorator[], modifiers: Modifier[], parameters: ParameterDeclaration[], body: Block): ConstructorDeclaration;
    function createGetAccessor(decorators: Decorator[], modifiers: Modifier[], name: string | PropertyName, parameters: ParameterDeclaration[], type: TypeNode, body: Block): GetAccessorDeclaration;
    function updateGetAccessor(node: GetAccessorDeclaration, decorators: Decorator[], modifiers: Modifier[], name: PropertyName, parameters: ParameterDeclaration[], type: TypeNode, body: Block): GetAccessorDeclaration;
    function createSetAccessor(decorators: Decorator[], modifiers: Modifier[], name: string | PropertyName, parameters: ParameterDeclaration[], body: Block): SetAccessorDeclaration;
    function updateSetAccessor(node: SetAccessorDeclaration, decorators: Decorator[], modifiers: Modifier[], name: PropertyName, parameters: ParameterDeclaration[], body: Block): SetAccessorDeclaration;
    function createObjectBindingPattern(elements: BindingElement[]): ObjectBindingPattern;
    function updateObjectBindingPattern(node: ObjectBindingPattern, elements: BindingElement[]): ObjectBindingPattern;
    function createArrayBindingPattern(elements: ArrayBindingElement[]): ArrayBindingPattern;
    function updateArrayBindingPattern(node: ArrayBindingPattern, elements: ArrayBindingElement[]): ArrayBindingPattern;
    function createBindingElement(propertyName: string | PropertyName, dotDotDotToken: DotDotDotToken, name: string | BindingName, initializer?: Expression): BindingElement;
    function updateBindingElement(node: BindingElement, dotDotDotToken: DotDotDotToken, propertyName: PropertyName, name: BindingName, initializer: Expression): BindingElement;
    function createArrayLiteral(elements?: Expression[], multiLine?: boolean): ArrayLiteralExpression;
    function updateArrayLiteral(node: ArrayLiteralExpression, elements: Expression[]): ArrayLiteralExpression;
    function createObjectLiteral(properties?: ObjectLiteralElementLike[], multiLine?: boolean): ObjectLiteralExpression;
    function updateObjectLiteral(node: ObjectLiteralExpression, properties: ObjectLiteralElementLike[]): ObjectLiteralExpression;
    function createPropertyAccess(expression: Expression, name: string | Identifier): PropertyAccessExpression;
    function updatePropertyAccess(node: PropertyAccessExpression, expression: Expression, name: Identifier): PropertyAccessExpression;
    function createElementAccess(expression: Expression, index: number | Expression): ElementAccessExpression;
    function updateElementAccess(node: ElementAccessExpression, expression: Expression, argumentExpression: Expression): ElementAccessExpression;
    function createCall(expression: Expression, typeArguments: TypeNode[], argumentsArray: Expression[]): CallExpression;
    function updateCall(node: CallExpression, expression: Expression, typeArguments: TypeNode[], argumentsArray: Expression[]): CallExpression;
    function createNew(expression: Expression, typeArguments: TypeNode[], argumentsArray: Expression[]): NewExpression;
    function updateNew(node: NewExpression, expression: Expression, typeArguments: TypeNode[], argumentsArray: Expression[]): NewExpression;
    function createTaggedTemplate(tag: Expression, template: TemplateLiteral): TaggedTemplateExpression;
    function updateTaggedTemplate(node: TaggedTemplateExpression, tag: Expression, template: TemplateLiteral): TaggedTemplateExpression;
    function createTypeAssertion(type: TypeNode, expression: Expression): TypeAssertion;
    function updateTypeAssertion(node: TypeAssertion, type: TypeNode, expression: Expression): TypeAssertion;
    function createParen(expression: Expression): ParenthesizedExpression;
    function updateParen(node: ParenthesizedExpression, expression: Expression): ParenthesizedExpression;
    function createFunctionExpression(modifiers: Modifier[], asteriskToken: AsteriskToken, name: string | Identifier, typeParameters: TypeParameterDeclaration[], parameters: ParameterDeclaration[], type: TypeNode, body: Block): FunctionExpression;
    function updateFunctionExpression(node: FunctionExpression, modifiers: Modifier[], name: Identifier, typeParameters: TypeParameterDeclaration[], parameters: ParameterDeclaration[], type: TypeNode, body: Block): FunctionExpression;
    function createArrowFunction(modifiers: Modifier[], typeParameters: TypeParameterDeclaration[], parameters: ParameterDeclaration[], type: TypeNode, equalsGreaterThanToken: EqualsGreaterThanToken, body: ConciseBody): ArrowFunction;
    function updateArrowFunction(node: ArrowFunction, modifiers: Modifier[], typeParameters: TypeParameterDeclaration[], parameters: ParameterDeclaration[], type: TypeNode, body: ConciseBody): ArrowFunction;
    function createDelete(expression: Expression): DeleteExpression;
    function updateDelete(node: DeleteExpression, expression: Expression): DeleteExpression;
    function createTypeOf(expression: Expression): TypeOfExpression;
    function updateTypeOf(node: TypeOfExpression, expression: Expression): TypeOfExpression;
    function createVoid(expression: Expression): VoidExpression;
    function updateVoid(node: VoidExpression, expression: Expression): VoidExpression;
    function createAwait(expression: Expression): AwaitExpression;
    function updateAwait(node: AwaitExpression, expression: Expression): AwaitExpression;
    function createPrefix(operator: PrefixUnaryOperator, operand: Expression): PrefixUnaryExpression;
    function updatePrefix(node: PrefixUnaryExpression, operand: Expression): PrefixUnaryExpression;
    function createPostfix(operand: Expression, operator: PostfixUnaryOperator): PostfixUnaryExpression;
    function updatePostfix(node: PostfixUnaryExpression, operand: Expression): PostfixUnaryExpression;
    function createBinary(left: Expression, operator: BinaryOperator | BinaryOperatorToken, right: Expression): BinaryExpression;
    function updateBinary(node: BinaryExpression, left: Expression, right: Expression): BinaryExpression;
    function createConditional(condition: Expression, whenTrue: Expression, whenFalse: Expression): ConditionalExpression;
    function createConditional(condition: Expression, questionToken: QuestionToken, whenTrue: Expression, colonToken: ColonToken, whenFalse: Expression): ConditionalExpression;
    function updateConditional(node: ConditionalExpression, condition: Expression, whenTrue: Expression, whenFalse: Expression): ConditionalExpression;
    function createTemplateExpression(head: TemplateHead, templateSpans: TemplateSpan[]): TemplateExpression;
    function updateTemplateExpression(node: TemplateExpression, head: TemplateHead, templateSpans: TemplateSpan[]): TemplateExpression;
    function createYield(expression?: Expression): YieldExpression;
    function createYield(asteriskToken: AsteriskToken, expression: Expression): YieldExpression;
    function updateYield(node: YieldExpression, expression: Expression): YieldExpression;
    function createSpread(expression: Expression): SpreadElement;
    function updateSpread(node: SpreadElement, expression: Expression): SpreadElement;
    function createClassExpression(modifiers: Modifier[], name: string | Identifier, typeParameters: TypeParameterDeclaration[], heritageClauses: HeritageClause[], members: ClassElement[]): ClassExpression;
    function updateClassExpression(node: ClassExpression, modifiers: Modifier[], name: Identifier, typeParameters: TypeParameterDeclaration[], heritageClauses: HeritageClause[], members: ClassElement[]): ClassExpression;
    function createOmittedExpression(): OmittedExpression;
    function createExpressionWithTypeArguments(typeArguments: TypeNode[], expression: Expression): ExpressionWithTypeArguments;
    function updateExpressionWithTypeArguments(node: ExpressionWithTypeArguments, typeArguments: TypeNode[], expression: Expression): ExpressionWithTypeArguments;
    function createAsExpression(expression: Expression, type: TypeNode): AsExpression;
    function updateAsExpression(node: AsExpression, expression: Expression, type: TypeNode): AsExpression;
    function createNonNullExpression(expression: Expression): NonNullExpression;
    function updateNonNullExpression(node: NonNullExpression, expression: Expression): NonNullExpression;
    function createTemplateSpan(expression: Expression, literal: TemplateMiddle | TemplateTail): TemplateSpan;
    function updateTemplateSpan(node: TemplateSpan, expression: Expression, literal: TemplateMiddle | TemplateTail): TemplateSpan;
    function createBlock(statements: Statement[], multiLine?: boolean): Block;
    function updateBlock(node: Block, statements: Statement[]): Block;
    function createVariableStatement(modifiers: Modifier[], declarationList: VariableDeclarationList | VariableDeclaration[]): VariableStatement;
    function updateVariableStatement(node: VariableStatement, modifiers: Modifier[], declarationList: VariableDeclarationList): VariableStatement;
    function createVariableDeclarationList(declarations: VariableDeclaration[], flags?: NodeFlags): VariableDeclarationList;
    function updateVariableDeclarationList(node: VariableDeclarationList, declarations: VariableDeclaration[]): VariableDeclarationList;
    function createVariableDeclaration(name: string | BindingName, type?: TypeNode, initializer?: Expression): VariableDeclaration;
    function updateVariableDeclaration(node: VariableDeclaration, name: BindingName, type: TypeNode, initializer: Expression): VariableDeclaration;
    function createEmptyStatement(): EmptyStatement;
    function createStatement(expression: Expression): ExpressionStatement;
    function updateStatement(node: ExpressionStatement, expression: Expression): ExpressionStatement;
    function createIf(expression: Expression, thenStatement: Statement, elseStatement?: Statement): IfStatement;
    function updateIf(node: IfStatement, expression: Expression, thenStatement: Statement, elseStatement: Statement): IfStatement;
    function createDo(statement: Statement, expression: Expression): DoStatement;
    function updateDo(node: DoStatement, statement: Statement, expression: Expression): DoStatement;
    function createWhile(expression: Expression, statement: Statement): WhileStatement;
    function updateWhile(node: WhileStatement, expression: Expression, statement: Statement): WhileStatement;
    function createFor(initializer: ForInitializer, condition: Expression, incrementor: Expression, statement: Statement): ForStatement;
    function updateFor(node: ForStatement, initializer: ForInitializer, condition: Expression, incrementor: Expression, statement: Statement): ForStatement;
    function createForIn(initializer: ForInitializer, expression: Expression, statement: Statement): ForInStatement;
    function updateForIn(node: ForInStatement, initializer: ForInitializer, expression: Expression, statement: Statement): ForInStatement;
    function createForOf(initializer: ForInitializer, expression: Expression, statement: Statement): ForOfStatement;
    function updateForOf(node: ForOfStatement, initializer: ForInitializer, expression: Expression, statement: Statement): ForOfStatement;
    function createContinue(label?: string | Identifier): ContinueStatement;
    function updateContinue(node: ContinueStatement, label: Identifier): ContinueStatement;
    function createBreak(label?: string | Identifier): BreakStatement;
    function updateBreak(node: BreakStatement, label: Identifier): BreakStatement;
    function createReturn(expression?: Expression): ReturnStatement;
    function updateReturn(node: ReturnStatement, expression: Expression): ReturnStatement;
    function createWith(expression: Expression, statement: Statement): WithStatement;
    function updateWith(node: WithStatement, expression: Expression, statement: Statement): WithStatement;
    function createSwitch(expression: Expression, caseBlock: CaseBlock): SwitchStatement;
    function updateSwitch(node: SwitchStatement, expression: Expression, caseBlock: CaseBlock): SwitchStatement;
    function createLabel(label: string | Identifier, statement: Statement): LabeledStatement;
    function updateLabel(node: LabeledStatement, label: Identifier, statement: Statement): LabeledStatement;
    function createThrow(expression: Expression): ThrowStatement;
    function updateThrow(node: ThrowStatement, expression: Expression): ThrowStatement;
    function createTry(tryBlock: Block, catchClause: CatchClause, finallyBlock: Block): TryStatement;
    function updateTry(node: TryStatement, tryBlock: Block, catchClause: CatchClause, finallyBlock: Block): TryStatement;
    function createFunctionDeclaration(decorators: Decorator[], modifiers: Modifier[], asteriskToken: AsteriskToken, name: string | Identifier, typeParameters: TypeParameterDeclaration[], parameters: ParameterDeclaration[], type: TypeNode, body: Block): FunctionDeclaration;
    function updateFunctionDeclaration(node: FunctionDeclaration, decorators: Decorator[], modifiers: Modifier[], name: Identifier, typeParameters: TypeParameterDeclaration[], parameters: ParameterDeclaration[], type: TypeNode, body: Block): FunctionDeclaration;
    function createClassDeclaration(decorators: Decorator[], modifiers: Modifier[], name: string | Identifier, typeParameters: TypeParameterDeclaration[], heritageClauses: HeritageClause[], members: ClassElement[]): ClassDeclaration;
    function updateClassDeclaration(node: ClassDeclaration, decorators: Decorator[], modifiers: Modifier[], name: Identifier, typeParameters: TypeParameterDeclaration[], heritageClauses: HeritageClause[], members: ClassElement[]): ClassDeclaration;
    function createEnumDeclaration(decorators: Decorator[], modifiers: Modifier[], name: string | Identifier, members: EnumMember[]): EnumDeclaration;
    function updateEnumDeclaration(node: EnumDeclaration, decorators: Decorator[], modifiers: Modifier[], name: Identifier, members: EnumMember[]): EnumDeclaration;
    function createModuleDeclaration(decorators: Decorator[], modifiers: Modifier[], name: ModuleName, body: ModuleBody, flags?: NodeFlags): ModuleDeclaration;
    function updateModuleDeclaration(node: ModuleDeclaration, decorators: Decorator[], modifiers: Modifier[], name: ModuleName, body: ModuleBody): ModuleDeclaration;
    function createModuleBlock(statements: Statement[]): ModuleBlock;
    function updateModuleBlock(node: ModuleBlock, statements: Statement[]): ModuleBlock;
    function createCaseBlock(clauses: CaseOrDefaultClause[]): CaseBlock;
    function updateCaseBlock(node: CaseBlock, clauses: CaseOrDefaultClause[]): CaseBlock;
    function createImportEqualsDeclaration(decorators: Decorator[], modifiers: Modifier[], name: string | Identifier, moduleReference: ModuleReference): ImportEqualsDeclaration;
    function updateImportEqualsDeclaration(node: ImportEqualsDeclaration, decorators: Decorator[], modifiers: Modifier[], name: Identifier, moduleReference: ModuleReference): ImportEqualsDeclaration;
    function createImportDeclaration(decorators: Decorator[], modifiers: Modifier[], importClause: ImportClause, moduleSpecifier?: Expression): ImportDeclaration;
    function updateImportDeclaration(node: ImportDeclaration, decorators: Decorator[], modifiers: Modifier[], importClause: ImportClause, moduleSpecifier: Expression): ImportDeclaration;
    function createImportClause(name: Identifier, namedBindings: NamedImportBindings): ImportClause;
    function updateImportClause(node: ImportClause, name: Identifier, namedBindings: NamedImportBindings): ImportClause;
    function createNamespaceImport(name: Identifier): NamespaceImport;
    function updateNamespaceImport(node: NamespaceImport, name: Identifier): NamespaceImport;
    function createNamedImports(elements: ImportSpecifier[]): NamedImports;
    function updateNamedImports(node: NamedImports, elements: ImportSpecifier[]): NamedImports;
    function createImportSpecifier(propertyName: Identifier, name: Identifier): ImportSpecifier;
    function updateImportSpecifier(node: ImportSpecifier, propertyName: Identifier, name: Identifier): ImportSpecifier;
    function createExportAssignment(decorators: Decorator[], modifiers: Modifier[], isExportEquals: boolean, expression: Expression): ExportAssignment;
    function updateExportAssignment(node: ExportAssignment, decorators: Decorator[], modifiers: Modifier[], expression: Expression): ExportAssignment;
    function createExportDeclaration(decorators: Decorator[], modifiers: Modifier[], exportClause: NamedExports, moduleSpecifier?: Expression): ExportDeclaration;
    function updateExportDeclaration(node: ExportDeclaration, decorators: Decorator[], modifiers: Modifier[], exportClause: NamedExports, moduleSpecifier: Expression): ExportDeclaration;
    function createNamedExports(elements: ExportSpecifier[]): NamedExports;
    function updateNamedExports(node: NamedExports, elements: ExportSpecifier[]): NamedExports;
    function createExportSpecifier(name: string | Identifier, propertyName?: string | Identifier): ExportSpecifier;
    function updateExportSpecifier(node: ExportSpecifier, name: Identifier, propertyName: Identifier): ExportSpecifier;
    function createExternalModuleReference(expression: Expression): ExternalModuleReference;
    function updateExternalModuleReference(node: ExternalModuleReference, expression: Expression): ExternalModuleReference;
    function createJsxElement(openingElement: JsxOpeningElement, children: JsxChild[], closingElement: JsxClosingElement): JsxElement;
    function updateJsxElement(node: JsxElement, openingElement: JsxOpeningElement, children: JsxChild[], closingElement: JsxClosingElement): JsxElement;
    function createJsxSelfClosingElement(tagName: JsxTagNameExpression, attributes: JsxAttributeLike[]): JsxSelfClosingElement;
    function updateJsxSelfClosingElement(node: JsxSelfClosingElement, tagName: JsxTagNameExpression, attributes: JsxAttributeLike[]): JsxSelfClosingElement;
    function createJsxOpeningElement(tagName: JsxTagNameExpression, attributes: JsxAttributeLike[]): JsxOpeningElement;
    function updateJsxOpeningElement(node: JsxOpeningElement, tagName: JsxTagNameExpression, attributes: JsxAttributeLike[]): JsxOpeningElement;
    function createJsxClosingElement(tagName: JsxTagNameExpression): JsxClosingElement;
    function updateJsxClosingElement(node: JsxClosingElement, tagName: JsxTagNameExpression): JsxClosingElement;
    function createJsxAttribute(name: Identifier, initializer: StringLiteral | JsxExpression): JsxAttribute;
    function updateJsxAttribute(node: JsxAttribute, name: Identifier, initializer: StringLiteral | JsxExpression): JsxAttribute;
    function createJsxSpreadAttribute(expression: Expression): JsxSpreadAttribute;
    function updateJsxSpreadAttribute(node: JsxSpreadAttribute, expression: Expression): JsxSpreadAttribute;
    function createJsxExpression(expression: Expression, dotDotDotToken: DotDotDotToken): JsxExpression;
    function updateJsxExpression(node: JsxExpression, expression: Expression): JsxExpression;
    function createHeritageClause(token: SyntaxKind, types: ExpressionWithTypeArguments[]): HeritageClause;
    function updateHeritageClause(node: HeritageClause, types: ExpressionWithTypeArguments[]): HeritageClause;
    function createCaseClause(expression: Expression, statements: Statement[]): CaseClause;
    function updateCaseClause(node: CaseClause, expression: Expression, statements: Statement[]): CaseClause;
    function createDefaultClause(statements: Statement[]): DefaultClause;
    function updateDefaultClause(node: DefaultClause, statements: Statement[]): DefaultClause;
    function createCatchClause(variableDeclaration: string | VariableDeclaration, block: Block): CatchClause;
    function updateCatchClause(node: CatchClause, variableDeclaration: VariableDeclaration, block: Block): CatchClause;
    function createPropertyAssignment(name: string | PropertyName, initializer: Expression): PropertyAssignment;
    function updatePropertyAssignment(node: PropertyAssignment, name: PropertyName, initializer: Expression): PropertyAssignment;
    function createShorthandPropertyAssignment(name: string | Identifier, objectAssignmentInitializer: Expression): ShorthandPropertyAssignment;
    function createSpreadAssignment(expression: Expression): SpreadAssignment;
    function updateShorthandPropertyAssignment(node: ShorthandPropertyAssignment, name: Identifier, objectAssignmentInitializer: Expression): ShorthandPropertyAssignment;
    function updateSpreadAssignment(node: SpreadAssignment, expression: Expression): SpreadAssignment;
    function createEnumMember(name: string | PropertyName, initializer?: Expression): EnumMember;
    function updateEnumMember(node: EnumMember, name: PropertyName, initializer: Expression | undefined): EnumMember;
    function updateSourceFileNode(node: SourceFile, statements: Statement[]): SourceFile;
    /**
     * Creates a shallow, memberwise clone of a node for mutation.
     */
    function getMutableClone<T extends Node>(node: T): T;
    /**
     * Creates a synthetic statement to act as a placeholder for a not-emitted statement in
     * order to preserve comments.
     *
     * @param original The original statement.
     */
    function createNotEmittedStatement(original: Node): NotEmittedStatement;
    /**
     * Creates a synthetic expression to act as a placeholder for a not-emitted expression in
     * order to preserve comments or sourcemap positions.
     *
     * @param expression The inner expression to emit.
     * @param original The original outer expression.
     * @param location The location for the expression. Defaults to the positions from "original" if provided.
     */
    function createPartiallyEmittedExpression(expression: Expression, original?: Node): PartiallyEmittedExpression;
    function updatePartiallyEmittedExpression(node: PartiallyEmittedExpression, expression: Expression): PartiallyEmittedExpression;
    function createBundle(sourceFiles: SourceFile[]): Bundle;
    function updateBundle(node: Bundle, sourceFiles: SourceFile[]): Bundle;
    function createComma(left: Expression, right: Expression): Expression;
    function createLessThan(left: Expression, right: Expression): Expression;
    function createAssignment(left: ObjectLiteralExpression | ArrayLiteralExpression, right: Expression): DestructuringAssignment;
    function createAssignment(left: Expression, right: Expression): BinaryExpression;
    function createStrictEquality(left: Expression, right: Expression): BinaryExpression;
    function createStrictInequality(left: Expression, right: Expression): BinaryExpression;
    function createAdd(left: Expression, right: Expression): BinaryExpression;
    function createSubtract(left: Expression, right: Expression): BinaryExpression;
    function createPostfixIncrement(operand: Expression): PostfixUnaryExpression;
    function createLogicalAnd(left: Expression, right: Expression): BinaryExpression;
    function createLogicalOr(left: Expression, right: Expression): BinaryExpression;
    function createLogicalNot(operand: Expression): PrefixUnaryExpression;
    function createVoidZero(): VoidExpression;
    function createExportDefault(expression: Expression): ExportAssignment;
    function createExternalModuleExport(exportName: Identifier): ExportDeclaration;
    /**
     * Clears any EmitNode entries from parse-tree nodes.
     * @param sourceFile A source file.
     */
    function disposeEmitNodes(sourceFile: SourceFile): void;
    function setTextRange<T extends TextRange>(range: T, location: TextRange | undefined): T;
    /**
     * Gets flags that control emit behavior of a node.
     */
    function getEmitFlags(node: Node): EmitFlags;
    /**
     * Sets flags that control emit behavior of a node.
     */
    function setEmitFlags<T extends Node>(node: T, emitFlags: EmitFlags): T;
    /**
     * Gets a custom text range to use when emitting source maps.
     */
    function getSourceMapRange(node: Node): TextRange;
    /**
     * Sets a custom text range to use when emitting source maps.
     */
    function setSourceMapRange<T extends Node>(node: T, range: TextRange): T;
    /**
     * Gets the TextRange to use for source maps for a token of a node.
     */
    function getTokenSourceMapRange(node: Node, token: SyntaxKind): TextRange;
    /**
     * Sets the TextRange to use for source maps for a token of a node.
     */
    function setTokenSourceMapRange<T extends Node>(node: T, token: SyntaxKind, range: TextRange): T;
    /**
     * Gets a custom text range to use when emitting comments.
     */
    function getCommentRange(node: Node): TextRange;
    /**
     * Sets a custom text range to use when emitting comments.
     */
    function setCommentRange<T extends Node>(node: T, range: TextRange): T;
    /**
     * Gets the constant value to emit for an expression.
     */
    function getConstantValue(node: PropertyAccessExpression | ElementAccessExpression): number;
    /**
     * Sets the constant value to emit for an expression.
     */
    function setConstantValue(node: PropertyAccessExpression | ElementAccessExpression, value: number): PropertyAccessExpression | ElementAccessExpression;
    /**
     * Adds an EmitHelper to a node.
     */
    function addEmitHelper<T extends Node>(node: T, helper: EmitHelper): T;
    /**
     * Add EmitHelpers to a node.
     */
    function addEmitHelpers<T extends Node>(node: T, helpers: EmitHelper[] | undefined): T;
    /**
     * Removes an EmitHelper from a node.
     */
    function removeEmitHelper(node: Node, helper: EmitHelper): boolean;
    /**
     * Gets the EmitHelpers of a node.
     */
    function getEmitHelpers(node: Node): EmitHelper[] | undefined;
    /**
     * Moves matching emit helpers from a source node to a target node.
     */
    function moveEmitHelpers(source: Node, target: Node, predicate: (helper: EmitHelper) => boolean): void;
    function setOriginalNode<T extends Node>(node: T, original: Node): T;
}
declare namespace ts {
    function createNode(kind: SyntaxKind, pos?: number, end?: number): Node;
    function forEachChild<T>(node: Node, cbNode: (node: Node) => T, cbNodeArray?: (nodes: Node[]) => T): T;
    function createSourceFile(fileName: string, sourceText: string, languageVersion: ScriptTarget, setParentNodes?: boolean, scriptKind?: ScriptKind): SourceFile;
    function parseIsolatedEntityName(text: string, languageVersion: ScriptTarget): EntityName;
    function isExternalModule(file: SourceFile): boolean;
    function updateSourceFile(sourceFile: SourceFile, newText: string, textChangeRange: TextChangeRange, aggressiveChecks?: boolean): SourceFile;
}
declare namespace ts {
    function createPrinter(printerOptions?: PrinterOptions, handlers?: PrintHandlers): Printer;
}
declare namespace ts {
    function findConfigFile(searchPath: string, fileExists: (fileName: string) => boolean, configName?: string): string;
    function resolveTripleslashReference(moduleName: string, containingFile: string): string;
    function createCompilerHost(options: CompilerOptions, setParentNodes?: boolean): CompilerHost;
    function getPreEmitDiagnostics(program: Program, sourceFile?: SourceFile, cancellationToken?: CancellationToken): Diagnostic[];
    interface FormatDiagnosticsHost {
        getCurrentDirectory(): string;
        getCanonicalFileName(fileName: string): string;
        getNewLine(): string;
    }
    function formatDiagnostics(diagnostics: Diagnostic[], host: FormatDiagnosticsHost): string;
    function flattenDiagnosticMessageText(messageText: string | DiagnosticMessageChain, newLine: string): string;
    function createProgram(rootNames: string[], options: CompilerOptions, host?: CompilerHost, oldProgram?: Program): Program;
}
declare namespace ts {
    interface Node {
        getSourceFile(): SourceFile;
        getChildCount(sourceFile?: SourceFile): number;
        getChildAt(index: number, sourceFile?: SourceFile): Node;
        getChildren(sourceFile?: SourceFile): Node[];
        getStart(sourceFile?: SourceFile, includeJsDocComment?: boolean): number;
        getFullStart(): number;
        getEnd(): number;
        getWidth(sourceFile?: SourceFile): number;
        getFullWidth(): number;
        getLeadingTriviaWidth(sourceFile?: SourceFile): number;
        getFullText(sourceFile?: SourceFile): string;
        getText(sourceFile?: SourceFile): string;
        getFirstToken(sourceFile?: SourceFile): Node;
        getLastToken(sourceFile?: SourceFile): Node;
    }
    interface Symbol {
        getFlags(): SymbolFlags;
        getName(): string;
        getDeclarations(): Declaration[];
        getDocumentationComment(): SymbolDisplayPart[];
    }
    interface Type {
        getFlags(): TypeFlags;
        getSymbol(): Symbol;
        getProperties(): Symbol[];
        getProperty(propertyName: string): Symbol;
        getApparentProperties(): Symbol[];
        getCallSignatures(): Signature[];
        getConstructSignatures(): Signature[];
        getStringIndexType(): Type;
        getNumberIndexType(): Type;
        getBaseTypes(): BaseType[];
        getNonNullableType(): Type;
    }
    interface Signature {
        getDeclaration(): SignatureDeclaration;
        getTypeParameters(): Type[];
        getParameters(): Symbol[];
        getReturnType(): Type;
        getDocumentationComment(): SymbolDisplayPart[];
    }
    interface SourceFile {
        getLineAndCharacterOfPosition(pos: number): LineAndCharacter;
        getLineEndOfPosition(pos: number): number;
        getLineStarts(): number[];
        getPositionOfLineAndCharacter(line: number, character: number): number;
        update(newText: string, textChangeRange: TextChangeRange): SourceFile;
    }
    /**
     * Represents an immutable snapshot of a script at a specified time.Once acquired, the
     * snapshot is observably immutable. i.e. the same calls with the same parameters will return
     * the same values.
     */
    interface IScriptSnapshot {
        /** Gets a portion of the script snapshot specified by [start, end). */
        getText(start: number, end: number): string;
        /** Gets the length of this script snapshot. */
        getLength(): number;
        /**
         * Gets the TextChangeRange that describe how the text changed between this text and
         * an older version.  This information is used by the incremental parser to determine
         * what sections of the script need to be re-parsed.  'undefined' can be returned if the
         * change range cannot be determined.  However, in that case, incremental parsing will
         * not happen and the entire document will be re - parsed.
         */
        getChangeRange(oldSnapshot: IScriptSnapshot): TextChangeRange | undefined;
        /** Releases all resources held by this script snapshot */
        dispose?(): void;
    }
    namespace ScriptSnapshot {
        function fromString(text: string): IScriptSnapshot;
    }
    interface PreProcessedFileInfo {
        referencedFiles: FileReference[];
        typeReferenceDirectives: FileReference[];
        importedFiles: FileReference[];
        ambientExternalModules: string[];
        isLibFile: boolean;
    }
    interface HostCancellationToken {
        isCancellationRequested(): boolean;
    }
    interface LanguageServiceHost {
        getCompilationSettings(): CompilerOptions;
        getNewLine?(): string;
        getProjectVersion?(): string;
        getScriptFileNames(): string[];
        getScriptKind?(fileName: string): ScriptKind;
        getScriptVersion(fileName: string): string;
        getScriptSnapshot(fileName: string): IScriptSnapshot | undefined;
        getLocalizedDiagnosticMessages?(): any;
        getCancellationToken?(): HostCancellationToken;
        getCurrentDirectory(): string;
        getDefaultLibFileName(options: CompilerOptions): string;
        log?(s: string): void;
        trace?(s: string): void;
        error?(s: string): void;
        useCaseSensitiveFileNames?(): boolean;
        readDirectory?(path: string, extensions?: string[], exclude?: string[], include?: string[]): string[];
        readFile?(path: string, encoding?: string): string;
        fileExists?(path: string): boolean;
        getTypeRootsVersion?(): number;
        resolveModuleNames?(moduleNames: string[], containingFile: string): ResolvedModule[];
        resolveTypeReferenceDirectives?(typeDirectiveNames: string[], containingFile: string): ResolvedTypeReferenceDirective[];
        directoryExists?(directoryName: string): boolean;
        getDirectories?(directoryName: string): string[];
    }
    interface LanguageService {
        cleanupSemanticCache(): void;
        getSyntacticDiagnostics(fileName: string): Diagnostic[];
        getSemanticDiagnostics(fileName: string): Diagnostic[];
        getCompilerOptionsDiagnostics(): Diagnostic[];
        /**
         * @deprecated Use getEncodedSyntacticClassifications instead.
         */
        getSyntacticClassifications(fileName: string, span: TextSpan): ClassifiedSpan[];
        /**
         * @deprecated Use getEncodedSemanticClassifications instead.
         */
        getSemanticClassifications(fileName: string, span: TextSpan): ClassifiedSpan[];
        getEncodedSyntacticClassifications(fileName: string, span: TextSpan): Classifications;
        getEncodedSemanticClassifications(fileName: string, span: TextSpan): Classifications;
        getCompletionsAtPosition(fileName: string, position: number): CompletionInfo;
        getCompletionEntryDetails(fileName: string, position: number, entryName: string): CompletionEntryDetails;
        getCompletionEntrySymbol(fileName: string, position: number, entryName: string): Symbol;
        getQuickInfoAtPosition(fileName: string, position: number): QuickInfo;
        getNameOrDottedNameSpan(fileName: string, startPos: number, endPos: number): TextSpan;
        getBreakpointStatementAtPosition(fileName: string, position: number): TextSpan;
        getSignatureHelpItems(fileName: string, position: number): SignatureHelpItems;
        getRenameInfo(fileName: string, position: number): RenameInfo;
        findRenameLocations(fileName: string, position: number, findInStrings: boolean, findInComments: boolean): RenameLocation[];
        getDefinitionAtPosition(fileName: string, position: number): DefinitionInfo[];
        getTypeDefinitionAtPosition(fileName: string, position: number): DefinitionInfo[];
        getImplementationAtPosition(fileName: string, position: number): ImplementationLocation[];
        getReferencesAtPosition(fileName: string, position: number): ReferenceEntry[];
        findReferences(fileName: string, position: number): ReferencedSymbol[];
        getDocumentHighlights(fileName: string, position: number, filesToSearch: string[]): DocumentHighlights[];
        /** @deprecated */
        getOccurrencesAtPosition(fileName: string, position: number): ReferenceEntry[];
        getNavigateToItems(searchValue: string, maxResultCount?: number, fileName?: string, excludeDtsFiles?: boolean): NavigateToItem[];
        getNavigationBarItems(fileName: string): NavigationBarItem[];
        getNavigationTree(fileName: string): NavigationTree;
        getOutliningSpans(fileName: string): OutliningSpan[];
        getTodoComments(fileName: string, descriptors: TodoCommentDescriptor[]): TodoComment[];
        getBraceMatchingAtPosition(fileName: string, position: number): TextSpan[];
        getIndentationAtPosition(fileName: string, position: number, options: EditorOptions | EditorSettings): number;
        getFormattingEditsForRange(fileName: string, start: number, end: number, options: FormatCodeOptions | FormatCodeSettings): TextChange[];
        getFormattingEditsForDocument(fileName: string, options: FormatCodeOptions | FormatCodeSettings): TextChange[];
        getFormattingEditsAfterKeystroke(fileName: string, position: number, key: string, options: FormatCodeOptions | FormatCodeSettings): TextChange[];
        getDocCommentTemplateAtPosition(fileName: string, position: number): TextInsertion;
        isValidBraceCompletionAtPosition(fileName: string, position: number, openingBrace: number): boolean;
        getCodeFixesAtPosition(fileName: string, start: number, end: number, errorCodes: number[]): CodeAction[];
        getEmitOutput(fileName: string, emitOnlyDtsFiles?: boolean): EmitOutput;
        getProgram(): Program;
        dispose(): void;
    }
    interface Classifications {
        spans: number[];
        endOfLineState: EndOfLineState;
    }
    interface ClassifiedSpan {
        textSpan: TextSpan;
        classificationType: string;
    }
    /**
     * Navigation bar interface designed for visual studio's dual-column layout.
     * This does not form a proper tree.
     * The navbar is returned as a list of top-level items, each of which has a list of child items.
     * Child items always have an empty array for their `childItems`.
     */
    interface NavigationBarItem {
        text: string;
        kind: string;
        kindModifiers: string;
        spans: TextSpan[];
        childItems: NavigationBarItem[];
        indent: number;
        bolded: boolean;
        grayed: boolean;
    }
    /**
     * Node in a tree of nested declarations in a file.
     * The top node is always a script or module node.
     */
    interface NavigationTree {
        /** Name of the declaration, or a short description, e.g. "<class>". */
        text: string;
        /** A ScriptElementKind */
        kind: string;
        /** ScriptElementKindModifier separated by commas, e.g. "public,abstract" */
        kindModifiers: string;
        /**
         * Spans of the nodes that generated this declaration.
         * There will be more than one if this is the result of merging.
         */
        spans: TextSpan[];
        /** Present if non-empty */
        childItems?: NavigationTree[];
    }
    interface TodoCommentDescriptor {
        text: string;
        priority: number;
    }
    interface TodoComment {
        descriptor: TodoCommentDescriptor;
        message: string;
        position: number;
    }
    class TextChange {
        span: TextSpan;
        newText: string;
    }
    interface FileTextChanges {
        fileName: string;
        textChanges: TextChange[];
    }
    interface CodeAction {
        /** Description of the code action to display in the UI of the editor */
        description: string;
        /** Text changes to apply to each file as part of the code action */
        changes: FileTextChanges[];
    }
    interface TextInsertion {
        newText: string;
        /** The position in newText the caret should point to after the insertion. */
        caretOffset: number;
    }
    interface RenameLocation {
        textSpan: TextSpan;
        fileName: string;
    }
    interface ReferenceEntry {
        textSpan: TextSpan;
        fileName: string;
        isWriteAccess: boolean;
        isDefinition: boolean;
    }
    interface ImplementationLocation {
        textSpan: TextSpan;
        fileName: string;
    }
    interface DocumentHighlights {
        fileName: string;
        highlightSpans: HighlightSpan[];
    }
    namespace HighlightSpanKind {
        const none = "none";
        const definition = "definition";
        const reference = "reference";
        const writtenReference = "writtenReference";
    }
    interface HighlightSpan {
        fileName?: string;
        textSpan: TextSpan;
        kind: string;
    }
    interface NavigateToItem {
        name: string;
        kind: string;
        kindModifiers: string;
        matchKind: string;
        isCaseSensitive: boolean;
        fileName: string;
        textSpan: TextSpan;
        containerName: string;
        containerKind: string;
    }
    enum IndentStyle {
        None = 0,
        Block = 1,
        Smart = 2,
    }
    interface EditorOptions {
        BaseIndentSize?: number;
        IndentSize: number;
        TabSize: number;
        NewLineCharacter: string;
        ConvertTabsToSpaces: boolean;
        IndentStyle: IndentStyle;
    }
    interface EditorSettings {
        baseIndentSize?: number;
        indentSize?: number;
        tabSize?: number;
        newLineCharacter?: string;
        convertTabsToSpaces?: boolean;
        indentStyle?: IndentStyle;
    }
    interface FormatCodeOptions extends EditorOptions {
        InsertSpaceAfterCommaDelimiter: boolean;
        InsertSpaceAfterSemicolonInForStatements: boolean;
        InsertSpaceBeforeAndAfterBinaryOperators: boolean;
        InsertSpaceAfterConstructor?: boolean;
        InsertSpaceAfterKeywordsInControlFlowStatements: boolean;
        InsertSpaceAfterFunctionKeywordForAnonymousFunctions: boolean;
        InsertSpaceAfterOpeningAndBeforeClosingNonemptyParenthesis: boolean;
        InsertSpaceAfterOpeningAndBeforeClosingNonemptyBrackets: boolean;
        InsertSpaceAfterOpeningAndBeforeClosingNonemptyBraces?: boolean;
        InsertSpaceAfterOpeningAndBeforeClosingTemplateStringBraces: boolean;
        InsertSpaceAfterOpeningAndBeforeClosingJsxExpressionBraces?: boolean;
        InsertSpaceAfterTypeAssertion?: boolean;
        InsertSpaceBeforeFunctionParenthesis?: boolean;
        PlaceOpenBraceOnNewLineForFunctions: boolean;
        PlaceOpenBraceOnNewLineForControlBlocks: boolean;
    }
    interface FormatCodeSettings extends EditorSettings {
        insertSpaceAfterCommaDelimiter?: boolean;
        insertSpaceAfterSemicolonInForStatements?: boolean;
        insertSpaceBeforeAndAfterBinaryOperators?: boolean;
        insertSpaceAfterConstructor?: boolean;
        insertSpaceAfterKeywordsInControlFlowStatements?: boolean;
        insertSpaceAfterFunctionKeywordForAnonymousFunctions?: boolean;
        insertSpaceAfterOpeningAndBeforeClosingNonemptyParenthesis?: boolean;
        insertSpaceAfterOpeningAndBeforeClosingNonemptyBrackets?: boolean;
        insertSpaceAfterOpeningAndBeforeClosingNonemptyBraces?: boolean;
        insertSpaceAfterOpeningAndBeforeClosingTemplateStringBraces?: boolean;
        insertSpaceAfterOpeningAndBeforeClosingJsxExpressionBraces?: boolean;
        insertSpaceAfterTypeAssertion?: boolean;
        insertSpaceBeforeFunctionParenthesis?: boolean;
        placeOpenBraceOnNewLineForFunctions?: boolean;
        placeOpenBraceOnNewLineForControlBlocks?: boolean;
    }
    interface DefinitionInfo {
        fileName: string;
        textSpan: TextSpan;
        kind: string;
        name: string;
        containerKind: string;
        containerName: string;
    }
    interface ReferencedSymbolDefinitionInfo extends DefinitionInfo {
        displayParts: SymbolDisplayPart[];
    }
    interface ReferencedSymbol {
        definition: ReferencedSymbolDefinitionInfo;
        references: ReferenceEntry[];
    }
    enum SymbolDisplayPartKind {
        aliasName = 0,
        className = 1,
        enumName = 2,
        fieldName = 3,
        interfaceName = 4,
        keyword = 5,
        lineBreak = 6,
        numericLiteral = 7,
        stringLiteral = 8,
        localName = 9,
        methodName = 10,
        moduleName = 11,
        operator = 12,
        parameterName = 13,
        propertyName = 14,
        punctuation = 15,
        space = 16,
        text = 17,
        typeParameterName = 18,
        enumMemberName = 19,
        functionName = 20,
        regularExpressionLiteral = 21,
    }
    interface SymbolDisplayPart {
        text: string;
        kind: string;
    }
    interface QuickInfo {
        kind: string;
        kindModifiers: string;
        textSpan: TextSpan;
        displayParts: SymbolDisplayPart[];
        documentation: SymbolDisplayPart[];
    }
    interface RenameInfo {
        canRename: boolean;
        localizedErrorMessage: string;
        displayName: string;
        fullDisplayName: string;
        kind: string;
        kindModifiers: string;
        triggerSpan: TextSpan;
    }
    interface SignatureHelpParameter {
        name: string;
        documentation: SymbolDisplayPart[];
        displayParts: SymbolDisplayPart[];
        isOptional: boolean;
    }
    /**
     * Represents a single signature to show in signature help.
     * The id is used for subsequent calls into the language service to ask questions about the
     * signature help item in the context of any documents that have been updated.  i.e. after
     * an edit has happened, while signature help is still active, the host can ask important
     * questions like 'what parameter is the user currently contained within?'.
     */
    interface SignatureHelpItem {
        isVariadic: boolean;
        prefixDisplayParts: SymbolDisplayPart[];
        suffixDisplayParts: SymbolDisplayPart[];
        separatorDisplayParts: SymbolDisplayPart[];
        parameters: SignatureHelpParameter[];
        documentation: SymbolDisplayPart[];
    }
    /**
     * Represents a set of signature help items, and the preferred item that should be selected.
     */
    interface SignatureHelpItems {
        items: SignatureHelpItem[];
        applicableSpan: TextSpan;
        selectedItemIndex: number;
        argumentIndex: number;
        argumentCount: number;
    }
    interface CompletionInfo {
        isGlobalCompletion: boolean;
        isMemberCompletion: boolean;
        /**
         * true when the current location also allows for a new identifier
         */
        isNewIdentifierLocation: boolean;
        entries: CompletionEntry[];
    }
    interface CompletionEntry {
        name: string;
        kind: string;
        kindModifiers: string;
        sortText: string;
        /**
          * An optional span that indicates the text to be replaced by this completion item. It will be
          * set if the required span differs from the one generated by the default replacement behavior and should
          * be used in that case
          */
        replacementSpan?: TextSpan;
    }
    interface CompletionEntryDetails {
        name: string;
        kind: string;
        kindModifiers: string;
        displayParts: SymbolDisplayPart[];
        documentation: SymbolDisplayPart[];
    }
    interface OutliningSpan {
        /** The span of the document to actually collapse. */
        textSpan: TextSpan;
        /** The span of the document to display when the user hovers over the collapsed span. */
        hintSpan: TextSpan;
        /** The text to display in the editor for the collapsed region. */
        bannerText: string;
        /**
          * Whether or not this region should be automatically collapsed when
          * the 'Collapse to Definitions' command is invoked.
          */
        autoCollapse: boolean;
    }
    interface EmitOutput {
        outputFiles: OutputFile[];
        emitSkipped: boolean;
    }
    const enum OutputFileType {
        JavaScript = 0,
        SourceMap = 1,
        Declaration = 2,
    }
    interface OutputFile {
        name: string;
        writeByteOrderMark: boolean;
        text: string;
    }
    const enum EndOfLineState {
        None = 0,
        InMultiLineCommentTrivia = 1,
        InSingleQuoteStringLiteral = 2,
        InDoubleQuoteStringLiteral = 3,
        InTemplateHeadOrNoSubstitutionTemplate = 4,
        InTemplateMiddleOrTail = 5,
        InTemplateSubstitutionPosition = 6,
    }
    enum TokenClass {
        Punctuation = 0,
        Keyword = 1,
        Operator = 2,
        Comment = 3,
        Whitespace = 4,
        Identifier = 5,
        NumberLiteral = 6,
        StringLiteral = 7,
        RegExpLiteral = 8,
    }
    interface ClassificationResult {
        finalLexState: EndOfLineState;
        entries: ClassificationInfo[];
    }
    interface ClassificationInfo {
        length: number;
        classification: TokenClass;
    }
    interface Classifier {
        /**
         * Gives lexical classifications of tokens on a line without any syntactic context.
         * For instance, a token consisting of the text 'string' can be either an identifier
         * named 'string' or the keyword 'string', however, because this classifier is not aware,
         * it relies on certain heuristics to give acceptable results. For classifications where
         * speed trumps accuracy, this function is preferable; however, for true accuracy, the
         * syntactic classifier is ideal. In fact, in certain editing scenarios, combining the
         * lexical, syntactic, and semantic classifiers may issue the best user experience.
         *
         * @param text                      The text of a line to classify.
         * @param lexState                  The state of the lexical classifier at the end of the previous line.
         * @param syntacticClassifierAbsent Whether the client is *not* using a syntactic classifier.
         *                                  If there is no syntactic classifier (syntacticClassifierAbsent=true),
         *                                  certain heuristics may be used in its place; however, if there is a
         *                                  syntactic classifier (syntacticClassifierAbsent=false), certain
         *                                  classifications which may be incorrectly categorized will be given
         *                                  back as Identifiers in order to allow the syntactic classifier to
         *                                  subsume the classification.
         * @deprecated Use getLexicalClassifications instead.
         */
        getClassificationsForLine(text: string, lexState: EndOfLineState, syntacticClassifierAbsent: boolean): ClassificationResult;
        getEncodedLexicalClassifications(text: string, endOfLineState: EndOfLineState, syntacticClassifierAbsent: boolean): Classifications;
    }
    namespace ScriptElementKind {
        const unknown = "";
        const warning = "warning";
        /** predefined type (void) or keyword (class) */
        const keyword = "keyword";
        /** top level script node */
        const scriptElement = "script";
        /** module foo {} */
        const moduleElement = "module";
        /** class X {} */
        const classElement = "class";
        /** var x = class X {} */
        const localClassElement = "local class";
        /** interface Y {} */
        const interfaceElement = "interface";
        /** type T = ... */
        const typeElement = "type";
        /** enum E */
        const enumElement = "enum";
        const enumMemberElement = "const";
        /**
         * Inside module and script only
         * const v = ..
         */
        const variableElement = "var";
        /** Inside function */
        const localVariableElement = "local var";
        /**
         * Inside module and script only
         * function f() { }
         */
        const functionElement = "function";
        /** Inside function */
        const localFunctionElement = "local function";
        /** class X { [public|private]* foo() {} } */
        const memberFunctionElement = "method";
        /** class X { [public|private]* [get|set] foo:number; } */
        const memberGetAccessorElement = "getter";
        const memberSetAccessorElement = "setter";
        /**
         * class X { [public|private]* foo:number; }
         * interface Y { foo:number; }
         */
        const memberVariableElement = "property";
        /** class X { constructor() { } } */
        const constructorImplementationElement = "constructor";
        /** interface Y { ():number; } */
        const callSignatureElement = "call";
        /** interface Y { []:number; } */
        const indexSignatureElement = "index";
        /** interface Y { new():Y; } */
        const constructSignatureElement = "construct";
        /** function foo(*Y*: string) */
        const parameterElement = "parameter";
        const typeParameterElement = "type parameter";
        const primitiveType = "primitive type";
        const label = "label";
        const alias = "alias";
        const constElement = "const";
        const letElement = "let";
        const directory = "directory";
        const externalModuleName = "external module name";
    }
    namespace ScriptElementKindModifier {
        const none = "";
        const publicMemberModifier = "public";
        const privateMemberModifier = "private";
        const protectedMemberModifier = "protected";
        const exportedModifier = "export";
        const ambientModifier = "declare";
        const staticModifier = "static";
        const abstractModifier = "abstract";
    }
    class ClassificationTypeNames {
        static comment: string;
        static identifier: string;
        static keyword: string;
        static numericLiteral: string;
        static operator: string;
        static stringLiteral: string;
        static whiteSpace: string;
        static text: string;
        static punctuation: string;
        static className: string;
        static enumName: string;
        static interfaceName: string;
        static moduleName: string;
        static typeParameterName: string;
        static typeAliasName: string;
        static parameterName: string;
        static docCommentTagName: string;
        static jsxOpenTagName: string;
        static jsxCloseTagName: string;
        static jsxSelfClosingTagName: string;
        static jsxAttribute: string;
        static jsxText: string;
        static jsxAttributeStringLiteralValue: string;
    }
    const enum ClassificationType {
        comment = 1,
        identifier = 2,
        keyword = 3,
        numericLiteral = 4,
        operator = 5,
        stringLiteral = 6,
        regularExpressionLiteral = 7,
        whiteSpace = 8,
        text = 9,
        punctuation = 10,
        className = 11,
        enumName = 12,
        interfaceName = 13,
        moduleName = 14,
        typeParameterName = 15,
        typeAliasName = 16,
        parameterName = 17,
        docCommentTagName = 18,
        jsxOpenTagName = 19,
        jsxCloseTagName = 20,
        jsxSelfClosingTagName = 21,
        jsxAttribute = 22,
        jsxText = 23,
        jsxAttributeStringLiteralValue = 24,
    }
}
declare namespace ts {
    function createClassifier(): Classifier;
}
declare namespace ts {
    /**
      * The document registry represents a store of SourceFile objects that can be shared between
      * multiple LanguageService instances. A LanguageService instance holds on the SourceFile (AST)
      * of files in the context.
      * SourceFile objects account for most of the memory usage by the language service. Sharing
      * the same DocumentRegistry instance between different instances of LanguageService allow
      * for more efficient memory utilization since all projects will share at least the library
      * file (lib.d.ts).
      *
      * A more advanced use of the document registry is to serialize sourceFile objects to disk
      * and re-hydrate them when needed.
      *
      * To create a default DocumentRegistry, use createDocumentRegistry to create one, and pass it
      * to all subsequent createLanguageService calls.
      */
    interface DocumentRegistry {
        /**
          * Request a stored SourceFile with a given fileName and compilationSettings.
          * The first call to acquire will call createLanguageServiceSourceFile to generate
          * the SourceFile if was not found in the registry.
          *
          * @param fileName The name of the file requested
          * @param compilationSettings Some compilation settings like target affects the
          * shape of a the resulting SourceFile. This allows the DocumentRegistry to store
          * multiple copies of the same file for different compilation settings.
          * @parm scriptSnapshot Text of the file. Only used if the file was not found
          * in the registry and a new one was created.
          * @parm version Current version of the file. Only used if the file was not found
          * in the registry and a new one was created.
          */
        acquireDocument(fileName: string, compilationSettings: CompilerOptions, scriptSnapshot: IScriptSnapshot, version: string, scriptKind?: ScriptKind): SourceFile;
        acquireDocumentWithKey(fileName: string, path: Path, compilationSettings: CompilerOptions, key: DocumentRegistryBucketKey, scriptSnapshot: IScriptSnapshot, version: string, scriptKind?: ScriptKind): SourceFile;
        /**
          * Request an updated version of an already existing SourceFile with a given fileName
          * and compilationSettings. The update will in-turn call updateLanguageServiceSourceFile
          * to get an updated SourceFile.
          *
          * @param fileName The name of the file requested
          * @param compilationSettings Some compilation settings like target affects the
          * shape of a the resulting SourceFile. This allows the DocumentRegistry to store
          * multiple copies of the same file for different compilation settings.
          * @param scriptSnapshot Text of the file.
          * @param version Current version of the file.
          */
        updateDocument(fileName: string, compilationSettings: CompilerOptions, scriptSnapshot: IScriptSnapshot, version: string, scriptKind?: ScriptKind): SourceFile;
        updateDocumentWithKey(fileName: string, path: Path, compilationSettings: CompilerOptions, key: DocumentRegistryBucketKey, scriptSnapshot: IScriptSnapshot, version: string, scriptKind?: ScriptKind): SourceFile;
        getKeyForCompilationSettings(settings: CompilerOptions): DocumentRegistryBucketKey;
        /**
          * Informs the DocumentRegistry that a file is not needed any longer.
          *
          * Note: It is not allowed to call release on a SourceFile that was not acquired from
          * this registry originally.
          *
          * @param fileName The name of the file to be released
          * @param compilationSettings The compilation settings used to acquire the file
          */
        releaseDocument(fileName: string, compilationSettings: CompilerOptions): void;
        releaseDocumentWithKey(path: Path, key: DocumentRegistryBucketKey): void;
        reportStats(): string;
    }
    type DocumentRegistryBucketKey = string & {
        __bucketKey: any;
    };
    function createDocumentRegistry(useCaseSensitiveFileNames?: boolean, currentDirectory?: string): DocumentRegistry;
}
declare namespace ts {
    function preProcessFile(sourceText: string, readImportFiles?: boolean, detectJavaScriptImports?: boolean): PreProcessedFileInfo;
}
declare namespace ts {
    interface TranspileOptions {
        compilerOptions?: CompilerOptions;
        fileName?: string;
        reportDiagnostics?: boolean;
        moduleName?: string;
        renamedDependencies?: MapLike<string>;
    }
    interface TranspileOutput {
        outputText: string;
        diagnostics?: Diagnostic[];
        sourceMapText?: string;
    }
    function transpileModule(input: string, transpileOptions: TranspileOptions): TranspileOutput;
    function transpile(input: string, compilerOptions?: CompilerOptions, fileName?: string, diagnostics?: Diagnostic[], moduleName?: string): string;
}
declare namespace ts {
    /** The version of the language service API */
    const servicesVersion = "0.5";
    interface DisplayPartsSymbolWriter extends SymbolWriter {
        displayParts(): SymbolDisplayPart[];
    }
    function toEditorSettings(options: EditorOptions | EditorSettings): EditorSettings;
    function displayPartsToString(displayParts: SymbolDisplayPart[]): string;
    function getDefaultCompilerOptions(): CompilerOptions;
    function getSupportedCodeFixes(): string[];
    function createLanguageServiceSourceFile(fileName: string, scriptSnapshot: IScriptSnapshot, scriptTarget: ScriptTarget, version: string, setNodeParents: boolean, scriptKind?: ScriptKind): SourceFile;
    let disableIncrementalParsing: boolean;
    function updateLanguageServiceSourceFile(sourceFile: SourceFile, scriptSnapshot: IScriptSnapshot, version: string, textChangeRange: TextChangeRange, aggressiveChecks?: boolean): SourceFile;
    function createLanguageService(host: LanguageServiceHost, documentRegistry?: DocumentRegistry): LanguageService;
    /**
      * Get the path of the default library files (lib.d.ts) as distributed with the typescript
      * node package.
      * The functionality is not supported if the ts module is consumed outside of a node module.
      */
    function getDefaultLibFilePath(options: CompilerOptions): string;
}
declare namespace ts.server {
    interface CompressedData {
        length: number;
        compressionKind: string;
        data: any;
    }
    type RequireResult = {
        module: {};
        error: undefined;
    } | {
        module: undefined;
        error: {};
    };
    interface ServerHost extends System {
        setTimeout(callback: (...args: any[]) => void, ms: number, ...args: any[]): any;
        clearTimeout(timeoutId: any): void;
        setImmediate(callback: (...args: any[]) => void, ...args: any[]): any;
        clearImmediate(timeoutId: any): void;
        gc?(): void;
        trace?(s: string): void;
        require?(initialPath: string, moduleName: string): RequireResult;
    }
    interface SortedReadonlyArray<T> extends ReadonlyArray<T> {
        " __sortedReadonlyArrayBrand": any;
    }
    interface TypingInstallerRequest {
        readonly projectName: string;
        readonly kind: "discover" | "closeProject";
    }
    interface DiscoverTypings extends TypingInstallerRequest {
        readonly fileNames: string[];
        readonly projectRootPath: ts.Path;
        readonly compilerOptions: ts.CompilerOptions;
        readonly typeAcquisition: ts.TypeAcquisition;
        readonly unresolvedImports: SortedReadonlyArray<string>;
        readonly cachePath?: string;
        readonly kind: "discover";
    }
    interface CloseProject extends TypingInstallerRequest {
        readonly kind: "closeProject";
    }
    type ActionSet = "action::set";
    type ActionInvalidate = "action::invalidate";
    type EventBeginInstallTypes = "event::beginInstallTypes";
    type EventEndInstallTypes = "event::endInstallTypes";
    interface TypingInstallerResponse {
        readonly kind: ActionSet | ActionInvalidate | EventBeginInstallTypes | EventEndInstallTypes;
    }
    interface ProjectResponse extends TypingInstallerResponse {
        readonly projectName: string;
    }
    interface SetTypings extends ProjectResponse {
        readonly typeAcquisition: ts.TypeAcquisition;
        readonly compilerOptions: ts.CompilerOptions;
        readonly typings: string[];
        readonly unresolvedImports: SortedReadonlyArray<string>;
        readonly kind: ActionSet;
    }
    interface InvalidateCachedTypings extends ProjectResponse {
        readonly kind: ActionInvalidate;
    }
    interface InstallTypes extends ProjectResponse {
        readonly kind: EventBeginInstallTypes | EventEndInstallTypes;
        readonly eventId: number;
        readonly typingsInstallerVersion: string;
        readonly packagesToInstall: ReadonlyArray<string>;
    }
    interface BeginInstallTypes extends InstallTypes {
        readonly kind: EventBeginInstallTypes;
    }
    interface EndInstallTypes extends InstallTypes {
        readonly kind: EventEndInstallTypes;
        readonly installSuccess: boolean;
    }
}
declare namespace ts.server {
    const ActionSet: ActionSet;
    const ActionInvalidate: ActionInvalidate;
    const EventBeginInstallTypes: EventBeginInstallTypes;
    const EventEndInstallTypes: EventEndInstallTypes;
    namespace Arguments {
        const GlobalCacheLocation = "--globalTypingsCacheLocation";
        const LogFile = "--logFile";
        const EnableTelemetry = "--enableTelemetry";
    }
    function hasArgument(argumentName: string): boolean;
    function findArgument(argumentName: string): string;
}
declare namespace ts.server {
    enum LogLevel {
        terse = 0,
        normal = 1,
        requestTime = 2,
        verbose = 3,
    }
    const emptyArray: ReadonlyArray<any>;
    interface Logger {
        close(): void;
        hasLevel(level: LogLevel): boolean;
        loggingEnabled(): boolean;
        perftrc(s: string): void;
        info(s: string): void;
        startGroup(): void;
        endGroup(): void;
        msg(s: string, type?: Msg.Types): void;
        getLogFileName(): string;
    }
    namespace Msg {
        type Err = "Err";
        const Err: Err;
        type Info = "Info";
        const Info: Info;
        type Perf = "Perf";
        const Perf: Perf;
        type Types = Err | Info | Perf;
    }
    function createInstallTypingsRequest(project: Project, typeAcquisition: TypeAcquisition, unresolvedImports: SortedReadonlyArray<string>, cachePath?: string): DiscoverTypings;
    namespace Errors {
        function ThrowNoProject(): never;
        function ThrowProjectLanguageServiceDisabled(): never;
        function ThrowProjectDoesNotContainDocument(fileName: string, project: Project): never;
    }
    function getDefaultFormatCodeSettings(host: ServerHost): FormatCodeSettings;
    function mergeMapLikes(target: MapLike<any>, source: MapLike<any>): void;
    function removeItemFromSet<T>(items: T[], itemToRemove: T): void;
    type NormalizedPath = string & {
        __normalizedPathTag: any;
    };
    function toNormalizedPath(fileName: string): NormalizedPath;
    function normalizedPathToPath(normalizedPath: NormalizedPath, currentDirectory: string, getCanonicalFileName: (f: string) => string): Path;
    function asNormalizedPath(fileName: string): NormalizedPath;
    interface NormalizedPathMap<T> {
        get(path: NormalizedPath): T;
        set(path: NormalizedPath, value: T): void;
        contains(path: NormalizedPath): boolean;
        remove(path: NormalizedPath): void;
    }
    function createNormalizedPathMap<T>(): NormalizedPathMap<T>;
    interface ProjectOptions {
        /**
         * true if config file explicitly listed files
         **/
        configHasFilesProperty?: boolean;
        /**
         * these fields can be present in the project file
         **/
        files?: string[];
        wildcardDirectories?: Map<WatchDirectoryFlags>;
        compilerOptions?: CompilerOptions;
        typeAcquisition?: TypeAcquisition;
        compileOnSave?: boolean;
    }
    function isInferredProjectName(name: string): boolean;
    function makeInferredProjectName(counter: number): string;
    function toSortedReadonlyArray(arr: string[]): SortedReadonlyArray<string>;
    class ThrottledOperations {
        private readonly host;
        private pendingTimeouts;
        constructor(host: ServerHost);
        schedule(operationId: string, delay: number, cb: () => void): void;
        private static run(self, operationId, cb);
    }
    class GcTimer {
        private readonly host;
        private readonly delay;
        private readonly logger;
        private timerId;
        constructor(host: ServerHost, delay: number, logger: Logger);
        scheduleCollect(): void;
        private static run(self);
    }
}
/**
  * Declaration module describing the TypeScript Server protocol
  */
declare namespace ts.server.protocol {
    namespace CommandTypes {
        type Brace = "brace";
        type BraceCompletion = "braceCompletion";
        type Change = "change";
        type Close = "close";
        type Completions = "completions";
        type CompletionDetails = "completionEntryDetails";
        type CompileOnSaveAffectedFileList = "compileOnSaveAffectedFileList";
        type CompileOnSaveEmitFile = "compileOnSaveEmitFile";
        type Configure = "configure";
        type Definition = "definition";
        type Implementation = "implementation";
        type Exit = "exit";
        type Format = "format";
        type Formatonkey = "formatonkey";
        type Geterr = "geterr";
        type GeterrForProject = "geterrForProject";
        type SemanticDiagnosticsSync = "semanticDiagnosticsSync";
        type SyntacticDiagnosticsSync = "syntacticDiagnosticsSync";
        type NavBar = "navbar";
        type Navto = "navto";
        type NavTree = "navtree";
        type NavTreeFull = "navtree-full";
        type Occurrences = "occurrences";
        type DocumentHighlights = "documentHighlights";
        type Open = "open";
        type Quickinfo = "quickinfo";
        type References = "references";
        type Reload = "reload";
        type Rename = "rename";
        type Saveto = "saveto";
        type SignatureHelp = "signatureHelp";
        type TypeDefinition = "typeDefinition";
        type ProjectInfo = "projectInfo";
        type ReloadProjects = "reloadProjects";
        type Unknown = "unknown";
        type OpenExternalProject = "openExternalProject";
        type OpenExternalProjects = "openExternalProjects";
        type CloseExternalProject = "closeExternalProject";
        type TodoComments = "todoComments";
        type Indentation = "indentation";
        type DocCommentTemplate = "docCommentTemplate";
        type CompilerOptionsForInferredProjects = "compilerOptionsForInferredProjects";
        type GetCodeFixes = "getCodeFixes";
        type GetSupportedCodeFixes = "getSupportedCodeFixes";
    }
    /**
      * A TypeScript Server message
      */
    interface Message {
        /**
          * Sequence number of the message
          */
        seq: number;
        /**
          * One of "request", "response", or "event"
          */
        type: "request" | "response" | "event";
    }
    /**
      * Client-initiated request message
      */
    interface Request extends Message {
        /**
          * The command to execute
          */
        command: string;
        /**
          * Object containing arguments for the command
          */
        arguments?: any;
    }
    /**
      * Request to reload the project structure for all the opened files
      */
    interface ReloadProjectsRequest extends Message {
        command: CommandTypes.ReloadProjects;
    }
    /**
      * Server-initiated event message
      */
    interface Event extends Message {
        /**
          * Name of event
          */
        event: string;
        /**
          * Event-specific information
          */
        body?: any;
    }
    /**
      * Response by server to client request message.
      */
    interface Response extends Message {
        /**
          * Sequence number of the request message.
          */
        request_seq: number;
        /**
          * Outcome of the request.
          */
        success: boolean;
        /**
          * The command requested.
          */
        command: string;
        /**
          * Contains error message if success === false.
          */
        message?: string;
        /**
          * Contains message body if success === true.
          */
        body?: any;
    }
    /**
      * Arguments for FileRequest messages.
      */
    interface FileRequestArgs {
        /**
          * The file for the request (absolute pathname required).
          */
        file: string;
        projectFileName?: string;
    }
    /**
     * Requests a JS Doc comment template for a given position
     */
    interface DocCommentTemplateRequest extends FileLocationRequest {
        command: CommandTypes.DocCommentTemplate;
    }
    /**
     * Response to DocCommentTemplateRequest
     */
    interface DocCommandTemplateResponse extends Response {
        body?: TextInsertion;
    }
    /**
     * A request to get TODO comments from the file
     */
    interface TodoCommentRequest extends FileRequest {
        command: CommandTypes.TodoComments;
        arguments: TodoCommentRequestArgs;
    }
    /**
     * Arguments for TodoCommentRequest request.
     */
    interface TodoCommentRequestArgs extends FileRequestArgs {
        /**
         * Array of target TodoCommentDescriptors that describes TODO comments to be found
         */
        descriptors: TodoCommentDescriptor[];
    }
    /**
     * Response for TodoCommentRequest request.
     */
    interface TodoCommentsResponse extends Response {
        body?: TodoComment[];
    }
    /**
     * A request to get indentation for a location in file
     */
    interface IndentationRequest extends FileLocationRequest {
        command: CommandTypes.Indentation;
        arguments: IndentationRequestArgs;
    }
    /**
     * Response for IndentationRequest request.
     */
    interface IndentationResponse extends Response {
        body?: IndentationResult;
    }
    /**
     * Indentation result representing where indentation should be placed
     */
    interface IndentationResult {
        /**
         * The base position in the document that the indent should be relative to
         */
        position: number;
        /**
         * The number of columns the indent should be at relative to the position's column.
         */
        indentation: number;
    }
    /**
     * Arguments for IndentationRequest request.
     */
    interface IndentationRequestArgs extends FileLocationRequestArgs {
        /**
         * An optional set of settings to be used when computing indentation.
         * If argument is omitted - then it will use settings for file that were previously set via 'configure' request or global settings.
         */
        options?: EditorSettings;
    }
    /**
      * Arguments for ProjectInfoRequest request.
      */
    interface ProjectInfoRequestArgs extends FileRequestArgs {
        /**
          * Indicate if the file name list of the project is needed
          */
        needFileNameList: boolean;
    }
    /**
      * A request to get the project information of the current file.
      */
    interface ProjectInfoRequest extends Request {
        command: CommandTypes.ProjectInfo;
        arguments: ProjectInfoRequestArgs;
    }
    /**
     * A request to retrieve compiler options diagnostics for a project
     */
    interface CompilerOptionsDiagnosticsRequest extends Request {
        arguments: CompilerOptionsDiagnosticsRequestArgs;
    }
    /**
     * Arguments for CompilerOptionsDiagnosticsRequest request.
     */
    interface CompilerOptionsDiagnosticsRequestArgs {
        /**
         * Name of the project to retrieve compiler options diagnostics.
         */
        projectFileName: string;
    }
    /**
      * Response message body for "projectInfo" request
      */
    interface ProjectInfo {
        /**
          * For configured project, this is the normalized path of the 'tsconfig.json' file
          * For inferred project, this is undefined
          */
        configFileName: string;
        /**
          * The list of normalized file name in the project, including 'lib.d.ts'
          */
        fileNames?: string[];
        /**
          * Indicates if the project has a active language service instance
          */
        languageServiceDisabled?: boolean;
    }
    /**
     * Represents diagnostic info that includes location of diagnostic in two forms
     * - start position and length of the error span
     * - startLocation and endLocation - a pair of Location objects that store start/end line and offset of the error span.
     */
    interface DiagnosticWithLinePosition {
        message: string;
        start: number;
        length: number;
        startLocation: Location;
        endLocation: Location;
        category: string;
        code: number;
    }
    /**
      * Response message for "projectInfo" request
      */
    interface ProjectInfoResponse extends Response {
        body?: ProjectInfo;
    }
    /**
      * Request whose sole parameter is a file name.
      */
    interface FileRequest extends Request {
        arguments: FileRequestArgs;
    }
    /**
      * Instances of this interface specify a location in a source file:
      * (file, line, character offset), where line and character offset are 1-based.
      */
    interface FileLocationRequestArgs extends FileRequestArgs {
        /**
          * The line number for the request (1-based).
          */
        line: number;
        /**
          * The character offset (on the line) for the request (1-based).
          */
        offset: number;
    }
    /**
      * Request for the available codefixes at a specific position.
      */
    interface CodeFixRequest extends Request {
        command: CommandTypes.GetCodeFixes;
        arguments: CodeFixRequestArgs;
    }
    /**
      * Instances of this interface specify errorcodes on a specific location in a sourcefile.
      */
    interface CodeFixRequestArgs extends FileRequestArgs {
        /**
          * The line number for the request (1-based).
          */
        startLine: number;
        /**
          * The character offset (on the line) for the request (1-based).
          */
        startOffset: number;
        /**
          * The line number for the request (1-based).
          */
        endLine: number;
        /**
          * The character offset (on the line) for the request (1-based).
          */
        endOffset: number;
        /**
          * Errorcodes we want to get the fixes for.
          */
        errorCodes?: number[];
    }
    /**
     * Response for GetCodeFixes request.
     */
    interface GetCodeFixesResponse extends Response {
        body?: CodeAction[];
    }
    /**
      * A request whose arguments specify a file location (file, line, col).
      */
    interface FileLocationRequest extends FileRequest {
        arguments: FileLocationRequestArgs;
    }
    /**
     * A request to get codes of supported code fixes.
     */
    interface GetSupportedCodeFixesRequest extends Request {
        command: CommandTypes.GetSupportedCodeFixes;
    }
    /**
     * A response for GetSupportedCodeFixesRequest request.
     */
    interface GetSupportedCodeFixesResponse extends Response {
        /**
         * List of error codes supported by the server.
         */
        body?: string[];
    }
    /**
     * Arguments for EncodedSemanticClassificationsRequest request.
     */
    interface EncodedSemanticClassificationsRequestArgs extends FileRequestArgs {
        /**
         * Start position of the span.
         */
        start: number;
        /**
         * Length of the span.
         */
        length: number;
    }
    /**
      * Arguments in document highlight request; include: filesToSearch, file,
      * line, offset.
      */
    interface DocumentHighlightsRequestArgs extends FileLocationRequestArgs {
        /**
         * List of files to search for document highlights.
         */
        filesToSearch: string[];
    }
    /**
      * Go to definition request; value of command field is
      * "definition". Return response giving the file locations that
      * define the symbol found in file at location line, col.
      */
    interface DefinitionRequest extends FileLocationRequest {
        command: CommandTypes.Definition;
    }
    /**
      * Go to type request; value of command field is
      * "typeDefinition". Return response giving the file locations that
      * define the type for the symbol found in file at location line, col.
      */
    interface TypeDefinitionRequest extends FileLocationRequest {
        command: CommandTypes.TypeDefinition;
    }
    /**
      * Go to implementation request; value of command field is
      * "implementation". Return response giving the file locations that
      * implement the symbol found in file at location line, col.
      */
    interface ImplementationRequest extends FileLocationRequest {
        command: CommandTypes.Implementation;
    }
    /**
      * Location in source code expressed as (one-based) line and character offset.
      */
    interface Location {
        line: number;
        offset: number;
    }
    /**
      * Object found in response messages defining a span of text in source code.
      */
    interface TextSpan {
        /**
          * First character of the definition.
          */
        start: Location;
        /**
          * One character past last character of the definition.
          */
        end: Location;
    }
    /**
      * Object found in response messages defining a span of text in a specific source file.
      */
    interface FileSpan extends TextSpan {
        /**
          * File containing text span.
          */
        file: string;
    }
    /**
      * Definition response message.  Gives text range for definition.
      */
    interface DefinitionResponse extends Response {
        body?: FileSpan[];
    }
    /**
      * Definition response message.  Gives text range for definition.
      */
    interface TypeDefinitionResponse extends Response {
        body?: FileSpan[];
    }
    /**
      * Implementation response message.  Gives text range for implementations.
      */
    interface ImplementationResponse extends Response {
        body?: FileSpan[];
    }
    /**
     * Request to get brace completion for a location in the file.
     */
    interface BraceCompletionRequest extends FileLocationRequest {
        command: CommandTypes.BraceCompletion;
        arguments: BraceCompletionRequestArgs;
    }
    /**
     * Argument for BraceCompletionRequest request.
     */
    interface BraceCompletionRequestArgs extends FileLocationRequestArgs {
        /**
         * Kind of opening brace
         */
        openingBrace: string;
    }
    /**
      * Get occurrences request; value of command field is
      * "occurrences". Return response giving spans that are relevant
      * in the file at a given line and column.
      */
    interface OccurrencesRequest extends FileLocationRequest {
        command: CommandTypes.Occurrences;
    }
    interface OccurrencesResponseItem extends FileSpan {
        /**
          * True if the occurrence is a write location, false otherwise.
          */
        isWriteAccess: boolean;
    }
    interface OccurrencesResponse extends Response {
        body?: OccurrencesResponseItem[];
    }
    /**
      * Get document highlights request; value of command field is
      * "documentHighlights". Return response giving spans that are relevant
      * in the file at a given line and column.
      */
    interface DocumentHighlightsRequest extends FileLocationRequest {
        command: CommandTypes.DocumentHighlights;
        arguments: DocumentHighlightsRequestArgs;
    }
    /**
     * Span augmented with extra information that denotes the kind of the highlighting to be used for span.
     * Kind is taken from HighlightSpanKind type.
     */
    interface HighlightSpan extends TextSpan {
        kind: string;
    }
    /**
     * Represents a set of highligh spans for a give name
     */
    interface DocumentHighlightsItem {
        /**
          * File containing highlight spans.
          */
        file: string;
        /**
          * Spans to highlight in file.
          */
        highlightSpans: HighlightSpan[];
    }
    /**
     * Response for a DocumentHighlightsRequest request.
     */
    interface DocumentHighlightsResponse extends Response {
        body?: DocumentHighlightsItem[];
    }
    /**
      * Find references request; value of command field is
      * "references". Return response giving the file locations that
      * reference the symbol found in file at location line, col.
      */
    interface ReferencesRequest extends FileLocationRequest {
        command: CommandTypes.References;
    }
    interface ReferencesResponseItem extends FileSpan {
        /** Text of line containing the reference.  Including this
          *  with the response avoids latency of editor loading files
          * to show text of reference line (the server already has
          * loaded the referencing files).
          */
        lineText: string;
        /**
          * True if reference is a write location, false otherwise.
          */
        isWriteAccess: boolean;
        /**
         * True if reference is a definition, false otherwise.
         */
        isDefinition: boolean;
    }
    /**
      * The body of a "references" response message.
      */
    interface ReferencesResponseBody {
        /**
          * The file locations referencing the symbol.
          */
        refs: ReferencesResponseItem[];
        /**
          * The name of the symbol.
          */
        symbolName: string;
        /**
          * The start character offset of the symbol (on the line provided by the references request).
          */
        symbolStartOffset: number;
        /**
          * The full display name of the symbol.
          */
        symbolDisplayString: string;
    }
    /**
      * Response to "references" request.
      */
    interface ReferencesResponse extends Response {
        body?: ReferencesResponseBody;
    }
    /**
     * Argument for RenameRequest request.
     */
    interface RenameRequestArgs extends FileLocationRequestArgs {
        /**
         * Should text at specified location be found/changed in comments?
         */
        findInComments?: boolean;
        /**
         * Should text at specified location be found/changed in strings?
         */
        findInStrings?: boolean;
    }
    /**
      * Rename request; value of command field is "rename". Return
      * response giving the file locations that reference the symbol
      * found in file at location line, col. Also return full display
      * name of the symbol so that client can print it unambiguously.
      */
    interface RenameRequest extends FileLocationRequest {
        command: CommandTypes.Rename;
        arguments: RenameRequestArgs;
    }
    /**
      * Information about the item to be renamed.
      */
    interface RenameInfo {
        /**
          * True if item can be renamed.
          */
        canRename: boolean;
        /**
          * Error message if item can not be renamed.
          */
        localizedErrorMessage?: string;
        /**
          * Display name of the item to be renamed.
          */
        displayName: string;
        /**
          * Full display name of item to be renamed.
          */
        fullDisplayName: string;
        /**
          * The items's kind (such as 'className' or 'parameterName' or plain 'text').
          */
        kind: string;
        /**
          * Optional modifiers for the kind (such as 'public').
          */
        kindModifiers: string;
    }
    /**
     *  A group of text spans, all in 'file'.
     */
    interface SpanGroup {
        /** The file to which the spans apply */
        file: string;
        /** The text spans in this group */
        locs: TextSpan[];
    }
    interface RenameResponseBody {
        /**
         * Information about the item to be renamed.
         */
        info: RenameInfo;
        /**
         * An array of span groups (one per file) that refer to the item to be renamed.
         */
        locs: SpanGroup[];
    }
    /**
      * Rename response message.
      */
    interface RenameResponse extends Response {
        body?: RenameResponseBody;
    }
    /**
     * Represents a file in external project.
     * External project is project whose set of files, compilation options and open\close state
     * is maintained by the client (i.e. if all this data come from .csproj file in Visual Studio).
     * External project will exist even if all files in it are closed and should be closed explicity.
     * If external project includes one or more tsconfig.json/jsconfig.json files then tsserver will
     * create configured project for every config file but will maintain a link that these projects were created
     * as a result of opening external project so they should be removed once external project is closed.
     */
    interface ExternalFile {
        /**
         * Name of file file
         */
        fileName: string;
        /**
         * Script kind of the file
         */
        scriptKind?: ScriptKindName | ts.ScriptKind;
        /**
         * Whether file has mixed content (i.e. .cshtml file that combines html markup with C#/JavaScript)
         */
        hasMixedContent?: boolean;
        /**
         * Content of the file
         */
        content?: string;
    }
    /**
     * Represent an external project
     */
    interface ExternalProject {
        /**
         * Project name
         */
        projectFileName: string;
        /**
         * List of root files in project
         */
        rootFiles: ExternalFile[];
        /**
         * Compiler options for the project
         */
        options: ExternalProjectCompilerOptions;
        /**
         * @deprecated typingOptions. Use typeAcquisition instead
         */
        typingOptions?: TypeAcquisition;
        /**
         * Explicitly specified type acquisition for the project
         */
        typeAcquisition?: TypeAcquisition;
    }
    interface CompileOnSaveMixin {
        /**
         * If compile on save is enabled for the project
         */
        compileOnSave?: boolean;
    }
    /**
     * For external projects, some of the project settings are sent together with
     * compiler settings.
     */
    type ExternalProjectCompilerOptions = CompilerOptions & CompileOnSaveMixin;
    /**
     * Represents a set of changes that happen in project
     */
    interface ProjectChanges {
        /**
         * List of added files
         */
        added: string[];
        /**
         * List of removed files
         */
        removed: string[];
        /**
         * List of updated files
         */
        updated: string[];
    }
    /**
      * Information found in a configure request.
      */
    interface ConfigureRequestArguments {
        /**
          * Information about the host, for example 'Emacs 24.4' or
          * 'Sublime Text version 3075'
          */
        hostInfo?: string;
        /**
          * If present, tab settings apply only to this file.
          */
        file?: string;
        /**
         * The format options to use during formatting and other code editing features.
         */
        formatOptions?: FormatCodeSettings;
        /**
         * The host's additional supported .js file extensions
         */
        extraFileExtensions?: JsFileExtensionInfo[];
    }
    /**
      *  Configure request; value of command field is "configure".  Specifies
      *  host information, such as host type, tab size, and indent size.
      */
    interface ConfigureRequest extends Request {
        command: CommandTypes.Configure;
        arguments: ConfigureRequestArguments;
    }
    /**
      * Response to "configure" request.  This is just an acknowledgement, so
      * no body field is required.
      */
    interface ConfigureResponse extends Response {
    }
    /**
      *  Information found in an "open" request.
      */
    interface OpenRequestArgs extends FileRequestArgs {
        /**
         * Used when a version of the file content is known to be more up to date than the one on disk.
         * Then the known content will be used upon opening instead of the disk copy
         */
        fileContent?: string;
        /**
         * Used to specify the script kind of the file explicitly. It could be one of the following:
         *      "TS", "JS", "TSX", "JSX"
         */
        scriptKindName?: ScriptKindName;
    }
    type ScriptKindName = "TS" | "JS" | "TSX" | "JSX";
    /**
      * Open request; value of command field is "open". Notify the
      * server that the client has file open.  The server will not
      * monitor the filesystem for changes in this file and will assume
      * that the client is updating the server (using the change and/or
      * reload messages) when the file changes. Server does not currently
      * send a response to an open request.
      */
    interface OpenRequest extends Request {
        command: CommandTypes.Open;
        arguments: OpenRequestArgs;
    }
    /**
     * Request to open or update external project
     */
    interface OpenExternalProjectRequest extends Request {
        command: CommandTypes.OpenExternalProject;
        arguments: OpenExternalProjectArgs;
    }
    /**
     * Arguments to OpenExternalProjectRequest request
     */
    type OpenExternalProjectArgs = ExternalProject;
    /**
     * Request to open multiple external projects
     */
    interface OpenExternalProjectsRequest extends Request {
        command: CommandTypes.OpenExternalProjects;
        arguments: OpenExternalProjectsArgs;
    }
    /**
     * Arguments to OpenExternalProjectsRequest
     */
    interface OpenExternalProjectsArgs {
        /**
         * List of external projects to open or update
         */
        projects: ExternalProject[];
    }
    /**
     * Response to OpenExternalProjectRequest request. This is just an acknowledgement, so
     * no body field is required.
     */
    interface OpenExternalProjectResponse extends Response {
    }
    /**
     * Response to OpenExternalProjectsRequest request. This is just an acknowledgement, so
     * no body field is required.
     */
    interface OpenExternalProjectsResponse extends Response {
    }
    /**
     * Request to close external project.
     */
    interface CloseExternalProjectRequest extends Request {
        command: CommandTypes.CloseExternalProject;
        arguments: CloseExternalProjectRequestArgs;
    }
    /**
     * Arguments to CloseExternalProjectRequest request
     */
    interface CloseExternalProjectRequestArgs {
        /**
         * Name of the project to close
         */
        projectFileName: string;
    }
    /**
     * Response to CloseExternalProjectRequest request. This is just an acknowledgement, so
     * no body field is required.
     */
    interface CloseExternalProjectResponse extends Response {
    }
    /**
     * Request to set compiler options for inferred projects.
     * External projects are opened / closed explicitly.
     * Configured projects are opened when user opens loose file that has 'tsconfig.json' or 'jsconfig.json' anywhere in one of containing folders.
     * This configuration file will be used to obtain a list of files and configuration settings for the project.
     * Inferred projects are created when user opens a loose file that is not the part of external project
     * or configured project and will contain only open file and transitive closure of referenced files if 'useOneInferredProject' is false,
     * or all open loose files and its transitive closure of referenced files if 'useOneInferredProject' is true.
     */
    interface SetCompilerOptionsForInferredProjectsRequest extends Request {
        command: CommandTypes.CompilerOptionsForInferredProjects;
        arguments: SetCompilerOptionsForInferredProjectsArgs;
    }
    /**
     * Argument for SetCompilerOptionsForInferredProjectsRequest request.
     */
    interface SetCompilerOptionsForInferredProjectsArgs {
        /**
         * Compiler options to be used with inferred projects.
         */
        options: ExternalProjectCompilerOptions;
    }
    /**
      * Response to SetCompilerOptionsForInferredProjectsResponse request. This is just an acknowledgement, so
      * no body field is required.
      */
    interface SetCompilerOptionsForInferredProjectsResponse extends Response {
    }
    /**
      *  Exit request; value of command field is "exit".  Ask the server process
      *  to exit.
      */
    interface ExitRequest extends Request {
        command: CommandTypes.Exit;
    }
    /**
      * Close request; value of command field is "close". Notify the
      * server that the client has closed a previously open file.  If
      * file is still referenced by open files, the server will resume
      * monitoring the filesystem for changes to file.  Server does not
      * currently send a response to a close request.
      */
    interface CloseRequest extends FileRequest {
        command: CommandTypes.Close;
    }
    /**
     * Request to obtain the list of files that should be regenerated if target file is recompiled.
     * NOTE: this us query-only operation and does not generate any output on disk.
     */
    interface CompileOnSaveAffectedFileListRequest extends FileRequest {
        command: CommandTypes.CompileOnSaveAffectedFileList;
    }
    /**
     * Contains a list of files that should be regenerated in a project
     */
    interface CompileOnSaveAffectedFileListSingleProject {
        /**
         * Project name
         */
        projectFileName: string;
        /**
         * List of files names that should be recompiled
         */
        fileNames: string[];
        /**
         * true if project uses outFile or out compiler option
         */
        projectUsesOutFile: boolean;
    }
    /**
     * Response for CompileOnSaveAffectedFileListRequest request;
     */
    interface CompileOnSaveAffectedFileListResponse extends Response {
        body: CompileOnSaveAffectedFileListSingleProject[];
    }
    /**
     * Request to recompile the file. All generated outputs (.js, .d.ts or .js.map files) is written on disk.
     */
    interface CompileOnSaveEmitFileRequest extends FileRequest {
        command: CommandTypes.CompileOnSaveEmitFile;
        arguments: CompileOnSaveEmitFileRequestArgs;
    }
    /**
     * Arguments for CompileOnSaveEmitFileRequest
     */
    interface CompileOnSaveEmitFileRequestArgs extends FileRequestArgs {
        /**
         * if true - then file should be recompiled even if it does not have any changes.
         */
        forced?: boolean;
    }
    /**
      * Quickinfo request; value of command field is
      * "quickinfo". Return response giving a quick type and
      * documentation string for the symbol found in file at location
      * line, col.
      */
    interface QuickInfoRequest extends FileLocationRequest {
        command: CommandTypes.Quickinfo;
    }
    /**
      * Body of QuickInfoResponse.
      */
    interface QuickInfoResponseBody {
        /**
          * The symbol's kind (such as 'className' or 'parameterName' or plain 'text').
          */
        kind: string;
        /**
          * Optional modifiers for the kind (such as 'public').
          */
        kindModifiers: string;
        /**
          * Starting file location of symbol.
          */
        start: Location;
        /**
          * One past last character of symbol.
          */
        end: Location;
        /**
          * Type and kind of symbol.
          */
        displayString: string;
        /**
          * Documentation associated with symbol.
          */
        documentation: string;
    }
    /**
      * Quickinfo response message.
      */
    interface QuickInfoResponse extends Response {
        body?: QuickInfoResponseBody;
    }
    /**
      * Arguments for format messages.
      */
    interface FormatRequestArgs extends FileLocationRequestArgs {
        /**
          * Last line of range for which to format text in file.
          */
        endLine: number;
        /**
          * Character offset on last line of range for which to format text in file.
          */
        endOffset: number;
        /**
         * Format options to be used.
         */
        options?: FormatCodeSettings;
    }
    /**
      * Format request; value of command field is "format".  Return
      * response giving zero or more edit instructions.  The edit
      * instructions will be sorted in file order.  Applying the edit
      * instructions in reverse to file will result in correctly
      * reformatted text.
      */
    interface FormatRequest extends FileLocationRequest {
        command: CommandTypes.Format;
        arguments: FormatRequestArgs;
    }
    /**
      * Object found in response messages defining an editing
      * instruction for a span of text in source code.  The effect of
      * this instruction is to replace the text starting at start and
      * ending one character before end with newText. For an insertion,
      * the text span is empty.  For a deletion, newText is empty.
      */
    interface CodeEdit {
        /**
          * First character of the text span to edit.
          */
        start: Location;
        /**
          * One character past last character of the text span to edit.
          */
        end: Location;
        /**
          * Replace the span defined above with this string (may be
          * the empty string).
          */
        newText: string;
    }
    interface FileCodeEdits {
        fileName: string;
        textChanges: CodeEdit[];
    }
    interface CodeFixResponse extends Response {
        /** The code actions that are available */
        body?: CodeAction[];
    }
    interface CodeAction {
        /** Description of the code action to display in the UI of the editor */
        description: string;
        /** Text changes to apply to each file as part of the code action */
        changes: FileCodeEdits[];
    }
    /**
      * Format and format on key response message.
      */
    interface FormatResponse extends Response {
        body?: CodeEdit[];
    }
    /**
      * Arguments for format on key messages.
      */
    interface FormatOnKeyRequestArgs extends FileLocationRequestArgs {
        /**
          * Key pressed (';', '\n', or '}').
          */
        key: string;
        options?: FormatCodeSettings;
    }
    /**
      * Format on key request; value of command field is
      * "formatonkey". Given file location and key typed (as string),
      * return response giving zero or more edit instructions.  The
      * edit instructions will be sorted in file order.  Applying the
      * edit instructions in reverse to file will result in correctly
      * reformatted text.
      */
    interface FormatOnKeyRequest extends FileLocationRequest {
        command: CommandTypes.Formatonkey;
        arguments: FormatOnKeyRequestArgs;
    }
    /**
      * Arguments for completions messages.
      */
    interface CompletionsRequestArgs extends FileLocationRequestArgs {
        /**
          * Optional prefix to apply to possible completions.
          */
        prefix?: string;
    }
    /**
      * Completions request; value of command field is "completions".
      * Given a file location (file, line, col) and a prefix (which may
      * be the empty string), return the possible completions that
      * begin with prefix.
      */
    interface CompletionsRequest extends FileLocationRequest {
        command: CommandTypes.Completions;
        arguments: CompletionsRequestArgs;
    }
    /**
      * Arguments for completion details request.
      */
    interface CompletionDetailsRequestArgs extends FileLocationRequestArgs {
        /**
          * Names of one or more entries for which to obtain details.
          */
        entryNames: string[];
    }
    /**
      * Completion entry details request; value of command field is
      * "completionEntryDetails".  Given a file location (file, line,
      * col) and an array of completion entry names return more
      * detailed information for each completion entry.
      */
    interface CompletionDetailsRequest extends FileLocationRequest {
        command: CommandTypes.CompletionDetails;
        arguments: CompletionDetailsRequestArgs;
    }
    /**
      * Part of a symbol description.
      */
    interface SymbolDisplayPart {
        /**
          * Text of an item describing the symbol.
          */
        text: string;
        /**
          * The symbol's kind (such as 'className' or 'parameterName' or plain 'text').
          */
        kind: string;
    }
    /**
      * An item found in a completion response.
      */
    interface CompletionEntry {
        /**
          * The symbol's name.
          */
        name: string;
        /**
          * The symbol's kind (such as 'className' or 'parameterName').
          */
        kind: string;
        /**
          * Optional modifiers for the kind (such as 'public').
          */
        kindModifiers: string;
        /**
         * A string that is used for comparing completion items so that they can be ordered.  This
         * is often the same as the name but may be different in certain circumstances.
         */
        sortText: string;
        /**
         * An optional span that indicates the text to be replaced by this completion item. If present,
         * this span should be used instead of the default one.
         */
        replacementSpan?: TextSpan;
    }
    /**
      * Additional completion entry details, available on demand
      */
    interface CompletionEntryDetails {
        /**
          * The symbol's name.
          */
        name: string;
        /**
          * The symbol's kind (such as 'className' or 'parameterName').
          */
        kind: string;
        /**
          * Optional modifiers for the kind (such as 'public').
          */
        kindModifiers: string;
        /**
          * Display parts of the symbol (similar to quick info).
          */
        displayParts: SymbolDisplayPart[];
        /**
          * Documentation strings for the symbol.
          */
        documentation: SymbolDisplayPart[];
    }
    interface CompletionsResponse extends Response {
        body?: CompletionEntry[];
    }
    interface CompletionDetailsResponse extends Response {
        body?: CompletionEntryDetails[];
    }
    /**
     * Signature help information for a single parameter
     */
    interface SignatureHelpParameter {
        /**
         * The parameter's name
         */
        name: string;
        /**
          * Documentation of the parameter.
          */
        documentation: SymbolDisplayPart[];
        /**
          * Display parts of the parameter.
          */
        displayParts: SymbolDisplayPart[];
        /**
         * Whether the parameter is optional or not.
         */
        isOptional: boolean;
    }
    /**
     * Represents a single signature to show in signature help.
     */
    interface SignatureHelpItem {
        /**
         * Whether the signature accepts a variable number of arguments.
         */
        isVariadic: boolean;
        /**
         * The prefix display parts.
         */
        prefixDisplayParts: SymbolDisplayPart[];
        /**
         * The suffix display parts.
         */
        suffixDisplayParts: SymbolDisplayPart[];
        /**
         * The separator display parts.
         */
        separatorDisplayParts: SymbolDisplayPart[];
        /**
         * The signature helps items for the parameters.
         */
        parameters: SignatureHelpParameter[];
        /**
         * The signature's documentation
         */
        documentation: SymbolDisplayPart[];
    }
    /**
     * Signature help items found in the response of a signature help request.
     */
    interface SignatureHelpItems {
        /**
         * The signature help items.
         */
        items: SignatureHelpItem[];
        /**
         * The span for which signature help should appear on a signature
         */
        applicableSpan: TextSpan;
        /**
         * The item selected in the set of available help items.
         */
        selectedItemIndex: number;
        /**
         * The argument selected in the set of parameters.
         */
        argumentIndex: number;
        /**
         * The argument count
         */
        argumentCount: number;
    }
    /**
     * Arguments of a signature help request.
     */
    interface SignatureHelpRequestArgs extends FileLocationRequestArgs {
    }
    /**
      * Signature help request; value of command field is "signatureHelp".
      * Given a file location (file, line, col), return the signature
      * help.
      */
    interface SignatureHelpRequest extends FileLocationRequest {
        command: CommandTypes.SignatureHelp;
        arguments: SignatureHelpRequestArgs;
    }
    /**
     * Response object for a SignatureHelpRequest.
     */
    interface SignatureHelpResponse extends Response {
        body?: SignatureHelpItems;
    }
    /**
      * Synchronous request for semantic diagnostics of one file.
      */
    interface SemanticDiagnosticsSyncRequest extends FileRequest {
        command: CommandTypes.SemanticDiagnosticsSync;
        arguments: SemanticDiagnosticsSyncRequestArgs;
    }
    interface SemanticDiagnosticsSyncRequestArgs extends FileRequestArgs {
        includeLinePosition?: boolean;
    }
    /**
      * Response object for synchronous sematic diagnostics request.
      */
    interface SemanticDiagnosticsSyncResponse extends Response {
        body?: Diagnostic[] | DiagnosticWithLinePosition[];
    }
    /**
      * Synchronous request for syntactic diagnostics of one file.
      */
    interface SyntacticDiagnosticsSyncRequest extends FileRequest {
        command: CommandTypes.SyntacticDiagnosticsSync;
        arguments: SyntacticDiagnosticsSyncRequestArgs;
    }
    interface SyntacticDiagnosticsSyncRequestArgs extends FileRequestArgs {
        includeLinePosition?: boolean;
    }
    /**
      * Response object for synchronous syntactic diagnostics request.
      */
    interface SyntacticDiagnosticsSyncResponse extends Response {
        body?: Diagnostic[] | DiagnosticWithLinePosition[];
    }
    /**
    * Arguments for GeterrForProject request.
    */
    interface GeterrForProjectRequestArgs {
        /**
          * the file requesting project error list
          */
        file: string;
        /**
          * Delay in milliseconds to wait before starting to compute
          * errors for the files in the file list
          */
        delay: number;
    }
    /**
      * GeterrForProjectRequest request; value of command field is
      * "geterrForProject". It works similarly with 'Geterr', only
      * it request for every file in this project.
      */
    interface GeterrForProjectRequest extends Request {
        command: CommandTypes.GeterrForProject;
        arguments: GeterrForProjectRequestArgs;
    }
    /**
      * Arguments for geterr messages.
      */
    interface GeterrRequestArgs {
        /**
          * List of file names for which to compute compiler errors.
          * The files will be checked in list order.
          */
        files: string[];
        /**
          * Delay in milliseconds to wait before starting to compute
          * errors for the files in the file list
          */
        delay: number;
    }
    /**
      * Geterr request; value of command field is "geterr". Wait for
      * delay milliseconds and then, if during the wait no change or
      * reload messages have arrived for the first file in the files
      * list, get the syntactic errors for the file, field requests,
      * and then get the semantic errors for the file.  Repeat with a
      * smaller delay for each subsequent file on the files list.  Best
      * practice for an editor is to send a file list containing each
      * file that is currently visible, in most-recently-used order.
      */
    interface GeterrRequest extends Request {
        command: CommandTypes.Geterr;
        arguments: GeterrRequestArgs;
    }
    type RequestCompletedEventName = "requestCompleted";
    /**
     * Event that is sent when server have finished processing request with specified id.
     */
    interface RequestCompletedEvent extends Event {
        event: RequestCompletedEventName;
        body: RequestCompletedEventBody;
    }
    interface RequestCompletedEventBody {
        request_seq: number;
    }
    /**
      * Item of diagnostic information found in a DiagnosticEvent message.
      */
    interface Diagnostic {
        /**
          * Starting file location at which text applies.
          */
        start: Location;
        /**
          * The last file location at which the text applies.
          */
        end: Location;
        /**
          * Text of diagnostic message.
          */
        text: string;
        /**
          * The error code of the diagnostic message.
          */
        code?: number;
    }
    interface DiagnosticEventBody {
        /**
          * The file for which diagnostic information is reported.
          */
        file: string;
        /**
          * An array of diagnostic information items.
          */
        diagnostics: Diagnostic[];
    }
    /**
      * Event message for "syntaxDiag" and "semanticDiag" event types.
      * These events provide syntactic and semantic errors for a file.
      */
    interface DiagnosticEvent extends Event {
        body?: DiagnosticEventBody;
    }
    interface ConfigFileDiagnosticEventBody {
        /**
         * The file which trigged the searching and error-checking of the config file
         */
        triggerFile: string;
        /**
         * The name of the found config file.
         */
        configFile: string;
        /**
         * An arry of diagnostic information items for the found config file.
         */
        diagnostics: Diagnostic[];
    }
    /**
     * Event message for "configFileDiag" event type.
     * This event provides errors for a found config file.
     */
    interface ConfigFileDiagnosticEvent extends Event {
        body?: ConfigFileDiagnosticEventBody;
        event: "configFileDiag";
    }
    type ProjectLanguageServiceStateEventName = "projectLanguageServiceState";
    interface ProjectLanguageServiceStateEvent extends Event {
        event: ProjectLanguageServiceStateEventName;
        body?: ProjectLanguageServiceStateEventBody;
    }
    interface ProjectLanguageServiceStateEventBody {
        /**
         * Project name that has changes in the state of language service.
         * For configured projects this will be the config file path.
         * For external projects this will be the name of the projects specified when project was open.
         * For inferred projects this event is not raised.
         */
        projectName: string;
        /**
         * True if language service state switched from disabled to enabled
         * and false otherwise.
         */
        languageServiceEnabled: boolean;
    }
    /**
      * Arguments for reload request.
      */
    interface ReloadRequestArgs extends FileRequestArgs {
        /**
          * Name of temporary file from which to reload file
          * contents. May be same as file.
          */
        tmpfile: string;
    }
    /**
      * Reload request message; value of command field is "reload".
      * Reload contents of file with name given by the 'file' argument
      * from temporary file with name given by the 'tmpfile' argument.
      * The two names can be identical.
      */
    interface ReloadRequest extends FileRequest {
        command: CommandTypes.Reload;
        arguments: ReloadRequestArgs;
    }
    /**
      * Response to "reload" request. This is just an acknowledgement, so
      * no body field is required.
      */
    interface ReloadResponse extends Response {
    }
    /**
      * Arguments for saveto request.
      */
    interface SavetoRequestArgs extends FileRequestArgs {
        /**
          * Name of temporary file into which to save server's view of
          * file contents.
          */
        tmpfile: string;
    }
    /**
      * Saveto request message; value of command field is "saveto".
      * For debugging purposes, save to a temporaryfile (named by
      * argument 'tmpfile') the contents of file named by argument
      * 'file'.  The server does not currently send a response to a
      * "saveto" request.
      */
    interface SavetoRequest extends FileRequest {
        command: CommandTypes.Saveto;
        arguments: SavetoRequestArgs;
    }
    /**
      * Arguments for navto request message.
      */
    interface NavtoRequestArgs extends FileRequestArgs {
        /**
          * Search term to navigate to from current location; term can
          * be '.*' or an identifier prefix.
          */
        searchValue: string;
        /**
          *  Optional limit on the number of items to return.
          */
        maxResultCount?: number;
        /**
          * Optional flag to indicate we want results for just the current file
          * or the entire project.
          */
        currentFileOnly?: boolean;
        projectFileName?: string;
    }
    /**
      * Navto request message; value of command field is "navto".
      * Return list of objects giving file locations and symbols that
      * match the search term given in argument 'searchTerm'.  The
      * context for the search is given by the named file.
      */
    interface NavtoRequest extends FileRequest {
        command: CommandTypes.Navto;
        arguments: NavtoRequestArgs;
    }
    /**
      * An item found in a navto response.
      */
    interface NavtoItem {
        /**
          * The symbol's name.
          */
        name: string;
        /**
          * The symbol's kind (such as 'className' or 'parameterName').
          */
        kind: string;
        /**
          * exact, substring, or prefix.
          */
        matchKind?: string;
        /**
          * If this was a case sensitive or insensitive match.
          */
        isCaseSensitive?: boolean;
        /**
          * Optional modifiers for the kind (such as 'public').
          */
        kindModifiers?: string;
        /**
          * The file in which the symbol is found.
          */
        file: string;
        /**
          * The location within file at which the symbol is found.
          */
        start: Location;
        /**
          * One past the last character of the symbol.
          */
        end: Location;
        /**
          * Name of symbol's container symbol (if any); for example,
          * the class name if symbol is a class member.
          */
        containerName?: string;
        /**
          * Kind of symbol's container symbol (if any).
          */
        containerKind?: string;
    }
    /**
      * Navto response message. Body is an array of navto items.  Each
      * item gives a symbol that matched the search term.
      */
    interface NavtoResponse extends Response {
        body?: NavtoItem[];
    }
    /**
      * Arguments for change request message.
      */
    interface ChangeRequestArgs extends FormatRequestArgs {
        /**
          * Optional string to insert at location (file, line, offset).
          */
        insertString?: string;
    }
    /**
      * Change request message; value of command field is "change".
      * Update the server's view of the file named by argument 'file'.
      * Server does not currently send a response to a change request.
      */
    interface ChangeRequest extends FileLocationRequest {
        command: CommandTypes.Change;
        arguments: ChangeRequestArgs;
    }
    /**
      * Response to "brace" request.
      */
    interface BraceResponse extends Response {
        body?: TextSpan[];
    }
    /**
      * Brace matching request; value of command field is "brace".
      * Return response giving the file locations of matching braces
      * found in file at location line, offset.
      */
    interface BraceRequest extends FileLocationRequest {
        command: CommandTypes.Brace;
    }
    /**
      * NavBar items request; value of command field is "navbar".
      * Return response giving the list of navigation bar entries
      * extracted from the requested file.
      */
    interface NavBarRequest extends FileRequest {
        command: CommandTypes.NavBar;
    }
    /**
     * NavTree request; value of command field is "navtree".
     * Return response giving the navigation tree of the requested file.
     */
    interface NavTreeRequest extends FileRequest {
        command: CommandTypes.NavTree;
    }
    interface NavigationBarItem {
        /**
          * The item's display text.
          */
        text: string;
        /**
          * The symbol's kind (such as 'className' or 'parameterName').
          */
        kind: string;
        /**
          * Optional modifiers for the kind (such as 'public').
          */
        kindModifiers?: string;
        /**
          * The definition locations of the item.
          */
        spans: TextSpan[];
        /**
          * Optional children.
          */
        childItems?: NavigationBarItem[];
        /**
          * Number of levels deep this item should appear.
          */
        indent: number;
    }
    /** protocol.NavigationTree is identical to ts.NavigationTree, except using protocol.TextSpan instead of ts.TextSpan */
    interface NavigationTree {
        text: string;
        kind: string;
        kindModifiers: string;
        spans: TextSpan[];
        childItems?: NavigationTree[];
    }
    type TelemetryEventName = "telemetry";
    interface TelemetryEvent extends Event {
        event: TelemetryEventName;
        body: TelemetryEventBody;
    }
    interface TelemetryEventBody {
        telemetryEventName: string;
        payload: any;
    }
    type TypingsInstalledTelemetryEventName = "typingsInstalled";
    interface TypingsInstalledTelemetryEventBody extends TelemetryEventBody {
        telemetryEventName: TypingsInstalledTelemetryEventName;
        payload: TypingsInstalledTelemetryEventPayload;
    }
    interface TypingsInstalledTelemetryEventPayload {
        /**
         * Comma separated list of installed typing packages
         */
        installedPackages: string;
        /**
         * true if install request succeeded, otherwise - false
         */
        installSuccess: boolean;
        /**
         * version of typings installer
         */
        typingsInstallerVersion: string;
    }
    type BeginInstallTypesEventName = "beginInstallTypes";
    type EndInstallTypesEventName = "endInstallTypes";
    interface BeginInstallTypesEvent extends Event {
        event: BeginInstallTypesEventName;
        body: BeginInstallTypesEventBody;
    }
    interface EndInstallTypesEvent extends Event {
        event: EndInstallTypesEventName;
        body: EndInstallTypesEventBody;
    }
    interface InstallTypesEventBody {
        /**
         * correlation id to match begin and end events
         */
        eventId: number;
        /**
         * list of packages to install
         */
        packages: ReadonlyArray<string>;
    }
    interface BeginInstallTypesEventBody extends InstallTypesEventBody {
    }
    interface EndInstallTypesEventBody extends InstallTypesEventBody {
        /**
         * true if installation succeeded, otherwise false
         */
        success: boolean;
    }
    interface NavBarResponse extends Response {
        body?: NavigationBarItem[];
    }
    interface NavTreeResponse extends Response {
        body?: NavigationTree;
    }
    namespace IndentStyle {
        type None = "None";
        type Block = "Block";
        type Smart = "Smart";
    }
    type IndentStyle = IndentStyle.None | IndentStyle.Block | IndentStyle.Smart;
    interface EditorSettings {
        baseIndentSize?: number;
        indentSize?: number;
        tabSize?: number;
        newLineCharacter?: string;
        convertTabsToSpaces?: boolean;
        indentStyle?: IndentStyle | ts.IndentStyle;
    }
    interface FormatCodeSettings extends EditorSettings {
        insertSpaceAfterCommaDelimiter?: boolean;
        insertSpaceAfterSemicolonInForStatements?: boolean;
        insertSpaceBeforeAndAfterBinaryOperators?: boolean;
        insertSpaceAfterConstructor?: boolean;
        insertSpaceAfterKeywordsInControlFlowStatements?: boolean;
        insertSpaceAfterFunctionKeywordForAnonymousFunctions?: boolean;
        insertSpaceAfterOpeningAndBeforeClosingNonemptyParenthesis?: boolean;
        insertSpaceAfterOpeningAndBeforeClosingNonemptyBrackets?: boolean;
        insertSpaceAfterOpeningAndBeforeClosingTemplateStringBraces?: boolean;
        insertSpaceAfterOpeningAndBeforeClosingJsxExpressionBraces?: boolean;
        insertSpaceBeforeFunctionParenthesis?: boolean;
        placeOpenBraceOnNewLineForFunctions?: boolean;
        placeOpenBraceOnNewLineForControlBlocks?: boolean;
    }
    interface CompilerOptions {
        allowJs?: boolean;
        allowSyntheticDefaultImports?: boolean;
        allowUnreachableCode?: boolean;
        allowUnusedLabels?: boolean;
        baseUrl?: string;
        charset?: string;
        declaration?: boolean;
        declarationDir?: string;
        disableSizeLimit?: boolean;
        emitBOM?: boolean;
        emitDecoratorMetadata?: boolean;
        experimentalDecorators?: boolean;
        forceConsistentCasingInFileNames?: boolean;
        inlineSourceMap?: boolean;
        inlineSources?: boolean;
        isolatedModules?: boolean;
        jsx?: JsxEmit | ts.JsxEmit;
        lib?: string[];
        locale?: string;
        mapRoot?: string;
        maxNodeModuleJsDepth?: number;
        module?: ModuleKind | ts.ModuleKind;
        moduleResolution?: ModuleResolutionKind | ts.ModuleResolutionKind;
        newLine?: NewLineKind | ts.NewLineKind;
        noEmit?: boolean;
        noEmitHelpers?: boolean;
        noEmitOnError?: boolean;
        noErrorTruncation?: boolean;
        noFallthroughCasesInSwitch?: boolean;
        noImplicitAny?: boolean;
        noImplicitReturns?: boolean;
        noImplicitThis?: boolean;
        noUnusedLocals?: boolean;
        noUnusedParameters?: boolean;
        noImplicitUseStrict?: boolean;
        noLib?: boolean;
        noResolve?: boolean;
        out?: string;
        outDir?: string;
        outFile?: string;
        paths?: MapLike<string[]>;
        plugins?: PluginImport[];
        preserveConstEnums?: boolean;
        project?: string;
        reactNamespace?: string;
        removeComments?: boolean;
        rootDir?: string;
        rootDirs?: string[];
        skipLibCheck?: boolean;
        skipDefaultLibCheck?: boolean;
        sourceMap?: boolean;
        sourceRoot?: string;
        strictNullChecks?: boolean;
        suppressExcessPropertyErrors?: boolean;
        suppressImplicitAnyIndexErrors?: boolean;
        target?: ScriptTarget | ts.ScriptTarget;
        traceResolution?: boolean;
        types?: string[];
        /** Paths used to used to compute primary types search locations */
        typeRoots?: string[];
        [option: string]: CompilerOptionsValue | undefined;
    }
    namespace JsxEmit {
        type None = "None";
        type Preserve = "Preserve";
        type ReactNative = "ReactNative";
        type React = "React";
    }
    type JsxEmit = JsxEmit.None | JsxEmit.Preserve | JsxEmit.React | JsxEmit.ReactNative;
    namespace ModuleKind {
        type None = "None";
        type CommonJS = "CommonJS";
        type AMD = "AMD";
        type UMD = "UMD";
        type System = "System";
        type ES6 = "ES6";
        type ES2015 = "ES2015";
    }
    type ModuleKind = ModuleKind.None | ModuleKind.CommonJS | ModuleKind.AMD | ModuleKind.UMD | ModuleKind.System | ModuleKind.ES6 | ModuleKind.ES2015;
    namespace ModuleResolutionKind {
        type Classic = "Classic";
        type Node = "Node";
    }
    type ModuleResolutionKind = ModuleResolutionKind.Classic | ModuleResolutionKind.Node;
    namespace NewLineKind {
        type Crlf = "Crlf";
        type Lf = "Lf";
    }
    type NewLineKind = NewLineKind.Crlf | NewLineKind.Lf;
    namespace ScriptTarget {
        type ES3 = "ES3";
        type ES5 = "ES5";
        type ES6 = "ES6";
        type ES2015 = "ES2015";
    }
    type ScriptTarget = ScriptTarget.ES3 | ScriptTarget.ES5 | ScriptTarget.ES6 | ScriptTarget.ES2015;
}
declare namespace ts.server {
    interface ServerCancellationToken extends HostCancellationToken {
        setRequest(requestId: number): void;
        resetRequest(requestId: number): void;
    }
    const nullCancellationToken: ServerCancellationToken;
    interface PendingErrorCheck {
        fileName: NormalizedPath;
        project: Project;
    }
    interface EventSender {
        event(payload: any, eventName: string): void;
    }
    namespace CommandNames {
        const Brace: protocol.CommandTypes.Brace;
        const BraceCompletion: protocol.CommandTypes.BraceCompletion;
        const Change: protocol.CommandTypes.Change;
        const Close: protocol.CommandTypes.Close;
        const Completions: protocol.CommandTypes.Completions;
        const CompletionDetails: protocol.CommandTypes.CompletionDetails;
        const CompileOnSaveAffectedFileList: protocol.CommandTypes.CompileOnSaveAffectedFileList;
        const CompileOnSaveEmitFile: protocol.CommandTypes.CompileOnSaveEmitFile;
        const Configure: protocol.CommandTypes.Configure;
        const Definition: protocol.CommandTypes.Definition;
        const Exit: protocol.CommandTypes.Exit;
        const Format: protocol.CommandTypes.Format;
        const Formatonkey: protocol.CommandTypes.Formatonkey;
        const Geterr: protocol.CommandTypes.Geterr;
        const GeterrForProject: protocol.CommandTypes.GeterrForProject;
        const Implementation: protocol.CommandTypes.Implementation;
        const SemanticDiagnosticsSync: protocol.CommandTypes.SemanticDiagnosticsSync;
        const SyntacticDiagnosticsSync: protocol.CommandTypes.SyntacticDiagnosticsSync;
        const NavBar: protocol.CommandTypes.NavBar;
        const NavTree: protocol.CommandTypes.NavTree;
        const NavTreeFull: protocol.CommandTypes.NavTreeFull;
        const Navto: protocol.CommandTypes.Navto;
        const Occurrences: protocol.CommandTypes.Occurrences;
        const DocumentHighlights: protocol.CommandTypes.DocumentHighlights;
        const Open: protocol.CommandTypes.Open;
        const Quickinfo: protocol.CommandTypes.Quickinfo;
        const References: protocol.CommandTypes.References;
        const Reload: protocol.CommandTypes.Reload;
        const Rename: protocol.CommandTypes.Rename;
        const Saveto: protocol.CommandTypes.Saveto;
        const SignatureHelp: protocol.CommandTypes.SignatureHelp;
        const TypeDefinition: protocol.CommandTypes.TypeDefinition;
        const ProjectInfo: protocol.CommandTypes.ProjectInfo;
        const ReloadProjects: protocol.CommandTypes.ReloadProjects;
        const Unknown: protocol.CommandTypes.Unknown;
        const OpenExternalProject: protocol.CommandTypes.OpenExternalProject;
        const OpenExternalProjects: protocol.CommandTypes.OpenExternalProjects;
        const CloseExternalProject: protocol.CommandTypes.CloseExternalProject;
        const TodoComments: protocol.CommandTypes.TodoComments;
        const Indentation: protocol.CommandTypes.Indentation;
        const DocCommentTemplate: protocol.CommandTypes.DocCommentTemplate;
        const CompilerOptionsForInferredProjects: protocol.CommandTypes.CompilerOptionsForInferredProjects;
        const GetCodeFixes: protocol.CommandTypes.GetCodeFixes;
        const GetSupportedCodeFixes: protocol.CommandTypes.GetSupportedCodeFixes;
    }
    function formatMessage<T extends protocol.Message>(msg: T, logger: server.Logger, byteLength: (s: string, encoding: string) => number, newLine: string): string;
    class Session implements EventSender {
        private host;
        private readonly cancellationToken;
        protected readonly typingsInstaller: ITypingsInstaller;
        private byteLength;
        private hrtime;
        protected logger: Logger;
        protected readonly canUseEvents: boolean;
        private readonly gcTimer;
        protected projectService: ProjectService;
        private changeSeq;
        private currentRequestId;
        private errorCheck;
        private eventHander;
        constructor(host: ServerHost, cancellationToken: ServerCancellationToken, useSingleInferredProject: boolean, typingsInstaller: ITypingsInstaller, byteLength: (buf: string, encoding?: string) => number, hrtime: (start?: number[]) => number[], logger: Logger, canUseEvents: boolean, eventHandler?: ProjectServiceEventHandler);
        private sendRequestCompletedEvent(requestId);
        private defaultEventHandler(event);
        logError(err: Error, cmd: string): void;
        send(msg: protocol.Message): void;
        configFileDiagnosticEvent(triggerFile: string, configFile: string, diagnostics: ts.Diagnostic[]): void;
        event(info: any, eventName: string): void;
        output(info: any, cmdName: string, reqSeq?: number, errorMsg?: string): void;
        private semanticCheck(file, project);
        private syntacticCheck(file, project);
        private updateProjectStructure(seq, matchSeq, ms?);
        private updateErrorCheck(next, checkList, seq, matchSeq, ms?, followMs?, requireOpen?);
        private cleanProjects(caption, projects);
        private cleanup();
        private getEncodedSemanticClassifications(args);
        private getProject(projectFileName);
        private getCompilerOptionsDiagnostics(args);
        private convertToDiagnosticsWithLinePosition(diagnostics, scriptInfo);
        private getDiagnosticsWorker(args, isSemantic, selector, includeLinePosition);
        private getDefinition(args, simplifiedResult);
        private getTypeDefinition(args);
        private getImplementation(args, simplifiedResult);
        private getOccurrences(args);
        private getSyntacticDiagnosticsSync(args);
        private getSemanticDiagnosticsSync(args);
        private getDocumentHighlights(args, simplifiedResult);
        private setCompilerOptionsForInferredProjects(args);
        private getProjectInfo(args);
        private getProjectInfoWorker(uncheckedFileName, projectFileName, needFileNameList);
        private getRenameInfo(args);
        private getProjects(args);
        private getRenameLocations(args, simplifiedResult);
        private getReferences(args, simplifiedResult);
        /**
         * @param fileName is the name of the file to be opened
         * @param fileContent is a version of the file content that is known to be more up to date than the one on disk
         */
        private openClientFile(fileName, fileContent?, scriptKind?);
        private getPosition(args, scriptInfo);
        private getFileAndProject(args, errorOnMissingProject?);
        private getFileAndProjectWithoutRefreshingInferredProjects(args, errorOnMissingProject?);
        private getFileAndProjectWorker(uncheckedFileName, projectFileName, refreshInferredProjects, errorOnMissingProject);
        private getOutliningSpans(args);
        private getTodoComments(args);
        private getDocCommentTemplate(args);
        private getIndentation(args);
        private getBreakpointStatement(args);
        private getNameOrDottedNameSpan(args);
        private isValidBraceCompletion(args);
        private getQuickInfoWorker(args, simplifiedResult);
        private getFormattingEditsForRange(args);
        private getFormattingEditsForRangeFull(args);
        private getFormattingEditsForDocumentFull(args);
        private getFormattingEditsAfterKeystrokeFull(args);
        private getFormattingEditsAfterKeystroke(args);
        private getCompletions(args, simplifiedResult);
        private getCompletionEntryDetails(args);
        private getCompileOnSaveAffectedFileList(args);
        private emitFile(args);
        private getSignatureHelpItems(args, simplifiedResult);
        private getDiagnostics(next, delay, fileNames);
        private change(args);
        private reload(args, reqSeq);
        private saveToTmp(fileName, tempFileName);
        private closeClientFile(fileName);
        private decorateNavigationBarItems(items, scriptInfo);
        private getNavigationBarItems(args, simplifiedResult);
        private decorateNavigationTree(tree, scriptInfo);
        private decorateSpan(span, scriptInfo);
        private getNavigationTree(args, simplifiedResult);
        private getNavigateToItems(args, simplifiedResult);
        private getSupportedCodeFixes();
        private getCodeFixes(args, simplifiedResult);
        private mapCodeAction(codeAction, scriptInfo);
        private convertTextChangeToCodeEdit(change, scriptInfo);
        private getBraceMatching(args, simplifiedResult);
        private getDiagnosticsForProject(next, delay, fileName);
        getCanonicalFileName(fileName: string): string;
        exit(): void;
        private notRequired();
        private requiredResponse(response);
        private handlers;
        addProtocolHandler(command: string, handler: (request: protocol.Request) => {
            response?: any;
            responseRequired: boolean;
        }): void;
        private setCurrentRequest(requestId);
        private resetCurrentRequest(requestId);
        executeWithRequestId<T>(requestId: number, f: () => T): T;
        executeCommand(request: protocol.Request): {
            response?: any;
            responseRequired?: boolean;
        };
        onMessage(message: string): void;
    }
}
declare namespace ts.server {
    interface LineCollection {
        charCount(): number;
        lineCount(): number;
        isLeaf(): boolean;
        walk(rangeStart: number, rangeLength: number, walkFns: ILineIndexWalker): void;
    }
    interface ILineInfo {
        line: number;
        offset: number;
        text?: string;
        leaf?: LineLeaf;
    }
    enum CharRangeSection {
        PreStart = 0,
        Start = 1,
        Entire = 2,
        Mid = 3,
        End = 4,
        PostEnd = 5,
    }
    interface ILineIndexWalker {
        goSubtree: boolean;
        done: boolean;
        leaf(relativeStart: number, relativeLength: number, lineCollection: LineLeaf): void;
        pre?(relativeStart: number, relativeLength: number, lineCollection: LineCollection, parent: LineNode, nodeType: CharRangeSection): LineCollection;
        post?(relativeStart: number, relativeLength: number, lineCollection: LineCollection, parent: LineNode, nodeType: CharRangeSection): LineCollection;
    }
    class TextChange {
        pos: number;
        deleteLen: number;
        insertedText: string;
        constructor(pos: number, deleteLen: number, insertedText?: string);
        getTextChangeRange(): TextChangeRange;
    }
    class ScriptVersionCache {
        changes: TextChange[];
        versions: LineIndexSnapshot[];
        minVersion: number;
        private host;
        private currentVersion;
        static changeNumberThreshold: number;
        static changeLengthThreshold: number;
        static maxVersions: number;
        private versionToIndex(version);
        private currentVersionToIndex();
        edit(pos: number, deleteLen: number, insertedText?: string): void;
        latest(): LineIndexSnapshot;
        latestVersion(): number;
        reloadFromFile(filename: string): void;
        reload(script: string): void;
        getSnapshot(): LineIndexSnapshot;
        getTextChangesBetweenVersions(oldVersion: number, newVersion: number): TextChangeRange;
        static fromString(host: ServerHost, script: string): ScriptVersionCache;
    }
    class LineIndexSnapshot implements ts.IScriptSnapshot {
        readonly version: number;
        readonly cache: ScriptVersionCache;
        index: LineIndex;
        changesSincePreviousVersion: TextChange[];
        constructor(version: number, cache: ScriptVersionCache);
        getText(rangeStart: number, rangeEnd: number): string;
        getLength(): number;
        getLineStartPositions(): number[];
        getLineMapper(): (line: number) => number;
        getTextChangeRangeSinceVersion(scriptVersion: number): TextChangeRange;
        getChangeRange(oldSnapshot: ts.IScriptSnapshot): ts.TextChangeRange;
    }
    class LineIndex {
        root: LineNode;
        checkEdits: boolean;
        charOffsetToLineNumberAndPos(charOffset: number): ILineInfo;
        lineNumberToInfo(lineNumber: number): ILineInfo;
        load(lines: string[]): void;
        walk(rangeStart: number, rangeLength: number, walkFns: ILineIndexWalker): void;
        getText(rangeStart: number, rangeLength: number): string;
        getLength(): number;
        every(f: (ll: LineLeaf, s: number, len: number) => boolean, rangeStart: number, rangeEnd?: number): boolean;
        edit(pos: number, deleteLength: number, newText?: string): LineIndex;
        static buildTreeFromBottom(nodes: LineCollection[]): LineNode;
        static linesFromText(text: string): {
            lines: string[];
            lineMap: number[];
        };
    }
    class LineNode implements LineCollection {
        totalChars: number;
        totalLines: number;
        children: LineCollection[];
        isLeaf(): boolean;
        updateCounts(): void;
        execWalk(rangeStart: number, rangeLength: number, walkFns: ILineIndexWalker, childIndex: number, nodeType: CharRangeSection): boolean;
        skipChild(relativeStart: number, relativeLength: number, childIndex: number, walkFns: ILineIndexWalker, nodeType: CharRangeSection): void;
        walk(rangeStart: number, rangeLength: number, walkFns: ILineIndexWalker): void;
        charOffsetToLineNumberAndPos(lineNumber: number, charOffset: number): ILineInfo;
        lineNumberToInfo(lineNumber: number, charOffset: number): ILineInfo;
        childFromLineNumber(lineNumber: number, charOffset: number): {
            child: LineCollection;
            childIndex: number;
            relativeLineNumber: number;
            charOffset: number;
        };
        childFromCharOffset(lineNumber: number, charOffset: number): {
            child: LineCollection;
            childIndex: number;
            charOffset: number;
            lineNumber: number;
        };
        splitAfter(childIndex: number): LineNode;
        remove(child: LineCollection): void;
        findChildIndex(child: LineCollection): number;
        insertAt(child: LineCollection, nodes: LineCollection[]): LineNode[];
        add(collection: LineCollection): boolean;
        charCount(): number;
        lineCount(): number;
    }
    class LineLeaf implements LineCollection {
        text: string;
        constructor(text: string);
        isLeaf(): boolean;
        walk(rangeStart: number, rangeLength: number, walkFns: ILineIndexWalker): void;
        charCount(): number;
        lineCount(): number;
    }
}
declare namespace ts.server {
    class ScriptInfo {
        private readonly host;
        readonly fileName: NormalizedPath;
        readonly scriptKind: ScriptKind;
        hasMixedContent: boolean;
        /**
         * All projects that include this file
         */
        readonly containingProjects: Project[];
        private formatCodeSettings;
        readonly path: Path;
        private fileWatcher;
        private textStorage;
        private isOpen;
        constructor(host: ServerHost, fileName: NormalizedPath, scriptKind: ScriptKind, hasMixedContent?: boolean);
        isScriptOpen(): boolean;
        open(newText: string): void;
        close(): void;
        getSnapshot(): IScriptSnapshot;
        getFormatCodeSettings(): FormatCodeSettings;
        attachToProject(project: Project): boolean;
        isAttached(project: Project): boolean;
        detachFromProject(project: Project): void;
        detachAllProjects(): void;
        getDefaultProject(): Project;
        registerFileUpdate(): void;
        setFormatOptions(formatSettings: FormatCodeSettings): void;
        setWatcher(watcher: FileWatcher): void;
        stopWatcher(): void;
        getLatestVersion(): string;
        reload(script: string): void;
        saveTo(fileName: string): void;
        reloadFromFile(tempFileName?: NormalizedPath): void;
        getLineInfo(line: number): ILineInfo;
        editContent(start: number, end: number, newText: string): void;
        markContainingProjectsAsDirty(): void;
        /**
         *  @param line 1 based index
         */
        lineToTextSpan(line: number): TextSpan;
        /**
         * @param line 1 based index
         * @param offset 1 based index
         */
        lineOffsetToPosition(line: number, offset: number): number;
        /**
         * @param line 1-based index
         * @param offset 1-based index
         */
        positionToLineOffset(position: number): ILineInfo;
        isJavaScript(): boolean;
    }
}
declare namespace ts.server {
    class LSHost implements ts.LanguageServiceHost, ModuleResolutionHost {
        private readonly host;
        private readonly project;
        private readonly cancellationToken;
        private compilationSettings;
        private readonly resolvedModuleNames;
        private readonly resolvedTypeReferenceDirectives;
        private readonly getCanonicalFileName;
        private filesWithChangedSetOfUnresolvedImports;
        private readonly resolveModuleName;
        readonly trace: (s: string) => void;
        readonly realpath?: (path: string) => string;
        constructor(host: ServerHost, project: Project, cancellationToken: HostCancellationToken);
        startRecordingFilesWithChangedResolutions(): void;
        finishRecordingFilesWithChangedResolutions(): Path[];
        private resolveNamesWithLocalCache<T, R>(names, containingFile, cache, loader, getResult, getResultFileName, logChanges);
        getNewLine(): string;
        getProjectVersion(): string;
        getCompilationSettings(): CompilerOptions;
        useCaseSensitiveFileNames(): boolean;
        getCancellationToken(): HostCancellationToken;
        resolveTypeReferenceDirectives(typeDirectiveNames: string[], containingFile: string): ResolvedTypeReferenceDirective[];
        resolveModuleNames(moduleNames: string[], containingFile: string): ResolvedModuleFull[];
        getDefaultLibFileName(): string;
        getScriptSnapshot(filename: string): ts.IScriptSnapshot;
        getScriptFileNames(): string[];
        getTypeRootsVersion(): number;
        getScriptKind(fileName: string): ScriptKind;
        getScriptVersion(filename: string): string;
        getCurrentDirectory(): string;
        resolvePath(path: string): string;
        fileExists(path: string): boolean;
        readFile(fileName: string): string;
        directoryExists(path: string): boolean;
        readDirectory(path: string, extensions?: string[], exclude?: string[], include?: string[]): string[];
        getDirectories(path: string): string[];
        notifyFileRemoved(info: ScriptInfo): void;
        setCompilationSettings(opt: ts.CompilerOptions): void;
    }
}
declare namespace ts.server {
    interface ITypingsInstaller {
        enqueueInstallTypingsRequest(p: Project, typeAcquisition: TypeAcquisition, unresolvedImports: SortedReadonlyArray<string>): void;
        attach(projectService: ProjectService): void;
        onProjectClosed(p: Project): void;
        readonly globalTypingsCacheLocation: string;
    }
    const nullTypingsInstaller: ITypingsInstaller;
    class TypingsCache {
        private readonly installer;
        private readonly perProjectCache;
        constructor(installer: ITypingsInstaller);
        getTypingsForProject(project: Project, unresolvedImports: SortedReadonlyArray<string>, forceRefresh: boolean): SortedReadonlyArray<string>;
        updateTypingsForProject(projectName: string, compilerOptions: CompilerOptions, typeAcquisition: TypeAcquisition, unresolvedImports: SortedReadonlyArray<string>, newTypings: string[]): void;
        deleteTypingsForProject(projectName: string): void;
        onProjectClosed(project: Project): void;
    }
}
declare namespace ts.server {
    function shouldEmitFile(scriptInfo: ScriptInfo): boolean;
    /**
     * An abstract file info that maintains a shape signature.
     */
    class BuilderFileInfo {
        readonly scriptInfo: ScriptInfo;
        readonly project: Project;
        private lastCheckedShapeSignature;
        constructor(scriptInfo: ScriptInfo, project: Project);
        isExternalModuleOrHasOnlyAmbientExternalModules(): boolean;
        /**
         * For script files that contains only ambient external modules, although they are not actually external module files,
         * they can only be consumed via importing elements from them. Regular script files cannot consume them. Therefore,
         * there are no point to rebuild all script files if these special files have changed. However, if any statement
         * in the file is not ambient external module, we treat it as a regular script file.
         */
        private containsOnlyAmbientModules(sourceFile);
        private computeHash(text);
        private getSourceFile();
        /**
         * @return {boolean} indicates if the shape signature has changed since last update.
         */
        updateShapeSignature(): boolean;
    }
    interface Builder {
        readonly project: Project;
        getFilesAffectedBy(scriptInfo: ScriptInfo): string[];
        onProjectUpdateGraph(): void;
        emitFile(scriptInfo: ScriptInfo, writeFile: (path: string, data: string, writeByteOrderMark?: boolean) => void): boolean;
        clear(): void;
    }
    function createBuilder(project: Project): Builder;
}
declare namespace ts.server {
    enum ProjectKind {
        Inferred = 0,
        Configured = 1,
        External = 2,
    }
    function allRootFilesAreJsOrDts(project: Project): boolean;
    function allFilesAreJsOrDts(project: Project): boolean;
    class UnresolvedImportsMap {
        readonly perFileMap: FileMap<ReadonlyArray<string>>;
        private version;
        clear(): void;
        getVersion(): number;
        remove(path: Path): void;
        get(path: Path): ReadonlyArray<string>;
        set(path: Path, value: ReadonlyArray<string>): void;
    }
    interface PluginCreateInfo {
        project: Project;
        languageService: LanguageService;
        languageServiceHost: LanguageServiceHost;
        serverHost: ServerHost;
        config: any;
    }
    interface PluginModule {
        create(createInfo: PluginCreateInfo): LanguageService;
        getExternalFiles?(proj: Project): string[];
    }
    interface PluginModuleFactory {
        (mod: {
            typescript: typeof ts;
        }): PluginModule;
    }
    abstract class Project {
        private readonly projectName;
        readonly projectKind: ProjectKind;
        readonly projectService: ProjectService;
        private documentRegistry;
        private compilerOptions;
        compileOnSaveEnabled: boolean;
        private rootFiles;
        private rootFilesMap;
        private program;
        private cachedUnresolvedImportsPerFile;
        private lastCachedUnresolvedImportsList;
        protected languageService: LanguageService;
        languageServiceEnabled: boolean;
        protected readonly lsHost: LSHost;
        builder: Builder;
        /**
         * Set of files names that were updated since the last call to getChangesSinceVersion.
         */
        private updatedFileNames;
        /**
         * Set of files that was returned from the last call to getChangesSinceVersion.
         */
        private lastReportedFileNames;
        /**
         * Last version that was reported.
         */
        private lastReportedVersion;
        /**
         * Current project structure version.
         * This property is changed in 'updateGraph' based on the set of files in program
         */
        private projectStructureVersion;
        /**
         * Current version of the project state. It is changed when:
         * - new root file was added/removed
         * - edit happen in some file that is currently included in the project.
         * This property is different from projectStructureVersion since in most cases edits don't affect set of files in the project
         */
        private projectStateVersion;
        private typingFiles;
        protected projectErrors: Diagnostic[];
        typesVersion: number;
        isNonTsProject(): boolean;
        isJsOnlyProject(): boolean;
        getCachedUnresolvedImportsPerFile_TestOnly(): UnresolvedImportsMap;
        static resolveModule(moduleName: string, initialDir: string, host: ServerHost, log: (message: string) => void): {};
        constructor(projectName: string, projectKind: ProjectKind, projectService: ProjectService, documentRegistry: ts.DocumentRegistry, hasExplicitListOfFiles: boolean, languageServiceEnabled: boolean, compilerOptions: CompilerOptions, compileOnSaveEnabled: boolean);
        private setInternalCompilerOptionsForEmittingJsFiles();
        getProjectErrors(): Diagnostic[];
        getLanguageService(ensureSynchronized?: boolean): LanguageService;
        getCompileOnSaveAffectedFileList(scriptInfo: ScriptInfo): string[];
        getProjectVersion(): string;
        enableLanguageService(): void;
        disableLanguageService(): void;
        getProjectName(): string;
        abstract getProjectRootPath(): string | undefined;
        abstract getTypeAcquisition(): TypeAcquisition;
        getExternalFiles(): string[];
        getSourceFile(path: Path): SourceFile;
        updateTypes(): void;
        close(): void;
        getCompilerOptions(): CompilerOptions;
        hasRoots(): boolean;
        getRootFiles(): NormalizedPath[];
        getRootFilesLSHost(): string[];
        getRootScriptInfos(): ScriptInfo[];
        getScriptInfos(): ScriptInfo[];
        getFileEmitOutput(info: ScriptInfo, emitOnlyDtsFiles: boolean): EmitOutput;
        getFileNames(excludeFilesFromExternalLibraries?: boolean): NormalizedPath[];
        getAllEmittableFiles(): string[];
        containsScriptInfo(info: ScriptInfo): boolean;
        containsFile(filename: NormalizedPath, requireOpen?: boolean): boolean;
        isRoot(info: ScriptInfo): boolean;
        addRoot(info: ScriptInfo): void;
        removeFile(info: ScriptInfo, detachFromProject?: boolean): void;
        registerFileUpdate(fileName: string): void;
        markAsDirty(): void;
        private extractUnresolvedImportsFromSourceFile(file, result);
        /**
         * Updates set of files that contribute to this project
         * @returns: true if set of files in the project stays the same and false - otherwise.
         */
        updateGraph(): boolean;
        private setTypings(typings);
        private updateGraphWorker();
        getScriptInfoLSHost(fileName: string): ScriptInfo;
        getScriptInfoForNormalizedPath(fileName: NormalizedPath): ScriptInfo;
        getScriptInfo(uncheckedFileName: string): ScriptInfo;
        filesToString(): string;
        setCompilerOptions(compilerOptions: CompilerOptions): void;
        reloadScript(filename: NormalizedPath, tempFileName?: NormalizedPath): boolean;
        getReferencedFiles(path: Path): Path[];
        protected removeRoot(info: ScriptInfo): void;
    }
    class InferredProject extends Project {
        private static newName;
        private _isJsInferredProject;
        toggleJsInferredProject(isJsInferredProject: boolean): void;
        setCompilerOptions(options?: CompilerOptions): void;
        directoriesWatchedForTsconfig: string[];
        constructor(projectService: ProjectService, documentRegistry: ts.DocumentRegistry, compilerOptions: CompilerOptions);
        addRoot(info: ScriptInfo): void;
        removeRoot(info: ScriptInfo): void;
        getProjectRootPath(): string;
        close(): void;
        getTypeAcquisition(): TypeAcquisition;
    }
    class ConfiguredProject extends Project {
        private configFileName;
        private wildcardDirectories;
        compileOnSaveEnabled: boolean;
        private typeAcquisition;
        private projectFileWatcher;
        private directoryWatcher;
        private directoriesWatchedForWildcards;
        private typeRootsWatchers;
        readonly canonicalConfigFilePath: NormalizedPath;
        private plugins;
        /** Used for configured projects which may have multiple open roots */
        openRefCount: number;
        constructor(configFileName: NormalizedPath, projectService: ProjectService, documentRegistry: ts.DocumentRegistry, hasExplicitListOfFiles: boolean, compilerOptions: CompilerOptions, wildcardDirectories: Map<WatchDirectoryFlags>, languageServiceEnabled: boolean, compileOnSaveEnabled: boolean);
        getConfigFilePath(): string;
        enablePlugins(): void;
        private enableProxy(pluginModuleFactory, configEntry);
        getProjectRootPath(): string;
        setProjectErrors(projectErrors: Diagnostic[]): void;
        setTypeAcquisition(newTypeAcquisition: TypeAcquisition): void;
        getTypeAcquisition(): TypeAcquisition;
        getExternalFiles(): string[];
        watchConfigFile(callback: (project: ConfiguredProject) => void): void;
        watchTypeRoots(callback: (project: ConfiguredProject, path: string) => void): void;
        watchConfigDirectory(callback: (project: ConfiguredProject, path: string) => void): void;
        watchWildcards(callback: (project: ConfiguredProject, path: string) => void): void;
        stopWatchingDirectory(): void;
        close(): void;
        addOpenRef(): void;
        deleteOpenRef(): number;
        getEffectiveTypeRoots(): string[];
    }
    class ExternalProject extends Project {
        compileOnSaveEnabled: boolean;
        private readonly projectFilePath;
        private typeAcquisition;
        constructor(externalProjectName: string, projectService: ProjectService, documentRegistry: ts.DocumentRegistry, compilerOptions: CompilerOptions, languageServiceEnabled: boolean, compileOnSaveEnabled: boolean, projectFilePath?: string);
        getProjectRootPath(): string;
        getTypeAcquisition(): TypeAcquisition;
        setProjectErrors(projectErrors: Diagnostic[]): void;
        setTypeAcquisition(newTypeAcquisition: TypeAcquisition): void;
    }
}
declare namespace ts.server {
    const maxProgramSizeForNonTsFiles: number;
    const ContextEvent = "context";
    const ConfigFileDiagEvent = "configFileDiag";
    const ProjectLanguageServiceStateEvent = "projectLanguageServiceState";
    interface ContextEvent {
        eventName: typeof ContextEvent;
        data: {
            project: Project;
            fileName: NormalizedPath;
        };
    }
    interface ConfigFileDiagEvent {
        eventName: typeof ConfigFileDiagEvent;
        data: {
            triggerFile: string;
            configFileName: string;
            diagnostics: Diagnostic[];
        };
    }
    interface ProjectLanguageServiceStateEvent {
        eventName: typeof ProjectLanguageServiceStateEvent;
        data: {
            project: Project;
            languageServiceEnabled: boolean;
        };
    }
    type ProjectServiceEvent = ContextEvent | ConfigFileDiagEvent | ProjectLanguageServiceStateEvent;
    interface ProjectServiceEventHandler {
        (event: ProjectServiceEvent): void;
    }
    function convertFormatOptions(protocolOptions: protocol.FormatCodeSettings): FormatCodeSettings;
    function convertCompilerOptions(protocolOptions: protocol.ExternalProjectCompilerOptions): CompilerOptions & protocol.CompileOnSaveMixin;
    function tryConvertScriptKindName(scriptKindName: protocol.ScriptKindName | ScriptKind): ScriptKind;
    function convertScriptKindName(scriptKindName: protocol.ScriptKindName): ScriptKind.Unknown | ScriptKind.JS | ScriptKind.JSX | ScriptKind.TS | ScriptKind.TSX;
    /**
     * This helper function processes a list of projects and return the concatenated, sortd and deduplicated output of processing each project.
     */
    function combineProjectOutput<T>(projects: Project[], action: (project: Project) => T[], comparer?: (a: T, b: T) => number, areEqual?: (a: T, b: T) => boolean): T[];
    interface HostConfiguration {
        formatCodeOptions: FormatCodeSettings;
        hostInfo: string;
        extraFileExtensions?: JsFileExtensionInfo[];
    }
    interface OpenConfiguredProjectResult {
        configFileName?: NormalizedPath;
        configFileErrors?: Diagnostic[];
    }
    class ProjectService {
        readonly host: ServerHost;
        readonly logger: Logger;
        readonly cancellationToken: HostCancellationToken;
        readonly useSingleInferredProject: boolean;
        readonly typingsInstaller: ITypingsInstaller;
        private readonly eventHandler;
        readonly typingsCache: TypingsCache;
        private readonly documentRegistry;
        /**
         * Container of all known scripts
         */
        private readonly filenameToScriptInfo;
        /**
         * maps external project file name to list of config files that were the part of this project
         */
        private readonly externalProjectToConfiguredProjectMap;
        /**
         * external projects (configuration and list of root files is not controlled by tsserver)
         */
        readonly externalProjects: ExternalProject[];
        /**
         * projects built from openFileRoots
         **/
        readonly inferredProjects: InferredProject[];
        /**
         * projects specified by a tsconfig.json file
         **/
        readonly configuredProjects: ConfiguredProject[];
        /**
         * list of open files
         */
        readonly openFiles: ScriptInfo[];
        private compilerOptionsForInferredProjects;
        private compileOnSaveForInferredProjects;
        private readonly directoryWatchers;
        private readonly throttledOperations;
        private readonly hostConfiguration;
        private changedFiles;
        readonly toCanonicalFileName: (f: string) => string;
        lastDeletedFile: ScriptInfo;
        constructor(host: ServerHost, logger: Logger, cancellationToken: HostCancellationToken, useSingleInferredProject: boolean, typingsInstaller?: ITypingsInstaller, eventHandler?: ProjectServiceEventHandler);
        ensureInferredProjectsUpToDate_TestOnly(): void;
        getCompilerOptionsForInferredProjects(): CompilerOptions;
        onUpdateLanguageServiceStateForProject(project: Project, languageServiceEnabled: boolean): void;
        updateTypingsForProject(response: SetTypings | InvalidateCachedTypings): void;
        setCompilerOptionsForInferredProjects(projectCompilerOptions: protocol.ExternalProjectCompilerOptions): void;
        stopWatchingDirectory(directory: string): void;
        findProject(projectName: string): Project;
        getDefaultProjectForFile(fileName: NormalizedPath, refreshInferredProjects: boolean): Project;
        private ensureInferredProjectsUpToDate();
        private findContainingExternalProject(fileName);
        getFormatCodeOptions(file?: NormalizedPath): FormatCodeSettings;
        private updateProjectGraphs(projects);
        private onSourceFileChanged(fileName);
        private handleDeletedFile(info);
        private onTypeRootFileChanged(project, fileName);
        /**
         * This is the callback function when a watched directory has added or removed source code files.
         * @param project the project that associates with this directory watcher
         * @param fileName the absolute file name that changed in watched directory
         */
        private onSourceFileInDirectoryChangedForConfiguredProject(project, fileName);
        private handleChangeInSourceFileForConfiguredProject(project, triggerFile);
        private onConfigChangedForConfiguredProject(project);
        /**
         * This is the callback function when a watched directory has an added tsconfig file.
         */
        private onConfigFileAddedForInferredProject(fileName);
        private getCanonicalFileName(fileName);
        private removeProject(project);
        private assignScriptInfoToInferredProjectIfNecessary(info, addToListOfOpenFiles);
        /**
          * Remove this file from the set of open, non-configured files.
          * @param info The file that has been closed or newly configured
          */
        private closeOpenFile(info);
        /**
         * This function tries to search for a tsconfig.json for the given file. If we found it,
         * we first detect if there is already a configured project created for it: if so, we re-read
         * the tsconfig file content and update the project; otherwise we create a new one.
         */
        private openOrUpdateConfiguredProjectForFile(fileName);
        private findConfigFile(searchPath);
        private printProjects();
        private findConfiguredProjectByProjectName(configFileName);
        private findExternalProjectByProjectName(projectFileName);
        private convertConfigFileContentToProjectOptions(configFilename);
        private exceededTotalSizeLimitForNonTsFiles<T>(options, fileNames, propertyReader);
        private createAndAddExternalProject(projectFileName, files, options, typeAcquisition);
        private reportConfigFileDiagnostics(configFileName, diagnostics, triggerFile);
        private createAndAddConfiguredProject(configFileName, projectOptions, configFileErrors, clientFileName?);
        private watchConfigDirectoryForProject(project, options);
        private addFilesToProjectAndUpdateGraph<T>(project, files, propertyReader, clientFileName, typeAcquisition, configFileErrors);
        private openConfigFile(configFileName, clientFileName?);
        private updateNonInferredProject<T>(project, newUncheckedFiles, propertyReader, newOptions, newTypeAcquisition, compileOnSave, configFileErrors);
        private updateConfiguredProject(project);
        createInferredProjectWithRootFileIfNecessary(root: ScriptInfo): InferredProject;
        /**
         * @param uncheckedFileName is absolute pathname
         * @param fileContent is a known version of the file content that is more up to date than the one on disk
         */
        getOrCreateScriptInfo(uncheckedFileName: string, openedByClient: boolean, fileContent?: string, scriptKind?: ScriptKind): ScriptInfo;
        getScriptInfo(uncheckedFileName: string): ScriptInfo;
        getOrCreateScriptInfoForNormalizedPath(fileName: NormalizedPath, openedByClient: boolean, fileContent?: string, scriptKind?: ScriptKind, hasMixedContent?: boolean): ScriptInfo;
        getScriptInfoForNormalizedPath(fileName: NormalizedPath): ScriptInfo;
        getScriptInfoForPath(fileName: Path): ScriptInfo;
        setHostConfiguration(args: protocol.ConfigureRequestArguments): void;
        closeLog(): void;
        /**
         * This function rebuilds the project for every file opened by the client
         */
        reloadProjects(): void;
        /**
         * This function is to update the project structure for every projects.
         * It is called on the premise that all the configured projects are
         * up to date.
         */
        refreshInferredProjects(): void;
        /**
         * Open file whose contents is managed by the client
         * @param filename is absolute pathname
         * @param fileContent is a known version of the file content that is more up to date than the one on disk
         */
        openClientFile(fileName: string, fileContent?: string, scriptKind?: ScriptKind): OpenConfiguredProjectResult;
        openClientFileWithNormalizedPath(fileName: NormalizedPath, fileContent?: string, scriptKind?: ScriptKind, hasMixedContent?: boolean): OpenConfiguredProjectResult;
        /**
         * Close file whose contents is managed by the client
         * @param filename is absolute pathname
         */
        closeClientFile(uncheckedFileName: string): void;
        private collectChanges(lastKnownProjectVersions, currentProjects, result);
        private closeConfiguredProject(configFile);
        closeExternalProject(uncheckedFileName: string, suppressRefresh?: boolean): void;
        openExternalProjects(projects: protocol.ExternalProject[]): void;
        openExternalProject(proj: protocol.ExternalProject, suppressRefreshOfInferredProjects?: boolean): void;
    }
}

export = ts;
export as namespace ts;