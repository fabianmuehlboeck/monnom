using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Parser;
using Nom.TypeChecker;
using Nom.Project;
using Nom.Language.SpecExtensions;

namespace Nom.Bytecode
{
    public class CodeGenerator
    {
        private readonly NamespaceRep GlobalNamespace = new NamespaceRep("", Optional<Language.INamespaceSpec>.Empty);
        public AssemblyUnit GenerateBytecode(Nom.TypeChecker.Program program, NomProject project)
        {
            AssemblyUnit au = new AssemblyUnit(project);
            //foreach(TDClassDef tdcd in program.Classes)
            //{
            //    ClassRep cr = ClassRep.Create(tdcd.Name.Name.Name);
            //    if(tdcd.Parent!=null)
            //    {
            //        cr.ParentConstant = tdcd.Parent.Visit(NamespaceConstantVisitor.Instance).InjectOptional();
            //    }
            //    cr.TypeArgumentCount = 0; //TODO: Implement
            //    cr.SuperTypeConstant = tdcd.Extends.Elem.
            //    foreach(TDMethodDef tdmd in tdcd.DeclaredMethods)
            //    {
            //        MethodDefRep mdr = new MethodDefRep();
            //    }
            //}
            //BytecodeUnit bcu = new BytecodeUnit(/*ccls.FullQualifiedName*/ manifest.MainClass.Elem, au);
            ConvertNamespace(program.GlobalNamespace, GlobalNamespace, au /*bcu*/);
            //au.AddUnit(bcu);
            return au;
        }

        public void ConvertNamespace(Nom.TypeChecker.INamespace ns, NamespaceRep nsr, AssemblyUnit au /*BytecodeUnit bcu*/)
        {
            foreach (TypeChecker.INamespace cns in ns.Namespaces)
            {
                NamespaceRep cnsr = new NamespaceRep(cns.Name, nsr.InjectOptional());
                nsr.AddNamespace(cnsr);
                ConvertNamespace(cns, cnsr, au /*bcu*/);
            }
            foreach (TypeChecker.IInterface cifc in ns.Interfaces)
            {
                ConvertInterface(cifc, au /*bcu*/);
            }
            foreach (TypeChecker.IClass ccls in ns.Classes)
            {
                ConvertClass(ccls, au /*bcu*/);
            }
        }

        public InterfaceRep ConvertInterface(Nom.TypeChecker.IInterface ifc, AssemblyUnit au /*BytecodeUnit bcu*/)
        {
            BytecodeUnit bcu = new BytecodeUnit(ifc.FullQualifiedName, au);
            InterfaceRep ir = new InterfaceRep(bcu.GetStringConstant(ifc.FullQualifiedName), bcu.GetTypeParametersConstant(ifc.AllTypeParameters), bcu.GetSuperInterfacesConstant(ifc.Implements), ifc.IsShape, ifc.Visibility, bcu.AssemblyUnit);
            foreach (Language.IMethodSpec method in ifc.Methods)
            {
                MethodDeclRep mdr = new MethodDeclRep(bcu.GetStringConstant(method.Name), bcu.GetTypeParametersConstant(method.TypeParameters), bcu.GetTypeConstant(method.ReturnType), bcu.GetTypeListConstant(method.Parameters.Entries.Select(ps => ps.Type)), method.Visibility, method.IsFinal);
                ir.AddMethodDecl(mdr);
            }
            bcu.AddInterface(ir);
            au.AddUnit(bcu);
            foreach (TypeChecker.IInterface cifc in ifc.Interfaces)
            {
                ConvertInterface(cifc, au /*bcu*/);
            }
            return ir;
        }

