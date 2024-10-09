using System;
using System.Collections.Generic;
using System.Text;
using Nom.Parser;
using System.Linq;
using Nom.TypeChecker.TransformExtensions;
using Nom.Language;
using System.Collections;
using Nom.TypeChecker.StdLib;

namespace Nom.TypeChecker
{
    class CodeTransformer : Parser.IStmtVisitor<ICodeTransformEnvironment, IStmtTransformResult>, Parser.IExprVisitor<ICodeTransformEnvironment, IExprTransformResult>
    {
        public static CodeTransformer Instance { get; } = new CodeTransformer();
        public Func<ExprStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitExprStmt => (stmt, env) =>
        {
            return new StmtTransformResult(stmt.Expression.Visit(this, env), env, false, false, false);
        };

        public Func<DeclStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitDeclStmt => (stmt, env) =>
        {
            IVariableReference varref = env.AddLocalVariable(stmt.Var, stmt.Type.TransformType(env.Context));
            if (stmt.Expr != null)
            {
                IExprTransformResult expResult = stmt.Expr.Visit(this, env);
                if (expResult.Type.IsSubtypeOf(varref.Type, true))
                {
                    return new StmtTransformResult(varref.GenerateWriteAccess(expResult, env), env, false, false, false);
                }
                throw new TypeCheckException("$0 is not a subtype of $1 (@2)", expResult.Type, varref.Type, stmt.Expr.Locs);
            }
            else
            {
                return new StmtTransformResult(new List<IInstruction>(), env);
            }
        };

        public Func<ErrorStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitErrorStmt => (stmt, env) =>
        {
            IExprTransformResult etr = stmt.Expr.Visit(this, env);
            return new StmtTransformResult(etr.Snoc(new ErrorInstruction(etr.Register)), env, false, true, true);
        };

        public Func<ContinueStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitContinueStmt => (stmt, env) =>
        {
            return new StmtTransformResult(new BranchInstruction(env.GetContinuePHI(stmt.Depth), env).Singleton(), env, false, false, false, true, true);
        };

        public Func<ForeachStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitForeachStmt => (stmt, env) =>
        {
            IExprTransformResult rangeOrig = stmt.Range.Visit(this, env);
            IOptional<INamedType> enumerableInstantiation = rangeOrig.Type.Visit(new Language.TypeVisitor<object, IOptional<INamedType>>()
            {
                VisitClassType = (ct, o) => ct.Instantiate(StdLib.IEnumerableInterface.Instance),
                VisitInterfaceType = (it, o) => it.Instantiate(StdLib.IEnumerableInterface.Instance),
                DefaultAction = (t, o) => Optional<INamedType>.Empty
            });

            IExprTransformResult rangeEtr;
            var variableType = stmt.Var.Type.TransformType(env.Context);
            if (enumerableInstantiation.HasElem && enumerableInstantiation.Elem.Arguments.ElementAt(0).AsType.IsSubtypeOf(variableType, false))
            {
                rangeEtr = rangeOrig;
            }
            else
            {
                rangeEtr = rangeOrig;
            }
            var getenum = env.GetMethod(rangeEtr, new AArgIdentifier<Identifier, Language.IType>(new Identifier("GetEnumerator", stmt.Range.Locs), new List<Language.IType>()), new List<Language.IType>()).Elem;
            IExprTransformResult enumerator = getenum.GenerateCall(new List<IExprTransformResult>(), env);
            var enumeratorRef = new ExprTransformResultReference(enumerator);

            LoopOutPhiNode outNode = new LoopOutPhiNode(env);
            LoopPhiNode phi = new LoopPhiNode(outNode.LoopEnv);
            BranchInstruction enter = new BranchInstruction(phi, env);
            var moveNext = phi.GetMethod(enumeratorRef, new AArgIdentifier<Identifier, Language.IType>(new Identifier("MoveNext", stmt.Range.Locs), new List<Language.IType>()), new List<Language.IType>()).Elem;
            IExprTransformResult gexpr = moveNext.GenerateCall(new List<IExprTransformResult>(), phi);
            gexpr = gexpr.EnsureType(StdLib.StdLib.BoolType, phi);
            TargetPhiNode loopNode = new TargetPhiNode();
            phi.RegisterTargets(outNode, phi);
            CondBranchInstruction cbi = new CondBranchInstruction(gexpr.Register, loopNode, outNode, phi);

            IVariableReference varref = loopNode.AddLocalVariable(stmt.Var.Name, stmt.Var.Type.TransformType(loopNode.Context));
            var current = loopNode.GetMethod(enumeratorRef, new AArgIdentifier<Identifier, Language.IType>(new Identifier("Current", stmt.Range.Locs), new List<Language.IType>()), new List<Language.IType>()).Elem;
            IExprTransformResult currentExpr = current.GenerateCall(new List<IExprTransformResult>(), loopNode).EnsureType(variableType, loopNode);
            StmtTransformResult currentWrite = new StmtTransformResult(varref.GenerateWriteAccess(currentExpr, loopNode), loopNode, false, false, false);

            IStmtTransformResult str = stmt.Block.Visit(this, loopNode);
            if (!str.AlwaysGoesElsewhere)
            {
                str.AddBranch(phi, loopNode);
            }
            phi.FinalizeMerges();
            outNode.FinalizeMerges();
            env.MergeIn(outNode);
            return new StmtTransformResult(enumerator.Snoc(enter).Snoc(phi).Concat(gexpr).Snoc(cbi).Snoc(loopNode).Concat(currentWrite).Concat(str).Snoc(outNode), env, false, false, false);
        };

        public Func<DebugStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitDebugStmt => (stmt, env) =>
        {
            return new StmtTransformResult(new List<IInstruction> { new DebugInstruction(stmt.Message) }, env);
        };

        public Func<ReturnStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitReturnStmt => (stmt, env) =>
        {
            IExprTransformResult etr = stmt.Expr.Visit(this, env);
            if (!env.ExpectedReturnType.HasElem)
            {
                throw new TypeCheckException("Return statement encountered in context that does not allow returns (@0)", stmt.Locs);
            }
            if (!etr.Type.IsSubtypeOf(env.ExpectedReturnType.Elem, true))
            {
                throw new TypeCheckException("Expression at @0 does not have expected return type " + env.ExpectedReturnType.ToString(), stmt.Expr.Locs);
            }
            etr = etr.EnsureType(env.ExpectedReturnType.Elem, env);
            env.BestReturnType = etr.Type; //Assignment here actually implemented as join
            return new StmtTransformResult(etr.Snoc(new ReturnInstruction(etr.Register)), env, true, false, true);
        };

        public Func<IfTypeStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitIfTypeStmt => throw new NotImplementedException();

        public Func<IfStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitIfStmt => (stmt, env) =>
        {
            MergePhiNode mergephi = new MergePhiNode(env);

            List<IInstruction> instructions = new List<IInstruction>();

            bool alwaysReturns = true;
            bool alwaysThrows = true;
            bool alwaysThrowsOrReturns = true;
            bool alwaysJumps = true;
            bool alwaysGoesElsewhere = true;

            ScopeTransformEnvironment ste = new ScopeTransformEnvironment(mergephi);
            IExprTransformResult guard = stmt.GuardExpression.Visit(this, ste).EnsureType(StdLib.StdLib.BoolType, ste);
            instructions.AddRange(guard);
            TargetPhiNode thenBlock = new TargetPhiNode();
            TargetPhiNode nextBlock = new TargetPhiNode();
            CondBranchInstruction cbi = new CondBranchInstruction(guard.Register, thenBlock, nextBlock, ste);
            instructions.Add(cbi);
            instructions.Add(thenBlock);
            IStmtTransformResult bte = stmt.ThenBlock.Visit(this, thenBlock);
            if (!bte.AlwaysGoesElsewhere)
            {
                bte.AddBranch(mergephi, thenBlock);
            }
            instructions.AddRange(bte);
            alwaysReturns = alwaysReturns && bte.IsReturning;
            alwaysThrows = alwaysThrows && bte.IsThrowing;
            alwaysJumps = alwaysJumps && bte.IsJumping;
            alwaysGoesElsewhere = alwaysGoesElsewhere && bte.AlwaysGoesElsewhere;
            alwaysThrowsOrReturns = alwaysThrowsOrReturns && bte.AlwaysThrowsOrReturns;

            foreach (var eip in stmt.ElseIfs)
            {
                PhiNode curBlock = nextBlock;
                instructions.Add(curBlock);
                guard = eip.Item1.Visit(this, nextBlock).EnsureType(StdLib.StdLib.BoolType, curBlock);
                thenBlock = new TargetPhiNode();
                nextBlock = new TargetPhiNode();
                cbi = new CondBranchInstruction(guard.Register, thenBlock, nextBlock, curBlock);
                instructions.Add(cbi);
                instructions.Add(thenBlock);
                bte = eip.Item2.Visit(this, thenBlock);
                if (!bte.AlwaysGoesElsewhere)
                {
                    bte.AddBranch(mergephi, thenBlock);
                }
                instructions.AddRange(bte);
                alwaysReturns = alwaysReturns && bte.IsReturning;
                alwaysThrows = alwaysThrows && bte.IsThrowing;
                alwaysJumps = alwaysJumps && bte.IsJumping;
                alwaysGoesElsewhere = alwaysGoesElsewhere && bte.AlwaysGoesElsewhere;
                alwaysThrowsOrReturns = alwaysThrowsOrReturns && bte.AlwaysThrowsOrReturns;
            }

            instructions.Add(nextBlock);
            if (stmt.ElseBlock.HasElem)
            {
                bte = stmt.ElseBlock.Elem.Visit(this, nextBlock);
                if (!bte.AlwaysGoesElsewhere)
                {
                    bte.AddBranch(mergephi, nextBlock);
                }
                alwaysReturns = alwaysReturns && bte.IsReturning;
                alwaysThrows = alwaysThrows && bte.IsThrowing;
                alwaysJumps = alwaysJumps && bte.IsJumping;
                alwaysGoesElsewhere = alwaysGoesElsewhere && bte.AlwaysGoesElsewhere;
                alwaysThrowsOrReturns = alwaysThrowsOrReturns && bte.AlwaysThrowsOrReturns;
                instructions.AddRange(bte);
            }
            else
            {
                alwaysReturns = false;
                alwaysThrows = false;
                alwaysThrowsOrReturns = false;
                alwaysJumps = false;
                alwaysGoesElsewhere = false;
                instructions.Add(new BranchInstruction(mergephi, nextBlock));
            }
            mergephi.FinalizeMerges();
            if (!alwaysThrowsOrReturns)
            {
                instructions.Add(mergephi);
            }
            env.MergeIn(mergephi);

            return new StmtTransformResult(instructions, mergephi, alwaysReturns, alwaysThrows, alwaysThrowsOrReturns, alwaysJumps, alwaysGoesElsewhere);


        };

