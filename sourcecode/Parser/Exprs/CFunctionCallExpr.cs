using System;
using System.Collections.Generic;
using System.Linq;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class CFunctionCallExpr : AExpr
    {
        public readonly String SourceName;
        public readonly DeclIdentifier Name;
        public readonly IEnumerable<IType> ArgTypes;
        public readonly IType ReturnType;
        public readonly IEnumerable<IType> TypeArgs;
        public readonly IEnumerable<IExpr> Arguments;
        public CFunctionCallExpr(IToken sourceName, DeclIdentifier name, IEnumerable<IType> argTypes, IType returnType, IEnumerable<IType> typeArgs, IEnumerable<IExpr> arguments, ISourceSpan locs) : base(locs)
        {
            SourceName = sourceName.Text?.Substring(1,sourceName.Text.Length-2)??"";
            Name = name;
            ArgTypes = argTypes.ToList();
            ReturnType = returnType;
            TypeArgs = typeArgs.ToList();
            Arguments = arguments.ToList();
        }
        public override IEnumerable<Identifier> FreeVars => Arguments.Select(a => a.FreeVars).Flatten();

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.WriteKeyword("CFUNCTION");
            p.WritePunctuation("[");
            p.Write("\"" + SourceName + "\"", Start);
            p.WritePunctuation("::");
            Name.PrettyPrint(p);
            p.WritePunctuation("(");
            bool first = true;
            foreach(var argType in ArgTypes)
            {
                if(!first)
                {
                    p.WritePunctuation(",");
                    p.WriteWhitespace();
                }
                first = false;
                argType.PrettyPrint(p);
            }
            p.WritePunctuation(")");
            p.WritePunctuation("]");
            p.WritePunctuation("<");
            first = true;
            foreach (var typeArg in TypeArgs)
            {
                if (!first)
                {
                    p.WritePunctuation(",");
                    p.WriteWhitespace();
                }
                first = false;
                typeArg.PrettyPrint(p);
            }
            p.WritePunctuation(">");
            p.WritePunctuation("(");
            first = true;
            foreach (var arg in Arguments)
            {
                if (!first)
                {
                    p.WritePunctuation(",");
                    p.WriteWhitespace();
                }
                first = false;
                arg.PrettyPrint(p);
            }
            p.WritePunctuation(")");
            p.WriteWhitespace();
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitCFunctionCallExpr(this, state);
        }
    }
    public partial interface IExprVisitor<in S, out R>
    {
        Func<CFunctionCallExpr, S, R> VisitCFunctionCallExpr { get; }
    }
}