        public ClassRep ConvertClass(Nom.TypeChecker.IClass cls, AssemblyUnit au /*BytecodeUnit bcu*/)
        {
            BytecodeUnit bcu = new BytecodeUnit(cls.FullQualifiedName, au);
            ClassRep cr = new ClassRep(bcu.GetStringConstant(cls.FullQualifiedName), bcu.GetTypeParametersConstant(cls.AllTypeParameters/*.SelectMany(tp=>tp.LowerBound.Singleton().Snoc(tp.UpperBound))*/), bcu.GetSuperClassConstant(cls.SuperClass.Elem), bcu.GetSuperInterfacesConstant(cls.GetParamRef<Language.IClassSpec, Language.IType>().AllImplementedInterfaces().Distinct(Language.ParamRefEqualityComparer<Language.IInterfaceSpec, Language.IType>.Instance)), cls.IsFinal, false, cls.IsShape, cls.Visibility, bcu.AssemblyUnit);
            foreach (TypeChecker.IFieldDecl fd in cls.Fields)
            {
                FieldRep fr = new FieldRep(cr, bcu.GetStringConstant(fd.Name), bcu.GetTypeConstant(fd.Type), fd.IsReadonly, fd.IsVolatile, fd.Visibility);
                cr.AddField(fr);
            }
            foreach (TypeChecker.IStaticMethodDef smd in cls.StaticMethods)
            {
                List<IInstruction> instructions = new List<IInstruction>();
                foreach (TypeChecker.IInstruction instruction in smd.Instructions)
                {
                    instructions.Add(instruction.Visit(InstructionConverter.Instance, bcu));
                }
                //TODO: fix type variable and parameter lists
                StaticMethodDefRep smdr = new StaticMethodDefRep(bcu.GetStringConstant(smd.Name), bcu.GetTypeConstant(smd.ReturnType), bcu.GetTypeParametersConstant(smd.TypeParameters), bcu.GetTypeListConstant(smd.Parameters.Entries.Select(ps => ps.Type)), smd.Visibility, instructions, smd.RegisterCount);
                cr.AddStaticMethod(smdr);
            }
            foreach (TypeChecker.IConstructorDef cd in cls.Constructors)
            {
                List<IInstruction> preInstructions = new List<IInstruction>();
                foreach (TypeChecker.IInstruction instruction in cd.PreInstructions)
                {
                    preInstructions.Add(instruction.Visit(InstructionConverter.Instance, bcu));
                }
                List<IInstruction> postInstructions = new List<IInstruction>();
                foreach (TypeChecker.IInstruction instruction in cd.PostInstructions)
                {
                    postInstructions.Add(instruction.Visit(InstructionConverter.Instance, bcu));
                }
                ConstructorDefRep cdr = new ConstructorDefRep(bcu.GetTypeListConstant(cd.Parameters.Entries.Select(ps => ps.Type)), cd.Visibility, preInstructions, cd.SuperConstructorArgs.Select(sca => sca.Index), postInstructions, cd.RegisterCount);
                cr.AddConstructor(cdr);
            }
            foreach (TypeChecker.IMethodDef method in cls.Methods)
            {
                List<IInstruction> instructions = new List<IInstruction>();
                foreach (TypeChecker.IInstruction instruction in method.Instructions)
                {
                    instructions.Add(instruction.Visit(InstructionConverter.Instance, bcu));
                }
                MethodDefRep mdr = new MethodDefRep(bcu.GetStringConstant(method.Name), bcu.GetTypeParametersConstant(method.TypeParameters), bcu.GetTypeConstant(method.ReturnType), bcu.GetTypeListConstant(method.Parameters.Entries.Select(ps => ps.Type)), method.Visibility, method.IsFinal, method.RegisterCount, instructions);
                cr.AddMethodDef(mdr);
            }
            foreach (TypeChecker.ITDLambda lambda in cls.Lambdas)
            {
                List<IInstruction> instructions = new List<IInstruction>();
                foreach (TypeChecker.IInstruction instruction in lambda.Instructions)
                {
                    instructions.Add(instruction.Visit(InstructionConverter.Instance, bcu));
                }
                LambdaRep lr = new LambdaRep(bcu.GetLambdaConstant(lambda), bcu.GetTypeParametersConstant(lambda.ClosureTypeParameters), bcu.GetTypeListConstant(lambda.Fields.Select(f=>f.Type)), lambda.Fields.Select(f => new LambdaFieldRep(bcu.GetStringConstant(f.Name), bcu.GetTypeConstant(f.Type))), bcu.GetTypeParametersConstant(lambda.TypeParameters), bcu.GetTypeConstant(lambda.ReturnType), bcu.GetTypeListConstant(lambda.Parameters.Entries.Select(ps => ps.Type)), lambda.RegisterCount, instructions);
                cr.AddLambda(lr);
            }
            foreach (TypeChecker.ITDStruct structdef in cls.Structs)
            {
                StructRep sr = new StructRep(bcu.GetStructConstant(structdef), bcu.GetTypeParametersConstant(structdef.ClosureTypeParameters), structdef.Fields.Select(f => new StructFieldRep(bcu.GetStringConstant(f.Name), bcu.GetTypeConstant(f.Type), f.IsReadOnly, f.InitializerExpr.Register.Index, f.InitializerExpr.Select(instr=>instr.Visit(InstructionConverter.Instance, bcu)))), bcu.GetTypeListConstant(structdef.InitializerArgs.Select(vr=>vr.Type)), structdef.InitializerRegisterCount, structdef.EndArgRegisterCount);
                foreach (var smethod in structdef.Methods)
                {
                    List<IInstruction> instructions = new List<IInstruction>();
                    foreach (TypeChecker.IInstruction instruction in smethod.Instructions)
                    {
                        instructions.Add(instruction.Visit(InstructionConverter.Instance, bcu));
                    }
                    MethodDefRep mdr = new MethodDefRep(bcu.GetStringConstant(smethod.Name), bcu.GetTypeParametersConstant(smethod.TypeParameters), bcu.GetTypeConstant(smethod.ReturnType), bcu.GetTypeListConstant(smethod.Parameters.Entries.Select(ps=>ps.Type)), Visibility.Public, true, smethod.RegisterCount, instructions);
                    sr.AddMethodDef(mdr);
                }
                cr.AddStruct(sr);
            }
            bcu.AddClass(cr);
            au.AddUnit(bcu);
            foreach (TypeChecker.IInterface cifc in cls.Interfaces)
            {
                ConvertInterface(cifc, au /*bcu*/);
            }
            foreach (TypeChecker.IClass ccls in cls.Classes)
            {
                ConvertClass(ccls, au /*bcu*/);
            }
            return cr;
        }