        public Func<AssignStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitAssignStmt => (stmt, env) =>
        {
            IExprTransformResult exprtr = stmt.Expression.Visit(this, env);
            AccessorTransformEnvironment<IVariableReference> ate = new AccessorTransformEnvironment<IVariableReference>(env,
                etr =>
                {
                    IOptional<IVariableReference> ret = env.GetFieldAccessVariable(etr, stmt.Variable);
                    if (!ret.HasElem)
                    {
                        throw new TypeCheckException("Field $0 does not exist", stmt.Variable);
                    }
                    return ret.Elem;
                },
                qn =>
                {
                    IOptional<IVariableReference> ret = env.Context.GetChild(qn).Visit(new ParamRefVisitor<object, IOptional<IVariableReference>, Language.IType>(
                        (elem, arg) => { throw new TypeCheckException("$0 is not a class", qn); },
                        (elem, arg) => { throw new TypeCheckException("$0 is not a class", qn); },
                        (elem, arg) => { return env.GetStaticFieldAccessVariable(elem, stmt.Variable); }));
                    if (!ret.HasElem)
                    {
                        throw new TypeCheckException("$0 does not contain a static field named $1", qn, stmt.Variable);
                    }
                    return ret.Elem;
                },
                etr=>
                {
                    IOptional<IVariableReference> ret = env[stmt.Variable];
                    if (!ret.HasElem)
                    {
                        throw new TypeCheckException("Field or local variable $0 does not exist", stmt.Variable);
                    }
                    return ret.Elem;
                });
            IVariableReference varref;
            if (stmt.Accessor.HasElem)
            {
                varref = stmt.Accessor.Elem.Visit(AccessorVisitor<IVariableReference>.Instance, ate);
            }
            else
            {
                IOptional<IVariableReference> localvar = env[stmt.Variable];
                if (!localvar.HasElem)
                {
                    throw new TypeCheckException("No local accessible variable named $0", stmt.Variable);
                }
                varref = localvar.Elem;
            }
            return varref.GenerateWriteAccess(exprtr, env);
        };


        public Func<Block, ICodeTransformEnvironment, IStmtTransformResult> VisitBlock => (stmt, env) =>
        {
            List<IInstruction> instructions = new List<IInstruction>();
            bool isThrowing = false;
            bool isReturning = false;
            bool isAlwaysReturningOrThrowing = false;
            bool isJumping = false;
            bool isAlwaysGoingElsewhere = false;
            foreach (IStmt istmt in stmt)
            {
                if (isThrowing || isReturning || isAlwaysReturningOrThrowing)
                {
                    CompilerOutput.Warn("Inaccessible statements at @0", istmt.Start);
                    break;
                }
                IStmtTransformResult str = istmt.Visit(this, env);
                instructions.AddRange(str);
                isThrowing = str.IsThrowing;
                isReturning = str.IsReturning;
                isAlwaysReturningOrThrowing = str.AlwaysThrowsOrReturns;
                isJumping = str.IsJumping;
                isAlwaysGoingElsewhere = str.AlwaysGoesElsewhere;
            }
            return new StmtTransformResult(instructions, env, isReturning, isThrowing, isAlwaysReturningOrThrowing, isJumping, isAlwaysGoingElsewhere);
        };

        public Func<BreakStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitBreakStmt => (stmt, env) =>
        {
            return new StmtTransformResult(new BranchInstruction(env.GetBreakPHI(stmt.Depth), env).Singleton(), env, false, false, false, true, true);
        };

        public Func<WhileStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitWhileStmt => (stmt, env) =>
        {

            LoopOutPhiNode outNode = new LoopOutPhiNode(env);
            LoopPhiNode phi = new LoopPhiNode(outNode.LoopEnv);
            BranchInstruction enter = new BranchInstruction(phi, env);
            IExprTransformResult gexpr = stmt.Guard.Visit(this, phi);
            gexpr = gexpr.EnsureType(StdLib.StdLib.BoolType, phi);
            TargetPhiNode loopNode = new TargetPhiNode();
            phi.RegisterTargets(outNode, phi);
            CondBranchInstruction cbi = new CondBranchInstruction(gexpr.Register, loopNode, outNode, phi);
            IStmtTransformResult str = stmt.Block.Visit(this, loopNode);
            if (!str.AlwaysGoesElsewhere)
            {
                str.AddBranch(phi, loopNode);
            }
            phi.FinalizeMerges();
            outNode.FinalizeMerges();
            env.MergeIn(outNode);
            return new StmtTransformResult(enter.Singleton<IInstruction>().Snoc(phi).Concat(gexpr).Snoc(cbi).Snoc(loopNode).Concat(str).Snoc(outNode), env, false, false, false);
        };

        public Func<SpecialStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitSpecialStmt => throw new NotImplementedException();

        public Func<BoolExpr, ICodeTransformEnvironment, IExprTransformResult> VisitBoolExpr => (expr, env) =>
        {
            expr.TypeAnnotation = Bool.Instance.ClassType.GetParserType();
            if (expr.Value.ToLower() == "true")
            {
                BoolTrueInstruction bti = new BoolTrueInstruction(env.CreateRegister());
                return new ExprTransformResult(StdLib.Bool.Instance.ClassType, bti.Register, bti.Singleton());
            }
            BoolFalseInstruction bfi = new BoolFalseInstruction(env.CreateRegister());
            return new ExprTransformResult(StdLib.Bool.Instance.ClassType, bfi.Register, bfi.Singleton());
        };

