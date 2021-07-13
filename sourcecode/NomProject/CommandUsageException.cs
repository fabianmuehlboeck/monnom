using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Text;

namespace Nom.Project
{
    public class CommandUsageException : Exception
    {
        public CommandUsageException()
        {
        }

        public CommandUsageException(string message) : base(message)
        {
        }

        public CommandUsageException(string message, Exception innerException) : base(message, innerException)
        {
        }

        protected CommandUsageException(SerializationInfo info, StreamingContext context) : base(info, context)
        {
        }
    }
}