        private class InstructionConverter : IInstructionVisitor<BytecodeUnit, IInstruction>
        {
            private InstructionConverter() { }
            public static InstructionConverter Instance = new InstructionConverter();
            public Func<TypeChecker.DebugInstruction, BytecodeUnit, IInstruction> VisitDebugInstruction => (instr, bcu) => new DebugInstruction(bcu.GetStringConstant(instr.Message));

            public Func<TypeChecker.ReturnVoidInstruction, BytecodeUnit, IInstruction> VisitReturnVoidInstruction => (instr, bcu) => new ReturnVoidInstruction();

            public Func<TypeChecker.CastInstruction, BytecodeUnit, IInstruction> VisitCastInstruction => (instr, bcu) => new CastInstruction(instr.Register.Index, instr.Argument.Index, bcu.GetTypeConstant(instr.Type));

            public Func<ReadInstanceFieldInstruction, BytecodeUnit, IInstruction> VisitReadInstanceFieldInstruction => (instr, bcu) => new ReadFieldInstruction(instr.Register.Index, instr.Receiver.Index, bcu.GetStringConstant(instr.Field.Name), bcu.GetClassConstant(instr.Field.Container));

            public Func<CallStaticMethodCheckedInstruction, BytecodeUnit, IInstruction> VisitCallStaticMethodCheckedInstruction => (instr, bcu) =>
            new CallCheckedStaticMethodInstruction(bcu.GetStaticMethodConstant(instr.Method), bcu.GetTypeListConstant(instr.ActualParameters), instr.Arguments.Select(reg => reg.Index), instr.Register.Index);

            public Func<BoolFalseInstruction, BytecodeUnit, IInstruction> VisitBoolFalseInstruction => (instr, bcu) => new LoadBoolConstantInstruction(false, instr.Register.Index);

            public Func<CallConstructorCheckedInstruction, BytecodeUnit, IInstruction> VisitCallConstructorCheckedInstruction => (instr, bcu) => 
            new CallCheckedConstructorInstruction(bcu.GetSuperClassConstant(instr.Constructor.Element.Container.MakeClassRef(instr.Constructor.Substitutions.Transform(t=>t.AsType))), bcu.GetTypeListConstant(instr.ActualParameters), instr.Arguments.Select(reg => reg.Index), instr.Register.Index);

            public Func<CallInstanceMethodCheckedInstruction, BytecodeUnit, IInstruction> VisitCallInstanceMethodCheckedInstruction => (instr, bcu) => new CallCheckedInstanceMethodInstruction(bcu.GetMethodConstant(instr.Method), bcu.GetTypeListConstant(instr.ActualParameters), instr.Arguments.Select(reg => reg.Index), instr.Register.Index, instr.Receiver.Index);

            public Func<WriteInstanceFieldInstruction, BytecodeUnit, IInstruction> VisitWriteInstanceFieldInstruction => (instr, bcu) => new WriteFieldInstruction(instr.Value.Index, instr.Receiver.Index, bcu.GetStringConstant(instr.Field.Name), bcu.GetClassConstant(instr.Field.Container));