        public Func<BinOpExpr, ICodeTransformEnvironment, IExprTransformResult> VisitBinOpExpr => (expr, env) =>
        {
            IExprTransformResult left = expr.Left.Visit(this, env);
            IExprTransformResult right = expr.Right.Visit(this, env);
            bool leftintopt = left.Type.IsSubtypeOf(StdLib.Int.Instance.ClassType, true);
            bool leftfloatopt = left.Type.IsSubtypeOf(StdLib.Float.Instance.ClassType, true);
            if (leftintopt || leftfloatopt)
            {
                bool rightintopt = right.Type.IsSubtypeOf(StdLib.Int.Instance.ClassType, true);
                bool rightfloatopt = right.Type.IsSubtypeOf(StdLib.Float.Instance.ClassType, true);
                if (rightintopt || rightfloatopt)
                {
                    bool leftint = left.Type.IsSubtypeOf(StdLib.Int.Instance.ClassType, false);
                    bool rightint = right.Type.IsSubtypeOf(StdLib.Int.Instance.ClassType, false);
                    bool leftfloat = left.Type.IsSubtypeOf(StdLib.Float.Instance.ClassType, false);
                    bool rightfloat = right.Type.IsSubtypeOf(StdLib.Float.Instance.ClassType, false);
                    switch (expr.Operator.Operator)
                    {
                        case BinaryOperator.Add:
                        case BinaryOperator.Divide:
                        case BinaryOperator.Subtract:
                        case BinaryOperator.Multiply:
                        case BinaryOperator.Mod:
                        case BinaryOperator.Power:
                            {
                                BinOpInstruction boi = new BinOpInstruction(left.Register, right.Register, expr.Operator.Operator, env.CreateRegister());
                                Language.IType returnType = new Language.DynamicType();
                                if (leftint && rightint)
                                {
                                    returnType = StdLib.Int.Instance.ClassType;
                                }
                                else if ((leftfloat && (rightint || rightfloat)) || (rightfloat && (leftint || rightint)))
                                {
                                    returnType = StdLib.Float.Instance.ClassType;
                                }
                                expr.TypeAnnotation = returnType.GetParserType();
                                return new ExprTransformResult(returnType, boi.Register, left.Concat(right).Snoc(boi));
                            }
                        case BinaryOperator.ShiftLeft:
                        case BinaryOperator.ShiftRight:
                        case BinaryOperator.BitAND:
                        case BinaryOperator.BitOR:
                        case BinaryOperator.BitXOR:
                            {
                                if (leftfloat || rightfloat)
                                {
                                    throw new TypeCheckException(expr.Operator.OpName + " is not an operation on Floats! (@0)", expr.Operator.Locs);
                                }
                                if (leftintopt && rightintopt)
                                {
                                    BinOpInstruction boi = new BinOpInstruction(left.Register, right.Register, expr.Operator.Operator, env.CreateRegister());
                                    Language.IType returnType = new Language.DynamicType();
                                    if (leftint && rightint)
                                    {
                                        returnType = StdLib.Int.Instance.ClassType;
                                    }
                                    expr.TypeAnnotation = returnType.GetParserType();
                                    return new ExprTransformResult(returnType, boi.Register, left.Concat(right).Snoc(boi));
                                }
                                break;
                            }
                        case BinaryOperator.Equals:
                        case BinaryOperator.GreaterOrEqualTo:
                        case BinaryOperator.GreaterThan:
                        case BinaryOperator.LessOrEqualTo:
                        case BinaryOperator.LessThan:
                        case BinaryOperator.RefEquals:
                            if ((leftintopt && rightintopt) || (leftfloatopt && rightfloatopt))
                            {
                                BinOpInstruction boi = new BinOpInstruction(left.Register, right.Register, expr.Operator.Operator, env.CreateRegister());
                                Language.IType returnType = new Language.DynamicType();
                                if (leftint && rightint)
                                {
                                    returnType = StdLib.Bool.Instance.ClassType;
                                }
                                if (leftfloat && rightfloat)
                                {
                                    returnType = StdLib.Bool.Instance.ClassType;
                                }
                                expr.TypeAnnotation = returnType.GetParserType();
                                return new ExprTransformResult(returnType, boi.Register, left.Concat(right).Snoc(boi));
                            }
                            break;
                        default:

                            break;
                    }
                }
            }
            if (left.Type.IsSubtypeOf(StdLib.Bool.Instance.ClassType, true) && right.Type.IsSubtypeOf(StdLib.Bool.Instance.ClassType, true))
            {
                switch (expr.Operator.Operator)
                {
                    case BinaryOperator.Equals:
                    case BinaryOperator.GreaterOrEqualTo:
                    case BinaryOperator.GreaterThan:
                    case BinaryOperator.LessOrEqualTo:
                    case BinaryOperator.LessThan:
                    case BinaryOperator.RefEquals:
                    case BinaryOperator.And:
                    case BinaryOperator.Or:
                        BinOpInstruction boi = new BinOpInstruction(left.Register, right.Register, expr.Operator.Operator, env.CreateRegister());
                        expr.TypeAnnotation = StdLib.Bool.Instance.ClassType.GetParserType();
                        return new ExprTransformResult(StdLib.Bool.Instance.ClassType, boi.Register, left.Concat(right).Snoc(boi));
                    default:
                        break;
                }
            }
            if (expr.Operator.Operator == BinaryOperator.RefEquals)
            {
                BinOpInstruction boi = new BinOpInstruction(left.Register, right.Register, expr.Operator.Operator, env.CreateRegister());
                expr.TypeAnnotation = StdLib.Bool.Instance.ClassType.GetParserType();
                return new ExprTransformResult(StdLib.Bool.Instance.ClassType, boi.Register, left.Concat(right).Snoc(boi));
            }
            var cexp = new CallExpr(new RefIdentExpr(expr.Left, new RefIdentifier(expr.Operator.OpName, new List<Parser.IType>(), expr.Operator.Locs)), expr.Right.Singleton(), expr.Locs).Visit(this, env);
            expr.TypeAnnotation = cexp.Type.GetParserType();
            return cexp;
        };

        public Func<IdentExpr, ICodeTransformEnvironment, IExprTransformResult> VisitIdentExpr => (expr, env) =>
        {
            IOptional<IVariableReference> varref = env[expr.Name];
            if (varref.HasElem)
            {
                expr.Annotation = new IdentExprAnnotation(varref.Elem.Identifier.Name);
                expr.TypeAnnotation = varref.Elem.Type.GetParserType();
                return varref.Elem.GenerateReadAccess(env);
            }
            throw new TypeCheckException("$0 is not an accessible member or variable", expr.Name);
        };

        public Func<CallExpr, ICodeTransformEnvironment, IExprTransformResult> VisitCallExpr => (expr, env) =>
        {
            List<IExprTransformResult> argResults = new List<IExprTransformResult>();
            foreach (IExpr arg in expr.Args)
            {
                try
                {
                    argResults.Add(arg.Visit(this, env));
                }
                catch (ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                    argResults.Add(new ExprTransformErrorResult());
                }
            }
            var ctr = expr.Receiver.Visit(CallReceiverVisitor.Instance, new CallReceiverTransformEnvironment(env, argResults));
            expr.TypeAnnotation = ctr.Type.GetParserType();
            return ctr;
        };

        public Func<NewExpr, ICodeTransformEnvironment, IExprTransformResult> VisitNewExpr => (expr, env) =>
        {
            List<IExprTransformResult> argResults = new List<IExprTransformResult>();
            foreach (IExpr argExp in expr.Args)
            {
                try
                {
                    argResults.Add(argExp.Visit(this, env));
                }
                catch (ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                    argResults.Add(new ExprTransformErrorResult());
                }
            }
            IParamRef<IClassSpec, Language.IType> classref = env.Context.GetChild(expr.NewCall.Type.Transform(i => i.Transform(n => n.Name, t => t.TransformType(env.Context)))).Visit(new ParamRefVisitor<object, IParamRef<IClassSpec, Language.IType>, Language.IType>(
                (ns, arg) => throw new TypeCheckException("$0 is not a class", expr.NewCall.Type),
                (ifc, arg) => throw new TypeCheckException("$0 is not a class", expr.NewCall.Type),
                (cls, arg) => cls));
            IOptional<ICallableReference> callref = env.GetConstructor(classref, argResults.Select(ar => ar.Type));
            if (!callref.HasElem)
            {
                throw new TypeCheckException("$0 does not have a constructor that matches the given arguments", expr.NewCall.Type);
            }
            expr.Annotation = classref;
            var result = callref.Elem.GenerateCall(argResults, env);
            expr.TypeAnnotation = result.Type.GetParserType();
            return result;
        };

        public Func<FloatExpr, ICodeTransformEnvironment, IExprTransformResult> VisitFloatExpr => (expr, env) =>
            {
                try
                {
                    LoadFloatConstantInstruction lfc = new LoadFloatConstantInstruction(Double.Parse(expr.Value), env.CreateRegister());
                    expr.TypeAnnotation = StdLib.Float.Instance.ClassType.GetParserType();
                    return new ExprTransformResult(StdLib.Float.Instance.ClassType, lfc.Register, lfc.Singleton());
                }
                catch (FormatException e)
                {
                    throw new TypeCheckException("Ill-formatted float", e, expr.Locs);
                }
                catch (OverflowException e)
                {
                    throw new TypeCheckException("Overflowing float", e, expr.Locs);
                }
            };

