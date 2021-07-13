using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;
using Nom.Language;

namespace Nom.Bytecode
{
    public class ClassRep : InterfaceRep, IClassSpec
    {
        public ClassRep(IConstantRef<StringConstant> name, IConstantRef<TypeParametersConstant> typeArgConstraints, IConstantRef<SuperClassConstant> superClass, IConstantRef<SuperInterfacesConstant> superInterfaces, bool isFinal, bool isAbstract, bool isShape, Visibility visibility, AssemblyUnit au) : base(name, typeArgConstraints, superInterfaces, isShape, visibility, au)
        {
            IsFinal = isFinal;
            IsAbstract = isAbstract;
            SuperClassConstant = superClass;
        }

        private List<ClassRep> classes = new List<ClassRep>();
        public void AddClass(ClassRep cr)
        {
            classes.Add(cr);
        }

        public IConstantRef<SuperClassConstant> SuperClassConstant { get; }
        public IOptional<IParamRef<IClassSpec, IType>> SuperClass { get; }

        public bool IsFinal { get; }

        public bool IsPartial => false;

        public bool IsAbstract { get; }

        private List<MethodDefRep> methods = new List<MethodDefRep>();
        public IEnumerable<IMethodRep> MethodDefs => methods;
        public void AddMethodDef(MethodDefRep mdr)
        {
            methods.Add(mdr);
        }

        private List<StaticMethodDefRep> staticMethods = new List<StaticMethodDefRep>();
        public IEnumerable<IStaticMethodSpec> StaticMethods => staticMethods;
        public void AddStaticMethod(StaticMethodDefRep smd)
        {
            staticMethods.Add(smd);
        }

        private List<FieldRep> fields = new List<FieldRep>();
        public IEnumerable<IFieldSpec> Fields => fields;

        public void AddField(FieldRep fr)
        {
            fields.Add(fr);
        }

        private List<LambdaRep> lambdas = new List<LambdaRep>();
        public void AddLambda(LambdaRep lambda)
        {
            lambdas.Add(lambda);
        }

        private List<StructRep> structs = new List<StructRep>();
        public void AddStruct(StructRep structrep)
        {
            structs.Add(structrep);
        }

        public IEnumerable<IStaticFieldSpec> StaticFields => throw new NotImplementedException();

        private List<ConstructorDefRep> constructors = new List<ConstructorDefRep>();
        public IEnumerable<IConstructorSpec> Constructors => constructors;

        public void AddConstructor(ConstructorDefRep cdr)
        {
            constructors.Add(cdr);
        }

        private List<InstanceDefRep> instances = new List<InstanceDefRep>();
        public IEnumerable<IInstanceSpec> Instances => throw new NotImplementedException();

        protected override BytecodeTopElementType BytecodeType => BytecodeTopElementType.Class;
        protected override IEnumerable<bool> GetFlagBools()
        {
            foreach (bool b in base.GetFlagBools())
            {
                yield return b;
            }
            yield return IsAbstract;
            yield return IsFinal;
        }
        protected override void EmitHeader(Stream ws)
        {
            base.EmitHeader(ws);
            ws.WriteValue(SuperClassConstant.ConstantID);
            //ws.WriteByte((byte)BytecodeTopElementType.Class);
        }
        protected override void EmitBody(Stream ws)
        {
            base.EmitBody(ws);
            UInt64 fieldCount = (ulong)fields.LongCount();
            ws.WriteValue(fieldCount);
            foreach (FieldRep fr in fields)
            {
                fr.WriteByteCode(ws);
            }
            UInt64 staticMethodCount = (ulong)staticMethods.LongCount();
            ws.WriteValue(staticMethodCount);
            foreach (StaticMethodDefRep smd in staticMethods)
            {
                smd.WriteByteCode(ws);
            }
            UInt64 constructorCount = (ulong)constructors.LongCount();
            ws.WriteValue(constructorCount);
            foreach (ConstructorDefRep cdr in constructors)
            {
                cdr.WriteByteCode(ws);
            }
            UInt64 lambdaCount = (ulong)lambdas.LongCount();
            ws.WriteValue(lambdaCount);
            foreach (LambdaRep lambda in lambdas)
            {
                lambda.WriteByteCode(ws);
            }

            UInt64 structCount = (ulong)structs.LongCount();
            ws.WriteValue(structCount);
            foreach (StructRep structrep in structs)
            {
                structrep.WriteByteCode(ws);
            }
        }
        protected override void EmitChildren(Stream ws)
        {
            base.EmitChildren(ws);
            foreach(ClassRep cr in classes)
            {
                cr.Emit(ws);
            }
        }
        new public static ClassRep Read(Stream ws, IReadConstantSource rcs)
        {
            var nameConstant = rcs.ReferenceStringConstant(ws.ReadULong());
            var tpconstraints = rcs.ReferenceTypeParametersConstant(ws.ReadULong());
            Visibility visibility = (Visibility)ws.ReadByte();
            IEnumerable<bool> flags = ws.Decompress(3);
            var isShape = flags.First();
            var isAbstract = flags.ElementAt(1);
            var isFinal = flags.ElementAt(2);
            var superInterfaces = rcs.ReferenceSuperInterfacesConstant(ws.ReadULong());
            var superClass = rcs.ReferenceSuperClassConstant(ws.ReadULong());
            var cls = new ClassRep(nameConstant, tpconstraints, superClass, superInterfaces, isFinal, isAbstract, isShape, visibility, null);
            UInt64 methodCount = ws.ReadULong();
            for (UInt64 i = 0; i < methodCount; i++)
            {
                cls.AddMethodDecl(MethodDeclRep.Read(cls, ws, rcs));
            }
            UInt64 fieldCount = ws.ReadULong();
            for (UInt64 i = 0; i < fieldCount; i++)
            {
                cls.AddField(FieldRep.Read(cls, ws, rcs));
            }
            UInt64 staticMethodCount = ws.ReadULong();
            for (UInt64 i = 0; i < staticMethodCount; i++)
            {
                cls.AddStaticMethod(StaticMethodDefRep.Read(cls, ws, rcs));
            }
            UInt64 constructorCount = ws.ReadULong();
            for (UInt64 i = 0; i < constructorCount; i++)
            {
                cls.AddConstructor(ConstructorDefRep.Read(cls, ws, rcs));
            }
            UInt64 lambdaCount = ws.ReadULong();
            for (UInt64 i = 0; i < lambdaCount; i++)
            {
                cls.AddLambda(LambdaRep.Read(cls, ws, rcs));
            }

            UInt64 structCount = ws.ReadULong();
            for (UInt64 i = 0; i < structCount; i++)
            {
                cls.AddStruct(StructRep.Read(cls, ws, rcs));
            }
            return cls;
        }