            public Func<ReadStaticFieldInstruction, BytecodeUnit, IInstruction> VisitReadStaticFieldInstruction => (instr, bcu) => throw new NotImplementedException();

            public Func<WriteExpandoFieldInstruction, BytecodeUnit, IInstruction> VisitWriteExpandoFieldInstruction => (instr, bcu) => new WriteFieldInstruction(instr.Value.Index, instr.Receiver.Index, bcu.GetStringConstant(instr.FieldName), bcu.GetEmptyClassConstant());

            public Func<BoolTrueInstruction, BytecodeUnit, IInstruction> VisitBoolTrueInstruction => (instr, bcu) => new LoadBoolConstantInstruction(true, instr.Register.Index);

            public Func<ReadExpandoFieldInstruction, BytecodeUnit, IInstruction> VisitReadExpandoFieldInstruction => (instr, bcu) => new ReadFieldInstruction(instr.Register.Index, instr.Receiver.Index, bcu.GetStringConstant(instr.FieldName), bcu.GetEmptyClassConstant());

            public Func<WriteStaticFieldInstruction, BytecodeUnit, IInstruction> VisitWriteStaticFieldInstruction => (instr, bcu) => throw new NotImplementedException();

            public Func<TypeChecker.ReturnInstruction, BytecodeUnit, IInstruction> VisitReturnInstruction => (instr, bcu) => new ReturnInstruction(instr.Register.Index);

            public Func<TypeChecker.LoadIntConstantInstruction, BytecodeUnit, IInstruction> VisitLoadIntConstantInstruction => (instr, bcu) => new LoadIntConstantInstruction(instr.Value, instr.Register.Index);

            public Func<TypeChecker.LoadStringConstantInstruction, BytecodeUnit, IInstruction> VisitLoadStringConstantInstruction => (instr, bcu) => new LoadStringConstantInstruction(bcu.GetStringConstant(instr.Value), instr.Register.Index);

            public Func<TypeChecker.BinOpInstruction, BytecodeUnit, IInstruction> VisitBinOpInstruction => (instr, bcu) => new BinOpInstruction(instr.Operator, instr.Left.Index, instr.Right.Index, instr.Register.Index);

            public Func<TypeChecker.CondBranchInstruction, BytecodeUnit, IInstruction> VisitCondBranchInstruction => (instr, bcu) => new CondBranchInstruction(instr.Condition.Index, instr.ThenTarget.Index, instr.ElseTarget.Index, instr.GetThenIncomings().Select(p => (p.Item1.Index, p.Item2.Index)), instr.GetElseIncomings().Select(p => (p.Item1.Index, p.Item2.Index)));
            public Func<TypeChecker.BranchInstruction, BytecodeUnit, IInstruction> VisitBranchInstruction => (instr, bcu) => new BranchInstruction(instr.Target.Index, instr.GetIncomings().Select(p => (p.Item1.Index, p.Item2.Index)));

            public Func<TypeChecker.PhiNode, BytecodeUnit, IInstruction> VisitPhiNode => (instr, bcu) => new PhiNode(instr.GetRegisters().Select(p => (p.Item1.Index, bcu.GetTypeConstant(p.Item2))), instr.IncomingCount);

            public Func<TypeChecker.CallExpandoMethodInstruction, BytecodeUnit, IInstruction> VisitCallExpandoMethodInstruction => (instr, bcu) => new CallExpandoMethodInstruction(instr.Receiver.Index, bcu.GetStringConstant(instr.MethodName), bcu.GetTypeListConstant(instr.TypeArguments), instr.Arguments.Select(arg => arg.Index), instr.Register.Index);

            public Func<TypeChecker.CreateClosureInstruction, BytecodeUnit, IInstruction> VisitCreateClosureInstruction => (instr, bcu) => new CreateClosureInstruction(bcu.GetLambdaConstant(instr.Lambda), bcu.GetTypeListConstant(instr.TypeArgs), instr.Arguments.Select(arg => arg.Index), instr.Register.Index);

            public Func<TypeChecker.ReadLambdaFieldInstruction, BytecodeUnit, IInstruction> VisitReadLambdaFieldInstruction => (instr, bcu) => new ReadFieldInstruction(instr.Register.Index, 0, bcu.GetStringConstant(instr.Field.Name), bcu.GetLambdaConstant(instr.Field.Lambda));