        public Func<UnaryOpExpr, ICodeTransformEnvironment, IExprTransformResult> VisitUnaryOpExpr => (expr, env) =>
        {
            IExprTransformResult argResult = expr.Expr.Visit(this, env);
            bool argIntOpt = argResult.Type.IsSubtypeOf(StdLib.Int.Instance.ClassType, true);
            bool argInt = argResult.Type.IsSubtypeOf(StdLib.Int.Instance.ClassType, false);
            bool argBoolOpt = argResult.Type.IsSubtypeOf(StdLib.Bool.Instance.ClassType, true);
            bool argBool = argResult.Type.IsSubtypeOf(StdLib.Bool.Instance.ClassType, false);
            bool argFloatOpt = argResult.Type.IsSubtypeOf(StdLib.Float.Instance.ClassType, true);
            bool argFloat = argResult.Type.IsSubtypeOf(StdLib.Float.Instance.ClassType, false);
            if (argIntOpt || argBoolOpt || argFloatOpt)
            {
                switch (expr.Operator)
                {
                    case UnaryOperator.Negate:
                        if (argIntOpt || argFloatOpt)
                        {
                            Language.IType returnType = new Nom.Language.DynamicType();
                            if (argInt)
                            {
                                returnType = StdLib.Int.Instance.ClassType;
                            }
                            else if (argFloat)
                            {
                                returnType = StdLib.Float.Instance.ClassType;
                            }
                            var instr = new UnaryOpInstruction(argResult.Register, expr.Operator, env.CreateRegister());
                            expr.TypeAnnotation = returnType.GetParserType();
                            return new ExprTransformResult(returnType, instr.Register, argResult.Snoc(instr), expr.Locs);
                        }
                        else
                        {
                            throw new TypeCheckException("Only values of numeric types can be negated (%0)", expr);
                        }
                    case UnaryOperator.Not:
                        if (argBoolOpt)
                        {
                            var instr = new UnaryOpInstruction(argResult.Register, expr.Operator, env.CreateRegister());
                            expr.TypeAnnotation = StdLib.Bool.Instance.ClassType.GetParserType();
                            return new ExprTransformResult(StdLib.Bool.Instance.ClassType, instr.Register, argResult.Snoc(instr), expr.Locs);
                        }
                        else
                        {
                            throw new TypeCheckException("Logical not is only defined on booleans (%0)", expr);
                        }
                    default:
                        throw new InternalException("Unknown Unary operator!");
                }
            }
            else
            {
                throw new TypeCheckException("Unary operators only apply to integers, floats, and booleans (%0)", expr);
            }
        };

        public Func<RefIdentExpr, ICodeTransformEnvironment, IExprTransformResult> VisitRefIdentExpr => (expr, env) =>
        {
            var ret = expr.Accessor.Visit(AccessorVisitor<IExprTransformResult>.Instance, new AccessorTransformEnvironment<IExprTransformResult>(env,
                etr =>
                expr.Identifier.Arguments.Count() == 0 ? env.GetFieldAccessVariable(etr, expr.Identifier.Name).Elem.GenerateReadAccess(env) : throw new NotImplementedException(),
                qn => throw new NotImplementedException(),
                etr => expr.Identifier.Arguments.Count() == 0 ? env[expr.Identifier.Name].Elem.GenerateReadAccess(env) : throw new NotImplementedException()));
            expr.TypeAnnotation = ret.Type.GetParserType();
            return ret;
        };

        public Func<InstanceExpr, ICodeTransformEnvironment, IExprTransformResult> VisitInstanceExpr => throw new NotImplementedException();

        public Func<NullExpr, ICodeTransformEnvironment, IExprTransformResult> VisitNullExpr => (expr, env) =>
        {
            var ni = new NullInstruction(env.CreateRegister());
            var ret = new ExprTransformResult(StdLib.StdLib.NullType, ni.Register, ni.Singleton(), expr.Locs);
            expr.TypeAnnotation = ret.Type.GetParserType();
            return ret;
        };

        public Func<ListExpr, ICodeTransformEnvironment, IExprTransformResult> VisitListExpr => throw new NotImplementedException();

        public Func<DefaultListExpr, ICodeTransformEnvironment, IExprTransformResult> VisitDefaultListExpr => throw new NotImplementedException();

        public Func<StringExpr, ICodeTransformEnvironment, IExprTransformResult> VisitStringExpr => (expr, env) =>
        {
            var lsc = new LoadStringConstantInstruction(expr.Value, env.CreateRegister());
            var ret = new ExprTransformResult(StdLib.String.Instance.ClassType, lsc.Register, lsc.Singleton());
            expr.TypeAnnotation = ret.Type.GetParserType();
            return ret;
        };

        public Func<RangeExpr, ICodeTransformEnvironment, IExprTransformResult> VisitRangeExpr => (expr, env) =>
        {
            List<IExprTransformResult> argResults = new List<IExprTransformResult>();
            foreach (IExpr argExp in new List<IExpr>() { expr.RangeStart, expr.RangeEnd, expr.RangeStep })
            {
                try
                {
                    argResults.Add(argExp.Visit(this, env).EnsureType(StdLib.Int.Instance.ClassType, env));
                }
                catch (ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                    argResults.Add(new ExprTransformErrorResult());
                }
            }
            IParamRef<IClassSpec, Language.IType> classref = new ClassRef<Language.IType>(StdLib.Range.Instance, new TypeEnvironment<Language.IType>());
            IOptional<ICallableReference> callref = env.GetConstructor(classref, argResults.Select(ar => ar.Type));
            if (!callref.HasElem)
            {
                throw new InternalException("Range constructor not found!");
            }
            var ret = callref.Elem.GenerateCall(argResults, env);
            expr.TypeAnnotation = ret.Type.GetParserType();
            return ret;
        };

        public Func<IntExpr, ICodeTransformEnvironment, IExprTransformResult> VisitIntExpr => (expr, env) =>
        {
            try
            {
                LoadIntConstantInstruction lic = new LoadIntConstantInstruction(Int64.Parse(expr.Value), env.CreateRegister());
                expr.TypeAnnotation = StdLib.Int.Instance.ClassType.GetParserType();
                return new ExprTransformResult(StdLib.Int.Instance.ClassType, lic.Register, lic.Singleton());
            }
            catch (FormatException e)
            {
                throw new TypeCheckException("Ill-formatted integer", e, expr.Locs);
            }
            catch (OverflowException e)
            {
                throw new TypeCheckException("Overflowing integer", e, expr.Locs);
            }
        };

        public Func<CastExpr, ICodeTransformEnvironment, IExprTransformResult> VisitCastExpr => (expr, env) =>
        {
            Language.IType targetType = expr.Type.TransformType(env.Context);
            IExprTransformResult etr = expr.Expr.Visit(this, env).EnsureType(targetType, env);
            expr.TypeAnnotation = etr.Type.GetParserType();
            return etr;
        };

        public Func<RefIdentRootExpr, ICodeTransformEnvironment, IExprTransformResult> VisitRefIdentRootExpr => (expr, env) =>
        {
            throw new TypeCheckException("$1 cannot be a freestanding expression. It needs to either specify a method in a method call or a prefix to a qualified name", expr);
        };

        public Func<LambdaExpr, ICodeTransformEnvironment, IExprTransformResult> VisitLambdaExpr => (expr, env) =>
        {
            var lenv = new LambdaTransformEnvironment(env, expr.Arguments.Select(arg => new ArgumentDeclDef(arg.Name, arg.Type.TransformType(env.Context))), expr.ReturnType.TransformType(env.Context), expr.Locs);
            expr.Annotation = lenv.Lambda.LambdaID;
            var ret = lenv.Compile(expr.Code, expr.Locs);
            expr.TypeAnnotation = ret.Type.GetParserType();
            return ret;
        };

        public Func<StructExpr, ICodeTransformEnvironment, IExprTransformResult> VisitStructExpr => (expr, env) =>
        {
            TDStruct tdstruct = env.Context.Container.CreateStruct();
            expr.Annotation = tdstruct.StructID;
            List<IExprTransformResult> argExprs = new List<IExprTransformResult>();
            var cenv = new StructInitializerTransformEnvironment(env);
            foreach (var sfd in expr.Fields)
            {
                try
                {
                    IExprTransformResult etr = sfd.InitExpr.Visit(this, cenv);
                    Language.IType fieldType = sfd.Type.TransformType(cenv.Context);
                    if (!etr.Type.IsSubtypeOf(fieldType, true))
                    {
                        throw new TypeCheckException("@0 does not have type $1 even optimistically", sfd.Ident, fieldType);
                    }
                    tdstruct.AddField(new TDStructField(sfd.Ident, fieldType, tdstruct, false, etr));
                }
                catch (ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                }
            }
            int afterInitializerRegCount = cenv.RegisterCount;

            argExprs.AddRange(cenv.CapturedVariables.Select(cv => cv.GenerateReadAccess(env)));
            List<(Parser.MethodDef, StructMethodDef)> methodPairs = new List<(Parser.MethodDef, StructMethodDef)>();
            foreach (var md in expr.Methods)
            {
                ParametersSpec ps = new ParametersSpec(md.Args.Select(arg => new ArgumentDeclDef(arg.Name, arg.Type.TransformType(env.Context))));
                TypeParametersSpec tps = new TypeParametersSpec(md.Name.Arguments.Select((arg, i) => new TDTypeArgDeclDef(arg.Name, env.Context.Container.OverallTypeParameterCount + i,/* arg.Variance,*/ arg.UpperBound.TransformType(env.Context), arg.LowerBound.TransformType(env.Context))));
                StructMethodDef smd = new StructMethodDef(md.Name.Name, tps, ps, md.Returns.TransformType(env.Context), tdstruct);
                tdstruct.AddMethod(smd);
                methodPairs.Add((md, smd));
            }
            var senv = new StructTransformEnvironment(env, cenv, tdstruct);

            foreach (var mp in methodPairs)
            {
                try
                {
                    var menv = new StructMethodTransformEnvironment(senv, mp.Item2);
                    IStmtTransformResult str = mp.Item1.Code.Visit(CodeTransformer.Instance, menv);
                    if (!str.AlwaysGoesElsewhere)
                    {
                        if (StdLib.StdLib.VoidType.IsSubtypeOf(mp.Item2.ReturnType, false))
                        {
                            mp.Item2.Instructions = str.Snoc(new ReturnVoidInstruction());
                        }
                        else
                        {
                            throw new TypeCheckException("Not all paths in $0 return a value", mp.Item1.Name);
                        }
                    }
                    else
                    {
                        mp.Item2.Instructions = str;
                    }
                    int index = 0;
                    foreach (PhiNode pn in mp.Item2.Instructions.Where(x => x is PhiNode))
                    {
                        pn.Index = index++;
                    }
                    mp.Item2.RegisterCount = menv.RegisterCount;
                }
                catch (ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                }
            }
            tdstruct.SetInitializerArgs(cenv.CapturedVariables);
            tdstruct.InitializerRegisterCount = cenv.RegisterCount;
            tdstruct.EndArgRegisterCount = cenv.RegisterCount - afterInitializerRegCount;

            IEnumerable<IExprTransformResult> closureReadResults = senv.GetClosureAccessors().ToList();

            tdstruct.ClosureTypeParameters = new TypeParametersSpec(senv.StructContext.StructTypeParameters);
            var csi = new ConstructStructInstruction(tdstruct, senv.StructContext.ConstructorTypeArguments, argExprs.Select(ae => ae.Register).Concat(closureReadResults.Select(crr => crr.Register)), env.CreateRegister());

            var csetr = new ExprTransformResult(new Language.DynamicType(), csi.Register, new List<IInstruction>(), expr.Locs);

            List<IStmtTransformResult> saetrs = new List<IStmtTransformResult>();
            StructAssignmentTransformEnvironment satenv = new StructAssignmentTransformEnvironment(env, csetr);

            foreach (var sass in expr.StructAssignments)
            {
                var saetr = sass.Expression.Visit(this, satenv);
                saetrs.Add(env.GetFieldAccessVariable(csetr, sass.Variable).Elem.GenerateWriteAccess(saetr, satenv));
            }

            var ret = new ExprTransformResult(new Language.DynamicType(), csi.Register, argExprs.Flatten().Concat(closureReadResults.Flatten()).Snoc(csi).Concat(saetrs.Flatten()), expr.Locs);
            expr.TypeAnnotation = ret.Type.GetParserType();
            return ret;
        };