        public override IEnumerable<MethodDeclRep> Methods => base.Methods.Concat(methods);
        public IParamRef<IClassSpec, IType> MakeClassRef(ITypeEnvironment<IType> env)
        {
            throw new NotImplementedException();
        }

        //public IClassTypeConstant SuperTypeConstant
        //{
        //    get;
        //    set;
        //}

        //public readonly ClassTypeRep SuperClass;
        //public readonly IEnumerable<FieldRep> Fields;
        //public readonly IEnumerable<MethodDefRep> Methods;
        //public readonly IEnumerable<StaticMethodDefRep> StaticMethods;

        //public static new ClassRep Create(String name)
        //{
        //    return new ClassRep(new StringConstant(name));
        //}

        //public static new ClassRep Create(IStringConstant nameConstant)
        //{
        //    return new ClassRep(nameConstant);
        //}

        //private ClassRep(IStringConstant nameConstant) : base(nameConstant)
        //{
        //}

        ////public override void Emit(Stream ws)
        ////{
        ////    using (MemoryStream s = new MemoryStream())
        ////    {
        ////        byte[] buf;
        ////        buf = Encoding.ASCII.GetBytes("C");
        ////        s.Write(buf, 0, buf.Length);
        ////        buf = BitConverter.GetBytes(Constants.GetStringConstant(QualifiedName.ToString()));
        ////        s.Write(buf, 0, buf.Length);
        ////        buf = BitConverter.GetBytes(TypeArgs.Count());
        ////        s.Write(buf, 0, buf.Length);
        ////        TypeArgRep.WriteListByteCode(this.TypeArgs, s);
        ////        buf = BitConverter.GetBytes(Constants.GetClassTypeConstant(SuperClass));
        ////        s.Write(buf, 0, buf.Length);
        ////        buf = BitConverter.GetBytes(Constants.GetClassTypeListConstant(SuperInterfaces));
        ////        s.Write(buf, 0, buf.Length);
        ////        buf = BitConverter.GetBytes(Fields.Count());
        ////        s.Write(buf, 0, buf.Length);
        ////        FieldRep.WriteListByteCode(Fields, s);

        ////        foreach(MethodDefRep method in Methods)
        ////        {

        ////        }
        ////        foreach(StaticMethodDefRep method in StaticMethods)
        ////        {
        ////            method.WriteByteCode(s);
        ////        }

        ////        buf = BitConverter.GetBytes(s.Length);
        ////        ws.Write(buf, 0, buf.Length);
        ////        s.CopyTo(ws);
        ////    }
        ////}

        ////public string ToByteCode()
        ////{
        ////    using (MemoryStream stream = new MemoryStream())
        ////    {
        ////        Emit(stream);
        ////         return stream.ToString();
        ////    }
        ////}
    }
}
