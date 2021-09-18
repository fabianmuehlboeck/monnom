using Antlr4.Runtime.Misc;
using Nom.Parser;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace Nom.BenchmarkGenerator
{
    public class ConversionVisitor<S> : IDefaultExprVisitor<S, Parser.IDefaultExpr>, IExprVisitor<S, Parser.IExpr>, ITypeVisitor<S, Parser.IType>, IStmtVisitor<S, Parser.IStmt>
    {
        internal virtual Func<FullDevolution.ReplacementExpr, S, IExpr> VisitReplacementExpr => (e, s) => 
        {
            throw new FullDevolution.UnreplacedReplacementExprException();
        };
        public virtual Func<DefaultBoolExpr, S, IDefaultExpr> VisitDefaultBoolExpr => (e, s) => { return e; };
        public virtual Func<BoolExpr, S, IExpr> VisitBoolExpr => (e, s) => { return e; };

        public virtual Func<BinOpExpr, S, IExpr> VisitBinOpExpr => (e, s) =>
        {
            IExpr e1 = e.Left.Visit(this, s);
            IExpr e2 = e.Right.Visit(this, s);
            if (e1 != e.Left || e2 != e.Right)
            {
                return new BinOpExpr(e1, e.Operator, e2, e.Locs);
            }
            return e;
        };

        public virtual Func<DefaultIdentExpr, S, IDefaultExpr> VisitDefaultIdentExpr => (e, s) => { return e; };
        public virtual Func<IdentExpr, S, IExpr> VisitIdentExpr => (e, s) => { return e; };

        public virtual Func<RefIdentExpr, S, IExpr> VisitRefIdentExpr => (e, s) => 
        {
            var accessor = e.Accessor.Visit(this, s);
            var ident = VisitRefIdentifier(e.Identifier, s);
            if(!(object.Equals(accessor,e.Accessor)&&object.Equals(ident,e.Identifier)))
            {
                return new RefIdentExpr(accessor, ident);
            }
            return e; 
        };

        public virtual Func<RefIdentRootExpr, S, IExpr> VisitRefIdentRootExpr => (e, s) => 
        {
            return e; 
        };

        public virtual Func<CallExpr, S, IExpr> VisitCallExpr => (e, s) =>
        {
            IExpr erec = e.Receiver?.Visit(this, s);
            bool different = erec != e.Receiver;
            List<IExpr> args = new List<IExpr>();
            foreach (var arg in e.Args)
            {
                IExpr earg = arg.Visit(this, s);
                if (arg != earg)
                {
                    different = true;
                }
                args.Add(earg);
            }
            if (different)
            {
                return new CallExpr(erec, args, e.Locs);
            }
            return e;
        };

        public virtual Func<NewExpr, S, IExpr> VisitNewExpr => (e, s) =>
        {
            bool different = false;
            List<IExpr> args = new List<IExpr>();
            foreach (var arg in e.Args)
            {
                IExpr earg = arg.Visit(this, s);
                if (arg != earg)
                {
                    different = true;
                }
                args.Add(earg);
            }
            if (different)
            {
                return new NewExpr(e.NewCall, args, e.Locs) { Annotation = e.Annotation };
            }
            return e;
        };

        public virtual Func<DefaultFloatExpr, S, IDefaultExpr> VisitDefaultFloatExpr => (e, s) => { return e; };
        public virtual Func<FloatExpr, S, IExpr> VisitFloatExpr => (e, s) => { return e; };

        public virtual Func<UnaryOpExpr, S, IExpr> VisitUnaryOpExpr => (e, s) =>
        {
            IExpr expr = e.Expr.Visit(this, s);
            if (expr != e.Expr)
            {
                return new UnaryOpExpr(expr, e.Operator, e.Locs);
            }
            return e;
        };

        public virtual Func<InstanceExpr, S, IExpr> VisitInstanceExpr => (e, s) =>
        {
            bool different = false;
            List<IExpr> args = new List<IExpr>();
            foreach (var arg in e.Arguments)
            {
                IExpr earg = arg.Visit(this, s);
                if (arg != earg)
                {
                    different = true;
                }
                args.Add(earg);
            }
            if (different)
            {
                return new InstanceExpr(e.Class, e.InstanceName, args, e.Locs);
            }
            return e;
        };

        public virtual Func<DefaultNullExpr, S, IDefaultExpr> VisitDefaultNullExpr => (e, s) => { return e; };
        public virtual Func<NullExpr, S, IExpr> VisitNullExpr => (e, s) => { return e; };

        public virtual Func<StructExpr, S, IExpr> VisitStructExpr => (e, s) =>
        {
            bool different = false;
            List<StructFieldDecl> fields = new List<StructFieldDecl>();
            foreach (var field in e.Fields)
            {
                var fld = VisitStructFieldDecl(field, s);
                if (fld != field)
                {
                    different = true;
                }
                fields.Add(fld);
            }

            List<MethodDef> methods = new List<MethodDef>();
            foreach (var method in e.Methods)
            {
                var md = VisitMethodDef(method, s);
                if (md != method)
                {
                    different = true;
                }
                methods.Add(md);
            }

            List<StructAssignment> sas = new List<StructAssignment>();
            foreach(var sa in e.StructAssignments)
            {
                var nae = sa.Expression.Visit(this, s);
                if (nae != sa.Expression)
                {
                    different = true;
                    sas.Add(new StructAssignment(sa.Variable, nae));
                }
                else
                {
                    sas.Add(sa);
                }
            }
            if (different)
            {
                return new StructExpr(fields, methods, sas, e.Locs);
            }
            return e;
        };


        public virtual Func<ListExpr, S, IExpr> VisitListExpr => (e, s) =>
        {
            bool different = false;
            List<IExpr> args = new List<IExpr>();
            foreach (var arg in e.Elements)
            {
                IExpr earg = arg.Visit(this, s);
                if (arg != earg)
                {
                    different = true;
                }
                args.Add(earg);
            }
            if (different)
            {
                return new ListExpr(args, e.Locs);
            }
            return e;
        };

        public virtual Func<DefaultListExpr, S, IDefaultExpr> VisitDefaultListExpr => (e, s) =>
        {
            bool different = false;
            List<IDefaultExpr> args = new List<IDefaultExpr>();
            foreach (var arg in e.Elements)
            {
                IDefaultExpr earg = arg.Visit(this as IDefaultExprVisitor<S, IDefaultExpr>, s);
                if (arg != earg)
                {
                    different = true;
                }
                args.Add(earg);
            }
            if (different)
            {
                return new DefaultListExpr(args, e.Locs);
            }
            return e;
        };
        Func<DefaultListExpr, S, IExpr> IDefaultExprVisitor<S, IExpr>.VisitDefaultListExpr => VisitDefaultListExpr;

        public virtual Func<LambdaExpr, S, IExpr> VisitLambdaExpr => (e, s) =>
        {
            Parser.IType retType = e.ReturnType.Visit(this, s);
            Parser.Block codeBlock = e.Code.Visit(this, s).AsBlock();
            bool different = !(retType == e.ReturnType && codeBlock == e.Code);
            List<ArgDecl> args = new List<ArgDecl>();
            foreach (var arg in e.Arguments)
            {
                var narg = VisitArgDecl(arg, s);
                if (narg != arg)
                {
                    different = true;
                }
                args.Add(narg);
            }
            if (different)
            {
                return new LambdaExpr(args, codeBlock, retType, e.Start);
            }
            return e;
        };

        public virtual Func<DefaultStringExpr, S, IDefaultExpr> VisitDefaultStringExpr => (e, s) => { return e; };
        public virtual Func<StringExpr, S, IExpr> VisitStringExpr => (e, s) => { return e; };

        public virtual Func<RangeExpr, S, IExpr> VisitRangeExpr => (e, s) =>
        {
            var start = e.RangeStart?.Visit(this, s);
            var end = e.RangeEnd?.Visit(this, s);
            var step = e.RangeStep?.Visit(this, s);
            if (start != e.RangeStart || end != e.RangeEnd || step != e.RangeStep)
            {
                return new RangeExpr(end, start, step, e.Locs);
            }
            return e;
        };

        public virtual Func<DefaultIntExpr, S, IDefaultExpr> VisitDefaultIntExpr => (e, s) => { return e; };
        public virtual Func<IntExpr, S, IExpr> VisitIntExpr => (e, s) => { return e; };

        public virtual Func<CastExpr, S, IExpr> VisitCastExpr => (e, s) =>
        {
            var type = e.Type.Visit(this, s);
            var expr = e.Expr.Visit(this, s);
            if (type != e.Type || expr != e.Expr)
            {
                return new CastExpr(type, expr, e.Locs);
            }
            return e;
        };

        public virtual Func<StructFieldDecl, S, StructFieldDecl> VisitStructFieldDecl => (sfd, s) =>
        {
            var expr = sfd.InitExpr?.Visit(this, s);
            var type = sfd.Type?.Visit(this, s);
            if (expr != sfd.InitExpr || type != sfd.Type)
            {
                return new StructFieldDecl(type, sfd.Ident, expr);
            }
            return sfd;
        };
        public virtual Func<MethodDef, S, MethodDef> VisitMethodDef => (md, s) =>
        {
            Block code = md.Code.Visit(this, s).AsBlock();
            IType returns = md.Returns.Visit(this, s);
            var varDecls = new List<VarDecl>();
            DeclIdentifier name = VisitDeclIdentifier(md.Name, s);
            bool different = !(object.Equals(name, md.Name) && object.Equals(code, md.Code) && object.Equals(returns, md.Returns));
            foreach (var vd in md.ArgDefs)
            {
                var nvd = VisitVarDecl(vd, s);
                if (nvd != vd)
                {
                    different = true;
                }
                varDecls.Add(nvd);
            }
            if (different)
            {
                return new MethodDef(md.IsFinal, md.IsVirtual, md.IsOverride, md.IsCallTarget, md.Visibility, name, varDecls, returns, code, md.Locs);
            }
            return md;
        };

        public virtual Func<MethodDecl, S, MethodDecl> VisitMethodDecl => (md, s) =>
        {
            IType returns = md.Returns.Visit(this, s);
            DeclIdentifier name = VisitDeclIdentifier(md.Name, s);
            var varDecls = new List<ArgDecl>();
            bool different = !(object.Equals(name, md.Name) && object.Equals(returns, md.Returns));
            foreach (var vd in md.Args)
            {
                var nvd = VisitArgDecl(vd, s);
                if (nvd != vd)
                {
                    different = true;
                }
                varDecls.Add(nvd);
            }
            if (different)
            {
                return new MethodDecl(md.Visibility, md.IsCallTarget, name, varDecls, returns, md.Locs);
            }
            return md;
        };
        public virtual Func<Constructor, S, Constructor> VisitConstructorDef => (cd, s) =>
        {
            var varDecls = new List<ArgDecl>();
            bool different = false;
            var argDecls = VisitList(cd.Args, s, VisitVarDecl, ref different);
            var superArgs = VisitList(cd.SuperCallArgs, s, (e, s) => e.Visit(this, s), ref different);
            var preBlock = cd.PreSuperStatements.Visit(this, s).AsBlock();
            var postBlock = cd.PastSuperStatements.Visit(this, s).AsBlock();
            if (different || !object.Equals(cd.PreSuperStatements, preBlock) || !object.Equals(cd.PastSuperStatements, postBlock))
            {
                return new Constructor(cd.Visibility, argDecls, preBlock, superArgs, postBlock, cd.Locs);
            }
            return cd;
        };

        public virtual Func<InheritanceDecl, S, InheritanceDecl> VisitInheritanceDecl => (ihd, s) =>
        {
            var parent = VisitRefQName(ihd.Parent, s);
            if(!object.Equals(parent,ihd.Parent))
            {
                return new InheritanceDecl(parent, ihd.Locs);
            }
            return ihd;
        };
        public virtual Func<FieldDecl, S, FieldDecl> VisitFieldDecl => (fd, s) =>
        {
            var expr = fd.InitExpr?.Visit(this, s);
            var type = fd.Type?.Visit(this, s);
            if (!(object.Equals(expr, fd.InitExpr) && object.Equals(type, fd.Type)))
            {
                return new FieldDecl(fd.Ident, type, expr, fd.Visibility, fd.IsReadOnly, fd.Locs);
            }
            return fd;
        };

        public virtual Func<StaticFieldDecl, S, StaticFieldDecl> VisitStaticFieldDecl => (sfd, s) =>
        {
            var expr = sfd.InitExpr?.Visit(this as IDefaultExprVisitor<S, IDefaultExpr>, s);
            var type = sfd.Type?.Visit(this, s);
            if (!(object.Equals(expr, sfd.InitExpr) && object.Equals(type, sfd.Type)))
            {
                return new StaticFieldDecl(sfd.Ident, type, sfd.Visibility, sfd.IsReadOnly, sfd.Locs);
            }
            return sfd;
        };
        public virtual Func<StaticMethodDef, S, StaticMethodDef> VisitStaticMethodDef => (smd, s) =>
        {
            var returnType = smd.Returns.Visit(this, s);
            var code = smd.Code.Visit(this, s).AsBlock();
            var name = VisitDeclIdentifier(smd.Name, s);
            bool different = !(object.Equals(name, smd.Name) && object.Equals(returnType, smd.Returns) && object.Equals(code, smd.Code));
            var argDecls = VisitList(smd.ArgDefs, s, VisitVarDecl, ref different);
            if (different)
            {
                return new StaticMethodDef(smd.Visibility, name, argDecls, returnType, code, smd.Locs);
            }
            return smd;
        };



        public virtual Func<InstanceFieldAssignment, S, InstanceFieldAssignment> VisitInstanceFieldAssignment => (ifa, s) =>
        {
            var expr = ifa.Expr.Visit(this as IDefaultExprVisitor<S, IDefaultExpr>, s);
            if (!object.Equals(expr, ifa.Expr))
            {
                return new InstanceFieldAssignment(ifa.FieldName, expr, ifa.Locs);
            }
            return ifa;
        };
        public virtual Func<InstanceDef, S, InstanceDef> VisitInstanceDef => (idef, s) =>
        {
            bool different = false;
            var fieldAssignments = VisitList(idef.Assignments, s, VisitInstanceFieldAssignment, ref different);
            var superArgs = VisitList(idef.SuperArgs, s, (e, s) => e.Visit(this as IDefaultExprVisitor<S, IDefaultExpr>, s), ref different);
            var argDecls = VisitList(idef.Arguments, s, VisitArgDecl, ref different);
            if (different)
            {
                return new InstanceDef(idef.Visibility, idef.Name, idef.SuperName, fieldAssignments, idef.IsMulti, idef.IsDefault, superArgs, argDecls, idef.Locs);
            }
            return idef;
        };

        protected IEnumerable<R> VisitList<T,R>(IEnumerable<T> l, S s, Func<T, S, R> visitor, ref bool different)
        {
            List<R> ret = new List<R>();
            foreach (T t in l)
            {
                R t2 = visitor(t, s);
                if ((t2 != null && !t2.Equals(t)) || (t2 == null && t != null))
                {
                    different = true;
                }
                ret.Add(t2);
            }
            return ret;
        }
        public virtual Func<InterfaceDef, S, InterfaceDef> VisitInterfaceDef => (idef, s) =>
        {
            var name = VisitDeclIdentifier(idef.Name, s);
            bool different = !(object.Equals(name, idef.Name));
            var superImpls = VisitList(idef.Implements, s, VisitInheritanceDecl, ref different);
            var methods = VisitList(idef.Methods, s, VisitMethodDecl, ref different);
            var interfaces = VisitList(idef.Interfaces, s, VisitInterfaceDef, ref different);
            if (different)
            {
                return new InterfaceDef(name, superImpls, methods, idef.Visibility, idef.IsPartial, idef.IsShape, idef.IsMaterial, interfaces, idef.Locs);
            }
            return idef;
        };

        public virtual Func<ClassDef, S, ClassDef> VisitClassDef => (cdef, s) =>
        {
            var superInh = cdef.SuperClass.Bind(sc => VisitInheritanceDecl(sc, s));
            var name = VisitDeclIdentifier(cdef.Name, s);
            bool different = !(object.Equals(name,cdef.Name) &&object.Equals(superInh, cdef.SuperClass));
            var superImpls = VisitList(cdef.Implements, s, this.VisitInheritanceDecl, ref different);
            var methods = VisitList(cdef.ImplementedMethods, s, this.VisitMethodDef, ref different);
            var fields = VisitList(cdef.Fields, s, this.VisitFieldDecl, ref different);
            var constructors = VisitList(cdef.Constructors, s, this.VisitConstructorDef, ref different);
            var staticFields = VisitList(cdef.StaticFields, s, this.VisitStaticFieldDecl, ref different);
            var staticMethods = VisitList(cdef.StaticMethods, s, this.VisitStaticMethodDef, ref different);
            var instances = VisitList(cdef.Instances, s, this.VisitInstanceDef, ref different);
            var interfaces = VisitList(cdef.Interfaces, s, this.VisitInterfaceDef, ref different);
            var classes = VisitList(cdef.Classes, s, this.VisitClassDef, ref different);
            if (different)
            {
                return new ClassDef(name, superInh, superImpls, methods, fields, constructors, staticFields, staticMethods, instances, cdef.IsFinal, cdef.Visibility, cdef.IsAbstract, cdef.IsPartial, cdef.IsShape, cdef.IsMaterial, interfaces, classes, cdef.Locs, cdef.IsSpecial);
            }
            return cdef;
        };

        public virtual Func<Namespace, S, Namespace> VisitNamespaceDef => (nsd, s) =>
        {
            var name = VisitDeclQName(nsd.Name, s);
            bool different = !(object.Equals(name,nsd.Name));
            var namespaces = VisitList(nsd.Namespaces, s, VisitNamespaceDef, ref different);
            var interfaces = VisitList(nsd.Interfaces, s, VisitInterfaceDef, ref different);
            var classes = VisitList(nsd.Classes, s, VisitClassDef, ref different);
            if (different)
            {
                return new Namespace(name, interfaces, classes, namespaces, nsd.Locs);
            }
            return nsd;
        };
        public virtual Func<CodeFile, S, CodeFile> VisitCodeFile => (nsd, s) =>
        {
            bool different = false;
            var namespaces = VisitList(nsd.Namespaces, s, VisitNamespaceDef, ref different);
            var interfaces = VisitList(nsd.Interfaces, s, VisitInterfaceDef, ref different);
            var classes = VisitList(nsd.Classes, s, VisitClassDef, ref different);
            if (different)
            {
                return new CodeFile(nsd.FileName, nsd.Usings, interfaces, classes, namespaces);
            }
            return nsd;
        };


        public virtual Func<ArgDecl, S, ArgDecl> VisitArgDecl => (arg, s) =>
        {
            var type = arg.Type.Visit(this, s);
            if (type != arg.Type)
            {
                return new ArgDecl(arg.Name, type, arg.Locs);
            }
            return arg;
        };
        public virtual Func<VarDecl, S, VarDecl> VisitVarDecl => (arg, s) =>
        {
            var type = arg.Type.Visit(this, s);
            if (type != arg.Type)
            {
                return new VarDecl(arg.Name, type, arg.Locs);
            }
            return arg;
        };

        public virtual Func<DynamicType, S, IType> VisitDynamicType => (t, s) => { return t; };

        public virtual Func<ArrayType, S, IType> VisitArrayType => (t, s) =>
        {
            var type = t.Type.Visit(this, s);
            if (type != t.Type)
            {
                return new ArrayType(t, t.Locs);
            }
            return t;
        };

        public virtual Func<ClassType, S, IType> VisitClassType => (t, s) =>
        {
            bool different = false;
            List<IArgIdentifier<Identifier, Parser.IType>> argids = new List<IArgIdentifier<Identifier, IType>>();
            List<IType> allArgs = new List<IType>();
            foreach(var part in t)
            {
                bool diffID = false;
                List<IType> types = new List<IType>();
                foreach(var type in part.Arguments)
                {
                    var newtype = type.Visit(this, s);
                    if (newtype != type)
                    {
                        different = true;
                        diffID = true;
                    }
                    types.Add(newtype);
                    allArgs.Add(newtype);
                }
                if(diffID)
                {
                    argids.Add(new AArgIdentifier<Identifier, IType>(part.Name, types));
                }
                else
                {
                    argids.Add(part);
                }
            }
            if(different)
            {
                var newType =  ClassType.GetInstance(new TypeQName(false, argids));
                if(t.Annotation!=null)
                {
                    newType.Annotation = t.Annotation.ReplaceArgsWith(allArgs.Select(t => t.Annotation));
                }
                return newType;
            }
            return t;
        };

        public virtual Func<ExprStmt, S, IStmt> VisitExprStmt => (stmt, s) =>
        {
            var expr = stmt.Expression.Visit(this, s);
            if (expr != stmt.Expression)
            {
                return new ExprStmt(expr, stmt.Locs);
            }
            return stmt;
        };

        public virtual Func<DeclStmt, S, IStmt> VisitDeclStmt => (stmt, s) =>
        {
            var expr = stmt.Expr?.Visit(this, s);
            var type = stmt.Type?.Visit(this, s);
            if (expr != stmt.Expr || type != stmt.Type)
            {
                return new DeclStmt(type, stmt.Var, expr, stmt.Locs);
            }
            return stmt;
        };

        public virtual Func<ErrorStmt, S, IStmt> VisitErrorStmt => (stmt, s) =>
        {
            var expr = stmt.Expr.Visit(this, s);
            if (expr != stmt.Expr)
            {
                return new ErrorStmt(expr, stmt.Locs);
            }
            return stmt;
        };

        public virtual Func<ContinueStmt, S, IStmt> VisitContinueStmt => (stmt, s) => { return stmt; };

        public virtual Func<ForeachStmt, S, IStmt> VisitForeachStmt => (stmt, s) =>
        {
            var vd = VisitVarDecl(stmt.Var, s);
            var range = stmt.Range.Visit(this, s);
            var block = stmt.Block.Visit(this, s).AsBlock();
            if (vd != stmt.Var || range != stmt.Range || block != stmt.Block)
            {
                return new ForeachStmt(vd, range, block, stmt.Locs);
            }
            return stmt;
        };

        public virtual Func<DebugStmt, S, IStmt> VisitDebugStmt => (stmt, s) => { return stmt; };

        public virtual Func<ReturnStmt, S, IStmt> VisitReturnStmt => (stmt, s) =>
        {
            var expr = stmt.Expr.Visit(this, s);
            if (expr != stmt.Expr)
            {
                return new ReturnStmt(expr, stmt.Locs);
            }
            return stmt;
        };

        public virtual Func<IfTypeStmt, S, IStmt> VisitIfTypeStmt => (stmt, s) =>
        {
            var type = stmt.Type.Visit(this, s);
            var then = stmt.Thenb.Visit(this, s).AsBlock();
            var elseb = stmt.Elseb?.Visit(this, s)?.AsBlock();
            if (type != stmt.Type || then != stmt.Thenb || elseb != stmt.Elseb)
            {
                return new IfTypeStmt(stmt.Var, type, then, elseb, stmt.Locs);
            }
            return stmt;
        };

        public virtual Func<IfStmt, S, IStmt> VisitIfStmt => (stmt, s) =>
        {
            var guard = stmt.GuardExpression.Visit(this, s);
            var thenb = stmt.ThenBlock.Visit(this, s).AsBlock();
            var elseb = stmt.ElseBlock.Bind(b => b.Visit(this, s).AsBlock());
            List<(IExpr, Block)> elseifs = new List<(IExpr, Block)>();
            bool different = !(object.Equals(guard, stmt.GuardExpression) && object.Equals(thenb, stmt.ThenBlock) && object.Equals(elseb, stmt.ElseBlock));
            foreach (var eif in stmt.ElseIfs)
            {
                var eifguard = eif.Item1.Visit(this, s);
                var eifblock = eif.Item2.Visit(this, s).AsBlock();
                if (!(object.Equals(eifguard, eif.Item1) && object.Equals(eifblock, eif.Item2)))
                {
                    different = true;
                    elseifs.Add((eifguard, eifblock));
                }
            }
            if (different)
            {
                return new IfStmt(guard, thenb, elseifs.Select(eif => eif.Item1), elseifs.Select(eif => eif.Item2), elseb.AsEnumerable(), stmt.Locs);
            }
            return stmt;
        };

        public virtual Func<AssignStmt, S, IStmt> VisitAssignStmt => (stmt, s) =>
        {
            var accessor = stmt.Accessor.Bind(a => a.Visit(this, s));
            var expr = stmt.Expression.Visit(this, s);
            if (!(object.Equals(accessor, stmt.Accessor) && object.Equals(expr, stmt.Expression)))
            {
                return new AssignStmt(accessor.Extract(x => x), stmt.Variable, expr);
            }
            return stmt;
        };

        public virtual Func<Block, S, IStmt> VisitBlock => (stmt, s) =>
        {
            bool different = false;
            IEnumerable<IStmt> stmts = stmt.Select(stmt2 => { var stmt3 = stmt2.Visit(this, s); different = different || stmt3 != stmt2; return stmt3; }).ToList();
            if (different)
            {
                return new Block(stmts, stmt.Locs);
            }
            return stmt;
        };

        public virtual Func<BreakStmt, S, IStmt> VisitBreakStmt => (stmt, s) => { return stmt; };

        public virtual Func<WhileStmt, S, IStmt> VisitWhileStmt => (stmt, s) =>
        {
            var guard = stmt.Guard.Visit(this, s);
            var block = stmt.Block.Visit(this, s).AsBlock();
            if (guard != stmt.Guard || block != stmt.Block)
            {
                return new WhileStmt(guard, block, stmt.Locs);
            }
            return stmt;
        };

        public virtual Func<RTCmdStmt, S, IStmt> VisitRTCmdStmt => (stmt, s) => { return stmt; };

        public virtual Func<SpecialStmt, S, IStmt> VisitSpecialStmt => (stmt, s) => { return stmt; };

        public virtual Func<DefaultInstanceExpr, S, IDefaultExpr> VisitDefaultInstanceExpr => VisitDefaultInstanceExpr;

        Func<DefaultBoolExpr, S, IExpr> IDefaultExprVisitor<S, IExpr>.VisitDefaultBoolExpr => VisitBoolExpr;

        Func<DefaultIdentExpr, S, IExpr> IDefaultExprVisitor<S, IExpr>.VisitDefaultIdentExpr => VisitIdentExpr;

        Func<DefaultFloatExpr, S, IExpr> IDefaultExprVisitor<S, IExpr>.VisitDefaultFloatExpr => VisitFloatExpr;

        Func<DefaultInstanceExpr, S, IExpr> IDefaultExprVisitor<S,IExpr>.VisitDefaultInstanceExpr => VisitDefaultInstanceExpr;

        Func<DefaultNullExpr, S, IExpr> IDefaultExprVisitor<S, IExpr>.VisitDefaultNullExpr => VisitDefaultNullExpr;

        Func<DefaultStringExpr, S, IExpr> IDefaultExprVisitor<S, IExpr>.VisitDefaultStringExpr => VisitStringExpr;

        Func<DefaultIntExpr, S, IExpr> IDefaultExprVisitor<S, IExpr>.VisitDefaultIntExpr => VisitIntExpr;

        public virtual Func<Parser.Program, S, Parser.Program> VisitProgram => (p, s) =>
        {
            bool different = false;
            var codefiles = VisitList(p.CodeFiles, s, VisitCodeFile, ref different);
            if (different)
            {
                return new Program(codefiles);
            }
            return p;
        };

        public virtual Func<TypeArgDecl, S, TypeArgDecl> VisitTypeArgDecl => (tad, s) =>
        {
            var lb = tad.LowerBound?.Visit(this, s);
            var ub = tad.UpperBound?.Visit(this, s);
            if(!object.Equals(lb,tad.LowerBound)&&object.Equals(ub,tad.UpperBound))
            {
                return new TypeArgDecl(tad.Name, ub, lb, tad.Locs);
            }
            return tad;
        };

        public virtual Func<IArgIdentifier<Identifier, TypeArgDecl>, S, DeclIdentifier> VisitDeclIdentifier => (id, s) =>
        {
            bool different = false;
            var arguments = VisitList(id.Arguments, s, VisitTypeArgDecl, ref different);
            if(different)
            {
                return new DeclIdentifier(id.Name, arguments, id.Name.Locs);
            }
            return id as DeclIdentifier ?? new DeclIdentifier(id.Name, arguments, id.Name.Locs);
        };

        public virtual Func<DeclQName, S, DeclQName> VisitDeclQName => (qn, s) =>
        {
            bool different = false;
            var items = VisitList(qn, s, VisitDeclIdentifier, ref different);
            if(different)
            {
                return new DeclQName(items, qn.Locs);
            }
            return qn;
        };

        public virtual Func<IArgIdentifier<Identifier, IType>, S, RefIdentifier> VisitRefIdentifier => (id, s) =>
        {
            bool different = false;
            var arguments = VisitList(id.Arguments, s, (t, s2) => t.Visit(this, s2), ref different);
            if (different)
            {
                return new RefIdentifier(id.Name, arguments, id.Name.Locs);
            }
            return id as RefIdentifier ?? new RefIdentifier(id.Name, arguments, id.Name.Locs);
        };

        public virtual Func<RefQName, S, RefQName> VisitRefQName => (qn, s) =>
        {
            bool different = false;
            var items = VisitList(qn, s, VisitRefIdentifier, ref different);
            if (different)
            {
                return new RefQName(qn.IsFromRootOnly, items, qn.Locs);
            }
            return qn;
        };

        public virtual Func<IArgIdentifier<Identifier, IType>, S, TypeIdentifier> VisitTypeIdentifier => (id, s) =>
        {
            bool different = false;
            var arguments = VisitList(id.Arguments, s, (t, s2) => t.Visit(this, s2), ref different);
            if (different)
            {
                return new TypeIdentifier(id.Name, arguments, id.Name.Locs);
            }
            return id as TypeIdentifier ?? new TypeIdentifier(id.Name, arguments, id.Name.Locs);
        };

        public virtual Func<TypeQName, S, TypeQName> VisitTypeQName => (qn, s) =>
        {
            bool different = false;
            var items = VisitList(qn, s, VisitTypeIdentifier, ref different);
            if (different)
            {
                return new TypeQName(qn.IsFromRootOnly, items, qn.Locs);
            }
            return qn;
        };

        public virtual Func<MaybeType, S, IType> VisitMaybeType => (tp, s) =>
        {
            var arg = tp.Type.Visit(this, s);
            if (arg != tp.Type)
            {
                var ret = new MaybeType(arg, tp.Locs);
                if(tp.Annotation!=null)
                {
                    ret.Annotation = tp.Annotation.ReplaceArgsWith(arg.Annotation.Singleton());
                }
                return ret;
            }   
            return tp;
        };

        public virtual Func<ProbablyType, S, IType> VisitProbablyType => (tp, s) =>
        {
            var arg = tp.Type.Visit(this, s);
            if (arg != tp.Type)
            {
                var ret = new ProbablyType(arg, tp.Locs);
                if (tp.Annotation != null)
                {
                    ret.Annotation = tp.Annotation.ReplaceArgsWith(arg.Annotation.Singleton());
                }
                return ret;
            }
            return tp;
        };

        public virtual Func<IfObjStmt, S, IStmt> VisitIfObjStmt => (stmt, s) =>
        {
            var thenB = stmt.ThenBlock.Visit(this, s).AsBlock();
            var elseB = stmt.ElseBlock.Bind(b => b.Visit(this, s).AsBlock());
            if(!object.Equals(thenB, stmt.ThenBlock)||!object.Equals(elseB, stmt.ElseBlock))
            {
                return new IfObjStmt(stmt.Ident, thenB, elseB.HasElem ? elseB.Elem : null, stmt.Locs);
            }
            return stmt;
        };

        public virtual Func<IfNullStmt, S, IStmt> VisitIfNullStmt => (stmt, s) =>
        {
            var thenB = stmt.ThenBlock.Visit(this, s).AsBlock();
            var elseB = stmt.ElseBlock.Bind(b => b.Visit(this, s).AsBlock());
            if (!object.Equals(thenB, stmt.ThenBlock) || !object.Equals(elseB, stmt.ElseBlock))
            {
                return new IfNullStmt(stmt.Ident, thenB, elseB.HasElem ? elseB.Elem : null, stmt.Locs);
            }
            return stmt;
        };

        public Func<LetExpr, S, IExpr> VisitLetExpr => (expr, s) =>
        {
            var btp = expr.Declaration.Type.Visit(this, s);
            var bindexp = expr.Declaration.InitExpr.Visit(this, s);
            var bodyexp = expr.Body.Visit(this, s);
            if(btp!=expr.Declaration.Type||bindexp!=expr.Declaration.InitExpr||bodyexp!=expr.Body)
            {
                return new LetExpr(new StructFieldDecl(btp, expr.Declaration.Ident, bindexp), bodyexp,expr.Locs);
            }
            return expr;
        };

        public Func<LetVarExpr, S, IExpr> VisitLetVarExpr => (expr, s) =>
        {
            var bindexp = expr.BindExpr.Visit(this, s);
            var bodyexp = expr.Body.Visit(this, s);
            if (bindexp != expr.BindExpr || bodyexp != expr.Body)
            {
                return new LetVarExpr(expr.Ident, bindexp, bodyexp, expr.Locs);
            }
            return expr;
        };
    }
}