        public Func<RTCmdStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitRTCmdStmt => (stmt, env) =>
        {
            return new StmtTransformResult(new RuntimeCmdInstruction(stmt.Cmd).Singleton(), env);
        };

        public Func<DefaultInstanceExpr, ICodeTransformEnvironment, IExprTransformResult> VisitDefaultInstanceExpr => VisitInstanceExpr;

        public Func<DefaultBoolExpr, ICodeTransformEnvironment, IExprTransformResult> VisitDefaultBoolExpr => VisitBoolExpr;

        public Func<DefaultIdentExpr, ICodeTransformEnvironment, IExprTransformResult> VisitDefaultIdentExpr => VisitIdentExpr;

        public Func<DefaultFloatExpr, ICodeTransformEnvironment, IExprTransformResult> VisitDefaultFloatExpr => VisitFloatExpr;

        public Func<DefaultNullExpr, ICodeTransformEnvironment, IExprTransformResult> VisitDefaultNullExpr => VisitNullExpr;

        public Func<DefaultStringExpr, ICodeTransformEnvironment, IExprTransformResult> VisitDefaultStringExpr => VisitStringExpr;

        public Func<DefaultIntExpr, ICodeTransformEnvironment, IExprTransformResult> VisitDefaultIntExpr => VisitIntExpr;

        public Func<IfNullStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitIfNullStmt => (stmt, env) =>
        {
            MergePhiNode mergephi = new MergePhiNode(env);

            List<IInstruction> instructions = new List<IInstruction>();

            bool alwaysReturns = true;
            bool alwaysThrows = true;
            bool alwaysThrowsOrReturns = true;
            bool alwaysJumps = true;
            bool alwaysGoesElsewhere = true;

            ScopeTransformEnvironment ste = new ScopeTransformEnvironment(mergephi);
            var dvar = env.GetLocalVariable(stmt.Ident.Name);
            if (!dvar.HasElem)
            {
                throw new TypeCheckException("Local variable not found: %0", stmt.Ident);
            }
            if (dvar.Elem.Type.IsSubtypeOf(StdLib.StdLib.NullType, false))
            {
                CompilerOutput.Warn("Local variable %0 is always null, then-branch always taken", stmt.Ident);
            }
            if (dvar.Elem.Type.IsDisjoint(StdLib.StdLib.NullType))
            {
                CompilerOutput.Warn("Local variable %0 is never null, then-branch never taken", stmt.Ident);
            }
            NullInstruction ni = new NullInstruction(env.CreateRegister());
            IRegister guardRegister = env.CreateRegister();
            var guardinst = new BinOpInstruction(dvar.Elem.CurrentRegister, ni.Register, BinaryOperator.RefEquals, guardRegister);
            IExprTransformResult guard = new ExprTransformResult(StdLib.StdLib.BoolType, guardRegister, new List<IInstruction>() { ni, guardinst }, stmt.Ident.Locs);
            instructions.AddRange(guard);
            TargetPhiNode thenBlock = new TargetPhiNode();
            TargetPhiNode nextBlock = new TargetPhiNode();
            CondBranchInstruction cbi = new CondBranchInstruction(guard.Register, thenBlock, nextBlock, ste);
            instructions.Add(cbi);
            instructions.Add(thenBlock);
            thenBlock.AddLocalVariable(stmt.Ident, StdLib.StdLib.NullType);
            IStmtTransformResult bte = stmt.ThenBlock.Visit(this, thenBlock);
            if (!bte.AlwaysGoesElsewhere)
            {
                bte.AddBranch(mergephi, thenBlock);
            }
            instructions.AddRange(bte);
            alwaysReturns = alwaysReturns && bte.IsReturning;
            alwaysThrows = alwaysThrows && bte.IsThrowing;
            alwaysJumps = alwaysJumps && bte.IsJumping;
            alwaysGoesElsewhere = alwaysGoesElsewhere && bte.AlwaysGoesElsewhere;
            alwaysThrowsOrReturns = alwaysThrowsOrReturns && bte.AlwaysThrowsOrReturns;

            instructions.Add(nextBlock);
            if (stmt.ElseBlock.HasElem)
            {
                nextBlock.AddLocalVariable(stmt.Ident, dvar.Elem.Type.Visit(new Nom.Language.NoargTypeVisitor<Nom.Language.IType>(t => t)
                {
                    VisitMaybeType = (mb) => mb.PotentialType,
                    VisitProbablyType = (pb) => pb.PotentialType
                }));
                bte = stmt.ElseBlock.Elem.Visit(this, nextBlock);
                if (!bte.AlwaysGoesElsewhere)
                {
                    bte.AddBranch(mergephi, nextBlock);
                }
                alwaysReturns = alwaysReturns && bte.IsReturning;
                alwaysThrows = alwaysThrows && bte.IsThrowing;
                alwaysJumps = alwaysJumps && bte.IsJumping;
                alwaysGoesElsewhere = alwaysGoesElsewhere && bte.AlwaysGoesElsewhere;
                alwaysThrowsOrReturns = alwaysThrowsOrReturns && bte.AlwaysThrowsOrReturns;
                instructions.AddRange(bte);
            }
            else
            {
                alwaysReturns = false;
                alwaysThrows = false;
                alwaysThrowsOrReturns = false;
                alwaysJumps = false;
                alwaysGoesElsewhere = false;
                instructions.Add(new BranchInstruction(mergephi, nextBlock));
            }
            mergephi.FinalizeMerges();
            if (!alwaysThrowsOrReturns)
            {
                instructions.Add(mergephi);
            }
            env.MergeIn(mergephi);

            return new StmtTransformResult(instructions, mergephi, alwaysReturns, alwaysThrows, alwaysThrowsOrReturns, alwaysJumps, alwaysGoesElsewhere);

        };

