using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public interface IExpr : IAstNode
    {
        IEnumerable<Identifier> FreeVars
        {
            get;
        }
        
        bool IsAtomic
        {
            get;
        }

        R Visit<S, R>(IExprVisitor<S, R> visitor, S state);

        IType TypeAnnotation
        {
            get;
            set;
        }

        //TDTransformResult TransformTD(TDLookup available);
        //T TransformAccessor<T>(TDLookup available, Func<TDTransformResult, T> transformRegular, Func<QName, T> transformTypeRef);
    }

    //public class TDTransformResult : IEnumerable<IInstruction>
    //{
    //    private IEnumerable<IInstruction> instructions;

    //    public Register Register
    //    {
    //        get;
    //        private set;
    //    }

    //    public TDTypeRange Type
    //    {
    //        get;
    //        private set;
    //    }
        

    //    public TDTransformResult(IEnumerable<IInstruction> instructions, Register register, TDTypeRange type)
    //    {
    //        this.instructions = instructions.ToList();
    //        this.Register = register;
    //        this.Type = type;
    //        if((register==null)!=(type==null))
    //        {
    //            throw new InternalException("TDTransformResult must either have a return type and register or none of the two");
    //        }
    //    }

    //    public IEnumerator<IInstruction> GetEnumerator()
    //    {
    //        return instructions.GetEnumerator();
    //    }

    //    IEnumerator IEnumerable.GetEnumerator()
    //    {
    //        return GetEnumerator();
    //    }
    //}
}
