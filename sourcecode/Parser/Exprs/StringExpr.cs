using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class StringExpr : ALiteralExpr, IExpr
    {
        private string text;
        public StringExpr(IToken token) : base(token)
        {
            this.text = escapeString(token.Text.Substring(1, token.Text.Length - 2));
            Value = text;
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitStringExpr(this, state);
        }

        private static string escapeString(string orig)
        {
            StringBuilder sb = new StringBuilder();
            for(int i=0;i<orig.Length;i++)
            {
                if (orig[i] == '\\')
                {
                    switch (orig[i+1])
                    {
                        case 'n':
                            sb.Append("\n");
                            break;
                        case 't':
                            sb.Append("\t");
                            break;
                        default:
                            sb.Append(orig[i + 1]);
                            break;
                    }
                    i++;
                }
                else
                {
                    sb.Append(orig[i]);
                }
            }
            return sb.ToString();
        }
        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("\""+Value+"\"", Start);
            p.WriteWhitespace();
        }
    }

    public class DefaultStringExpr : StringExpr, IDefaultExpr
    {
        public DefaultStringExpr(IToken token)
            : base(token)
        {

        }

        public R Visit<S, R>(IDefaultExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultStringExpr(this, state);
        }
        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultStringExpr(this, state);
        }
    }
    public partial interface IDefaultExprVisitor<in S, out R>
    {
        Func<DefaultStringExpr, S, R> VisitDefaultStringExpr { get; }
    }
    public partial interface IExprVisitor<in S, out R>
    {
        Func<StringExpr, S, R> VisitStringExpr { get; }
    }
}