        public Func<IfObjStmt, ICodeTransformEnvironment, IStmtTransformResult> VisitIfObjStmt => (stmt, env) =>
        {
            MergePhiNode mergephi = new MergePhiNode(env);

            List<IInstruction> instructions = new List<IInstruction>();

            bool alwaysReturns = true;
            bool alwaysThrows = true;
            bool alwaysThrowsOrReturns = true;
            bool alwaysJumps = true;
            bool alwaysGoesElsewhere = true;

            ScopeTransformEnvironment ste = new ScopeTransformEnvironment(mergephi);
            var dvar = env.GetLocalVariable(stmt.Ident.Name);
            if (!dvar.HasElem)
            {
                throw new TypeCheckException("Local variable not found: %0", stmt.Ident);
            }
            if (dvar.Elem.Type.IsSubtypeOf(StdLib.StdLib.NullType, false))
            {
                CompilerOutput.Warn("Local variable %0 is always null, then-branch always taken", stmt.Ident);
            }
            if (dvar.Elem.Type.IsDisjoint(StdLib.StdLib.NullType))
            {
                CompilerOutput.Warn("Local variable %0 is never null, then-branch never taken", stmt.Ident);
            }
            NullInstruction ni = new NullInstruction(env.CreateRegister());
            IRegister guardRegister = env.CreateRegister();
            var guardinst = new BinOpInstruction(dvar.Elem.CurrentRegister, ni.Register, BinaryOperator.RefEquals, guardRegister);
            IExprTransformResult guard = new ExprTransformResult(StdLib.StdLib.BoolType, guardRegister, new List<IInstruction>() { ni, guardinst }, stmt.Ident.Locs);
            instructions.AddRange(guard);
            TargetPhiNode thenBlock = new TargetPhiNode();
            TargetPhiNode nextBlock = new TargetPhiNode();
            CondBranchInstruction cbi = new CondBranchInstruction(guard.Register, thenBlock, nextBlock, ste);
            instructions.Add(cbi);
            instructions.Add(thenBlock);
            thenBlock.AddLocalVariable(stmt.Ident, dvar.Elem.Type.Visit(new Nom.Language.NoargTypeVisitor<Nom.Language.IType>(t => t)
            {
                VisitMaybeType = (mb) => mb.PotentialType,
                VisitProbablyType = (pb) => pb.PotentialType
            }));
            IStmtTransformResult bte = stmt.ThenBlock.Visit(this, thenBlock);
            if (!bte.AlwaysGoesElsewhere)
            {
                bte.AddBranch(mergephi, thenBlock);
            }
            instructions.AddRange(bte);
            alwaysReturns = alwaysReturns && bte.IsReturning;
            alwaysThrows = alwaysThrows && bte.IsThrowing;
            alwaysJumps = alwaysJumps && bte.IsJumping;
            alwaysGoesElsewhere = alwaysGoesElsewhere && bte.AlwaysGoesElsewhere;
            alwaysThrowsOrReturns = alwaysThrowsOrReturns && bte.AlwaysThrowsOrReturns;

            instructions.Add(nextBlock);
            if (stmt.ElseBlock.HasElem)
            {
                nextBlock.AddLocalVariable(stmt.Ident, StdLib.StdLib.NullType);
                bte = stmt.ElseBlock.Elem.Visit(this, nextBlock);
                if (!bte.AlwaysGoesElsewhere)
                {
                    bte.AddBranch(mergephi, nextBlock);
                }
                alwaysReturns = alwaysReturns && bte.IsReturning;
                alwaysThrows = alwaysThrows && bte.IsThrowing;
                alwaysJumps = alwaysJumps && bte.IsJumping;
                alwaysGoesElsewhere = alwaysGoesElsewhere && bte.AlwaysGoesElsewhere;
                alwaysThrowsOrReturns = alwaysThrowsOrReturns && bte.AlwaysThrowsOrReturns;
                instructions.AddRange(bte);
            }
            else
            {
                alwaysReturns = false;
                alwaysThrows = false;
                alwaysThrowsOrReturns = false;
                alwaysJumps = false;
                alwaysGoesElsewhere = false;
                instructions.Add(new BranchInstruction(mergephi, nextBlock));
            }
            mergephi.FinalizeMerges();
            if (!alwaysThrowsOrReturns)
            {
                instructions.Add(mergephi);
            }
            env.MergeIn(mergephi);

            return new StmtTransformResult(instructions, mergephi, alwaysReturns, alwaysThrows, alwaysThrowsOrReturns, alwaysJumps, alwaysGoesElsewhere);

        };

        public Func<LetExpr, ICodeTransformEnvironment, IExprTransformResult> VisitLetExpr => (expr, env) =>
            {
                var bindExpr = expr.Declaration.InitExpr.Visit(this, env);
                ScopeTransformEnvironment ste = new ScopeTransformEnvironment(env);
                var lvar = ste.AddLocalVariable(expr.Declaration.Ident, expr.Declaration.Type.TransformType(env.Context));
                IStmtTransformResult assignInstrs = lvar.GenerateWriteAccess(bindExpr, ste);
                var bodyExpr = expr.Body.Visit(this, ste);
                var ret = new ExprTransformResult(bodyExpr.Type, bodyExpr.Register, assignInstrs.Concat(bodyExpr), expr.Locs);
                expr.TypeAnnotation = ret.Type.GetParserType();
                return ret;
            };

        public Func<LetVarExpr, ICodeTransformEnvironment, IExprTransformResult> VisitLetVarExpr => (expr, env) =>
        {
            var bindExpr = expr.BindExpr.Visit(this, env);
            ScopeTransformEnvironment ste = new ScopeTransformEnvironment(env);
            var lvar = ste.AddLocalVariable(expr.Ident, bindExpr.Type);
            IStmtTransformResult assignInstrs = lvar.GenerateWriteAccess(bindExpr, ste);
            var bodyExpr = expr.Body.Visit(this, ste);
            var ret = new ExprTransformResult(bodyExpr.Type, bodyExpr.Register, assignInstrs.Concat(bodyExpr), expr.Locs);
            expr.TypeAnnotation = ret.Type.GetParserType();
            return ret;
        };

        public Func<CFunctionCallExpr, ICodeTransformEnvironment, IExprTransformResult> VisitCFunctionCallExpr => (expr, env) =>
        {
            List<Language.IType> typeArgs = expr.TypeArgs.Select(targ => targ.TransformType(env.Context)).ToList();
            List<TDTypeArgDeclDef> tdargDecls = new List<TDTypeArgDeclDef>();
            for (int i = 0; i < expr.Name.Arguments.Count(); i++)
            {
                tdargDecls.Add(new TDTypeArgDeclDef(expr.Name.Arguments.ElementAt(i).Name, i));
            }
            var newLookup = env.Context.PushVariables(tdargDecls);
            foreach (TDTypeArgDeclDef arg in tdargDecls)
            {
                var ub = expr.Name.Arguments.ElementAt(arg.Index).UpperBound.TransformType(env.Context);
                var lb = expr.Name.Arguments.ElementAt(arg.Index).LowerBound.TransformType(env.Context);
                arg.AdjustBounds(ub, lb);
                if (!typeArgs[arg.Index].IsSubtypeOf(ub))
                {
                    CompilerOutput.RegisterException(new TypeCheckException("Type argument %0 is not a subtype of upper bound %1", typeArgs[arg.Index], ub));
                }
                if (!typeArgs[arg.Index].IsSupertypeOf(lb))
                {
                    CompilerOutput.RegisterException(new TypeCheckException("Type argument %0 is not a supertype of lower bound %1", typeArgs[arg.Index], lb));
                }
            }

            List<IExprTransformResult> argResults = new List<IExprTransformResult>();
            var argTypes = expr.ArgTypes.Select(tp => tp.TransformType(env.Context)).ToList();
            var origArgTypes = argTypes.ToList();
            var retType = expr.ReturnType.TransformType(env.Context);
            var origRetType = retType.AsType;
            for (int j = 0; j < tdargDecls.Count; j++)
            {
                for (int i = 0; i < argTypes.Count; i++)
                {
                    argTypes[i] = ((ISubstitutable<Language.IType>)argTypes[i]).Substitute(tdargDecls[j], typeArgs[j]);
                }
                retType = ((ISubstitutable<Language.IType>)retType).Substitute(tdargDecls[j], typeArgs[j]);
            }

            int argpos = 0;                                                                                         
            foreach (IExpr arg in expr.Arguments)
            {
                try
                {
                    var argres = arg.Visit(this, env);
                    argResults.Add(argres);
                    if (!argres.Type.IsSubtypeOf(argTypes[argpos]))
                    {
                        CompilerOutput.RegisterException(new TypeCheckException("Expected " + argTypes[argpos].ToString() + ", but found " + argres.Type.ToString() + " at @0", argres));
                    }
                    argpos++;
                }
                catch (ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                    argResults.Add(new ExprTransformErrorResult());
                }
            }
            expr.TypeAnnotation = expr.ReturnType;
            expr.ReturnType.Annotation = retType;
            IRegister reg = env.CreateRegister(); 
            return new ExprTransformResult(retType, reg, argResults.Flatten().Snoc(new CallCFunctionInstruction(expr.SourceName, expr.Name.Name.Name, tdargDecls, typeArgs, origArgTypes, origRetType, argResults.Select(arg=>arg.Register), reg)));
        };

        private class CallReceiverTransformEnvironment : AUnscopedChildCodeTransformEnvironment
        {
            public IEnumerable<IExprTransformResult> Arguments { get; }

            public CallReceiverTransformEnvironment(ICodeTransformEnvironment env, IEnumerable<IExprTransformResult> argResults) : base(env)
            {
                Arguments = argResults;
            }

        }

