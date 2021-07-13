using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Antlr4.Runtime;
using Antlr4.Runtime.Misc;

namespace Nom.Parser
{
    public class NomParser
    {
        public NomParser()
        {
        }

        private class ParserErrorListener : IAntlrErrorListener<IToken>
        {
            public static ParserErrorListener Instance { get; } = new ParserErrorListener();
            private ParserErrorListener()
            {

            }

            public void SyntaxError([NotNull] IRecognizer recognizer, [Nullable] IToken offendingSymbol, int line, int charPositionInLine, [NotNull] string msg, [Nullable] RecognitionException e)
            {
                Console.WriteLine("ERROR (" + line.ToString() + ":" + charPositionInLine.ToString() + "): " + offendingSymbol?.ToString() + "\n" + msg);
            }

            public void SyntaxError(TextWriter output, IRecognizer recognizer, IToken offendingSymbol, int line, int charPositionInLine, string msg, RecognitionException e)
            {
                Console.WriteLine("ERROR (" + line.ToString() + ":" + charPositionInLine.ToString() + "): " + offendingSymbol?.ToString() + "\n" + msg);
            }
        }

        public CodeFile ParseFile(FileInfo fi)
        {
            if(fi.Exists)
            {
                try
                {
                    using (FileStream fs = fi.OpenRead())
                    {
                        var afs = new AntlrInputStream(fs);
                        afs.name = fi.Name;
                        NominalGradualLexer lexer = new NominalGradualLexer(afs);
                        CommonTokenStream tokenStream = new CommonTokenStream(lexer);
                        NominalGradualParser parser = new NominalGradualParser(tokenStream);
                        parser.AddErrorListener(ParserErrorListener.Instance);
                        return parser.file(fi.Name).cf;
                    }
                }
                catch(UnauthorizedAccessException e)
                {
                    throw e;
                }
                catch(DirectoryNotFoundException e)
                {
                    throw e;
                }
                catch(IOException e)
                {
                    throw e;
                }
            }
            throw new ParseException("File " + fi.Name + " does not exist!");
        }

        public Program Parse(IEnumerable<FileInfo> files)
        {
            List<CodeFile> codeFiles = new List<CodeFile>();
            foreach(FileInfo file in files)
            {
                try
                {
                    codeFiles.Add(ParseFile(file));
                }
                catch(ListableException e)
                {
                    CompilerOutput.RegisterException(e);
                }
            }
            return new Program(codeFiles);
        }
    }
}
