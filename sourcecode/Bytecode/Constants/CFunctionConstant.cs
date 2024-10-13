using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Bytecode
{
    public class CFunctionConstant : AConstant
    {
        public CFunctionConstant(ulong id, IConstantRef<StringConstant> libraryNameConstant, IConstantRef<StringConstant> functionNameConstant, IConstantRef<TypeParametersConstant> typeParametersConstant, IConstantRef<TypeListConstant> argumentTypesConstant, IConstantRef<ITypeConstant> returnTypeConstant) : base(ConstantType.CFunctionConstant, id)
        {
            LibraryNameConstant = libraryNameConstant;
            FunctionNameConstant = functionNameConstant;
            TypeParametersConstant = typeParametersConstant;
            ArgumentTypesConstant = argumentTypesConstant;
            ReturnTypeConstant = returnTypeConstant;
        }

        public IConstantRef<StringConstant> LibraryNameConstant { get; }
        public IConstantRef<StringConstant> FunctionNameConstant { get; }
        public IConstantRef<TypeParametersConstant> TypeParametersConstant { get; }
        public IConstantRef<TypeListConstant> ArgumentTypesConstant { get; }
        public IConstantRef<ITypeConstant> ReturnTypeConstant { get; }
        
        public override IEnumerable<IConstant> Dependencies
        {
            get
            {
                yield return LibraryNameConstant.Constant;
                yield return FunctionNameConstant.Constant;
                yield return TypeParametersConstant.Constant;
                yield return ArgumentTypesConstant.Constant;
                yield return ReturnTypeConstant.Constant;
                yield break;
            }
        }

        public override void EmitBody(Stream s)
        {
            s.WriteValue(LibraryNameConstant.ConstantID);
            s.WriteValue(FunctionNameConstant.ConstantID);
            s.WriteValue(TypeParametersConstant.ConstantID);
            s.WriteValue(ArgumentTypesConstant.ConstantID);
            s.WriteValue(ReturnTypeConstant.ConstantID);
        }
        public static CFunctionConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            ulong source = s.ReadULong();
            ulong name = s.ReadULong();
            ulong typeArgs = s.ReadULong();
            ulong argTypes = s.ReadULong();
            ulong returnType = s.ReadULong();
            return new CFunctionConstant(id, rcs.ReferenceStringConstant(source), rcs.ReferenceStringConstant(name), rcs.ReferenceTypeParametersConstant(typeArgs), rcs.ReferenceTypeListConstant(argTypes), rcs.ReferenceTypeConstant(returnType));
        }
    }
}
