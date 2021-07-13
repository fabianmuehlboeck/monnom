using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class StructAssignment : AAstNode
    {
        public StructAssignment(Identifier ident, IExpr expr) : base(ident, expr)
        {
            Variable = ident;
            Expression = expr;
        }
        public readonly Identifier Variable;
        public readonly IExpr Expression;

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitStructAssignment(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Variable.PrettyPrint(p);
            p.WriteWhitespace();
            p.WritePunctuation("=");
            p.WriteWhitespace();
            Expression.PrettyPrint(p);
        }
    }
    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<StructAssignment, S, R> VisitStructAssignment { get; }
    }
    public class StructExpr : AAnnotatedExpr<string>
    {
        public IEnumerable<StructFieldDecl> Fields { get; }
        public IEnumerable<MethodDef> Methods { get; }
        public IEnumerable<StructAssignment> StructAssignments { get; }
        public StructExpr(IEnumerable<StructFieldDecl> fields, IEnumerable<MethodDef> methods, IEnumerable<StructAssignment> structAssignments, IToken newtok, IToken rbrace) : base(newtok.ToSourcePos().SpanTo(rbrace))
        {
            Fields = fields.ToList();
            Methods = methods.ToList();
            StructAssignments = structAssignments.ToList();
        }
        public StructExpr(IEnumerable<StructFieldDecl> fields, IEnumerable<MethodDef> methods, IEnumerable<StructAssignment> structAssignments, ISourceSpan locs) : base(locs)
        {
            Fields = fields.ToList();
            Methods = methods.ToList();
            StructAssignments = structAssignments.ToList();
        }

        public override IEnumerable<Identifier> FreeVars => throw new NotImplementedException();

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitStructExpr(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("new", Start);
            if (Fields.Any())
            {
                p.WritePunctuation("(");
                p.PrintMembers(Fields, ",");
                p.WritePunctuation(")");
            }
            p.WriteLine();
            p.WritePunctuation(" {");
            p.IncreaseIndent();
            p.WriteLine();
            p.PrintMembers(Methods);
            p.PrintMembers(StructAssignments, ",");
            p.DecreaseIndent();
            p.WriteLine();
            p.WritePunctuation("}");
            p.WriteLine();
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<StructExpr, S, R> VisitStructExpr { get; }
    }
}
