using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public enum BinaryOperator : byte
    {
        Equals,
        RefEquals,
        Add,
        Subtract,
        Multiply,
        Divide,
        Power,
        Mod,
        Concat,
        And,
        Or,
        BitAND,
        BitOR,
        BitXOR,
        ShiftLeft,
        ShiftRight,
        LessThan,
        GreaterThan,
        LessOrEqualTo,
        GreaterOrEqualTo
    }

    public class BinaryOperatorNode:AAstNode
    {
        public readonly BinaryOperator Operator;
        public BinaryOperatorNode(BinaryOperator op, ISourceSpan locs =null):base(locs)
        {
            Operator = op;
        }

        public Identifier OpName
        {
            get
            {
                return new Identifier(Enum.GetName(typeof(BinaryOperator), Operator),Locs);
            }
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            switch (Operator)
            {
                case BinaryOperator.Equals:
                    p.WriteKeyword("===");
                    break;
                case BinaryOperator.RefEquals:
                    p.WriteKeyword("==");
                    break;
                case BinaryOperator.Add:
                    p.WriteKeyword("+");
                    break;
                case BinaryOperator.Subtract:
                    p.WriteKeyword("-"); 
                    break;
                case BinaryOperator.Multiply:
                    p.WriteKeyword("*");
                    break;
                case BinaryOperator.Divide:
                    p.WriteKeyword("/");
                    break;
                case BinaryOperator.Power:
                    p.WriteKeyword("**");
                    break;
                case BinaryOperator.Mod:
                    p.WriteKeyword("%");
                    break;
                case BinaryOperator.Concat:
                    p.WriteKeyword("++");
                    break;
                case BinaryOperator.And:
                    p.WriteKeyword("&&");
                    break;
                case BinaryOperator.Or:
                    p.WriteKeyword("||");
                    break;
                case BinaryOperator.BitAND:
                    p.WriteKeyword("&");
                    break;
                case BinaryOperator.BitOR:
                    p.WriteKeyword("|");
                    break;
                case BinaryOperator.BitXOR:
                    p.WriteKeyword("^");
                    break;
                case BinaryOperator.ShiftLeft:
                    p.WriteKeyword("<<");
                    break;
                case BinaryOperator.ShiftRight:
                    p.WriteKeyword(">>");
                    break;
                case BinaryOperator.LessThan:
                    p.WriteKeyword("<");
                    break;
                case BinaryOperator.GreaterThan:
                    p.WriteKeyword(">");
                    break;
                case BinaryOperator.LessOrEqualTo:
                    p.WriteKeyword("<=");
                    break;
                case BinaryOperator.GreaterOrEqualTo:
                    p.WriteKeyword(">=");
                    break;
            }
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitBinaryOperator(this, state);
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<BinaryOperatorNode, S, R> VisitBinaryOperator { get; }
    }
}