        private class AccessorTransformEnvironment<T> : AUnscopedChildCodeTransformEnvironment
        {
            public AccessorTransformEnvironment(ICodeTransformEnvironment env, Func<IExprTransformResult, T> valueResultHandler, Func<IQName<IArgIdentifier<string, Language.IType>>, T> typeResultHandler, Func<IExprTransformResult, T> thisHandler) : base(env)
            {
                ValueResultHandler = valueResultHandler;
                TypeResultHandler = typeResultHandler;
                ThisHandler = thisHandler;
            }
            public Func<IExprTransformResult, T> ValueResultHandler { get; }
            public Func<IQName<IArgIdentifier<string, Language.IType>>, T> TypeResultHandler { get; }
            public Func<IExprTransformResult, T> ThisHandler { get; }

        }

        private class AccessorVisitor<T> : Parser.IExprVisitor<AccessorTransformEnvironment<T>, T>
        {
            public static AccessorVisitor<T> Instance { get; } = new AccessorVisitor<T>();
            public Func<BoolExpr, AccessorTransformEnvironment<T>, T> VisitBoolExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<BinOpExpr, AccessorTransformEnvironment<T>, T> VisitBinOpExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<CallExpr, AccessorTransformEnvironment<T>, T> VisitCallExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<NewExpr, AccessorTransformEnvironment<T>, T> VisitNewExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<FloatExpr, AccessorTransformEnvironment<T>, T> VisitFloatExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<UnaryOpExpr, AccessorTransformEnvironment<T>, T> VisitUnaryOpExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<InstanceExpr, AccessorTransformEnvironment<T>, T> VisitInstanceExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<NullExpr, AccessorTransformEnvironment<T>, T> VisitNullExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<ListExpr, AccessorTransformEnvironment<T>, T> VisitListExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<DefaultListExpr, AccessorTransformEnvironment<T>, T> VisitDefaultListExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<StringExpr, AccessorTransformEnvironment<T>, T> VisitStringExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<RangeExpr, AccessorTransformEnvironment<T>, T> VisitRangeExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<IntExpr, AccessorTransformEnvironment<T>, T> VisitIntExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<CastExpr, AccessorTransformEnvironment<T>, T> VisitCastExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<IdentExpr, AccessorTransformEnvironment<T>, T> VisitIdentExpr => (expr, env) =>
            {
                IOptional<IVariableReference> varref = env[expr.Name];
                if (varref.HasElem)
                {
                    if(expr.Name.Name=="this")
                    {
                        return env.ThisHandler(varref.Elem.GenerateReadAccess(env));
                    }
                    return env.ValueResultHandler(varref.Elem.GenerateReadAccess(env));
                }
                IArgIdentifier<string, Language.IType> argident = new RefIdentifier(expr.Name, new List<Parser.IType>(), expr.Locs).Transform(n => n.Name, t => t.TransformType(env.Context));
                IQName<IArgIdentifier<string, Language.IType>> qn = new AQName<IArgIdentifier<string, Language.IType>>(false, argident.Singleton().ToArray(), expr.Locs);
                if (env.Context.HasChild(qn))
                {
                    return env.TypeResultHandler(qn);
                }
                throw new TypeCheckException("$0 is not an accessible variable, field, static field, namespace, interface, or class.", expr.Name);
            };
            public Func<RefIdentExpr, AccessorTransformEnvironment<T>, T> VisitRefIdentExpr => (expr, env) =>
            {
                return expr.Accessor.Visit(this, new AccessorTransformEnvironment<T>(env,
                    ctr =>
                    {
                        if (expr.Identifier.Arguments.Count() > 0)
                        {
                            throw new TypeCheckException("Type variables in member accesses are only allowed in method calls (@0)", expr.Identifier);
                        }
                        IOptional<IVariableReference> varref = env.GetFieldAccessVariable(ctr, expr.Identifier.Name);
                        if (!varref.HasElem)
                        {
                            throw new TypeCheckException("$0 does not have a field named $1", ctr.Type, expr.Identifier.Name);
                        }
                        return env.ValueResultHandler(varref.Elem.GenerateReadAccess(env));
                    },
                    typeref =>
                    {
                        IQName<IArgIdentifier<string, Language.IType>> qn = typeref.Snoc(expr.Identifier.Transform(n => n.Name, t => t.TransformType(env.Context)));
                        if (env.Context.HasChild(qn))
                        {
                            return env.TypeResultHandler(qn);
                        }
                        if (expr.Identifier.Arguments.Count() > 0)
                        {
                            throw new TypeCheckException("$0 is not an accessible namespace, interface, or class.", qn);
                        }
                        return env.Context.GetChild(qn).Visit(new Language.ParamRefVisitor<object, T, Language.IType>(
                            (elem, arg) => throw new TypeCheckException("$0 is not an accessible namespace, interface, or class.", qn),
                            (elem, arg) => throw new TypeCheckException("$0 is not an accessible namespace, interface, or class.", qn),
                            (elem, arg) =>
                            {
                                Language.IStaticFieldSpec sfs = elem.Element.StaticFields.FirstOrDefault(sf => sf.Name == expr.Identifier.Name.Name);
                                if (sfs == null)
                                {
                                    throw new TypeCheckException("$0 is not a static field of $1", expr.Identifier.Name, typeref);
                                }
                                if (sfs.Visibility < env.Context.MembersVisibleAt(elem.Element))
                                {
                                    throw new TypeCheckException("$0 is a static field of $2, but insufficiently accessible", expr.Identifier.Name, typeref);
                                }
                                return env.ValueResultHandler(null);//TODO: implement static field access
                            }));
                    },
                    etr=>
                    {
                        if (expr.Identifier.Arguments.Count() > 0)
                        {
                            throw new TypeCheckException("Type variables in member accesses are only allowed in method calls (@0)", expr.Identifier);
                        }
                        IOptional<IVariableReference> varref = env[expr.Identifier.Name];
                        if (!varref.HasElem)
                        {
                            throw new TypeCheckException("Current context does not have a field or variable named $0", expr.Identifier.Name);
                        }
                        return env.ValueResultHandler(varref.Elem.GenerateReadAccess(env));
                    }
                    ));
            };

            public Func<RefIdentRootExpr, AccessorTransformEnvironment<T>, T> VisitRefIdentRootExpr => (expr, env) =>
            {
                var qn = expr.Transform(id => id.Transform(n => n.Name, t => t.TransformType(env.Context)));
                if (env.Context.HasChild(qn))
                {
                    return env.TypeResultHandler(qn);
                }
                throw new TypeCheckException("$0 is not an accessible namespace, interface, or class", qn);
            };

            public Func<LambdaExpr, AccessorTransformEnvironment<T>, T> VisitLambdaExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<StructExpr, AccessorTransformEnvironment<T>, T> VisitStructExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<DefaultInstanceExpr, AccessorTransformEnvironment<T>, T> VisitDefaultInstanceExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<DefaultBoolExpr, AccessorTransformEnvironment<T>, T> VisitDefaultBoolExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<DefaultIdentExpr, AccessorTransformEnvironment<T>, T> VisitDefaultIdentExpr => VisitIdentExpr;

            public Func<DefaultFloatExpr, AccessorTransformEnvironment<T>, T> VisitDefaultFloatExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<DefaultNullExpr, AccessorTransformEnvironment<T>, T> VisitDefaultNullExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<DefaultStringExpr, AccessorTransformEnvironment<T>, T> VisitDefaultStringExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<DefaultIntExpr, AccessorTransformEnvironment<T>, T> VisitDefaultIntExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<LetExpr, AccessorTransformEnvironment<T>, T> VisitLetExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<LetVarExpr, AccessorTransformEnvironment<T>, T> VisitLetVarExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));

