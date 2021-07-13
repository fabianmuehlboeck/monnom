using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class ClassDef : InterfaceDef
    {
        public IOptional<InheritanceDecl> SuperClass
        {
            get;
            protected set;
        }

        public IEnumerable<FieldDecl> Fields
        {
            get;
            private set;
        }

        private IEnumerable<Constructor> constructors;
        public override IEnumerable<Constructor> Constructors
        {
            get
            {
                return constructors;
            }
        }

        public IEnumerable<MethodDef> ImplementedMethods
        {
            get;
            set;
        }

        public IEnumerable<StaticMethodDef> StaticMethods
        {
            get;
            set;
        }
        public IEnumerable<StaticFieldDecl> StaticFields
        {
            get;
            set;
        }

        public IEnumerable<InstanceDef> Instances
        {
            get;
            set;
        }

        public bool IsFinal
        {
            get;
            private set;
        }

        public bool IsAbstract
        {
            get;
            private set;
        }

        public bool IsSpecial
        {
            get;
            private set;
        }

        public IEnumerable<ClassDef> Classes
        {
            get;
            private set;
        }
        
        public ClassDef(DeclIdentifier name, IOptional<InheritanceDecl> extends, IEnumerable<InheritanceDecl> implements, IEnumerable<MethodDef> methods, IEnumerable<FieldDecl> fields, IEnumerable<Constructor> constructors, IEnumerable<StaticFieldDecl> staticFields, IEnumerable<StaticMethodDef> staticMethods, IEnumerable<InstanceDef> instances, bool isFinal, VisibilityNode visibility, bool isAbstract, bool isPartial, bool isShape, bool isMaterial, IEnumerable<InterfaceDef> interfaces, IEnumerable<ClassDef> classes, ISourceSpan locs, bool isSpecial = false)
            : base(name,implements, methods, visibility, isPartial, isShape, isMaterial, interfaces, locs)
        {
            this.SuperClass = extends;
            this.Fields = fields.Where(x=>x!=null).ToList();
            this.constructors = constructors.Where(x => x != null).ToList();
            this.ImplementedMethods = methods.Where(x => x != null).ToList();
            this.Classes = classes.Where(x => x != null).ToList();
            this.Instances = instances.Where(x => x != null).ToList();
            this.StaticMethods = staticMethods.Where(x => x != null).ToList();
            this.StaticFields = staticFields.Where(x => x != null).ToList();
            this.IsFinal = isFinal;
            this.IsAbstract = isAbstract;
            this.IsSpecial = isSpecial;
            foreach (FieldDecl fd in Fields)
            {
                fd.Class = this;
            }
            foreach (StaticFieldDecl fd in StaticFields)
            {
                fd.Class = this;
            }
        }
                        
        public override T Visit<T>(Func<InterfaceDef, T> ifun, Func<ClassDef, T> cfun)
        {
            return cfun(this);
        }

        public override R VisitNamedTypeDef<S, R>(INamedTypeDefVisitor<S, R> visitor, S state)
        {
            return visitor.VisitClassDef(this, state);
        }
        public override void PrettyPrint(PrettyPrinter p)
        {
            StringBuilder sb = new StringBuilder();
            if(IsAbstract)
            {
                sb.Append("abstract ");
            }
            if(IsFinal)
            {
                sb.Append("final ");
            }
            if (IsPartial)
            {
                sb.Append("partial ");
            }
            if (IsShape)
            {
                sb.Append("shape ");
            }
            if (IsMaterial)
            {
                sb.Append("material ");
            }
            sb.Append("class");
            p.Write(sb.ToString(), Start);
            p.WriteWhitespace();
            p.IncreaseIndent();
            Name.PrettyPrint(p);
            p.WriteWhitespace();
            if(SuperClass.HasElem)
            {
                p.WriteKeyword("extends");
                SuperClass.ActionBind(sc => sc.PrettyPrint(p));
                p.WriteWhitespace();
            }
            if (Implements.Any())
            {
                p.WriteKeyword("implements");
                p.PrintMembers(Implements, ",");
            }
            p.DecreaseIndent();
            p.WriteLine();
            p.WritePunctuation("{");
            p.IncreaseIndent();
            p.WriteLine();
            p.PrintMembers(this.ImplementedMethods.Cast<IAstNode>().Concat(this.Interfaces).Concat(this.Classes).Concat(this.Fields).Concat(this.StaticFields).Concat(this.StaticMethods).Concat(this.Constructors).Concat(this.Instances));
            p.DecreaseIndent();
            p.WriteLine();
            p.WritePunctuation("}");
            p.WriteLine();
        }
    }

    public partial interface INamedTypeDefVisitor<in S, out R>
    {
        Func<ClassDef, S, R> VisitClassDef { get; }
    }
}
