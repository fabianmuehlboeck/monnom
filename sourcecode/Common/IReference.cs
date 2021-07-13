using System;
using System.Collections.Generic;
using System.Text;

namespace Nom
{
    public interface IReference : IPrettyPrintable
    {
    }

    public interface IPositionalReference : IReference
    {
        string ReferencePosition { get; }
    }

    public interface INamedReference : IReference
    {
        string ReferenceName { get; }
    }

    public interface IPosNamedReference :IPositionalReference, INamedReference
    {
        string ReferencePosName { get; }
    }

    public static class ReferenceExtensions
    {
        public static string GetReferencePosition(this IReference r)
        {
            return (r as IPositionalReference)?.ReferencePosition ?? "[Unknown Position]";
        }
        public static string GetReferenceName(this IReference r)
        {
            return (r as INamedReference)?.ReferenceName ?? "[Unknown]";
        }
        public static string GetReferencePosName(this IReference r)
        {
            return (r as IPosNamedReference)?.ReferenceName ?? "["+r.GetReferencePosition()+": "+r.GetReferenceName()+"]";
        }
    }
}
