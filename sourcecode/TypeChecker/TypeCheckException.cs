using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class TypeCheckException : ListableException
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="message">@n : position of reference n ; $n : name of reference n ; %n : name + position of reference n (0-based)</param>
        /// <param name="innerException"></param>
        /// <param name="references">A number of references related to the error. If the first reference has a position, it is supposed to be the main location of the error.</param>
        public TypeCheckException(String message, Exception innerException, params IReference[] references) : base(message, innerException, references)
        {

        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="message">@n : position of reference n ; $n : name of reference n ; %n : name + position of reference n (0-based)</param>
        /// <param name="references">A number of references related to the error. If the first reference has a position, it is supposed to be the main location of the error.</param>
        public TypeCheckException(String message, params IReference[] references) : base(message, references)
        {

        }
    }
}
