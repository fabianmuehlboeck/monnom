using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Nom.Language;

namespace Nom.TypeChecker
{
    internal class TDClassDef : TDInterfaceDef
    {
        public TDClassDef(Parser.Identifier name, IEnumerable<TDTypeArgDeclDef> typeArgs, bool isPartial, bool isShape, bool isMaterial, Parser.VisibilityNode visibility, bool isAbstract, bool isFinal) : base(name, typeArgs, isPartial, isShape, isMaterial, visibility)
        {
            this.IsAbstract = isAbstract;
            this.IsFinal = isFinal;
        }

        public bool IsAbstract
        {
            get;
            private set;
        }

        public bool IsFinal
        {
            get;
            private set;
        }

        private IOptional<IParamRef<IClassSpec, IType>> superClass = Optional<IParamRef<IClassSpec, IType>>.Empty;
        public IOptional<IParamRef<IClassSpec, IType>> SuperClass
        {
            get => superClass;
            set
            {
                if(superClass.HasElem)
                {
                    throw new InternalException("Cannot set super class twice");
                }
                superClass = value;
            }
        }

        private List<MethodDef> methodDefinitions = new List<MethodDef>();
        public IEnumerable<MethodDef> MethodDefinitions => methodDefinitions.ToList();
        public void AddMethodDef(MethodDef md)
        {
            methodDefinitions.Add(md);
        }


        private List<FieldSpec> fieldDefinitions = new List<FieldSpec>();
        public IEnumerable<FieldSpec> FieldDefinitions => fieldDefinitions.ToList();
        public void AddFieldDef(FieldSpec fs)
        {
            fieldDefinitions.Add(fs);
        }


        private List<StaticMethodDef> staticMethodDefinitions = new List<StaticMethodDef>();
        public IEnumerable<StaticMethodDef> StaticMethodDefinitions => staticMethodDefinitions.ToList();
        public void AddStaticMethodDef(StaticMethodDef md)
        {
            staticMethodDefinitions.Add(md);
        }

        private List<ConstructorDef> constructorDefinitions = new List<ConstructorDef>();
        public IEnumerable<ConstructorDef> Constructors => constructorDefinitions.ToList();
        public void AddConstructorDef(ConstructorDef cd)
        {
            constructorDefinitions.Add(cd);
        }
    }
}