            public Func<TypeChecker.ReadStructFieldInstruction, BytecodeUnit, IInstruction> VisitReadStructFieldInstruction => (instr, bcu) => new ReadFieldInstruction(instr.Register.Index, 0, bcu.GetStringConstant(instr.Field.Name), bcu.GetStructConstant(instr.Field.Struct));

            public Func<TypeChecker.ConstructStructInstruction, BytecodeUnit, IInstruction> VisitConstructStructInstruction => (instr, bcu) => new ConstructStructInstruction(bcu.GetStructConstant(instr.Struct), bcu.GetTypeListConstant(instr.TypeArguments), instr.Arguments.Select(arg => arg.Index), instr.Register.Index);

            public Func<TypeChecker.ErrorInstruction, BytecodeUnit, IInstruction> VisitErrorInstruction => (instr, bcu) => new ErrorInstruction(instr.Register.Index);

            public Func<TypeChecker.RuntimeCmdInstruction, BytecodeUnit, IInstruction> VisitRuntimeCmdInstruction => (instr, bcu) => new RuntimeCmdInstruction(bcu.GetStringConstant(instr.Cmd));

            public Func<TypeChecker.LoadFloatConstantInstruction, BytecodeUnit, IInstruction> VisitLoadFloatConstantInstruction => (instr, bcu) => new LoadFloatConstantInstruction(instr.Value, instr.Register.Index);

            public Func<NullInstruction, BytecodeUnit, IInstruction> VisitNullInstruction => (instr, bcu) => new LoadNullConstantInstruction(instr.Register.Index);

            public Func<TypeChecker.UnaryOpInstruction, BytecodeUnit, IInstruction> VisitUnaryOpInstruction => (instr, bcu) => new UnaryOpInstruction(instr.Operator, instr.Arg.Index, instr.Register.Index);

            public Func<TypeChecker.EnsureCheckedMethodInstruction, BytecodeUnit, IInstruction> VisitEnsureCheckedMethodInstruction => (instr, bcu) => new EnsureCheckedMethodInstruction(bcu.GetStringConstant(instr.MethodName), instr.Receiver.Index);

            public Func<TypeChecker.EnsureDynamicMethodInstruction, BytecodeUnit, IInstruction> VisitEnsureDynamicMethodInstruction => (instr, bcu) => new EnsureDynamicMethodInstruction(bcu.GetStringConstant(instr.MethodName), instr.Receiver.Index);

            public Func<TypeChecker.CallCFunctionInstruction, BytecodeUnit, IInstruction> VisitCallCFunctionInstruction => (instr, bcu) =>
            {
                return new CallCFunctionInstruction(bcu.GetCFunctionConstant(instr.CFunction), bcu.GetTypeListConstant(instr.TypeArguments), instr.Arguments.Select(arg=>arg.Index), instr.Register.Index);
            };
        }

        //private class NamespaceConstantVisitor : ITDNamespaceVisitor<object, INamespaceConstant>
        //{
        //    private NamespaceConstantVisitor()
        //    {

        //    }
        //    public static readonly NamespaceConstantVisitor Instance = new NamespaceConstantVisitor();

        //    public INamespaceConstant VisitClass(TDClassDef tdcd, object a = null)
        //    {
        //        ClassConstant cc = ClassConstant.Create(tdcd.Name.Name.Name);
        //        cc.TypeArgumentCount = 0; //TODO: Implement
        //        cc.LibraryNameConstant = new StringConstant(tdcd.LibraryName);
        //        if(tdcd.Parent!=null)
        //        {
        //            cc.ParentConstant = tdcd.Parent.Visit(this).InjectOptional();
        //        }
        //        return cc;
        //    }

        //    public INamespaceConstant VisitInterface(TDInterfaceDef tdid, object a = null)
        //    {
        //        InterfaceConstant ic = InterfaceConstant.Create(tdid.Name.Name.Name);
        //        ic.TypeArgumentCount = 0; //TODO: Implement
        //        ic.LibraryNameConstant = new StringConstant(tdid.LibraryName);
        //        if (tdid.Parent != null)
        //        {
        //            ic.ParentConstant = tdid.Parent.Visit(this).InjectOptional();
        //        }
        //        return ic;
        //    }

        //    public INamespaceConstant VisitNamespace(TDNamespace tdns, object a = null)
        //    {
        //        NamespaceConstant nc = NamespaceConstant.Create(tdns.Name.Name.Name);
        //        if(tdns.Parent!=null)
        //        {
        //            nc.ParentConstant = tdns.Parent.Visit(this).InjectOptional();
        //        }
        //        return nc;
        //    }
        //}
    }
}
