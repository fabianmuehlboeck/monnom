using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class ClassType : TypeQName, IType
    {
        public Language.IType Annotation { get; set; }

        //public IType CovariantPart => this;

        //public IType ContravariantPart => this;

        public static ClassType GetInstance(IToken tok)
        {
            return GetInstance(tok.Text, tok.ToSourceSpan());
        }

        public static ClassType GetInstance(TypeQName name)
        {
            //if (name.Count() == 1 && BaseType.BaseNames.Contains(name.First().Name.Name))
            //{
            //    return BaseType.GetInstance(name.First().Name.Name, name.Locs);
            //}
            //else
            //{
                return new ClassType(name);
            //}
        }
        public static ClassType GetInstance(String name, ISourceSpan locs = null)
        {
            //if (BaseType.BaseNames.Contains(name))
            //{
            //    return BaseType.GetInstance(name, locs);
            //}
            //else
            //{
                return new ClassType(name, locs);
            //}
        }

        protected ClassType(TypeQName name):base(name.IsFromRootOnly, name.Locs, name.ToArray())
        {
        }
        //protected ClassType(IToken token)
        //    : base(token.Text, token.ToSourceSpan())
        //{
        //}

        protected ClassType(String name, ISourceSpan locs =null)
            : base(true, new TypeIdentifier(new Identifier(name, locs), new List<IType>(), locs ??new GenSourceSpan()).Singleton())
        {
        }


        public virtual T Visit<T>(TypeVisitor<T> visitor)
        {
            return visitor.ClassAction(this);
        }

        //public bool Equals(IType other)
        //{
        //    TypeVisitor<bool> tv = new TypeVisitor<bool> { ClassAction = c => c.Name == this.Name };
        //    return other.Visit(tv);
        //}

        //public override string ToString()
        //{
        //    return  ;
        //}

        //public TypeInterval ToStaticTI()
        //{
        //    return new TypeInterval(this, this, Locs);
        //}

        //public TypeInterval ToDynamicTI()
        //{
        //    return new TypeInterval(BaseType.Bot, this, Locs);
        //}

        //public ITDType TransformTD(TDLookup available)
        //{
        //    return available.GetType(this);
        //}
        
        public virtual R Visit<S, R>(ITypeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitClassType(this, state);
        }
    }

    public partial interface ITypeVisitor<in S, out R>
    {
        Func<ClassType, S, R> VisitClassType { get; }
    }
}
