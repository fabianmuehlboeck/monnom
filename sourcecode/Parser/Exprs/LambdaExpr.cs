using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.Parser
{
    public class LambdaExpr : AAnnotatedExpr<string>
    {
        public readonly Block Code;
        public readonly IEnumerable<ArgDecl> Arguments;
        public readonly IType ReturnType;
        public LambdaExpr(IEnumerable<VarDecl> argDecls, Block code, IType returnType, ISourcePos startPos):base(startPos.SpanTo(returnType?.End??code.End))
        {
            Code = code;
            Arguments = argDecls.Select(a=>a.ToArgDecl()).ToList();
            ReturnType = returnType ?? new DynamicType(code.Locs);
        }
        public LambdaExpr(IEnumerable<ArgDecl> argDecls, Block code, IType returnType, ISourcePos startPos) : base(startPos.SpanTo(returnType?.End ?? code.End))
        {
            Code = code;
            Arguments = argDecls.ToList();
            ReturnType = returnType ?? new DynamicType(code.Locs);
        }
        public override IEnumerable<Identifier> FreeVars => throw new NotImplementedException();

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("(", Start);
            p.PrintList(Arguments, ",");
            p.WritePunctuation(") =>");
            p.WriteWhitespace();
            Code.PrettyPrint(p);
            p.WriteKeyword(":");
            p.WriteWhitespace();
            ReturnType.PrettyPrint(p);
            p.WriteWhitespace();
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitLambdaExpr(this, state);
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<LambdaExpr, S, R> VisitLambdaExpr { get; }
    }
}
