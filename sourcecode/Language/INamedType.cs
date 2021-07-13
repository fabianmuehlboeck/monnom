using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface INamedType : IType, ISubstitutable<INamedType>, IParamRef<IInterfaceSpec, ITypeArgument>
    {
        IOptional<INamedType> Instantiate(IInterfaceSpec iface);
        

        /// <summary>
        /// Given two instantiations of the same class/interface type, creates a 
        /// new class/interface type whose type arguments represent the meet of 
        /// the arguments of the two given types.
        /// </summary>
        /// <param name="other">Must be referring to same class/interface</param>
        /// <returns>The meet of the two given types</returns>
        INamedType MeetInstantiation(INamedType other);
    }
}
