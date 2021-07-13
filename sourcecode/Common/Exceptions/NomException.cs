using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom
{
    public class NomException : Exception
    {
        public ISourceSpan ErrorLocation
        {
            get;
            protected set;
        }
        public ISourceLocs OtherLocations
        {
            get;
            protected set;
        }

        public NomException(String message, Exception innerException, ISourceSpan span = null):base((span==null?message:span.ToString()+": " + message), innerException)
        {
            this.ErrorLocation = span ?? new GenSourceSpan();
            this.OtherLocations = new SourceLocs();
        }
        public NomException(String message, ISourceSpan span = null) :base((span == null ? message : span.ToString() + ": " + message))
        {
            this.ErrorLocation = span ?? new GenSourceSpan();
            this.OtherLocations = new SourceLocs();
        }

        public NomException(String message, Exception innerException, ISourceLocs sourceLocation) :
            base((sourceLocation == null ? message : sourceLocation.ToString() + " : " + message), innerException)
        {
            ErrorLocation = sourceLocation?.Spans.FirstOrDefault()??new GenSourceSpan();
            OtherLocations = (sourceLocation?.Spans.Count() > 0 ? new SourceLocs(sourceLocation?.Spans.Skip(1)) : new SourceLocs());
        }

        public NomException(String message = "Unknown Compiler Exception", ISourceLocs sourceLocation = null) :
            base((sourceLocation == null ? message : sourceLocation.ToString() + " : " + message))
        {
            ErrorLocation = sourceLocation?.Spans.FirstOrDefault() ?? new GenSourceSpan();
            OtherLocations = (sourceLocation?.Spans.Count() > 0 ? new SourceLocs(sourceLocation?.Spans.Skip(1)) : new SourceLocs());
        }

        public String MessageTemplate
        {
            get;
        }
        private IReference[] references;
        public IReference this[int i]
        {
            get
            {
                return references[i];
            }
        }
        public int ReferenceCount
        {
            get;
        }

        public NomException(String message, params IReference[] references) : base(message.FillReferences(references))
        {
            this.references = references;
            this.MessageTemplate = message;
            this.ReferenceCount = references.Length;
        }
        public NomException(String message, Exception innerException, params IReference[] references):base(message.FillReferences(innerException, references), innerException)
        {
            this.references = references;
            this.MessageTemplate = message;
            this.ReferenceCount = references.Length;
        }
    }
}