            public Func<CFunctionCallExpr, AccessorTransformEnvironment<T>, T> VisitCFunctionCallExpr => (expr, env) => env.ValueResultHandler(expr.Visit(CodeTransformer.Instance, env));
        }

        private class CallReceiverVisitor : Parser.IExprVisitor<CallReceiverTransformEnvironment, ICallReceiverTransformResult>
        {
            public static CallReceiverVisitor Instance { get; } = new CallReceiverVisitor();

            private Func<IExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitValueExpr => (expr, env) =>
            {
                IExprTransformResult etr = expr.Visit(CodeTransformer.Instance, env);
                IOptional<ICallableReference> callableRef = env.AsCallable(etr, env.Arguments.Select(arg => arg.Type));
                if (callableRef.HasElem)
                {
                    return callableRef.Elem.GenerateCall(env.Arguments, env);
                }
                throw new TypeCheckException("$0 is not callable with given arguments (@1)", etr.Type, expr.Locs);
            };

            public Func<BoolExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitBoolExpr => VisitValueExpr;

            public Func<BinOpExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitBinOpExpr => VisitValueExpr;

            public Func<CallExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitCallExpr => VisitValueExpr;

            public Func<NewExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitNewExpr => VisitValueExpr;

            public Func<FloatExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitFloatExpr => VisitValueExpr;

            public Func<UnaryOpExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitUnaryOpExpr => VisitValueExpr;

            public Func<InstanceExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitInstanceExpr => VisitValueExpr;

            public Func<NullExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitNullExpr => VisitValueExpr;

            public Func<ListExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitListExpr => VisitValueExpr;

            public Func<DefaultListExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitDefaultListExpr => VisitValueExpr;

            public Func<StringExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitStringExpr => VisitValueExpr;

            public Func<RangeExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitRangeExpr => VisitValueExpr;

            public Func<IntExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitIntExpr => VisitValueExpr;

            public Func<CastExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitCastExpr => VisitValueExpr;

            public Func<RefIdentRootExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitRefIdentRootExpr => (expr, env) =>
            {
                if (expr.IsFromRootOnly)
                {
                    throw new TypeCheckException("No function $1 in global namespace", expr.Current.Elem);
                }
                IOptional<ICallableReference> callable = env.GetLocalCallable(expr.Current.Elem.TransformArg(t => t.TransformType(env.Context)), env.Arguments.Select(arg => arg.Type));
                if (callable.HasElem)
                {
                    return callable.Elem.GenerateCall(env.Arguments, env);
                }
                throw new TypeCheckException("There is no accessible static or dynamic method named $1 for given arguments (@2)", expr.Current.Elem, expr.Locs);
            };
            public Func<IdentExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitIdentExpr => (expr, env) =>
            {
                IOptional<ICallableReference> callable = env.GetLocalCallable(new AArgIdentifier<Identifier, Language.IType>(expr.Name, new List<Language.IType>(), expr.Locs), env.Arguments.Select(arg => arg.Type));
                if (callable.HasElem)
                {
                    return callable.Elem.GenerateCall(env.Arguments, env);
                }
                IOptional<IVariableReference> varref = env[expr.Name];
                if (varref.HasElem)
                {
                    callable = env.AsCallable(varref.Elem.GenerateReadAccess(env), env.Arguments.Select(arg => arg.Type));
                    if (callable.HasElem)
                    {
                        return callable.Elem.GenerateCall(env.Arguments, env);
                    }
                    throw new TypeCheckException("$0 is not callable with given arguments (@1)", varref.Elem.Type, expr.Name);
                }
                throw new TypeCheckException("$0 is not a name of an accessible field or method", expr.Name);
            };
            public Func<RefIdentExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitRefIdentExpr => (expr, env) =>
            {
                return expr.Accessor.Visit(AccessorVisitor<ICallReceiverTransformResult>.Instance, new AccessorTransformEnvironment<ICallReceiverTransformResult>(env,
                    etr =>
                    {
                        IOptional<ICallableReference> callableRef = env.GetMethod(etr, expr.Identifier.TransformArg(t => t.TransformType(env.Context)), env.Arguments.Select(arg => arg.Type)).Coalesce(() => (env.GetFieldAccessVariable(etr, expr.Identifier.Name).Join(fav => env.AsCallable(fav.GenerateReadAccess(env), env.Arguments.Select(arg => arg.Type)))));
                        if (callableRef.HasElem)
                        {
                            return callableRef.Elem.GenerateCall(env.Arguments, env);
                        }
                        throw new TypeCheckException("$0 does not have a method $1 for given argument types", etr.Type, expr.Identifier);
                    },
                    typeref =>
                    {
                        return env.Context.GetChild(typeref).Visit<object, ICallReceiverTransformResult>(new Language.ParamRefVisitor<object, ICallReceiverTransformResult, Language.IType>(
                             (ns, arg) => throw new TypeCheckException("Namespace $0 does not have a static method called $1", typeref, expr.Identifier),
                             (iface, arg) => throw new TypeCheckException("Interface $0 does not have a static method called $1", typeref, expr.Identifier),
                             (cls, arg) =>
                             {
                                 IOptional<ICallableReference> method = env.GetStaticMethod(cls, expr.Identifier.TransformArg(t => t.TransformType(env.Context)), env.Arguments.Select(argt => argt.Type));
                                 if (method.HasElem)
                                 {
                                     return method.Elem.GenerateCall(env.Arguments, env);
                                 }
                                 throw new TypeCheckException("Class $0 does not have a static method called $1", typeref, expr.Identifier);
                             }), null);
                    },
                    etr=>
                    {
                        IOptional<ICallableReference> callableRef = env.GetMethod(etr, expr.Identifier.TransformArg(t => t.TransformType(env.Context)), env.Arguments.Select(arg => arg.Type)).Coalesce(() => (env[expr.Identifier.Name].Join(fav => env.AsCallable(fav.GenerateReadAccess(env), env.Arguments.Select(arg => arg.Type)))));
                        if (callableRef.HasElem)
                        {
                            return callableRef.Elem.GenerateCall(env.Arguments, env);
                        }
                        throw new TypeCheckException("$0 does not have a method $1 for given argument types", etr.Type, expr.Identifier);
                    }
                    ));
            };

            public Func<LambdaExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitLambdaExpr => VisitValueExpr;

            public Func<StructExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitStructExpr => VisitValueExpr;

            public Func<DefaultInstanceExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitDefaultInstanceExpr => VisitValueExpr;

            public Func<DefaultBoolExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitDefaultBoolExpr => VisitValueExpr;

            public Func<DefaultIdentExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitDefaultIdentExpr => VisitIdentExpr;

            public Func<DefaultFloatExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitDefaultFloatExpr => VisitValueExpr;

            public Func<DefaultNullExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitDefaultNullExpr => VisitValueExpr;

            public Func<DefaultStringExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitDefaultStringExpr => VisitValueExpr;

            public Func<DefaultIntExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitDefaultIntExpr => VisitValueExpr;

            public Func<LetExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitLetExpr => VisitValueExpr;

            public Func<LetVarExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitLetVarExpr => VisitValueExpr;

            public Func<CFunctionCallExpr, CallReceiverTransformEnvironment, ICallReceiverTransformResult> VisitCFunctionCallExpr => VisitValueExpr;
        }
    }

    public interface IStmtTransformResult : IEnumerable<IInstruction>
    {
        bool IsJumping { get; }
        bool IsReturning { get; }
        bool IsThrowing { get; }
        bool AlwaysThrowsOrReturns { get; }
        bool AlwaysGoesElsewhere { get; }
        void AddBranch(PhiNode target, ICodeTransformEnvironment env);
        ICodeTransformEnvironment TransformEnvironment { get; }
    }

    public interface IExprTransformResult : IEnumerable<IInstruction>, IReference
    {
        Language.IType Type { get; }
        IRegister Register { get; }
        ISourceSpan Locs { get; }
    }

    public interface ICallReceiverTransformResult : IExprTransformResult
    {

    }
    public interface IAccessorTransformResult
    {

    }

    internal class CallReceiverTransformResult : ExprTransformResult, ICallReceiverTransformResult
    {
        public CallReceiverTransformResult(Language.IType type, IRegister register, IEnumerable<IInstruction> instructions) : base(type, register, instructions)
        {
        }
    }

    internal class ExprTransformResultReference : IExprTransformResult
    {
        public readonly IExprTransformResult Result;
        public ExprTransformResultReference(IExprTransformResult result)
        {
            Result = result;
        }
        public Language.IType Type => Result.Type;

        public IRegister Register => Result.Register;

        public ISourceSpan Locs => Result.Locs;

        public IEnumerator<IInstruction> GetEnumerator()
        {
            yield break;
        }

        public void PrettyPrint(PrettyPrinter p)
        {

        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }

    internal class StmtTransformResult : IStmtTransformResult
    {
        public IEnumerable<IInstruction> Instructions { get; private set; }
        public StmtTransformResult(IEnumerable<IInstruction> instructions, ICodeTransformEnvironment env, bool isReturning = false, bool isThrowing = false, bool alwaysThrowsOrReturns = false, bool isJumping = false, bool alwaysGoesElsewhere = false)
        {
            Instructions = instructions;
            IsReturning = isReturning;
            IsThrowing = isThrowing;
            AlwaysThrowsOrReturns = alwaysThrowsOrReturns || isReturning || isThrowing;
            TransformEnvironment = env;
            IsJumping = isJumping;
            AlwaysGoesElsewhere = alwaysGoesElsewhere || AlwaysThrowsOrReturns || IsJumping;
        }

        public void AddBranch(PhiNode phi, ICodeTransformEnvironment env)
        {
            this.Instructions = this.Instructions.Snoc(new BranchInstruction(phi, env)).ToList();
        }

        public ICodeTransformEnvironment TransformEnvironment
        {
            get;
        }

        public IEnumerator<IInstruction> GetEnumerator()
        {
            return Instructions.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public bool IsReturning { get; }
        public bool IsThrowing { get; }
        public bool AlwaysThrowsOrReturns { get; }

        public bool IsJumping { get; }

        public bool AlwaysGoesElsewhere { get; }
    }

    internal class ExprTransformErrorResult : IExprTransformResult
    {
        public Language.IType Type { get; } = new Language.DynamicType();

        public IRegister Register => throw new NotImplementedException();

        public ISourceSpan Locs { get; set; } = new GenSourceSpan();

        public IEnumerator<IInstruction> GetEnumerator()
        {
            yield break;
        }

        public void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }
}
