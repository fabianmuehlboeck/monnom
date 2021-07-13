// Generated from c:\Users\Fabian\Dropbox\Nom\compiler\Parser\Special\NominalGradual.g4 by ANTLR 4.7

using System.Linq;
#pragma warning disable 3021

import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class NominalGradualParser extends Parser {
	static { RuntimeMetaData.checkVersion("4.7", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		STRING=1, FLOAT=2, INT=3, TRUE=4, FALSE=5, USING=6, ABSTRACT=7, PARTIAL=8, 
		EXPANDO=9, READONLY=10, NAMESPACE=11, INTERFACE=12, CLASS=13, INSTANCE=14, 
		EXTENDS=15, IMPLEMENTS=16, DYN=17, STATIC=18, FINAL=19, NUL=20, VIRTUAL=21, 
		OVERRIDE=22, MULTI=23, DEFAULT=24, PUBLIC=25, PRIVATE=26, PROTECTED=27, 
		INTERNAL=28, INTERNAL_PROT=29, IFNULL=30, IFOBJ=31, IF=32, ELSE=33, ELSEIF=34, 
		WHILE=35, FOR=36, FOREACH=37, IN=38, RETURN=39, BREAK=40, CONTINUE=41, 
		FUN=42, SUPER=43, CONSTRUCT=44, NEW=45, DBG=46, ERR=47, SEMICOLON=48, 
		COLONCOLON=49, COLON=50, DOT=51, DOTDOT=52, COMMA=53, LPAREN=54, RPAREN=55, 
		LBRACKET=56, RBRACKET=57, LBRACE=58, RBRACE=59, LANGLE=60, RANGLE=61, 
		ARROW=62, QMARK=63, BANG=64, AND=65, OR=66, APPEND=67, PLUS=68, MINUS=69, 
		SLASH=70, TIMES=71, POW=72, PERCENT=73, EQ=74, EQEQ=75, NEQ=76, EQEQEQ=77, 
		LEQ=78, GEQ=79, BITXOR=80, BITAND=81, BITOR=82, PLUSEQ=83, MINUSEQ=84, 
		TIMESEQ=85, DIVEQ=86, BITSHL=87, BITSHR=88, ID=89, WS=90, COMMENT=91;
	public static final int
		RULE_file = 0, RULE_using = 1, RULE_qname = 2, RULE_ns = 3, RULE_visibility = 4, 
		RULE_defaultexpr = 5, RULE_expr = 6, RULE_rangeexpr = 7, RULE_divmultop = 8, 
		RULE_addsubop = 9, RULE_bitshop = 10, RULE_stmt = 11, RULE_block = 12, 
		RULE_ident = 13, RULE_ctype = 14, RULE_interfacedef = 15, RULE_classdef = 16, 
		RULE_constructor = 17, RULE_fielddecl = 18, RULE_staticfielddecl = 19, 
		RULE_instancedecl = 20, RULE_instanceassignment = 21, RULE_fundef = 22, 
		RULE_methdef = 23, RULE_staticmethdef = 24, RULE_methdecl = 25, RULE_type = 26, 
		RULE_typelist = 27, RULE_dtype = 28, RULE_argdecl = 29;
	public static final String[] ruleNames = {
		"file", "using", "qname", "ns", "visibility", "defaultexpr", "expr", "rangeexpr", 
		"divmultop", "addsubop", "bitshop", "stmt", "block", "ident", "ctype", 
		"interfacedef", "classdef", "constructor", "fielddecl", "staticfielddecl", 
		"instancedecl", "instanceassignment", "fundef", "methdef", "staticmethdef", 
		"methdecl", "type", "typelist", "dtype", "argdecl"
	};

	private static final String[] _LITERAL_NAMES = {
		null, null, null, null, "'true'", "'false'", "'using'", "'abstract'", 
		"'partial'", "'expando'", "'readonly'", "'namespace'", "'interface'", 
		"'class'", "'instance'", "'extends'", "'implements'", "'dyn'", "'static'", 
		"'final'", "'null'", "'virtual'", "'override'", "'multi'", "'default'", 
		"'public'", "'private'", "'protected'", "'internal'", null, "'ifnull'", 
		"'ifobj'", "'if'", "'else'", "'elseif'", "'while'", "'for'", "'foreach'", 
		"'in'", "'return'", "'break'", "'continue'", "'fun'", "'super'", "'constructor'", 
		"'new'", "'DEBUG'", "'ERROR'", "';'", "'::'", "':'", "'.'", "'..'", "','", 
		"'('", "')'", "'['", "']'", "'{'", "'}'", "'<'", "'>'", "'->'", "'?'", 
		"'!'", "'&&'", "'||'", "'++'", "'+'", "'-'", "'/'", "'*'", "'**'", "'%'", 
		"'='", "'=='", "'!='", "'==='", "'<='", "'>='", "'^'", "'&'", "'|'", "'+='", 
		"'-='", "'*='", "'/='", "'<<'", "'>>'"
	};
	private static final String[] _SYMBOLIC_NAMES = {
		null, "STRING", "FLOAT", "INT", "TRUE", "FALSE", "USING", "ABSTRACT", 
		"PARTIAL", "EXPANDO", "READONLY", "NAMESPACE", "INTERFACE", "CLASS", "INSTANCE", 
		"EXTENDS", "IMPLEMENTS", "DYN", "STATIC", "FINAL", "NUL", "VIRTUAL", "OVERRIDE", 
		"MULTI", "DEFAULT", "PUBLIC", "PRIVATE", "PROTECTED", "INTERNAL", "INTERNAL_PROT", 
		"IFNULL", "IFOBJ", "IF", "ELSE", "ELSEIF", "WHILE", "FOR", "FOREACH", 
		"IN", "RETURN", "BREAK", "CONTINUE", "FUN", "SUPER", "CONSTRUCT", "NEW", 
		"DBG", "ERR", "SEMICOLON", "COLONCOLON", "COLON", "DOT", "DOTDOT", "COMMA", 
		"LPAREN", "RPAREN", "LBRACKET", "RBRACKET", "LBRACE", "RBRACE", "LANGLE", 
		"RANGLE", "ARROW", "QMARK", "BANG", "AND", "OR", "APPEND", "PLUS", "MINUS", 
		"SLASH", "TIMES", "POW", "PERCENT", "EQ", "EQEQ", "NEQ", "EQEQEQ", "LEQ", 
		"GEQ", "BITXOR", "BITAND", "BITOR", "PLUSEQ", "MINUSEQ", "TIMESEQ", "DIVEQ", 
		"BITSHL", "BITSHR", "ID", "WS", "COMMENT"
	};
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}

	@Override
	public String getGrammarFileName() { return "NominalGradual.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }

	public NominalGradualParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}
	public static class FileContext extends ParserRuleContext {
		public CodeFile cf;
		public UsingContext using;
		public List<UsingContext> us = new ArrayList<UsingContext>();
		public ClassdefContext classdef;
		public List<ClassdefContext> cdefs = new ArrayList<ClassdefContext>();
		public InterfacedefContext interfacedef;
		public List<InterfacedefContext> idefs = new ArrayList<InterfacedefContext>();
		public NsContext ns;
		public List<NsContext> nss = new ArrayList<NsContext>();
		public List<UsingContext> using() {
			return getRuleContexts(UsingContext.class);
		}
		public UsingContext using(int i) {
			return getRuleContext(UsingContext.class,i);
		}
		public List<ClassdefContext> classdef() {
			return getRuleContexts(ClassdefContext.class);
		}
		public ClassdefContext classdef(int i) {
			return getRuleContext(ClassdefContext.class,i);
		}
		public List<InterfacedefContext> interfacedef() {
			return getRuleContexts(InterfacedefContext.class);
		}
		public InterfacedefContext interfacedef(int i) {
			return getRuleContext(InterfacedefContext.class,i);
		}
		public List<NsContext> ns() {
			return getRuleContexts(NsContext.class);
		}
		public NsContext ns(int i) {
			return getRuleContext(NsContext.class,i);
		}
		public FileContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_file; }
	}

	public final FileContext file() throws RecognitionException {
		FileContext _localctx = new FileContext(_ctx, getState());
		enterRule(_localctx, 0, RULE_file);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(63);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==USING) {
				{
				{
				setState(60);
				((FileContext)_localctx).using = using();
				((FileContext)_localctx).us.add(((FileContext)_localctx).using);
				}
				}
				setState(65);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(71);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << ABSTRACT) | (1L << PARTIAL) | (1L << NAMESPACE) | (1L << INTERFACE) | (1L << CLASS) | (1L << FINAL) | (1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
				{
				setState(69);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,1,_ctx) ) {
				case 1:
					{
					setState(66);
					((FileContext)_localctx).classdef = classdef();
					((FileContext)_localctx).cdefs.add(((FileContext)_localctx).classdef);
					}
					break;
				case 2:
					{
					setState(67);
					((FileContext)_localctx).interfacedef = interfacedef();
					((FileContext)_localctx).idefs.add(((FileContext)_localctx).interfacedef);
					}
					break;
				case 3:
					{
					setState(68);
					((FileContext)_localctx).ns = ns();
					((FileContext)_localctx).nss.add(((FileContext)_localctx).ns);
					}
					break;
				}
				}
				setState(73);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			((FileContext)_localctx).cf = new CodeFile(((FileContext)_localctx).us.Select(u=>u.u),((FileContext)_localctx).idefs.Select(i=>i.idef),((FileContext)_localctx).cdefs.Select(c=>c.cdef),((FileContext)_localctx).nss.Select(n=>n.n));
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class UsingContext extends ParserRuleContext {
		public QName u;
		public QnameContext qname;
		public TerminalNode USING() { return getToken(NominalGradualParser.USING, 0); }
		public QnameContext qname() {
			return getRuleContext(QnameContext.class,0);
		}
		public TerminalNode SEMICOLON() { return getToken(NominalGradualParser.SEMICOLON, 0); }
		public UsingContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_using; }
	}

	public final UsingContext using() throws RecognitionException {
		UsingContext _localctx = new UsingContext(_ctx, getState());
		enterRule(_localctx, 2, RULE_using);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(76);
			match(USING);
			setState(77);
			((UsingContext)_localctx).qname = qname();
			setState(78);
			match(SEMICOLON);
			((UsingContext)_localctx).u = ((UsingContext)_localctx).qname.q;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class QnameContext extends ParserRuleContext {
		public QName q;
		public IdentContext ident;
		public List<IdentContext> ids = new ArrayList<IdentContext>();
		public List<IdentContext> ident() {
			return getRuleContexts(IdentContext.class);
		}
		public IdentContext ident(int i) {
			return getRuleContext(IdentContext.class,i);
		}
		public List<TerminalNode> DOT() { return getTokens(NominalGradualParser.DOT); }
		public TerminalNode DOT(int i) {
			return getToken(NominalGradualParser.DOT, i);
		}
		public QnameContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_qname; }
	}

	public final QnameContext qname() throws RecognitionException {
		QnameContext _localctx = new QnameContext(_ctx, getState());
		enterRule(_localctx, 4, RULE_qname);
		try {
			int _alt;
			enterOuterAlt(_localctx, 1);
			{
			setState(86);
			_errHandler.sync(this);
			_alt = getInterpreter().adaptivePredict(_input,3,_ctx);
			while ( _alt!=2 && _alt!=org.antlr.v4.runtime.atn.ATN.INVALID_ALT_NUMBER ) {
				if ( _alt==1 ) {
					{
					{
					setState(81);
					((QnameContext)_localctx).ident = ident();
					((QnameContext)_localctx).ids.add(((QnameContext)_localctx).ident);
					setState(82);
					match(DOT);
					}
					} 
				}
				setState(88);
				_errHandler.sync(this);
				_alt = getInterpreter().adaptivePredict(_input,3,_ctx);
			}
			setState(89);
			((QnameContext)_localctx).ident = ident();
			((QnameContext)_localctx).ids.add(((QnameContext)_localctx).ident);
			((QnameContext)_localctx).q =  new QName(((QnameContext)_localctx).ids.Select(id=>id.i).ToArray());
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class NsContext extends ParserRuleContext {
		public Namespace n;
		public Token NAMESPACE;
		public QnameContext qname;
		public ClassdefContext classdef;
		public List<ClassdefContext> cdefs = new ArrayList<ClassdefContext>();
		public InterfacedefContext interfacedef;
		public List<InterfacedefContext> idefs = new ArrayList<InterfacedefContext>();
		public NsContext ns;
		public List<NsContext> nss = new ArrayList<NsContext>();
		public Token RBRACE;
		public TerminalNode NAMESPACE() { return getToken(NominalGradualParser.NAMESPACE, 0); }
		public QnameContext qname() {
			return getRuleContext(QnameContext.class,0);
		}
		public TerminalNode LBRACE() { return getToken(NominalGradualParser.LBRACE, 0); }
		public TerminalNode RBRACE() { return getToken(NominalGradualParser.RBRACE, 0); }
		public List<ClassdefContext> classdef() {
			return getRuleContexts(ClassdefContext.class);
		}
		public ClassdefContext classdef(int i) {
			return getRuleContext(ClassdefContext.class,i);
		}
		public List<InterfacedefContext> interfacedef() {
			return getRuleContexts(InterfacedefContext.class);
		}
		public InterfacedefContext interfacedef(int i) {
			return getRuleContext(InterfacedefContext.class,i);
		}
		public List<NsContext> ns() {
			return getRuleContexts(NsContext.class);
		}
		public NsContext ns(int i) {
			return getRuleContext(NsContext.class,i);
		}
		public NsContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_ns; }
	}

	public final NsContext ns() throws RecognitionException {
		NsContext _localctx = new NsContext(_ctx, getState());
		enterRule(_localctx, 6, RULE_ns);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(92);
			((NsContext)_localctx).NAMESPACE = match(NAMESPACE);
			setState(93);
			((NsContext)_localctx).qname = qname();
			setState(94);
			match(LBRACE);
			setState(100);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << ABSTRACT) | (1L << PARTIAL) | (1L << NAMESPACE) | (1L << INTERFACE) | (1L << CLASS) | (1L << FINAL) | (1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
				{
				setState(98);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,4,_ctx) ) {
				case 1:
					{
					setState(95);
					((NsContext)_localctx).classdef = classdef();
					((NsContext)_localctx).cdefs.add(((NsContext)_localctx).classdef);
					}
					break;
				case 2:
					{
					setState(96);
					((NsContext)_localctx).interfacedef = interfacedef();
					((NsContext)_localctx).idefs.add(((NsContext)_localctx).interfacedef);
					}
					break;
				case 3:
					{
					setState(97);
					((NsContext)_localctx).ns = ns();
					((NsContext)_localctx).nss.add(((NsContext)_localctx).ns);
					}
					break;
				}
				}
				setState(102);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(103);
			((NsContext)_localctx).RBRACE = match(RBRACE);
			((NsContext)_localctx).n = new Namespace(((NsContext)_localctx).qname.q, ((NsContext)_localctx).idefs.Select(i=>i.idef),((NsContext)_localctx).cdefs.Select(c=>c.cdef),((NsContext)_localctx).nss.Select(n=>n.n), ((NsContext)_localctx).NAMESPACE.ToSourceSpan(((NsContext)_localctx).RBRACE));
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class VisibilityContext extends ParserRuleContext {
		public VisibilityNode v;
		public Token PRIVATE;
		public Token PROTECTED;
		public Token INTERNAL_PROT;
		public Token INTERNAL;
		public Token PUBLIC;
		public TerminalNode PRIVATE() { return getToken(NominalGradualParser.PRIVATE, 0); }
		public TerminalNode PROTECTED() { return getToken(NominalGradualParser.PROTECTED, 0); }
		public TerminalNode INTERNAL_PROT() { return getToken(NominalGradualParser.INTERNAL_PROT, 0); }
		public TerminalNode INTERNAL() { return getToken(NominalGradualParser.INTERNAL, 0); }
		public TerminalNode PUBLIC() { return getToken(NominalGradualParser.PUBLIC, 0); }
		public VisibilityContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_visibility; }
	}

	public final VisibilityContext visibility() throws RecognitionException {
		VisibilityContext _localctx = new VisibilityContext(_ctx, getState());
		enterRule(_localctx, 8, RULE_visibility);
		try {
			setState(116);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case PRIVATE:
				enterOuterAlt(_localctx, 1);
				{
				setState(106);
				((VisibilityContext)_localctx).PRIVATE = match(PRIVATE);
				((VisibilityContext)_localctx).v = new VisibilityNode(Visibility.Private, ((VisibilityContext)_localctx).PRIVATE.ToSourceSpan());
				}
				break;
			case PROTECTED:
				enterOuterAlt(_localctx, 2);
				{
				setState(108);
				((VisibilityContext)_localctx).PROTECTED = match(PROTECTED);
				((VisibilityContext)_localctx).v = new VisibilityNode(Visibility.Protected, ((VisibilityContext)_localctx).PROTECTED.ToSourceSpan());
				}
				break;
			case INTERNAL_PROT:
				enterOuterAlt(_localctx, 3);
				{
				setState(110);
				((VisibilityContext)_localctx).INTERNAL_PROT = match(INTERNAL_PROT);
				((VisibilityContext)_localctx).v = new VisibilityNode(Visibility.ProtectedInternal, ((VisibilityContext)_localctx).INTERNAL_PROT.ToSourceSpan());
				}
				break;
			case INTERNAL:
				enterOuterAlt(_localctx, 4);
				{
				setState(112);
				((VisibilityContext)_localctx).INTERNAL = match(INTERNAL);
				((VisibilityContext)_localctx).v = new VisibilityNode(Visibility.Internal, ((VisibilityContext)_localctx).INTERNAL.ToSourceSpan());
				}
				break;
			case PUBLIC:
				enterOuterAlt(_localctx, 5);
				{
				setState(114);
				((VisibilityContext)_localctx).PUBLIC = match(PUBLIC);
				((VisibilityContext)_localctx).v = new VisibilityNode(Visibility.Public, ((VisibilityContext)_localctx).PUBLIC.ToSourceSpan());
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class DefaultexprContext extends ParserRuleContext {
		public IDefaultExpr exp;
		public Token STRING;
		public Token INT;
		public Token FLOAT;
		public Token TRUE;
		public Token FALSE;
		public Token NUL;
		public CtypeContext ctype;
		public IdentContext ident;
		public DefaultexprContext defaultexpr;
		public List<DefaultexprContext> args = new ArrayList<DefaultexprContext>();
		public Token RBRACKET;
		public Token LBRACKET;
		public List<DefaultexprContext> elems = new ArrayList<DefaultexprContext>();
		public TerminalNode STRING() { return getToken(NominalGradualParser.STRING, 0); }
		public TerminalNode INT() { return getToken(NominalGradualParser.INT, 0); }
		public TerminalNode MINUS() { return getToken(NominalGradualParser.MINUS, 0); }
		public TerminalNode FLOAT() { return getToken(NominalGradualParser.FLOAT, 0); }
		public TerminalNode TRUE() { return getToken(NominalGradualParser.TRUE, 0); }
		public TerminalNode FALSE() { return getToken(NominalGradualParser.FALSE, 0); }
		public TerminalNode NUL() { return getToken(NominalGradualParser.NUL, 0); }
		public CtypeContext ctype() {
			return getRuleContext(CtypeContext.class,0);
		}
		public TerminalNode COLONCOLON() { return getToken(NominalGradualParser.COLONCOLON, 0); }
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode LBRACKET() { return getToken(NominalGradualParser.LBRACKET, 0); }
		public TerminalNode RBRACKET() { return getToken(NominalGradualParser.RBRACKET, 0); }
		public List<DefaultexprContext> defaultexpr() {
			return getRuleContexts(DefaultexprContext.class);
		}
		public DefaultexprContext defaultexpr(int i) {
			return getRuleContext(DefaultexprContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(NominalGradualParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(NominalGradualParser.COMMA, i);
		}
		public DefaultexprContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_defaultexpr; }
	}

	public final DefaultexprContext defaultexpr() throws RecognitionException {
		DefaultexprContext _localctx = new DefaultexprContext(_ctx, getState());
		enterRule(_localctx, 10, RULE_defaultexpr);
		int _la;
		try {
			setState(171);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,12,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(118);
				((DefaultexprContext)_localctx).STRING = match(STRING);
				((DefaultexprContext)_localctx).exp =  new StringExpr(((DefaultexprContext)_localctx).STRING);
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(120);
				((DefaultexprContext)_localctx).INT = match(INT);
				((DefaultexprContext)_localctx).exp =  new IntExpr(((DefaultexprContext)_localctx).INT);
				}
				break;
			case 3:
				enterOuterAlt(_localctx, 3);
				{
				setState(122);
				match(MINUS);
				setState(123);
				((DefaultexprContext)_localctx).INT = match(INT);
				((DefaultexprContext)_localctx).exp =  new IntExpr(((DefaultexprContext)_localctx).INT,true);
				}
				break;
			case 4:
				enterOuterAlt(_localctx, 4);
				{
				setState(125);
				((DefaultexprContext)_localctx).FLOAT = match(FLOAT);
				((DefaultexprContext)_localctx).exp =  new FloatExpr(((DefaultexprContext)_localctx).FLOAT);
				}
				break;
			case 5:
				enterOuterAlt(_localctx, 5);
				{
				setState(127);
				match(MINUS);
				setState(128);
				((DefaultexprContext)_localctx).FLOAT = match(FLOAT);
				((DefaultexprContext)_localctx).exp =  new FloatExpr(((DefaultexprContext)_localctx).FLOAT,true);
				}
				break;
			case 6:
				enterOuterAlt(_localctx, 6);
				{
				setState(130);
				((DefaultexprContext)_localctx).TRUE = match(TRUE);
				((DefaultexprContext)_localctx).exp =  new BoolExpr(((DefaultexprContext)_localctx).TRUE);
				}
				break;
			case 7:
				enterOuterAlt(_localctx, 7);
				{
				setState(132);
				((DefaultexprContext)_localctx).FALSE = match(FALSE);
				((DefaultexprContext)_localctx).exp =  new BoolExpr(((DefaultexprContext)_localctx).FALSE);
				}
				break;
			case 8:
				enterOuterAlt(_localctx, 8);
				{
				setState(134);
				((DefaultexprContext)_localctx).NUL = match(NUL);
				((DefaultexprContext)_localctx).exp =  new NullExpr(((DefaultexprContext)_localctx).NUL);
				}
				break;
			case 9:
				enterOuterAlt(_localctx, 9);
				{
				setState(136);
				((DefaultexprContext)_localctx).ctype = ctype();
				setState(137);
				match(COLONCOLON);
				setState(138);
				((DefaultexprContext)_localctx).ident = ident();
				setState(151);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,9,_ctx) ) {
				case 1:
					{
					setState(139);
					match(LBRACKET);
					setState(148);
					_errHandler.sync(this);
					_la = _input.LA(1);
					if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << NUL) | (1L << LBRACKET))) != 0) || _la==MINUS || _la==ID) {
						{
						setState(140);
						((DefaultexprContext)_localctx).defaultexpr = defaultexpr();
						((DefaultexprContext)_localctx).args.add(((DefaultexprContext)_localctx).defaultexpr);
						setState(145);
						_errHandler.sync(this);
						_la = _input.LA(1);
						while (_la==COMMA) {
							{
							{
							setState(141);
							match(COMMA);
							setState(142);
							((DefaultexprContext)_localctx).defaultexpr = defaultexpr();
							((DefaultexprContext)_localctx).args.add(((DefaultexprContext)_localctx).defaultexpr);
							}
							}
							setState(147);
							_errHandler.sync(this);
							_la = _input.LA(1);
						}
						}
					}

					setState(150);
					((DefaultexprContext)_localctx).RBRACKET = match(RBRACKET);
					}
					break;
				}
				((DefaultexprContext)_localctx).exp =  new DefaultInstanceExpr(((DefaultexprContext)_localctx).ctype.ct, ((DefaultexprContext)_localctx).ident.i, ((DefaultexprContext)_localctx).args.Select(a=>a.exp), ((DefaultexprContext)_localctx).RBRACKET==null?((DefaultexprContext)_localctx).ctype.ct.Start.SpanTo(((DefaultexprContext)_localctx).ident.i.End):((DefaultexprContext)_localctx).ctype.ct.Start.SpanTo(((DefaultexprContext)_localctx).RBRACKET));
				}
				break;
			case 10:
				enterOuterAlt(_localctx, 10);
				{
				setState(155);
				((DefaultexprContext)_localctx).LBRACKET = match(LBRACKET);
				setState(164);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << NUL) | (1L << LBRACKET))) != 0) || _la==MINUS || _la==ID) {
					{
					setState(156);
					((DefaultexprContext)_localctx).defaultexpr = defaultexpr();
					((DefaultexprContext)_localctx).elems.add(((DefaultexprContext)_localctx).defaultexpr);
					setState(161);
					_errHandler.sync(this);
					_la = _input.LA(1);
					while (_la==COMMA) {
						{
						{
						setState(157);
						match(COMMA);
						setState(158);
						((DefaultexprContext)_localctx).defaultexpr = defaultexpr();
						((DefaultexprContext)_localctx).elems.add(((DefaultexprContext)_localctx).defaultexpr);
						}
						}
						setState(163);
						_errHandler.sync(this);
						_la = _input.LA(1);
					}
					}
				}

				setState(166);
				((DefaultexprContext)_localctx).RBRACKET = match(RBRACKET);
				((DefaultexprContext)_localctx).exp =  new DefaultListExpr(((DefaultexprContext)_localctx).elems.Select(e=>e.exp), ((DefaultexprContext)_localctx).LBRACKET.ToSourceSpan(((DefaultexprContext)_localctx).RBRACKET));
				}
				break;
			case 11:
				enterOuterAlt(_localctx, 11);
				{
				setState(168);
				((DefaultexprContext)_localctx).ident = ident();
				((DefaultexprContext)_localctx).exp = new IdentExpr(((DefaultexprContext)_localctx).ident.i);
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ExprContext extends ParserRuleContext {
		public IExpr exp;
		public ExprContext r;
		public ExprContext e1;
		public Token NEW;
		public CtypeContext ctype;
		public Token LPAREN;
		public ExprContext expr;
		public List<ExprContext> args = new ArrayList<ExprContext>();
		public Token RPAREN;
		public DtypeContext dtype;
		public IdentContext ident;
		public RangeexprContext rangeexpr;
		public List<ExprContext> elems = new ArrayList<ExprContext>();
		public Token LBRACE;
		public Token RBRACE;
		public DefaultexprContext defaultexpr;
		public Token BANG;
		public Token MINUS;
		public Token POW;
		public ExprContext e2;
		public DivmultopContext divmultop;
		public Token PERCENT;
		public AddsubopContext addsubop;
		public BitshopContext bitshop;
		public Token BITAND;
		public Token BITXOR;
		public Token BITOR;
		public Token APPEND;
		public Token LANGLE;
		public Token RANGLE;
		public Token LEQ;
		public Token GEQ;
		public Token EQEQ;
		public Token NEQ;
		public Token AND;
		public Token OR;
		public Token EQEQEQ;
		public TerminalNode NEW() { return getToken(NominalGradualParser.NEW, 0); }
		public CtypeContext ctype() {
			return getRuleContext(CtypeContext.class,0);
		}
		public TerminalNode LPAREN() { return getToken(NominalGradualParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(NominalGradualParser.RPAREN, 0); }
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(NominalGradualParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(NominalGradualParser.COMMA, i);
		}
		public DtypeContext dtype() {
			return getRuleContext(DtypeContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public RangeexprContext rangeexpr() {
			return getRuleContext(RangeexprContext.class,0);
		}
		public TerminalNode COLONCOLON() { return getToken(NominalGradualParser.COLONCOLON, 0); }
		public TerminalNode LBRACE() { return getToken(NominalGradualParser.LBRACE, 0); }
		public TerminalNode RBRACE() { return getToken(NominalGradualParser.RBRACE, 0); }
		public DefaultexprContext defaultexpr() {
			return getRuleContext(DefaultexprContext.class,0);
		}
		public TerminalNode BANG() { return getToken(NominalGradualParser.BANG, 0); }
		public TerminalNode MINUS() { return getToken(NominalGradualParser.MINUS, 0); }
		public TerminalNode POW() { return getToken(NominalGradualParser.POW, 0); }
		public DivmultopContext divmultop() {
			return getRuleContext(DivmultopContext.class,0);
		}
		public TerminalNode PERCENT() { return getToken(NominalGradualParser.PERCENT, 0); }
		public AddsubopContext addsubop() {
			return getRuleContext(AddsubopContext.class,0);
		}
		public BitshopContext bitshop() {
			return getRuleContext(BitshopContext.class,0);
		}
		public TerminalNode BITAND() { return getToken(NominalGradualParser.BITAND, 0); }
		public TerminalNode BITXOR() { return getToken(NominalGradualParser.BITXOR, 0); }
		public TerminalNode BITOR() { return getToken(NominalGradualParser.BITOR, 0); }
		public TerminalNode APPEND() { return getToken(NominalGradualParser.APPEND, 0); }
		public TerminalNode LANGLE() { return getToken(NominalGradualParser.LANGLE, 0); }
		public TerminalNode RANGLE() { return getToken(NominalGradualParser.RANGLE, 0); }
		public TerminalNode LEQ() { return getToken(NominalGradualParser.LEQ, 0); }
		public TerminalNode GEQ() { return getToken(NominalGradualParser.GEQ, 0); }
		public TerminalNode EQEQ() { return getToken(NominalGradualParser.EQEQ, 0); }
		public TerminalNode NEQ() { return getToken(NominalGradualParser.NEQ, 0); }
		public TerminalNode AND() { return getToken(NominalGradualParser.AND, 0); }
		public TerminalNode OR() { return getToken(NominalGradualParser.OR, 0); }
		public TerminalNode EQEQEQ() { return getToken(NominalGradualParser.EQEQEQ, 0); }
		public TerminalNode DOT() { return getToken(NominalGradualParser.DOT, 0); }
		public ExprContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_expr; }
	}

	public final ExprContext expr() throws RecognitionException {
		return expr(0);
	}

	private ExprContext expr(int _p) throws RecognitionException {
		ParserRuleContext _parentctx = _ctx;
		int _parentState = getState();
		ExprContext _localctx = new ExprContext(_ctx, _parentState);
		ExprContext _prevctx = _localctx;
		int _startState = 12;
		enterRecursionRule(_localctx, 12, RULE_expr, _p);
		int _la;
		try {
			int _alt;
			enterOuterAlt(_localctx, 1);
			{
			setState(260);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,21,_ctx) ) {
			case 1:
				{
				setState(174);
				((ExprContext)_localctx).NEW = match(NEW);
				setState(175);
				((ExprContext)_localctx).ctype = ctype();
				setState(176);
				((ExprContext)_localctx).LPAREN = match(LPAREN);
				setState(185);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << NUL) | (1L << NEW) | (1L << LPAREN) | (1L << LBRACKET) | (1L << LBRACE))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (BANG - 64)) | (1L << (MINUS - 64)) | (1L << (ID - 64)))) != 0)) {
					{
					setState(177);
					((ExprContext)_localctx).expr = expr(0);
					((ExprContext)_localctx).args.add(((ExprContext)_localctx).expr);
					setState(182);
					_errHandler.sync(this);
					_la = _input.LA(1);
					while (_la==COMMA) {
						{
						{
						setState(178);
						match(COMMA);
						setState(179);
						((ExprContext)_localctx).expr = expr(0);
						((ExprContext)_localctx).args.add(((ExprContext)_localctx).expr);
						}
						}
						setState(184);
						_errHandler.sync(this);
						_la = _input.LA(1);
					}
					}
				}

				setState(187);
				((ExprContext)_localctx).RPAREN = match(RPAREN);
				((ExprContext)_localctx).exp = new NewExpr(new NewCall(((ExprContext)_localctx).ctype.ct, ((ExprContext)_localctx).NEW.ToSourceSpan(((ExprContext)_localctx).ctype.ct.End)),((ExprContext)_localctx).args.Select(e=>e.exp), ((ExprContext)_localctx).NEW.ToSourceSpan(((ExprContext)_localctx).RPAREN));
				}
				break;
			case 2:
				{
				setState(190);
				((ExprContext)_localctx).LPAREN = match(LPAREN);
				setState(191);
				((ExprContext)_localctx).dtype = dtype();
				setState(192);
				((ExprContext)_localctx).RPAREN = match(RPAREN);
				setState(193);
				((ExprContext)_localctx).e1 = expr(29);
				((ExprContext)_localctx).exp = new CastExpr(((ExprContext)_localctx).dtype.t,((ExprContext)_localctx).e1.exp,((ExprContext)_localctx).LPAREN.ToSourceSpan(((ExprContext)_localctx).e1.exp.End));
				}
				break;
			case 3:
				{
				setState(196);
				((ExprContext)_localctx).ident = ident();
				setState(197);
				((ExprContext)_localctx).LPAREN = match(LPAREN);
				setState(206);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << NUL) | (1L << NEW) | (1L << LPAREN) | (1L << LBRACKET) | (1L << LBRACE))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (BANG - 64)) | (1L << (MINUS - 64)) | (1L << (ID - 64)))) != 0)) {
					{
					setState(198);
					((ExprContext)_localctx).expr = expr(0);
					((ExprContext)_localctx).args.add(((ExprContext)_localctx).expr);
					setState(203);
					_errHandler.sync(this);
					_la = _input.LA(1);
					while (_la==COMMA) {
						{
						{
						setState(199);
						match(COMMA);
						setState(200);
						((ExprContext)_localctx).expr = expr(0);
						((ExprContext)_localctx).args.add(((ExprContext)_localctx).expr);
						}
						}
						setState(205);
						_errHandler.sync(this);
						_la = _input.LA(1);
					}
					}
				}

				setState(208);
				((ExprContext)_localctx).RPAREN = match(RPAREN);
				((ExprContext)_localctx).exp = new CallExpr(null, ((ExprContext)_localctx).ident.i, ((ExprContext)_localctx).args.Select(e=>e.exp), ((ExprContext)_localctx).ident.i.Start.SpanTo(((ExprContext)_localctx).RPAREN));
				}
				break;
			case 4:
				{
				setState(211);
				((ExprContext)_localctx).LPAREN = match(LPAREN);
				setState(212);
				((ExprContext)_localctx).e1 = expr(0);
				setState(213);
				((ExprContext)_localctx).RPAREN = match(RPAREN);
				((ExprContext)_localctx).exp = ((ExprContext)_localctx).e1.exp;
				}
				break;
			case 5:
				{
				setState(216);
				((ExprContext)_localctx).rangeexpr = rangeexpr();
				((ExprContext)_localctx).exp =  ((ExprContext)_localctx).rangeexpr.rexp; 
				}
				break;
			case 6:
				{
				setState(219);
				((ExprContext)_localctx).ctype = ctype();
				setState(220);
				match(COLONCOLON);
				setState(221);
				((ExprContext)_localctx).ident = ident();
				setState(222);
				((ExprContext)_localctx).LPAREN = match(LPAREN);
				setState(231);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << NUL) | (1L << NEW) | (1L << LPAREN) | (1L << LBRACKET) | (1L << LBRACE))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (BANG - 64)) | (1L << (MINUS - 64)) | (1L << (ID - 64)))) != 0)) {
					{
					setState(223);
					((ExprContext)_localctx).expr = expr(0);
					((ExprContext)_localctx).elems.add(((ExprContext)_localctx).expr);
					setState(228);
					_errHandler.sync(this);
					_la = _input.LA(1);
					while (_la==COMMA) {
						{
						{
						setState(224);
						match(COMMA);
						setState(225);
						((ExprContext)_localctx).expr = expr(0);
						((ExprContext)_localctx).elems.add(((ExprContext)_localctx).expr);
						}
						}
						setState(230);
						_errHandler.sync(this);
						_la = _input.LA(1);
					}
					}
				}

				setState(233);
				((ExprContext)_localctx).RPAREN = match(RPAREN);
				((ExprContext)_localctx).exp =  new InstanceExpr(((ExprContext)_localctx).ctype.ct, ((ExprContext)_localctx).ident.i, ((ExprContext)_localctx).elems.Select(e=>e.exp), ((ExprContext)_localctx).ctype.ct.Start.SpanTo(((ExprContext)_localctx).RPAREN));
				}
				break;
			case 7:
				{
				setState(236);
				((ExprContext)_localctx).LBRACE = match(LBRACE);
				setState(245);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << NUL) | (1L << NEW) | (1L << LPAREN) | (1L << LBRACKET) | (1L << LBRACE))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (BANG - 64)) | (1L << (MINUS - 64)) | (1L << (ID - 64)))) != 0)) {
					{
					setState(237);
					((ExprContext)_localctx).expr = expr(0);
					((ExprContext)_localctx).elems.add(((ExprContext)_localctx).expr);
					setState(242);
					_errHandler.sync(this);
					_la = _input.LA(1);
					while (_la==COMMA) {
						{
						{
						setState(238);
						match(COMMA);
						setState(239);
						((ExprContext)_localctx).expr = expr(0);
						((ExprContext)_localctx).elems.add(((ExprContext)_localctx).expr);
						}
						}
						setState(244);
						_errHandler.sync(this);
						_la = _input.LA(1);
					}
					}
				}

				setState(247);
				((ExprContext)_localctx).RBRACE = match(RBRACE);
				((ExprContext)_localctx).exp =  new ListExpr(((ExprContext)_localctx).elems.Select(e=>e.exp), ((ExprContext)_localctx).LBRACE.ToSourceSpan(((ExprContext)_localctx).RBRACE));
				}
				break;
			case 8:
				{
				setState(249);
				((ExprContext)_localctx).defaultexpr = defaultexpr();
				((ExprContext)_localctx).exp =  ((ExprContext)_localctx).defaultexpr.exp;
				}
				break;
			case 9:
				{
				setState(252);
				((ExprContext)_localctx).BANG = match(BANG);
				setState(253);
				((ExprContext)_localctx).e1 = expr(20);
				((ExprContext)_localctx).exp =  new CallExpr(((ExprContext)_localctx).e1.exp,new Identifier("Not",((ExprContext)_localctx).BANG.ToSourceSpan()), new List<IExpr>(),((ExprContext)_localctx).BANG.ToSourceSpan(((ExprContext)_localctx).e1.exp.End)); 
				}
				break;
			case 10:
				{
				setState(256);
				((ExprContext)_localctx).MINUS = match(MINUS);
				setState(257);
				((ExprContext)_localctx).e1 = expr(19);
				((ExprContext)_localctx).exp =  new CallExpr(((ExprContext)_localctx).e1.exp,new Identifier("Neg",((ExprContext)_localctx).MINUS.ToSourceSpan()), new List<IExpr>(),((ExprContext)_localctx).MINUS.ToSourceSpan(((ExprContext)_localctx).e1.exp.End)); 
				}
				break;
			}
			_ctx.stop = _input.LT(-1);
			setState(376);
			_errHandler.sync(this);
			_alt = getInterpreter().adaptivePredict(_input,25,_ctx);
			while ( _alt!=2 && _alt!=org.antlr.v4.runtime.atn.ATN.INVALID_ALT_NUMBER ) {
				if ( _alt==1 ) {
					if ( _parseListeners!=null ) triggerExitRuleEvent();
					_prevctx = _localctx;
					{
					setState(374);
					_errHandler.sync(this);
					switch ( getInterpreter().adaptivePredict(_input,24,_ctx) ) {
					case 1:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(262);
						if (!(precpred(_ctx, 18))) throw new FailedPredicateException(this, "precpred(_ctx, 18)");
						setState(263);
						((ExprContext)_localctx).POW = match(POW);
						setState(264);
						((ExprContext)_localctx).e2 = expr(19);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.Power,((ExprContext)_localctx).POW.ToSourceSpan()),((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 2:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(267);
						if (!(precpred(_ctx, 17))) throw new FailedPredicateException(this, "precpred(_ctx, 17)");
						setState(268);
						((ExprContext)_localctx).divmultop = divmultop();
						setState(269);
						((ExprContext)_localctx).e2 = expr(18);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,((ExprContext)_localctx).divmultop.op,((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 3:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(272);
						if (!(precpred(_ctx, 16))) throw new FailedPredicateException(this, "precpred(_ctx, 16)");
						setState(273);
						((ExprContext)_localctx).PERCENT = match(PERCENT);
						setState(274);
						((ExprContext)_localctx).e2 = expr(17);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.Mod,((ExprContext)_localctx).PERCENT.ToSourceSpan()),((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 4:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(277);
						if (!(precpred(_ctx, 15))) throw new FailedPredicateException(this, "precpred(_ctx, 15)");
						setState(278);
						((ExprContext)_localctx).addsubop = addsubop();
						setState(279);
						((ExprContext)_localctx).e2 = expr(16);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,((ExprContext)_localctx).addsubop.op,((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 5:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(282);
						if (!(precpred(_ctx, 14))) throw new FailedPredicateException(this, "precpred(_ctx, 14)");
						setState(283);
						((ExprContext)_localctx).bitshop = bitshop();
						setState(284);
						((ExprContext)_localctx).e2 = expr(15);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,((ExprContext)_localctx).bitshop.op,((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 6:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(287);
						if (!(precpred(_ctx, 13))) throw new FailedPredicateException(this, "precpred(_ctx, 13)");
						setState(288);
						((ExprContext)_localctx).BITAND = match(BITAND);
						setState(289);
						((ExprContext)_localctx).e2 = expr(14);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.BitAND,((ExprContext)_localctx).BITAND.ToSourceSpan()),((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 7:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(292);
						if (!(precpred(_ctx, 12))) throw new FailedPredicateException(this, "precpred(_ctx, 12)");
						setState(293);
						((ExprContext)_localctx).BITXOR = match(BITXOR);
						setState(294);
						((ExprContext)_localctx).e2 = expr(13);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.BitXOR,((ExprContext)_localctx).BITXOR.ToSourceSpan()),((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 8:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(297);
						if (!(precpred(_ctx, 11))) throw new FailedPredicateException(this, "precpred(_ctx, 11)");
						setState(298);
						((ExprContext)_localctx).BITOR = match(BITOR);
						setState(299);
						((ExprContext)_localctx).e2 = expr(12);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.BitOR,((ExprContext)_localctx).BITOR.ToSourceSpan()),((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 9:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(302);
						if (!(precpred(_ctx, 10))) throw new FailedPredicateException(this, "precpred(_ctx, 10)");
						setState(303);
						((ExprContext)_localctx).APPEND = match(APPEND);
						setState(304);
						((ExprContext)_localctx).e2 = expr(11);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.Concat,((ExprContext)_localctx).APPEND.ToSourceSpan()),((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 10:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(307);
						if (!(precpred(_ctx, 9))) throw new FailedPredicateException(this, "precpred(_ctx, 9)");
						setState(308);
						((ExprContext)_localctx).LANGLE = match(LANGLE);
						setState(309);
						((ExprContext)_localctx).e2 = expr(10);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.LessThan,((ExprContext)_localctx).LANGLE.ToSourceSpan()),((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 11:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(312);
						if (!(precpred(_ctx, 8))) throw new FailedPredicateException(this, "precpred(_ctx, 8)");
						setState(313);
						((ExprContext)_localctx).RANGLE = match(RANGLE);
						setState(314);
						((ExprContext)_localctx).e2 = expr(9);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.GreaterThan,((ExprContext)_localctx).RANGLE.ToSourceSpan()),((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 12:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(317);
						if (!(precpred(_ctx, 7))) throw new FailedPredicateException(this, "precpred(_ctx, 7)");
						setState(318);
						((ExprContext)_localctx).LEQ = match(LEQ);
						setState(319);
						((ExprContext)_localctx).e2 = expr(7);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.LessOrEqualTo,((ExprContext)_localctx).LEQ.ToSourceSpan()),((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 13:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(322);
						if (!(precpred(_ctx, 6))) throw new FailedPredicateException(this, "precpred(_ctx, 6)");
						setState(323);
						((ExprContext)_localctx).GEQ = match(GEQ);
						setState(324);
						((ExprContext)_localctx).e2 = expr(6);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.GreaterOrEqualTo,((ExprContext)_localctx).GEQ.ToSourceSpan()),((ExprContext)_localctx).e2.exp,((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 14:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(327);
						if (!(precpred(_ctx, 5))) throw new FailedPredicateException(this, "precpred(_ctx, 5)");
						setState(328);
						((ExprContext)_localctx).EQEQ = match(EQEQ);
						setState(329);
						((ExprContext)_localctx).e2 = expr(5);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.RefEquals, ((ExprContext)_localctx).EQEQ.ToSourceSpan()), ((ExprContext)_localctx).e2.exp);
						}
						break;
					case 15:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(332);
						if (!(precpred(_ctx, 4))) throw new FailedPredicateException(this, "precpred(_ctx, 4)");
						setState(333);
						((ExprContext)_localctx).NEQ = match(NEQ);
						setState(334);
						((ExprContext)_localctx).e2 = expr(4);
						((ExprContext)_localctx).exp =  new CallExpr(new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.RefEquals, ((ExprContext)_localctx).NEQ.ToSourceSpan()), ((ExprContext)_localctx).e2.exp), new Identifier("Not", ((ExprContext)_localctx).NEQ.ToSourceSpan()), new List<IExpr>(), ((ExprContext)_localctx).e1.exp.Start.SpanTo(((ExprContext)_localctx).e2.exp.End));
						}
						break;
					case 16:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(337);
						if (!(precpred(_ctx, 3))) throw new FailedPredicateException(this, "precpred(_ctx, 3)");
						setState(338);
						((ExprContext)_localctx).AND = match(AND);
						setState(339);
						((ExprContext)_localctx).e2 = expr(3);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.And, ((ExprContext)_localctx).AND.ToSourceSpan()),((ExprContext)_localctx).e2.exp);
						}
						break;
					case 17:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(342);
						if (!(precpred(_ctx, 2))) throw new FailedPredicateException(this, "precpred(_ctx, 2)");
						setState(343);
						((ExprContext)_localctx).OR = match(OR);
						setState(344);
						((ExprContext)_localctx).e2 = expr(2);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.Or, ((ExprContext)_localctx).OR.ToSourceSpan()),((ExprContext)_localctx).e2.exp);
						}
						break;
					case 18:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.e1 = _prevctx;
						_localctx.e1 = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(347);
						if (!(precpred(_ctx, 1))) throw new FailedPredicateException(this, "precpred(_ctx, 1)");
						setState(348);
						((ExprContext)_localctx).EQEQEQ = match(EQEQEQ);
						setState(349);
						((ExprContext)_localctx).e2 = expr(1);
						((ExprContext)_localctx).exp =  new BinOpExpr(((ExprContext)_localctx).e1.exp,new BinaryOperatorNode(BinaryOperator.Equals, ((ExprContext)_localctx).EQEQEQ.ToSourceSpan()),((ExprContext)_localctx).e2.exp);
						}
						break;
					case 19:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.r = _prevctx;
						_localctx.r = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(352);
						if (!(precpred(_ctx, 28))) throw new FailedPredicateException(this, "precpred(_ctx, 28)");
						setState(353);
						match(DOT);
						setState(354);
						((ExprContext)_localctx).ident = ident();
						((ExprContext)_localctx).exp = new IdentExpr(((ExprContext)_localctx).ident.i, ((ExprContext)_localctx).r.exp);
						}
						break;
					case 20:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						_localctx.r = _prevctx;
						_localctx.r = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(357);
						if (!(precpred(_ctx, 27))) throw new FailedPredicateException(this, "precpred(_ctx, 27)");
						setState(358);
						match(DOT);
						setState(359);
						((ExprContext)_localctx).ident = ident();
						setState(360);
						((ExprContext)_localctx).LPAREN = match(LPAREN);
						setState(369);
						_errHandler.sync(this);
						_la = _input.LA(1);
						if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << NUL) | (1L << NEW) | (1L << LPAREN) | (1L << LBRACKET) | (1L << LBRACE))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (BANG - 64)) | (1L << (MINUS - 64)) | (1L << (ID - 64)))) != 0)) {
							{
							setState(361);
							((ExprContext)_localctx).expr = expr(0);
							((ExprContext)_localctx).args.add(((ExprContext)_localctx).expr);
							setState(366);
							_errHandler.sync(this);
							_la = _input.LA(1);
							while (_la==COMMA) {
								{
								{
								setState(362);
								match(COMMA);
								setState(363);
								((ExprContext)_localctx).expr = expr(0);
								((ExprContext)_localctx).args.add(((ExprContext)_localctx).expr);
								}
								}
								setState(368);
								_errHandler.sync(this);
								_la = _input.LA(1);
							}
							}
						}

						setState(371);
						((ExprContext)_localctx).RPAREN = match(RPAREN);
						((ExprContext)_localctx).exp = new CallExpr(((ExprContext)_localctx).r.exp, ((ExprContext)_localctx).ident.i, ((ExprContext)_localctx).args.Select(e=>e.exp), ((ExprContext)_localctx).r.exp.Start.SpanTo(((ExprContext)_localctx).RPAREN));
						}
						break;
					}
					} 
				}
				setState(378);
				_errHandler.sync(this);
				_alt = getInterpreter().adaptivePredict(_input,25,_ctx);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			unrollRecursionContexts(_parentctx);
		}
		return _localctx;
	}

	public static class RangeexprContext extends ParserRuleContext {
		public RangeExpr rexp;
		public Token LBRACKET;
		public ExprContext e1;
		public Token RBRACKET;
		public ExprContext e2;
		public ExprContext e3;
		public TerminalNode LBRACKET() { return getToken(NominalGradualParser.LBRACKET, 0); }
		public TerminalNode DOTDOT() { return getToken(NominalGradualParser.DOTDOT, 0); }
		public TerminalNode RBRACKET() { return getToken(NominalGradualParser.RBRACKET, 0); }
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode COLON() { return getToken(NominalGradualParser.COLON, 0); }
		public RangeexprContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_rangeexpr; }
	}

	public final RangeexprContext rangeexpr() throws RecognitionException {
		RangeexprContext _localctx = new RangeexprContext(_ctx, getState());
		enterRule(_localctx, 14, RULE_rangeexpr);
		try {
			setState(409);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,26,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(379);
				((RangeexprContext)_localctx).LBRACKET = match(LBRACKET);
				setState(380);
				match(DOTDOT);
				setState(381);
				((RangeexprContext)_localctx).e1 = expr(0);
				setState(382);
				((RangeexprContext)_localctx).RBRACKET = match(RBRACKET);
				((RangeexprContext)_localctx).rexp = new RangeExpr(((RangeexprContext)_localctx).e1.exp,null,null,((RangeexprContext)_localctx).LBRACKET.ToSourceSpan(((RangeexprContext)_localctx).RBRACKET));
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(385);
				((RangeexprContext)_localctx).LBRACKET = match(LBRACKET);
				setState(386);
				((RangeexprContext)_localctx).e2 = expr(0);
				setState(387);
				match(DOTDOT);
				setState(388);
				((RangeexprContext)_localctx).e1 = expr(0);
				setState(389);
				((RangeexprContext)_localctx).RBRACKET = match(RBRACKET);
				((RangeexprContext)_localctx).rexp = new RangeExpr(((RangeexprContext)_localctx).e1.exp,((RangeexprContext)_localctx).e2.exp,null,((RangeexprContext)_localctx).LBRACKET.ToSourceSpan(((RangeexprContext)_localctx).RBRACKET));
				}
				break;
			case 3:
				enterOuterAlt(_localctx, 3);
				{
				setState(392);
				((RangeexprContext)_localctx).LBRACKET = match(LBRACKET);
				setState(393);
				match(DOTDOT);
				setState(394);
				((RangeexprContext)_localctx).e1 = expr(0);
				setState(395);
				match(COLON);
				setState(396);
				((RangeexprContext)_localctx).e3 = expr(0);
				setState(397);
				((RangeexprContext)_localctx).RBRACKET = match(RBRACKET);
				((RangeexprContext)_localctx).rexp = new RangeExpr(((RangeexprContext)_localctx).e1.exp,null,((RangeexprContext)_localctx).e3.exp,((RangeexprContext)_localctx).LBRACKET.ToSourceSpan(((RangeexprContext)_localctx).RBRACKET));
				}
				break;
			case 4:
				enterOuterAlt(_localctx, 4);
				{
				setState(400);
				((RangeexprContext)_localctx).LBRACKET = match(LBRACKET);
				setState(401);
				((RangeexprContext)_localctx).e2 = expr(0);
				setState(402);
				match(DOTDOT);
				setState(403);
				((RangeexprContext)_localctx).e1 = expr(0);
				setState(404);
				match(COLON);
				setState(405);
				((RangeexprContext)_localctx).e3 = expr(0);
				setState(406);
				((RangeexprContext)_localctx).RBRACKET = match(RBRACKET);
				((RangeexprContext)_localctx).rexp = new RangeExpr(((RangeexprContext)_localctx).e1.exp,((RangeexprContext)_localctx).e2.exp,((RangeexprContext)_localctx).e3.exp,((RangeexprContext)_localctx).LBRACKET.ToSourceSpan(((RangeexprContext)_localctx).RBRACKET));
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class DivmultopContext extends ParserRuleContext {
		public BinaryOperatorNode op;
		public Token TIMES;
		public Token SLASH;
		public TerminalNode TIMES() { return getToken(NominalGradualParser.TIMES, 0); }
		public TerminalNode SLASH() { return getToken(NominalGradualParser.SLASH, 0); }
		public DivmultopContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_divmultop; }
	}

	public final DivmultopContext divmultop() throws RecognitionException {
		DivmultopContext _localctx = new DivmultopContext(_ctx, getState());
		enterRule(_localctx, 16, RULE_divmultop);
		try {
			setState(415);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case TIMES:
				enterOuterAlt(_localctx, 1);
				{
				setState(411);
				((DivmultopContext)_localctx).TIMES = match(TIMES);
				((DivmultopContext)_localctx).op = new BinaryOperatorNode(BinaryOperator.Multiply, ((DivmultopContext)_localctx).TIMES.ToSourceSpan());
				}
				break;
			case SLASH:
				enterOuterAlt(_localctx, 2);
				{
				setState(413);
				((DivmultopContext)_localctx).SLASH = match(SLASH);
				((DivmultopContext)_localctx).op = new BinaryOperatorNode(BinaryOperator.Divide, ((DivmultopContext)_localctx).SLASH.ToSourceSpan());
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class AddsubopContext extends ParserRuleContext {
		public BinaryOperatorNode op;
		public Token PLUS;
		public Token MINUS;
		public TerminalNode PLUS() { return getToken(NominalGradualParser.PLUS, 0); }
		public TerminalNode MINUS() { return getToken(NominalGradualParser.MINUS, 0); }
		public AddsubopContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_addsubop; }
	}

	public final AddsubopContext addsubop() throws RecognitionException {
		AddsubopContext _localctx = new AddsubopContext(_ctx, getState());
		enterRule(_localctx, 18, RULE_addsubop);
		try {
			setState(421);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case PLUS:
				enterOuterAlt(_localctx, 1);
				{
				setState(417);
				((AddsubopContext)_localctx).PLUS = match(PLUS);
				((AddsubopContext)_localctx).op = new BinaryOperatorNode(BinaryOperator.Add, ((AddsubopContext)_localctx).PLUS.ToSourceSpan());
				}
				break;
			case MINUS:
				enterOuterAlt(_localctx, 2);
				{
				setState(419);
				((AddsubopContext)_localctx).MINUS = match(MINUS);
				((AddsubopContext)_localctx).op = new BinaryOperatorNode(BinaryOperator.Subtract, ((AddsubopContext)_localctx).MINUS.ToSourceSpan());
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class BitshopContext extends ParserRuleContext {
		public BinaryOperatorNode op;
		public Token BITSHL;
		public Token BITSHR;
		public TerminalNode BITSHL() { return getToken(NominalGradualParser.BITSHL, 0); }
		public TerminalNode BITSHR() { return getToken(NominalGradualParser.BITSHR, 0); }
		public BitshopContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_bitshop; }
	}

	public final BitshopContext bitshop() throws RecognitionException {
		BitshopContext _localctx = new BitshopContext(_ctx, getState());
		enterRule(_localctx, 20, RULE_bitshop);
		try {
			setState(427);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case BITSHL:
				enterOuterAlt(_localctx, 1);
				{
				setState(423);
				((BitshopContext)_localctx).BITSHL = match(BITSHL);
				((BitshopContext)_localctx).op = new BinaryOperatorNode(BinaryOperator.ShiftLeft, ((BitshopContext)_localctx).BITSHL.ToSourceSpan());
				}
				break;
			case BITSHR:
				enterOuterAlt(_localctx, 2);
				{
				setState(425);
				((BitshopContext)_localctx).BITSHR = match(BITSHR);
				((BitshopContext)_localctx).op = new BinaryOperatorNode(BinaryOperator.ShiftRight, ((BitshopContext)_localctx).BITSHR.ToSourceSpan());
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class StmtContext extends ParserRuleContext {
		public IStmt s;
		public ExprContext expr;
		public List<ExprContext> ae = new ArrayList<ExprContext>();
		public IdentContext ident;
		public ExprContext e;
		public Token SEMICOLON;
		public Token PLUSEQ;
		public Token MINUSEQ;
		public Token TIMESEQ;
		public Token DIVEQ;
		public DtypeContext dtype;
		public Token DBG;
		public Token STRING;
		public Token ERR;
		public Token BREAK;
		public Token INT;
		public Token CONTINUE;
		public Token RETURN;
		public List<ExprContext> r = new ArrayList<ExprContext>();
		public Token IF;
		public BlockContext then;
		public List<ExprContext> ee = new ArrayList<ExprContext>();
		public BlockContext block;
		public List<BlockContext> elseifs = new ArrayList<BlockContext>();
		public List<BlockContext> elseb = new ArrayList<BlockContext>();
		public TypeContext type;
		public BlockContext b;
		public ArgdeclContext i;
		public ArgdeclContext argdecl;
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode EQ() { return getToken(NominalGradualParser.EQ, 0); }
		public TerminalNode SEMICOLON() { return getToken(NominalGradualParser.SEMICOLON, 0); }
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode DOT() { return getToken(NominalGradualParser.DOT, 0); }
		public TerminalNode PLUSEQ() { return getToken(NominalGradualParser.PLUSEQ, 0); }
		public TerminalNode MINUSEQ() { return getToken(NominalGradualParser.MINUSEQ, 0); }
		public TerminalNode TIMESEQ() { return getToken(NominalGradualParser.TIMESEQ, 0); }
		public TerminalNode DIVEQ() { return getToken(NominalGradualParser.DIVEQ, 0); }
		public DtypeContext dtype() {
			return getRuleContext(DtypeContext.class,0);
		}
		public TerminalNode DBG() { return getToken(NominalGradualParser.DBG, 0); }
		public List<TerminalNode> LPAREN() { return getTokens(NominalGradualParser.LPAREN); }
		public TerminalNode LPAREN(int i) {
			return getToken(NominalGradualParser.LPAREN, i);
		}
		public TerminalNode STRING() { return getToken(NominalGradualParser.STRING, 0); }
		public List<TerminalNode> RPAREN() { return getTokens(NominalGradualParser.RPAREN); }
		public TerminalNode RPAREN(int i) {
			return getToken(NominalGradualParser.RPAREN, i);
		}
		public TerminalNode ERR() { return getToken(NominalGradualParser.ERR, 0); }
		public TerminalNode BREAK() { return getToken(NominalGradualParser.BREAK, 0); }
		public TerminalNode INT() { return getToken(NominalGradualParser.INT, 0); }
		public TerminalNode CONTINUE() { return getToken(NominalGradualParser.CONTINUE, 0); }
		public TerminalNode RETURN() { return getToken(NominalGradualParser.RETURN, 0); }
		public TerminalNode IF() { return getToken(NominalGradualParser.IF, 0); }
		public List<BlockContext> block() {
			return getRuleContexts(BlockContext.class);
		}
		public BlockContext block(int i) {
			return getRuleContext(BlockContext.class,i);
		}
		public List<TerminalNode> ELSEIF() { return getTokens(NominalGradualParser.ELSEIF); }
		public TerminalNode ELSEIF(int i) {
			return getToken(NominalGradualParser.ELSEIF, i);
		}
		public TerminalNode ELSE() { return getToken(NominalGradualParser.ELSE, 0); }
		public TerminalNode COLON() { return getToken(NominalGradualParser.COLON, 0); }
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode WHILE() { return getToken(NominalGradualParser.WHILE, 0); }
		public TerminalNode FOREACH() { return getToken(NominalGradualParser.FOREACH, 0); }
		public TerminalNode IN() { return getToken(NominalGradualParser.IN, 0); }
		public ArgdeclContext argdecl() {
			return getRuleContext(ArgdeclContext.class,0);
		}
		public StmtContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_stmt; }
	}

	public final StmtContext stmt() throws RecognitionException {
		StmtContext _localctx = new StmtContext(_ctx, getState());
		enterRule(_localctx, 22, RULE_stmt);
		int _la;
		try {
			setState(582);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,41,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(432);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,30,_ctx) ) {
				case 1:
					{
					setState(429);
					((StmtContext)_localctx).expr = expr(0);
					((StmtContext)_localctx).ae.add(((StmtContext)_localctx).expr);
					setState(430);
					match(DOT);
					}
					break;
				}
				setState(434);
				((StmtContext)_localctx).ident = ident();
				setState(435);
				match(EQ);
				setState(436);
				((StmtContext)_localctx).e = expr(0);
				setState(437);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new AssignStmt(((StmtContext)_localctx).ae.SingleOrDefault()?.exp, ((StmtContext)_localctx).ident.i,((StmtContext)_localctx).e.exp,((StmtContext)_localctx).ident.i.Start.SpanTo(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(443);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,31,_ctx) ) {
				case 1:
					{
					setState(440);
					((StmtContext)_localctx).expr = expr(0);
					((StmtContext)_localctx).ae.add(((StmtContext)_localctx).expr);
					setState(441);
					match(DOT);
					}
					break;
				}
				setState(445);
				((StmtContext)_localctx).ident = ident();
				setState(446);
				((StmtContext)_localctx).PLUSEQ = match(PLUSEQ);
				setState(447);
				((StmtContext)_localctx).e = expr(0);
				setState(448);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new AssignStmt(((StmtContext)_localctx).ae.SingleOrDefault()?.exp, ((StmtContext)_localctx).ident.i,new BinOpExpr(new IdentExpr(((StmtContext)_localctx).ident.i, ((StmtContext)_localctx).ae.SingleOrDefault()?.exp),new BinaryOperatorNode(BinaryOperator.Add, ((StmtContext)_localctx).PLUSEQ.ToSourceSpan()),((StmtContext)_localctx).e.exp),((StmtContext)_localctx).ident.i.Start.SpanTo(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 3:
				enterOuterAlt(_localctx, 3);
				{
				setState(454);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,32,_ctx) ) {
				case 1:
					{
					setState(451);
					((StmtContext)_localctx).expr = expr(0);
					((StmtContext)_localctx).ae.add(((StmtContext)_localctx).expr);
					setState(452);
					match(DOT);
					}
					break;
				}
				setState(456);
				((StmtContext)_localctx).ident = ident();
				setState(457);
				((StmtContext)_localctx).MINUSEQ = match(MINUSEQ);
				setState(458);
				((StmtContext)_localctx).e = expr(0);
				setState(459);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new AssignStmt(((StmtContext)_localctx).ae.SingleOrDefault()?.exp, ((StmtContext)_localctx).ident.i,new BinOpExpr(new IdentExpr(((StmtContext)_localctx).ident.i, ((StmtContext)_localctx).ae.SingleOrDefault()?.exp),new BinaryOperatorNode(BinaryOperator.Subtract, ((StmtContext)_localctx).MINUSEQ.ToSourceSpan()),((StmtContext)_localctx).e.exp),((StmtContext)_localctx).ident.i.Start.SpanTo(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 4:
				enterOuterAlt(_localctx, 4);
				{
				setState(465);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,33,_ctx) ) {
				case 1:
					{
					setState(462);
					((StmtContext)_localctx).expr = expr(0);
					((StmtContext)_localctx).ae.add(((StmtContext)_localctx).expr);
					setState(463);
					match(DOT);
					}
					break;
				}
				setState(467);
				((StmtContext)_localctx).ident = ident();
				setState(468);
				((StmtContext)_localctx).TIMESEQ = match(TIMESEQ);
				setState(469);
				((StmtContext)_localctx).e = expr(0);
				setState(470);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new AssignStmt(((StmtContext)_localctx).ae.SingleOrDefault()?.exp, ((StmtContext)_localctx).ident.i,new BinOpExpr(new IdentExpr(((StmtContext)_localctx).ident.i, ((StmtContext)_localctx).ae.SingleOrDefault()?.exp),new BinaryOperatorNode(BinaryOperator.Multiply, ((StmtContext)_localctx).TIMESEQ.ToSourceSpan()),((StmtContext)_localctx).e.exp),((StmtContext)_localctx).ident.i.Start.SpanTo(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 5:
				enterOuterAlt(_localctx, 5);
				{
				setState(476);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,34,_ctx) ) {
				case 1:
					{
					setState(473);
					((StmtContext)_localctx).expr = expr(0);
					((StmtContext)_localctx).ae.add(((StmtContext)_localctx).expr);
					setState(474);
					match(DOT);
					}
					break;
				}
				setState(478);
				((StmtContext)_localctx).ident = ident();
				setState(479);
				((StmtContext)_localctx).DIVEQ = match(DIVEQ);
				setState(480);
				((StmtContext)_localctx).e = expr(0);
				setState(481);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new AssignStmt(((StmtContext)_localctx).ae.SingleOrDefault()?.exp, ((StmtContext)_localctx).ident.i,new BinOpExpr(new IdentExpr(((StmtContext)_localctx).ident.i, ((StmtContext)_localctx).ae.SingleOrDefault()?.exp),new BinaryOperatorNode(BinaryOperator.Divide, ((StmtContext)_localctx).DIVEQ.ToSourceSpan()),((StmtContext)_localctx).e.exp),((StmtContext)_localctx).ident.i.Start.SpanTo(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 6:
				enterOuterAlt(_localctx, 6);
				{
				setState(484);
				((StmtContext)_localctx).dtype = dtype();
				setState(485);
				((StmtContext)_localctx).ident = ident();
				setState(486);
				match(EQ);
				setState(487);
				((StmtContext)_localctx).expr = expr(0);
				setState(488);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new DeclStmt(((StmtContext)_localctx).dtype.t, ((StmtContext)_localctx).ident.i, ((StmtContext)_localctx).expr.exp,((StmtContext)_localctx).dtype.t.Start.SpanTo(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 7:
				enterOuterAlt(_localctx, 7);
				{
				setState(491);
				((StmtContext)_localctx).dtype = dtype();
				setState(492);
				((StmtContext)_localctx).ident = ident();
				setState(493);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new DeclStmt(((StmtContext)_localctx).dtype.t, ((StmtContext)_localctx).ident.i, null,((StmtContext)_localctx).dtype.t.Start.SpanTo(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 8:
				enterOuterAlt(_localctx, 8);
				{
				setState(496);
				((StmtContext)_localctx).expr = expr(0);
				setState(497);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new ExprStmt(((StmtContext)_localctx).expr.exp,((StmtContext)_localctx).expr.exp.Start.SpanTo(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 9:
				enterOuterAlt(_localctx, 9);
				{
				setState(500);
				((StmtContext)_localctx).DBG = match(DBG);
				setState(501);
				match(LPAREN);
				setState(502);
				((StmtContext)_localctx).STRING = match(STRING);
				setState(503);
				match(RPAREN);
				setState(504);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new DebugStmt(((StmtContext)_localctx).STRING,((StmtContext)_localctx).DBG.ToSourceSpan(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 10:
				enterOuterAlt(_localctx, 10);
				{
				setState(506);
				((StmtContext)_localctx).ERR = match(ERR);
				setState(507);
				match(LPAREN);
				setState(508);
				((StmtContext)_localctx).expr = expr(0);
				setState(509);
				match(RPAREN);
				setState(510);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new ErrorStmt(((StmtContext)_localctx).expr.exp,((StmtContext)_localctx).ERR.ToSourceSpan(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 11:
				enterOuterAlt(_localctx, 11);
				{
				setState(513);
				((StmtContext)_localctx).BREAK = match(BREAK);
				setState(515);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==INT) {
					{
					setState(514);
					((StmtContext)_localctx).INT = match(INT);
					}
				}

				setState(517);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new BreakStmt(((StmtContext)_localctx).INT,((StmtContext)_localctx).BREAK.ToSourceSpan(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 12:
				enterOuterAlt(_localctx, 12);
				{
				setState(519);
				((StmtContext)_localctx).CONTINUE = match(CONTINUE);
				setState(521);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==INT) {
					{
					setState(520);
					((StmtContext)_localctx).INT = match(INT);
					}
				}

				setState(523);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new ContinueStmt(((StmtContext)_localctx).INT,((StmtContext)_localctx).CONTINUE.ToSourceSpan(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 13:
				enterOuterAlt(_localctx, 13);
				{
				setState(525);
				((StmtContext)_localctx).RETURN = match(RETURN);
				setState(527);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << NUL) | (1L << NEW) | (1L << LPAREN) | (1L << LBRACKET) | (1L << LBRACE))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (BANG - 64)) | (1L << (MINUS - 64)) | (1L << (ID - 64)))) != 0)) {
					{
					setState(526);
					((StmtContext)_localctx).expr = expr(0);
					((StmtContext)_localctx).r.add(((StmtContext)_localctx).expr);
					}
				}

				setState(529);
				((StmtContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StmtContext)_localctx).s =  new ReturnStmt(((StmtContext)_localctx).r.SingleOrDefault()?.exp,((StmtContext)_localctx).RETURN.ToSourceSpan(((StmtContext)_localctx).SEMICOLON));
				}
				break;
			case 14:
				enterOuterAlt(_localctx, 14);
				{
				setState(531);
				((StmtContext)_localctx).IF = match(IF);
				setState(532);
				match(LPAREN);
				setState(533);
				((StmtContext)_localctx).e = expr(0);
				setState(534);
				match(RPAREN);
				setState(535);
				((StmtContext)_localctx).then = block();
				setState(544);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==ELSEIF) {
					{
					{
					setState(536);
					match(ELSEIF);
					setState(537);
					match(LPAREN);
					setState(538);
					((StmtContext)_localctx).expr = expr(0);
					((StmtContext)_localctx).ee.add(((StmtContext)_localctx).expr);
					setState(539);
					match(RPAREN);
					setState(540);
					((StmtContext)_localctx).block = block();
					((StmtContext)_localctx).elseifs.add(((StmtContext)_localctx).block);
					}
					}
					setState(546);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(549);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==ELSE) {
					{
					setState(547);
					match(ELSE);
					setState(548);
					((StmtContext)_localctx).block = block();
					((StmtContext)_localctx).elseb.add(((StmtContext)_localctx).block);
					}
				}

				((StmtContext)_localctx).s = new IfStmt(((StmtContext)_localctx).e.exp,((StmtContext)_localctx).then.b,((StmtContext)_localctx).ee.Select(e=>e.exp),((StmtContext)_localctx).elseifs.Select(b=>b.b),((StmtContext)_localctx).elseb.Select(b=>b.b),((StmtContext)_localctx).IF.ToSourceSpan((((StmtContext)_localctx).elseb.Count()>0?((StmtContext)_localctx).elseb.Single().b.End:(((StmtContext)_localctx).elseifs.Count()>0?((StmtContext)_localctx).elseifs.Last().b.End:((StmtContext)_localctx).then.b.End))));
				}
				break;
			case 15:
				enterOuterAlt(_localctx, 15);
				{
				setState(553);
				match(IF);
				setState(554);
				match(LPAREN);
				setState(555);
				((StmtContext)_localctx).ident = ident();
				setState(556);
				match(COLON);
				setState(557);
				((StmtContext)_localctx).type = type(0);
				setState(558);
				match(RPAREN);
				setState(559);
				((StmtContext)_localctx).then = block();
				setState(562);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==ELSE) {
					{
					setState(560);
					match(ELSE);
					setState(561);
					((StmtContext)_localctx).block = block();
					((StmtContext)_localctx).elseb.add(((StmtContext)_localctx).block);
					}
				}

				((StmtContext)_localctx).s = new IfTypeStmt(((StmtContext)_localctx).ident.i,((StmtContext)_localctx).type.t,((StmtContext)_localctx).then.b, ((StmtContext)_localctx).elseb.Select(b=>b.b).SingleOrDefault(), _localctx.start.ToSourceSpan(((StmtContext)_localctx).elseb.Count()>0?((StmtContext)_localctx).elseb.Single().b.End:((StmtContext)_localctx).then.b.End));
				}
				break;
			case 16:
				enterOuterAlt(_localctx, 16);
				{
				setState(566);
				match(WHILE);
				setState(567);
				match(LPAREN);
				setState(568);
				((StmtContext)_localctx).e = expr(0);
				setState(569);
				match(RPAREN);
				setState(570);
				((StmtContext)_localctx).b = block();
				((StmtContext)_localctx).s = new WhileStmt(((StmtContext)_localctx).e.exp,((StmtContext)_localctx).b.b, _localctx.start.ToSourceSpan(((StmtContext)_localctx).b.b.End));
				}
				break;
			case 17:
				enterOuterAlt(_localctx, 17);
				{
				setState(573);
				match(FOREACH);
				setState(574);
				match(LPAREN);
				setState(575);
				((StmtContext)_localctx).i = ((StmtContext)_localctx).argdecl = argdecl();
				setState(576);
				match(IN);
				setState(577);
				((StmtContext)_localctx).e = expr(0);
				setState(578);
				match(RPAREN);
				setState(579);
				((StmtContext)_localctx).block = block();
				((StmtContext)_localctx).s = new ForeachStmt(((StmtContext)_localctx).argdecl.v,((StmtContext)_localctx).e.exp,((StmtContext)_localctx).block.b,_localctx.start.ToSourceSpan(((StmtContext)_localctx).block.b.End));
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class BlockContext extends ParserRuleContext {
		public Block b;
		public Token LBRACE;
		public StmtContext stmt;
		public List<StmtContext> ss = new ArrayList<StmtContext>();
		public Token RBRACE;
		public TerminalNode LBRACE() { return getToken(NominalGradualParser.LBRACE, 0); }
		public TerminalNode RBRACE() { return getToken(NominalGradualParser.RBRACE, 0); }
		public List<StmtContext> stmt() {
			return getRuleContexts(StmtContext.class);
		}
		public StmtContext stmt(int i) {
			return getRuleContext(StmtContext.class,i);
		}
		public BlockContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_block; }
	}

	public final BlockContext block() throws RecognitionException {
		BlockContext _localctx = new BlockContext(_ctx, getState());
		enterRule(_localctx, 24, RULE_block);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(584);
			((BlockContext)_localctx).LBRACE = match(LBRACE);
			setState(588);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << DYN) | (1L << NUL) | (1L << IF) | (1L << WHILE) | (1L << FOREACH) | (1L << RETURN) | (1L << BREAK) | (1L << CONTINUE) | (1L << NEW) | (1L << DBG) | (1L << ERR) | (1L << LPAREN) | (1L << LBRACKET) | (1L << LBRACE))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (BANG - 64)) | (1L << (MINUS - 64)) | (1L << (ID - 64)))) != 0)) {
				{
				{
				setState(585);
				((BlockContext)_localctx).stmt = stmt();
				((BlockContext)_localctx).ss.add(((BlockContext)_localctx).stmt);
				}
				}
				setState(590);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(591);
			((BlockContext)_localctx).RBRACE = match(RBRACE);
			((BlockContext)_localctx).b = new Block(((BlockContext)_localctx).ss.Select(s=>s.s),((BlockContext)_localctx).LBRACE.ToSourceSpan(((BlockContext)_localctx).RBRACE));
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class IdentContext extends ParserRuleContext {
		public Identifier i;
		public IToken tok;
		public Token ID;
		public TerminalNode ID() { return getToken(NominalGradualParser.ID, 0); }
		public IdentContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_ident; }
	}

	public final IdentContext ident() throws RecognitionException {
		IdentContext _localctx = new IdentContext(_ctx, getState());
		enterRule(_localctx, 26, RULE_ident);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(594);
			((IdentContext)_localctx).ID = match(ID);
			((IdentContext)_localctx).i = new Identifier(((IdentContext)_localctx).ID); ((IdentContext)_localctx).tok = ((IdentContext)_localctx).ID;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class CtypeContext extends ParserRuleContext {
		public ClassType ct;
		public QnameContext qname;
		public QnameContext qname() {
			return getRuleContext(QnameContext.class,0);
		}
		public CtypeContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_ctype; }
	}

	public final CtypeContext ctype() throws RecognitionException {
		CtypeContext _localctx = new CtypeContext(_ctx, getState());
		enterRule(_localctx, 28, RULE_ctype);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(597);
			((CtypeContext)_localctx).qname = qname();
			((CtypeContext)_localctx).ct = ClassType.GetInstance(((CtypeContext)_localctx).qname.q);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class InterfacedefContext extends ParserRuleContext {
		public InterfaceDef idef;
		public VisibilityContext visibility;
		public List<VisibilityContext> v = new ArrayList<VisibilityContext>();
		public Token PARTIAL;
		public List<Token> p = new ArrayList<Token>();
		public IdentContext n;
		public CtypeContext ctype;
		public List<CtypeContext> si = new ArrayList<CtypeContext>();
		public MethdeclContext methdecl;
		public List<MethdeclContext> md = new ArrayList<MethdeclContext>();
		public InterfacedefContext interfacedef;
		public List<InterfacedefContext> ifdef = new ArrayList<InterfacedefContext>();
		public Token RBRACE;
		public TerminalNode INTERFACE() { return getToken(NominalGradualParser.INTERFACE, 0); }
		public TerminalNode LBRACE() { return getToken(NominalGradualParser.LBRACE, 0); }
		public TerminalNode RBRACE() { return getToken(NominalGradualParser.RBRACE, 0); }
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode EXTENDS() { return getToken(NominalGradualParser.EXTENDS, 0); }
		public List<VisibilityContext> visibility() {
			return getRuleContexts(VisibilityContext.class);
		}
		public VisibilityContext visibility(int i) {
			return getRuleContext(VisibilityContext.class,i);
		}
		public List<TerminalNode> PARTIAL() { return getTokens(NominalGradualParser.PARTIAL); }
		public TerminalNode PARTIAL(int i) {
			return getToken(NominalGradualParser.PARTIAL, i);
		}
		public List<CtypeContext> ctype() {
			return getRuleContexts(CtypeContext.class);
		}
		public CtypeContext ctype(int i) {
			return getRuleContext(CtypeContext.class,i);
		}
		public List<MethdeclContext> methdecl() {
			return getRuleContexts(MethdeclContext.class);
		}
		public MethdeclContext methdecl(int i) {
			return getRuleContext(MethdeclContext.class,i);
		}
		public List<InterfacedefContext> interfacedef() {
			return getRuleContexts(InterfacedefContext.class);
		}
		public InterfacedefContext interfacedef(int i) {
			return getRuleContext(InterfacedefContext.class,i);
		}
		public TerminalNode COMMA() { return getToken(NominalGradualParser.COMMA, 0); }
		public InterfacedefContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_interfacedef; }
	}

	public final InterfacedefContext interfacedef() throws RecognitionException {
		InterfacedefContext _localctx = new InterfacedefContext(_ctx, getState());
		enterRule(_localctx, 30, RULE_interfacedef);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(604);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PARTIAL) | (1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
				{
				setState(602);
				_errHandler.sync(this);
				switch (_input.LA(1)) {
				case PUBLIC:
				case PRIVATE:
				case PROTECTED:
				case INTERNAL:
				case INTERNAL_PROT:
					{
					setState(600);
					((InterfacedefContext)_localctx).visibility = visibility();
					((InterfacedefContext)_localctx).v.add(((InterfacedefContext)_localctx).visibility);
					}
					break;
				case PARTIAL:
					{
					setState(601);
					((InterfacedefContext)_localctx).PARTIAL = match(PARTIAL);
					((InterfacedefContext)_localctx).p.add(((InterfacedefContext)_localctx).PARTIAL);
					}
					break;
				default:
					throw new NoViableAltException(this);
				}
				}
				setState(606);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(607);
			match(INTERFACE);
			setState(608);
			((InterfacedefContext)_localctx).n = ident();
			setState(618);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==EXTENDS) {
				{
				setState(609);
				match(EXTENDS);
				setState(610);
				((InterfacedefContext)_localctx).ctype = ctype();
				((InterfacedefContext)_localctx).si.add(((InterfacedefContext)_localctx).ctype);
				{
				setState(611);
				match(COMMA);
				setState(615);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==ID) {
					{
					{
					setState(612);
					((InterfacedefContext)_localctx).ctype = ctype();
					((InterfacedefContext)_localctx).si.add(((InterfacedefContext)_localctx).ctype);
					}
					}
					setState(617);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
				}
			}

			setState(620);
			match(LBRACE);
			setState(625);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PARTIAL) | (1L << INTERFACE) | (1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT) | (1L << FUN))) != 0)) {
				{
				setState(623);
				_errHandler.sync(this);
				switch (_input.LA(1)) {
				case FUN:
					{
					setState(621);
					((InterfacedefContext)_localctx).methdecl = methdecl();
					((InterfacedefContext)_localctx).md.add(((InterfacedefContext)_localctx).methdecl);
					}
					break;
				case PARTIAL:
				case INTERFACE:
				case PUBLIC:
				case PRIVATE:
				case PROTECTED:
				case INTERNAL:
				case INTERNAL_PROT:
					{
					setState(622);
					((InterfacedefContext)_localctx).interfacedef = interfacedef();
					((InterfacedefContext)_localctx).ifdef.add(((InterfacedefContext)_localctx).interfacedef);
					}
					break;
				default:
					throw new NoViableAltException(this);
				}
				}
				setState(627);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(628);
			((InterfacedefContext)_localctx).RBRACE = match(RBRACE);

				    if(((InterfacedefContext)_localctx).v.Count()>1)
					{
						throw new ParseException("An interface can have at most one visibility modifier!", new SourceLocs(((InterfacedefContext)_localctx).v.Select(vn=>vn.v.Locs)));
					}
				    if(((InterfacedefContext)_localctx).p.Count()>1)
					{
						throw new ParseException("An interface can have at most one partiality marker!", new SourceLocs(((InterfacedefContext)_localctx).p.Select(pp=>pp.ToSourceSpan())));
					}
					((InterfacedefContext)_localctx).idef = new InterfaceDef(((InterfacedefContext)_localctx).n.i,((InterfacedefContext)_localctx).si.Select(c=>c.ct),((InterfacedefContext)_localctx).md.Select(m=>m.m),((InterfacedefContext)_localctx).v.SingleOrDefault()?.v, ((InterfacedefContext)_localctx).p.Count()==1, ((InterfacedefContext)_localctx).ifdef.Select(ii=>ii.idef), _localctx.start.ToSourceSpan(((InterfacedefContext)_localctx).RBRACE));
				  
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ClassdefContext extends ParserRuleContext {
		public ClassDef cdef;
		public Token FINAL;
		public List<Token> f = new ArrayList<Token>();
		public VisibilityContext visibility;
		public List<VisibilityContext> v = new ArrayList<VisibilityContext>();
		public Token ABSTRACT;
		public List<Token> a = new ArrayList<Token>();
		public Token PARTIAL;
		public List<Token> p = new ArrayList<Token>();
		public Token CLASS;
		public IdentContext n;
		public CtypeContext ctype;
		public List<CtypeContext> sc = new ArrayList<CtypeContext>();
		public List<CtypeContext> si = new ArrayList<CtypeContext>();
		public MethdefContext methdef;
		public List<MethdefContext> md = new ArrayList<MethdefContext>();
		public FielddeclContext fielddecl;
		public List<FielddeclContext> fd = new ArrayList<FielddeclContext>();
		public ConstructorContext constructor;
		public List<ConstructorContext> c = new ArrayList<ConstructorContext>();
		public StaticmethdefContext staticmethdef;
		public List<StaticmethdefContext> smd = new ArrayList<StaticmethdefContext>();
		public StaticfielddeclContext staticfielddecl;
		public List<StaticfielddeclContext> sfd = new ArrayList<StaticfielddeclContext>();
		public ClassdefContext classdef;
		public List<ClassdefContext> cd = new ArrayList<ClassdefContext>();
		public InterfacedefContext interfacedef;
		public List<InterfacedefContext> ifdef = new ArrayList<InterfacedefContext>();
		public InstancedeclContext instancedecl;
		public List<InstancedeclContext> idecls = new ArrayList<InstancedeclContext>();
		public Token RBRACE;
		public TerminalNode CLASS() { return getToken(NominalGradualParser.CLASS, 0); }
		public TerminalNode LBRACE() { return getToken(NominalGradualParser.LBRACE, 0); }
		public TerminalNode RBRACE() { return getToken(NominalGradualParser.RBRACE, 0); }
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode EXTENDS() { return getToken(NominalGradualParser.EXTENDS, 0); }
		public TerminalNode IMPLEMENTS() { return getToken(NominalGradualParser.IMPLEMENTS, 0); }
		public List<TerminalNode> FINAL() { return getTokens(NominalGradualParser.FINAL); }
		public TerminalNode FINAL(int i) {
			return getToken(NominalGradualParser.FINAL, i);
		}
		public List<VisibilityContext> visibility() {
			return getRuleContexts(VisibilityContext.class);
		}
		public VisibilityContext visibility(int i) {
			return getRuleContext(VisibilityContext.class,i);
		}
		public List<TerminalNode> ABSTRACT() { return getTokens(NominalGradualParser.ABSTRACT); }
		public TerminalNode ABSTRACT(int i) {
			return getToken(NominalGradualParser.ABSTRACT, i);
		}
		public List<TerminalNode> PARTIAL() { return getTokens(NominalGradualParser.PARTIAL); }
		public TerminalNode PARTIAL(int i) {
			return getToken(NominalGradualParser.PARTIAL, i);
		}
		public List<CtypeContext> ctype() {
			return getRuleContexts(CtypeContext.class);
		}
		public CtypeContext ctype(int i) {
			return getRuleContext(CtypeContext.class,i);
		}
		public List<MethdefContext> methdef() {
			return getRuleContexts(MethdefContext.class);
		}
		public MethdefContext methdef(int i) {
			return getRuleContext(MethdefContext.class,i);
		}
		public List<FielddeclContext> fielddecl() {
			return getRuleContexts(FielddeclContext.class);
		}
		public FielddeclContext fielddecl(int i) {
			return getRuleContext(FielddeclContext.class,i);
		}
		public List<ConstructorContext> constructor() {
			return getRuleContexts(ConstructorContext.class);
		}
		public ConstructorContext constructor(int i) {
			return getRuleContext(ConstructorContext.class,i);
		}
		public List<StaticmethdefContext> staticmethdef() {
			return getRuleContexts(StaticmethdefContext.class);
		}
		public StaticmethdefContext staticmethdef(int i) {
			return getRuleContext(StaticmethdefContext.class,i);
		}
		public List<StaticfielddeclContext> staticfielddecl() {
			return getRuleContexts(StaticfielddeclContext.class);
		}
		public StaticfielddeclContext staticfielddecl(int i) {
			return getRuleContext(StaticfielddeclContext.class,i);
		}
		public List<ClassdefContext> classdef() {
			return getRuleContexts(ClassdefContext.class);
		}
		public ClassdefContext classdef(int i) {
			return getRuleContext(ClassdefContext.class,i);
		}
		public List<InterfacedefContext> interfacedef() {
			return getRuleContexts(InterfacedefContext.class);
		}
		public InterfacedefContext interfacedef(int i) {
			return getRuleContext(InterfacedefContext.class,i);
		}
		public List<InstancedeclContext> instancedecl() {
			return getRuleContexts(InstancedeclContext.class);
		}
		public InstancedeclContext instancedecl(int i) {
			return getRuleContext(InstancedeclContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(NominalGradualParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(NominalGradualParser.COMMA, i);
		}
		public ClassdefContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_classdef; }
	}

	public final ClassdefContext classdef() throws RecognitionException {
		ClassdefContext _localctx = new ClassdefContext(_ctx, getState());
		enterRule(_localctx, 32, RULE_classdef);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(637);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << ABSTRACT) | (1L << PARTIAL) | (1L << FINAL) | (1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
				{
				setState(635);
				_errHandler.sync(this);
				switch (_input.LA(1)) {
				case FINAL:
					{
					setState(631);
					((ClassdefContext)_localctx).FINAL = match(FINAL);
					((ClassdefContext)_localctx).f.add(((ClassdefContext)_localctx).FINAL);
					}
					break;
				case PUBLIC:
				case PRIVATE:
				case PROTECTED:
				case INTERNAL:
				case INTERNAL_PROT:
					{
					setState(632);
					((ClassdefContext)_localctx).visibility = visibility();
					((ClassdefContext)_localctx).v.add(((ClassdefContext)_localctx).visibility);
					}
					break;
				case ABSTRACT:
					{
					setState(633);
					((ClassdefContext)_localctx).ABSTRACT = match(ABSTRACT);
					((ClassdefContext)_localctx).a.add(((ClassdefContext)_localctx).ABSTRACT);
					}
					break;
				case PARTIAL:
					{
					setState(634);
					((ClassdefContext)_localctx).PARTIAL = match(PARTIAL);
					((ClassdefContext)_localctx).p.add(((ClassdefContext)_localctx).PARTIAL);
					}
					break;
				default:
					throw new NoViableAltException(this);
				}
				}
				setState(639);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(640);
			((ClassdefContext)_localctx).CLASS = match(CLASS);
			setState(641);
			((ClassdefContext)_localctx).n = ident();
			setState(644);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==EXTENDS) {
				{
				setState(642);
				match(EXTENDS);
				setState(643);
				((ClassdefContext)_localctx).ctype = ctype();
				((ClassdefContext)_localctx).sc.add(((ClassdefContext)_localctx).ctype);
				}
			}

			setState(655);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==IMPLEMENTS) {
				{
				setState(646);
				match(IMPLEMENTS);
				setState(647);
				((ClassdefContext)_localctx).ctype = ctype();
				((ClassdefContext)_localctx).si.add(((ClassdefContext)_localctx).ctype);
				setState(652);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(648);
					match(COMMA);
					setState(649);
					((ClassdefContext)_localctx).ctype = ctype();
					((ClassdefContext)_localctx).si.add(((ClassdefContext)_localctx).ctype);
					}
					}
					setState(654);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(657);
			match(LBRACE);
			setState(668);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << ABSTRACT) | (1L << PARTIAL) | (1L << READONLY) | (1L << INTERFACE) | (1L << CLASS) | (1L << INSTANCE) | (1L << DYN) | (1L << STATIC) | (1L << FINAL) | (1L << VIRTUAL) | (1L << OVERRIDE) | (1L << MULTI) | (1L << DEFAULT) | (1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT) | (1L << FUN) | (1L << CONSTRUCT) | (1L << LPAREN))) != 0) || _la==ID) {
				{
				setState(666);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,54,_ctx) ) {
				case 1:
					{
					setState(658);
					((ClassdefContext)_localctx).methdef = methdef();
					((ClassdefContext)_localctx).md.add(((ClassdefContext)_localctx).methdef);
					}
					break;
				case 2:
					{
					setState(659);
					((ClassdefContext)_localctx).fielddecl = fielddecl();
					((ClassdefContext)_localctx).fd.add(((ClassdefContext)_localctx).fielddecl);
					}
					break;
				case 3:
					{
					setState(660);
					((ClassdefContext)_localctx).constructor = constructor();
					((ClassdefContext)_localctx).c.add(((ClassdefContext)_localctx).constructor);
					}
					break;
				case 4:
					{
					setState(661);
					((ClassdefContext)_localctx).staticmethdef = staticmethdef();
					((ClassdefContext)_localctx).smd.add(((ClassdefContext)_localctx).staticmethdef);
					}
					break;
				case 5:
					{
					setState(662);
					((ClassdefContext)_localctx).staticfielddecl = staticfielddecl();
					((ClassdefContext)_localctx).sfd.add(((ClassdefContext)_localctx).staticfielddecl);
					}
					break;
				case 6:
					{
					setState(663);
					((ClassdefContext)_localctx).classdef = classdef();
					((ClassdefContext)_localctx).cd.add(((ClassdefContext)_localctx).classdef);
					}
					break;
				case 7:
					{
					setState(664);
					((ClassdefContext)_localctx).interfacedef = interfacedef();
					((ClassdefContext)_localctx).ifdef.add(((ClassdefContext)_localctx).interfacedef);
					}
					break;
				case 8:
					{
					setState(665);
					((ClassdefContext)_localctx).instancedecl = instancedecl();
					((ClassdefContext)_localctx).idecls.add(((ClassdefContext)_localctx).instancedecl);
					}
					break;
				}
				}
				setState(670);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(671);
			((ClassdefContext)_localctx).RBRACE = match(RBRACE);

					if(((ClassdefContext)_localctx).f.Count()>1)
					{
						throw new ParseException("A class definition can have at most one finality marker!", new SourceLocs(((ClassdefContext)_localctx).f.Select(ff=>ff.ToSourceSpan())));
					}
					if(((ClassdefContext)_localctx).v.Count()>1)
					{
						throw new ParseException("A class definition can have at most one visibility modifier!", new SourceLocs(((ClassdefContext)_localctx).v.Select(vn=>vn.v.Locs)));
					}
					if(((ClassdefContext)_localctx).a.Count()>1)
					{
						throw new ParseException("A class definition can have at most one abstractness marker!", new SourceLocs(((ClassdefContext)_localctx).a.Select(aa=>aa.ToSourceSpan())));
					}
					if(((ClassdefContext)_localctx).p.Count()>1)
					{
						throw new ParseException("A class definition can have at most one partiality marker!", new SourceLocs(((ClassdefContext)_localctx).p.Select(pp=>pp.ToSourceSpan())));
					}
					((ClassdefContext)_localctx).cdef = new ClassDef(((ClassdefContext)_localctx).n.i,((ClassdefContext)_localctx).sc.Select(x=>x.ct).SingleOrDefault(),((ClassdefContext)_localctx).si.Select(x=>x.ct),((ClassdefContext)_localctx).md.Select(m=>m.m),((ClassdefContext)_localctx).fd.Select(f=>f.f),((ClassdefContext)_localctx).c.Select(c=>c.c),((ClassdefContext)_localctx).sfd.Select(f=>f.f),((ClassdefContext)_localctx).smd.Select(m=>m.m),((ClassdefContext)_localctx).idecls.Select(idecl=>idecl.i),((ClassdefContext)_localctx).f.Count()==1, ((ClassdefContext)_localctx).v.SingleOrDefault()?.v, ((ClassdefContext)_localctx).a.Count()==1, ((ClassdefContext)_localctx).p.Count()==1, ((ClassdefContext)_localctx).ifdef.Select(ii=>ii.idef), ((ClassdefContext)_localctx).cd.Select(cc=>cc.cdef), ((ClassdefContext)_localctx).CLASS.ToSourceSpan(((ClassdefContext)_localctx).RBRACE));
				
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ConstructorContext extends ParserRuleContext {
		public Constructor c;
		public VisibilityContext visibility;
		public List<VisibilityContext> v = new ArrayList<VisibilityContext>();
		public Token CONSTRUCT;
		public ArgdeclContext argdecl;
		public List<ArgdeclContext> args = new ArrayList<ArgdeclContext>();
		public Token RPAREN;
		public Token LBRACE;
		public StmtContext stmt;
		public List<StmtContext> preps = new ArrayList<StmtContext>();
		public Token SUPER;
		public ExprContext expr;
		public List<ExprContext> exprs = new ArrayList<ExprContext>();
		public List<StmtContext> afters = new ArrayList<StmtContext>();
		public Token RBRACE;
		public TerminalNode CONSTRUCT() { return getToken(NominalGradualParser.CONSTRUCT, 0); }
		public List<TerminalNode> LPAREN() { return getTokens(NominalGradualParser.LPAREN); }
		public TerminalNode LPAREN(int i) {
			return getToken(NominalGradualParser.LPAREN, i);
		}
		public List<TerminalNode> RPAREN() { return getTokens(NominalGradualParser.RPAREN); }
		public TerminalNode RPAREN(int i) {
			return getToken(NominalGradualParser.RPAREN, i);
		}
		public TerminalNode LBRACE() { return getToken(NominalGradualParser.LBRACE, 0); }
		public TerminalNode RBRACE() { return getToken(NominalGradualParser.RBRACE, 0); }
		public TerminalNode SUPER() { return getToken(NominalGradualParser.SUPER, 0); }
		public TerminalNode SEMICOLON() { return getToken(NominalGradualParser.SEMICOLON, 0); }
		public VisibilityContext visibility() {
			return getRuleContext(VisibilityContext.class,0);
		}
		public List<ArgdeclContext> argdecl() {
			return getRuleContexts(ArgdeclContext.class);
		}
		public ArgdeclContext argdecl(int i) {
			return getRuleContext(ArgdeclContext.class,i);
		}
		public List<StmtContext> stmt() {
			return getRuleContexts(StmtContext.class);
		}
		public StmtContext stmt(int i) {
			return getRuleContext(StmtContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(NominalGradualParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(NominalGradualParser.COMMA, i);
		}
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public ConstructorContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_constructor; }
	}

	public final ConstructorContext constructor() throws RecognitionException {
		ConstructorContext _localctx = new ConstructorContext(_ctx, getState());
		enterRule(_localctx, 34, RULE_constructor);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(675);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
				{
				setState(674);
				((ConstructorContext)_localctx).visibility = visibility();
				((ConstructorContext)_localctx).v.add(((ConstructorContext)_localctx).visibility);
				}
			}

			setState(677);
			((ConstructorContext)_localctx).CONSTRUCT = match(CONSTRUCT);
			setState(678);
			match(LPAREN);
			setState(687);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==DYN || _la==LPAREN || _la==ID) {
				{
				setState(679);
				((ConstructorContext)_localctx).argdecl = argdecl();
				((ConstructorContext)_localctx).args.add(((ConstructorContext)_localctx).argdecl);
				setState(684);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(680);
					match(COMMA);
					setState(681);
					((ConstructorContext)_localctx).argdecl = argdecl();
					((ConstructorContext)_localctx).args.add(((ConstructorContext)_localctx).argdecl);
					}
					}
					setState(686);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(689);
			((ConstructorContext)_localctx).RPAREN = match(RPAREN);
			setState(690);
			((ConstructorContext)_localctx).LBRACE = match(LBRACE);
			setState(694);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << DYN) | (1L << NUL) | (1L << IF) | (1L << WHILE) | (1L << FOREACH) | (1L << RETURN) | (1L << BREAK) | (1L << CONTINUE) | (1L << NEW) | (1L << DBG) | (1L << ERR) | (1L << LPAREN) | (1L << LBRACKET) | (1L << LBRACE))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (BANG - 64)) | (1L << (MINUS - 64)) | (1L << (ID - 64)))) != 0)) {
				{
				{
				setState(691);
				((ConstructorContext)_localctx).stmt = stmt();
				((ConstructorContext)_localctx).preps.add(((ConstructorContext)_localctx).stmt);
				}
				}
				setState(696);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(717);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==SUPER) {
				{
				setState(697);
				((ConstructorContext)_localctx).SUPER = match(SUPER);
				setState(698);
				match(LPAREN);
				setState(707);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << NUL) | (1L << NEW) | (1L << LPAREN) | (1L << LBRACKET) | (1L << LBRACE))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (BANG - 64)) | (1L << (MINUS - 64)) | (1L << (ID - 64)))) != 0)) {
					{
					setState(699);
					((ConstructorContext)_localctx).expr = expr(0);
					((ConstructorContext)_localctx).exprs.add(((ConstructorContext)_localctx).expr);
					setState(704);
					_errHandler.sync(this);
					_la = _input.LA(1);
					while (_la==COMMA) {
						{
						{
						setState(700);
						match(COMMA);
						setState(701);
						((ConstructorContext)_localctx).expr = expr(0);
						((ConstructorContext)_localctx).exprs.add(((ConstructorContext)_localctx).expr);
						}
						}
						setState(706);
						_errHandler.sync(this);
						_la = _input.LA(1);
					}
					}
				}

				setState(709);
				((ConstructorContext)_localctx).RPAREN = match(RPAREN);
				setState(710);
				match(SEMICOLON);
				setState(714);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << DYN) | (1L << NUL) | (1L << IF) | (1L << WHILE) | (1L << FOREACH) | (1L << RETURN) | (1L << BREAK) | (1L << CONTINUE) | (1L << NEW) | (1L << DBG) | (1L << ERR) | (1L << LPAREN) | (1L << LBRACKET) | (1L << LBRACE))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (BANG - 64)) | (1L << (MINUS - 64)) | (1L << (ID - 64)))) != 0)) {
					{
					{
					setState(711);
					((ConstructorContext)_localctx).stmt = stmt();
					((ConstructorContext)_localctx).afters.add(((ConstructorContext)_localctx).stmt);
					}
					}
					setState(716);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(719);
			((ConstructorContext)_localctx).RBRACE = match(RBRACE);
			((ConstructorContext)_localctx).c = new Constructor(((ConstructorContext)_localctx).v.SingleOrDefault()?.v??VisibilityNode.Private, ((ConstructorContext)_localctx).args.Select(a=>a.v),new Block(((ConstructorContext)_localctx).preps.Select(s=>s.s), ((ConstructorContext)_localctx).LBRACE.ToSourceSpan(((ConstructorContext)_localctx).SUPER??((ConstructorContext)_localctx).RBRACE)),((ConstructorContext)_localctx).exprs.Select(e=>e.exp),new Block(((ConstructorContext)_localctx).afters.Select(s=>s.s), (((ConstructorContext)_localctx).RPAREN??((ConstructorContext)_localctx).RBRACE).ToSourceSpan(((ConstructorContext)_localctx).RBRACE)),((ConstructorContext)_localctx).CONSTRUCT.ToSourceSpan(((ConstructorContext)_localctx).RBRACE));
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class FielddeclContext extends ParserRuleContext {
		public FieldDecl f;
		public VisibilityContext visibility;
		public List<VisibilityContext> v = new ArrayList<VisibilityContext>();
		public Token r;
		public DtypeContext dtype;
		public IdentContext ident;
		public Token SEMICOLON;
		public ExprContext expr;
		public DtypeContext dtype() {
			return getRuleContext(DtypeContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode SEMICOLON() { return getToken(NominalGradualParser.SEMICOLON, 0); }
		public VisibilityContext visibility() {
			return getRuleContext(VisibilityContext.class,0);
		}
		public TerminalNode READONLY() { return getToken(NominalGradualParser.READONLY, 0); }
		public TerminalNode EQ() { return getToken(NominalGradualParser.EQ, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public FielddeclContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_fielddecl; }
	}

	public final FielddeclContext fielddecl() throws RecognitionException {
		FielddeclContext _localctx = new FielddeclContext(_ctx, getState());
		enterRule(_localctx, 36, RULE_fielddecl);
		int _la;
		try {
			setState(768);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,72,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(723);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
					{
					setState(722);
					((FielddeclContext)_localctx).visibility = visibility();
					((FielddeclContext)_localctx).v.add(((FielddeclContext)_localctx).visibility);
					}
				}

				setState(726);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==READONLY) {
					{
					setState(725);
					((FielddeclContext)_localctx).r = match(READONLY);
					}
				}

				setState(728);
				((FielddeclContext)_localctx).dtype = dtype();
				setState(729);
				((FielddeclContext)_localctx).ident = ident();
				setState(730);
				((FielddeclContext)_localctx).SEMICOLON = match(SEMICOLON);
				((FielddeclContext)_localctx).f = new FieldDecl(((FielddeclContext)_localctx).ident.i,((FielddeclContext)_localctx).dtype.t, ((FielddeclContext)_localctx).v.SingleOrDefault()?.v, ((FielddeclContext)_localctx).r!=null,((FielddeclContext)_localctx).dtype.t.Start.SpanTo(((FielddeclContext)_localctx).SEMICOLON));
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(734);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
					{
					setState(733);
					((FielddeclContext)_localctx).visibility = visibility();
					((FielddeclContext)_localctx).v.add(((FielddeclContext)_localctx).visibility);
					}
				}

				setState(737);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==READONLY) {
					{
					setState(736);
					((FielddeclContext)_localctx).r = match(READONLY);
					}
				}

				setState(739);
				((FielddeclContext)_localctx).ident = ident();
				setState(740);
				((FielddeclContext)_localctx).SEMICOLON = match(SEMICOLON);
				((FielddeclContext)_localctx).f = new FieldDecl(((FielddeclContext)_localctx).ident.i, ((FielddeclContext)_localctx).v.SingleOrDefault()?.v, ((FielddeclContext)_localctx).r!=null,((FielddeclContext)_localctx).ident.i.Start.SpanTo(((FielddeclContext)_localctx).SEMICOLON));
				}
				break;
			case 3:
				enterOuterAlt(_localctx, 3);
				{
				setState(744);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
					{
					setState(743);
					((FielddeclContext)_localctx).visibility = visibility();
					((FielddeclContext)_localctx).v.add(((FielddeclContext)_localctx).visibility);
					}
				}

				setState(747);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==READONLY) {
					{
					setState(746);
					((FielddeclContext)_localctx).r = match(READONLY);
					}
				}

				setState(749);
				((FielddeclContext)_localctx).dtype = dtype();
				setState(750);
				((FielddeclContext)_localctx).ident = ident();
				setState(751);
				match(EQ);
				setState(752);
				((FielddeclContext)_localctx).expr = expr(0);
				setState(753);
				((FielddeclContext)_localctx).SEMICOLON = match(SEMICOLON);
				((FielddeclContext)_localctx).f = new FieldDecl(((FielddeclContext)_localctx).ident.i,((FielddeclContext)_localctx).dtype.t,((FielddeclContext)_localctx).expr.exp, ((FielddeclContext)_localctx).v.SingleOrDefault()?.v, ((FielddeclContext)_localctx).r!=null,((FielddeclContext)_localctx).dtype.t.Start.SpanTo(((FielddeclContext)_localctx).SEMICOLON));
				}
				break;
			case 4:
				enterOuterAlt(_localctx, 4);
				{
				setState(757);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
					{
					setState(756);
					((FielddeclContext)_localctx).visibility = visibility();
					((FielddeclContext)_localctx).v.add(((FielddeclContext)_localctx).visibility);
					}
				}

				setState(760);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==READONLY) {
					{
					setState(759);
					((FielddeclContext)_localctx).r = match(READONLY);
					}
				}

				setState(762);
				((FielddeclContext)_localctx).ident = ident();
				setState(763);
				match(EQ);
				setState(764);
				((FielddeclContext)_localctx).expr = expr(0);
				setState(765);
				((FielddeclContext)_localctx).SEMICOLON = match(SEMICOLON);
				((FielddeclContext)_localctx).f = new FieldDecl(((FielddeclContext)_localctx).ident.i,((FielddeclContext)_localctx).expr.exp, ((FielddeclContext)_localctx).v.SingleOrDefault()?.v, ((FielddeclContext)_localctx).r!=null,((FielddeclContext)_localctx).ident.i.Start.SpanTo(((FielddeclContext)_localctx).SEMICOLON));
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class StaticfielddeclContext extends ParserRuleContext {
		public StaticFieldDecl f;
		public VisibilityContext visibility;
		public List<VisibilityContext> v = new ArrayList<VisibilityContext>();
		public Token STATIC;
		public Token r;
		public DtypeContext dtype;
		public IdentContext ident;
		public Token SEMICOLON;
		public DefaultexprContext defaultexpr;
		public TerminalNode STATIC() { return getToken(NominalGradualParser.STATIC, 0); }
		public DtypeContext dtype() {
			return getRuleContext(DtypeContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode SEMICOLON() { return getToken(NominalGradualParser.SEMICOLON, 0); }
		public VisibilityContext visibility() {
			return getRuleContext(VisibilityContext.class,0);
		}
		public TerminalNode READONLY() { return getToken(NominalGradualParser.READONLY, 0); }
		public TerminalNode EQ() { return getToken(NominalGradualParser.EQ, 0); }
		public DefaultexprContext defaultexpr() {
			return getRuleContext(DefaultexprContext.class,0);
		}
		public StaticfielddeclContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_staticfielddecl; }
	}

	public final StaticfielddeclContext staticfielddecl() throws RecognitionException {
		StaticfielddeclContext _localctx = new StaticfielddeclContext(_ctx, getState());
		enterRule(_localctx, 38, RULE_staticfielddecl);
		int _la;
		try {
			setState(820);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,81,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(771);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
					{
					setState(770);
					((StaticfielddeclContext)_localctx).visibility = visibility();
					((StaticfielddeclContext)_localctx).v.add(((StaticfielddeclContext)_localctx).visibility);
					}
				}

				setState(773);
				((StaticfielddeclContext)_localctx).STATIC = match(STATIC);
				setState(775);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==READONLY) {
					{
					setState(774);
					((StaticfielddeclContext)_localctx).r = match(READONLY);
					}
				}

				setState(777);
				((StaticfielddeclContext)_localctx).dtype = dtype();
				setState(778);
				((StaticfielddeclContext)_localctx).ident = ident();
				setState(779);
				((StaticfielddeclContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StaticfielddeclContext)_localctx).f = new StaticFieldDecl(((StaticfielddeclContext)_localctx).ident.i,((StaticfielddeclContext)_localctx).dtype.t, ((StaticfielddeclContext)_localctx).v.SingleOrDefault()?.v, ((StaticfielddeclContext)_localctx).r!=null,((StaticfielddeclContext)_localctx).STATIC.ToSourceSpan(((StaticfielddeclContext)_localctx).SEMICOLON));
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(783);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
					{
					setState(782);
					((StaticfielddeclContext)_localctx).visibility = visibility();
					((StaticfielddeclContext)_localctx).v.add(((StaticfielddeclContext)_localctx).visibility);
					}
				}

				setState(785);
				((StaticfielddeclContext)_localctx).STATIC = match(STATIC);
				setState(787);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==READONLY) {
					{
					setState(786);
					((StaticfielddeclContext)_localctx).r = match(READONLY);
					}
				}

				setState(789);
				((StaticfielddeclContext)_localctx).ident = ident();
				setState(790);
				((StaticfielddeclContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StaticfielddeclContext)_localctx).f = new StaticFieldDecl(((StaticfielddeclContext)_localctx).ident.i, ((StaticfielddeclContext)_localctx).v.SingleOrDefault()?.v, ((StaticfielddeclContext)_localctx).r!=null, ((StaticfielddeclContext)_localctx).STATIC.ToSourceSpan(((StaticfielddeclContext)_localctx).SEMICOLON));
				}
				break;
			case 3:
				enterOuterAlt(_localctx, 3);
				{
				setState(794);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
					{
					setState(793);
					((StaticfielddeclContext)_localctx).visibility = visibility();
					((StaticfielddeclContext)_localctx).v.add(((StaticfielddeclContext)_localctx).visibility);
					}
				}

				setState(796);
				((StaticfielddeclContext)_localctx).STATIC = match(STATIC);
				setState(798);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==READONLY) {
					{
					setState(797);
					((StaticfielddeclContext)_localctx).r = match(READONLY);
					}
				}

				setState(800);
				((StaticfielddeclContext)_localctx).dtype = dtype();
				setState(801);
				((StaticfielddeclContext)_localctx).ident = ident();
				setState(802);
				match(EQ);
				setState(803);
				((StaticfielddeclContext)_localctx).defaultexpr = defaultexpr();
				setState(804);
				((StaticfielddeclContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StaticfielddeclContext)_localctx).f = new StaticFieldDecl(((StaticfielddeclContext)_localctx).ident.i,((StaticfielddeclContext)_localctx).dtype.t,((StaticfielddeclContext)_localctx).defaultexpr.exp, ((StaticfielddeclContext)_localctx).v.SingleOrDefault()?.v, ((StaticfielddeclContext)_localctx).r!=null,((StaticfielddeclContext)_localctx).STATIC.ToSourceSpan(((StaticfielddeclContext)_localctx).SEMICOLON));
				}
				break;
			case 4:
				enterOuterAlt(_localctx, 4);
				{
				setState(808);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
					{
					setState(807);
					((StaticfielddeclContext)_localctx).visibility = visibility();
					((StaticfielddeclContext)_localctx).v.add(((StaticfielddeclContext)_localctx).visibility);
					}
				}

				setState(810);
				((StaticfielddeclContext)_localctx).STATIC = match(STATIC);
				setState(812);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==READONLY) {
					{
					setState(811);
					((StaticfielddeclContext)_localctx).r = match(READONLY);
					}
				}

				setState(814);
				((StaticfielddeclContext)_localctx).ident = ident();
				setState(815);
				match(EQ);
				setState(816);
				((StaticfielddeclContext)_localctx).defaultexpr = defaultexpr();
				setState(817);
				((StaticfielddeclContext)_localctx).SEMICOLON = match(SEMICOLON);
				((StaticfielddeclContext)_localctx).f = new StaticFieldDecl(((StaticfielddeclContext)_localctx).ident.i,((StaticfielddeclContext)_localctx).defaultexpr.exp, ((StaticfielddeclContext)_localctx).v.SingleOrDefault()?.v, ((StaticfielddeclContext)_localctx).r!=null,((StaticfielddeclContext)_localctx).STATIC.ToSourceSpan(((StaticfielddeclContext)_localctx).SEMICOLON));
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class InstancedeclContext extends ParserRuleContext {
		public InstanceDef i;
		public VisibilityContext visibility;
		public List<VisibilityContext> v = new ArrayList<VisibilityContext>();
		public Token d;
		public Token m;
		public Token INSTANCE;
		public IdentContext ident;
		public List<IdentContext> sid = new ArrayList<IdentContext>();
		public DefaultexprContext defaultexpr;
		public List<DefaultexprContext> sargs = new ArrayList<DefaultexprContext>();
		public InstanceassignmentContext instanceassignment;
		public List<InstanceassignmentContext> ifas = new ArrayList<InstanceassignmentContext>();
		public Token RBRACE;
		public ArgdeclContext argdecl;
		public List<ArgdeclContext> iargs = new ArrayList<ArgdeclContext>();
		public TerminalNode INSTANCE() { return getToken(NominalGradualParser.INSTANCE, 0); }
		public List<IdentContext> ident() {
			return getRuleContexts(IdentContext.class);
		}
		public IdentContext ident(int i) {
			return getRuleContext(IdentContext.class,i);
		}
		public TerminalNode LBRACE() { return getToken(NominalGradualParser.LBRACE, 0); }
		public TerminalNode RBRACE() { return getToken(NominalGradualParser.RBRACE, 0); }
		public TerminalNode EXTENDS() { return getToken(NominalGradualParser.EXTENDS, 0); }
		public VisibilityContext visibility() {
			return getRuleContext(VisibilityContext.class,0);
		}
		public TerminalNode DEFAULT() { return getToken(NominalGradualParser.DEFAULT, 0); }
		public TerminalNode MULTI() { return getToken(NominalGradualParser.MULTI, 0); }
		public List<InstanceassignmentContext> instanceassignment() {
			return getRuleContexts(InstanceassignmentContext.class);
		}
		public InstanceassignmentContext instanceassignment(int i) {
			return getRuleContext(InstanceassignmentContext.class,i);
		}
		public List<TerminalNode> LPAREN() { return getTokens(NominalGradualParser.LPAREN); }
		public TerminalNode LPAREN(int i) {
			return getToken(NominalGradualParser.LPAREN, i);
		}
		public List<TerminalNode> RPAREN() { return getTokens(NominalGradualParser.RPAREN); }
		public TerminalNode RPAREN(int i) {
			return getToken(NominalGradualParser.RPAREN, i);
		}
		public List<DefaultexprContext> defaultexpr() {
			return getRuleContexts(DefaultexprContext.class);
		}
		public DefaultexprContext defaultexpr(int i) {
			return getRuleContext(DefaultexprContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(NominalGradualParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(NominalGradualParser.COMMA, i);
		}
		public List<ArgdeclContext> argdecl() {
			return getRuleContexts(ArgdeclContext.class);
		}
		public ArgdeclContext argdecl(int i) {
			return getRuleContext(ArgdeclContext.class,i);
		}
		public InstancedeclContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_instancedecl; }
	}

	public final InstancedeclContext instancedecl() throws RecognitionException {
		InstancedeclContext _localctx = new InstancedeclContext(_ctx, getState());
		enterRule(_localctx, 40, RULE_instancedecl);
		int _la;
		try {
			setState(907);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,98,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(823);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
					{
					setState(822);
					((InstancedeclContext)_localctx).visibility = visibility();
					((InstancedeclContext)_localctx).v.add(((InstancedeclContext)_localctx).visibility);
					}
				}

				setState(826);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==DEFAULT) {
					{
					setState(825);
					((InstancedeclContext)_localctx).d = match(DEFAULT);
					}
				}

				setState(829);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==MULTI) {
					{
					setState(828);
					((InstancedeclContext)_localctx).m = match(MULTI);
					}
				}

				setState(831);
				((InstancedeclContext)_localctx).INSTANCE = match(INSTANCE);
				setState(832);
				((InstancedeclContext)_localctx).ident = ident();
				setState(849);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==EXTENDS) {
					{
					setState(833);
					match(EXTENDS);
					setState(834);
					((InstancedeclContext)_localctx).ident = ((InstancedeclContext)_localctx).ident = ident();
					((InstancedeclContext)_localctx).sid.add(((InstancedeclContext)_localctx).ident);
					setState(847);
					_errHandler.sync(this);
					_la = _input.LA(1);
					if (_la==LPAREN) {
						{
						setState(835);
						match(LPAREN);
						setState(844);
						_errHandler.sync(this);
						_la = _input.LA(1);
						if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << NUL) | (1L << LBRACKET))) != 0) || _la==MINUS || _la==ID) {
							{
							setState(836);
							((InstancedeclContext)_localctx).defaultexpr = defaultexpr();
							((InstancedeclContext)_localctx).sargs.add(((InstancedeclContext)_localctx).defaultexpr);
							setState(841);
							_errHandler.sync(this);
							_la = _input.LA(1);
							while (_la==COMMA) {
								{
								{
								setState(837);
								match(COMMA);
								setState(838);
								((InstancedeclContext)_localctx).defaultexpr = defaultexpr();
								((InstancedeclContext)_localctx).sargs.add(((InstancedeclContext)_localctx).defaultexpr);
								}
								}
								setState(843);
								_errHandler.sync(this);
								_la = _input.LA(1);
							}
							}
						}

						setState(846);
						match(RPAREN);
						}
					}

					}
				}

				setState(851);
				match(LBRACE);
				setState(855);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==ID) {
					{
					{
					setState(852);
					((InstancedeclContext)_localctx).instanceassignment = instanceassignment();
					((InstancedeclContext)_localctx).ifas.add(((InstancedeclContext)_localctx).instanceassignment);
					}
					}
					setState(857);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(858);
				((InstancedeclContext)_localctx).RBRACE = match(RBRACE);
				((InstancedeclContext)_localctx).i = new InstanceDef(((InstancedeclContext)_localctx).v.SingleOrDefault()?.v??VisibilityNode.Private, ((InstancedeclContext)_localctx).ident.i, ((InstancedeclContext)_localctx).sid.Select(s=>s.i).SingleOrDefault(), ((InstancedeclContext)_localctx).ifas.Select(fa=>fa.ifa), ((InstancedeclContext)_localctx).m!=null, ((InstancedeclContext)_localctx).d!=null, ((InstancedeclContext)_localctx).sargs.Select(s=>s.exp), null, ((InstancedeclContext)_localctx).INSTANCE.ToSourceSpan(((InstancedeclContext)_localctx).RBRACE));
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(862);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
					{
					setState(861);
					((InstancedeclContext)_localctx).visibility = visibility();
					((InstancedeclContext)_localctx).v.add(((InstancedeclContext)_localctx).visibility);
					}
				}

				setState(864);
				((InstancedeclContext)_localctx).m = match(MULTI);
				setState(865);
				((InstancedeclContext)_localctx).INSTANCE = match(INSTANCE);
				setState(866);
				((InstancedeclContext)_localctx).ident = ident();
				setState(867);
				match(LPAREN);
				setState(876);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==DYN || _la==LPAREN || _la==ID) {
					{
					setState(868);
					((InstancedeclContext)_localctx).argdecl = argdecl();
					((InstancedeclContext)_localctx).iargs.add(((InstancedeclContext)_localctx).argdecl);
					setState(873);
					_errHandler.sync(this);
					_la = _input.LA(1);
					while (_la==COMMA) {
						{
						{
						setState(869);
						match(COMMA);
						setState(870);
						((InstancedeclContext)_localctx).argdecl = argdecl();
						((InstancedeclContext)_localctx).iargs.add(((InstancedeclContext)_localctx).argdecl);
						}
						}
						setState(875);
						_errHandler.sync(this);
						_la = _input.LA(1);
					}
					}
				}

				setState(878);
				match(RPAREN);
				setState(895);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==EXTENDS) {
					{
					setState(879);
					match(EXTENDS);
					setState(880);
					((InstancedeclContext)_localctx).ident = ((InstancedeclContext)_localctx).ident = ident();
					((InstancedeclContext)_localctx).sid.add(((InstancedeclContext)_localctx).ident);
					setState(893);
					_errHandler.sync(this);
					_la = _input.LA(1);
					if (_la==LPAREN) {
						{
						setState(881);
						match(LPAREN);
						setState(890);
						_errHandler.sync(this);
						_la = _input.LA(1);
						if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << NUL) | (1L << LBRACKET))) != 0) || _la==MINUS || _la==ID) {
							{
							setState(882);
							((InstancedeclContext)_localctx).defaultexpr = defaultexpr();
							((InstancedeclContext)_localctx).sargs.add(((InstancedeclContext)_localctx).defaultexpr);
							setState(887);
							_errHandler.sync(this);
							_la = _input.LA(1);
							while (_la==COMMA) {
								{
								{
								setState(883);
								match(COMMA);
								setState(884);
								((InstancedeclContext)_localctx).defaultexpr = defaultexpr();
								((InstancedeclContext)_localctx).sargs.add(((InstancedeclContext)_localctx).defaultexpr);
								}
								}
								setState(889);
								_errHandler.sync(this);
								_la = _input.LA(1);
							}
							}
						}

						setState(892);
						match(RPAREN);
						}
					}

					}
				}

				setState(897);
				match(LBRACE);
				setState(901);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==ID) {
					{
					{
					setState(898);
					((InstancedeclContext)_localctx).instanceassignment = instanceassignment();
					((InstancedeclContext)_localctx).ifas.add(((InstancedeclContext)_localctx).instanceassignment);
					}
					}
					setState(903);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(904);
				((InstancedeclContext)_localctx).RBRACE = match(RBRACE);
				((InstancedeclContext)_localctx).i = new InstanceDef(((InstancedeclContext)_localctx).v.SingleOrDefault()?.v??VisibilityNode.Private, ((InstancedeclContext)_localctx).ident.i, ((InstancedeclContext)_localctx).sid.Select(s=>s.i).SingleOrDefault(), ((InstancedeclContext)_localctx).ifas.Select(fa=>fa.ifa), ((InstancedeclContext)_localctx).m!=null, false, ((InstancedeclContext)_localctx).sargs.Select(s=>s.exp), ((InstancedeclContext)_localctx).iargs.Select(a=>a.v.ToArgDecl()), ((InstancedeclContext)_localctx).INSTANCE.ToSourceSpan(((InstancedeclContext)_localctx).RBRACE));
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class InstanceassignmentContext extends ParserRuleContext {
		public InstanceFieldAssignment ifa;
		public IdentContext ident;
		public DefaultexprContext defaultexpr;
		public Token SEMICOLON;
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode EQ() { return getToken(NominalGradualParser.EQ, 0); }
		public DefaultexprContext defaultexpr() {
			return getRuleContext(DefaultexprContext.class,0);
		}
		public TerminalNode SEMICOLON() { return getToken(NominalGradualParser.SEMICOLON, 0); }
		public InstanceassignmentContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_instanceassignment; }
	}

	public final InstanceassignmentContext instanceassignment() throws RecognitionException {
		InstanceassignmentContext _localctx = new InstanceassignmentContext(_ctx, getState());
		enterRule(_localctx, 42, RULE_instanceassignment);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(909);
			((InstanceassignmentContext)_localctx).ident = ident();
			setState(910);
			match(EQ);
			setState(911);
			((InstanceassignmentContext)_localctx).defaultexpr = defaultexpr();
			setState(912);
			((InstanceassignmentContext)_localctx).SEMICOLON = match(SEMICOLON);
			((InstanceassignmentContext)_localctx).ifa =  new InstanceFieldAssignment(((InstanceassignmentContext)_localctx).ident.i, ((InstanceassignmentContext)_localctx).defaultexpr.exp, ((InstanceassignmentContext)_localctx).ident.i.Start.SpanTo(((InstanceassignmentContext)_localctx).SEMICOLON));
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class FundefContext extends ParserRuleContext {
		public FunDef f;
		public Token FUN;
		public IdentContext ident;
		public ArgdeclContext argdecl;
		public List<ArgdeclContext> args = new ArrayList<ArgdeclContext>();
		public DtypeContext dtype;
		public List<DtypeContext> rets = new ArrayList<DtypeContext>();
		public StmtContext stmt;
		public List<StmtContext> ss = new ArrayList<StmtContext>();
		public Token RBRACE;
		public TerminalNode FUN() { return getToken(NominalGradualParser.FUN, 0); }
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode LPAREN() { return getToken(NominalGradualParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(NominalGradualParser.RPAREN, 0); }
		public TerminalNode LBRACE() { return getToken(NominalGradualParser.LBRACE, 0); }
		public TerminalNode RBRACE() { return getToken(NominalGradualParser.RBRACE, 0); }
		public TerminalNode COLON() { return getToken(NominalGradualParser.COLON, 0); }
		public List<ArgdeclContext> argdecl() {
			return getRuleContexts(ArgdeclContext.class);
		}
		public ArgdeclContext argdecl(int i) {
			return getRuleContext(ArgdeclContext.class,i);
		}
		public DtypeContext dtype() {
			return getRuleContext(DtypeContext.class,0);
		}
		public List<StmtContext> stmt() {
			return getRuleContexts(StmtContext.class);
		}
		public StmtContext stmt(int i) {
			return getRuleContext(StmtContext.class,i);
		}
		public TerminalNode COMMA() { return getToken(NominalGradualParser.COMMA, 0); }
		public FundefContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_fundef; }
	}

	public final FundefContext fundef() throws RecognitionException {
		FundefContext _localctx = new FundefContext(_ctx, getState());
		enterRule(_localctx, 44, RULE_fundef);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(915);
			((FundefContext)_localctx).FUN = match(FUN);
			setState(916);
			((FundefContext)_localctx).ident = ident();
			setState(917);
			match(LPAREN);
			setState(926);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==DYN || _la==LPAREN || _la==ID) {
				{
				setState(918);
				((FundefContext)_localctx).argdecl = argdecl();
				((FundefContext)_localctx).args.add(((FundefContext)_localctx).argdecl);
				{
				setState(919);
				match(COMMA);
				setState(923);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==DYN || _la==LPAREN || _la==ID) {
					{
					{
					setState(920);
					((FundefContext)_localctx).argdecl = argdecl();
					((FundefContext)_localctx).args.add(((FundefContext)_localctx).argdecl);
					}
					}
					setState(925);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
				}
			}

			setState(928);
			match(RPAREN);
			setState(931);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==COLON) {
				{
				setState(929);
				match(COLON);
				setState(930);
				((FundefContext)_localctx).dtype = dtype();
				((FundefContext)_localctx).rets.add(((FundefContext)_localctx).dtype);
				}
			}

			setState(933);
			match(LBRACE);
			setState(937);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << STRING) | (1L << FLOAT) | (1L << INT) | (1L << TRUE) | (1L << FALSE) | (1L << DYN) | (1L << NUL) | (1L << IF) | (1L << WHILE) | (1L << FOREACH) | (1L << RETURN) | (1L << BREAK) | (1L << CONTINUE) | (1L << NEW) | (1L << DBG) | (1L << ERR) | (1L << LPAREN) | (1L << LBRACKET) | (1L << LBRACE))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (BANG - 64)) | (1L << (MINUS - 64)) | (1L << (ID - 64)))) != 0)) {
				{
				{
				setState(934);
				((FundefContext)_localctx).stmt = stmt();
				((FundefContext)_localctx).ss.add(((FundefContext)_localctx).stmt);
				}
				}
				setState(939);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(940);
			((FundefContext)_localctx).RBRACE = match(RBRACE);
			((FundefContext)_localctx).f = new FunDef(((FundefContext)_localctx).ident.i,((FundefContext)_localctx).args.Select(a=>a.v),((FundefContext)_localctx).rets.Select(t=>t.t),((FundefContext)_localctx).ss.Select(s=>s.s),((FundefContext)_localctx).FUN.ToSourceSpan(((FundefContext)_localctx).RBRACE));
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class MethdefContext extends ParserRuleContext {
		public MethodDef m;
		public Token FINAL;
		public List<Token> f = new ArrayList<Token>();
		public VisibilityContext visibility;
		public List<VisibilityContext> v = new ArrayList<VisibilityContext>();
		public Token VIRTUAL;
		public List<Token> virt = new ArrayList<Token>();
		public Token OVERRIDE;
		public List<Token> o = new ArrayList<Token>();
		public Token FUN;
		public IdentContext ident;
		public ArgdeclContext argdecl;
		public List<ArgdeclContext> args = new ArrayList<ArgdeclContext>();
		public Token RPAREN;
		public DtypeContext dtype;
		public List<DtypeContext> rets = new ArrayList<DtypeContext>();
		public BlockContext block;
		public TerminalNode FUN() { return getToken(NominalGradualParser.FUN, 0); }
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode LPAREN() { return getToken(NominalGradualParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(NominalGradualParser.RPAREN, 0); }
		public BlockContext block() {
			return getRuleContext(BlockContext.class,0);
		}
		public TerminalNode COLON() { return getToken(NominalGradualParser.COLON, 0); }
		public List<TerminalNode> FINAL() { return getTokens(NominalGradualParser.FINAL); }
		public TerminalNode FINAL(int i) {
			return getToken(NominalGradualParser.FINAL, i);
		}
		public List<VisibilityContext> visibility() {
			return getRuleContexts(VisibilityContext.class);
		}
		public VisibilityContext visibility(int i) {
			return getRuleContext(VisibilityContext.class,i);
		}
		public List<TerminalNode> VIRTUAL() { return getTokens(NominalGradualParser.VIRTUAL); }
		public TerminalNode VIRTUAL(int i) {
			return getToken(NominalGradualParser.VIRTUAL, i);
		}
		public List<TerminalNode> OVERRIDE() { return getTokens(NominalGradualParser.OVERRIDE); }
		public TerminalNode OVERRIDE(int i) {
			return getToken(NominalGradualParser.OVERRIDE, i);
		}
		public List<ArgdeclContext> argdecl() {
			return getRuleContexts(ArgdeclContext.class);
		}
		public ArgdeclContext argdecl(int i) {
			return getRuleContext(ArgdeclContext.class,i);
		}
		public DtypeContext dtype() {
			return getRuleContext(DtypeContext.class,0);
		}
		public List<TerminalNode> COMMA() { return getTokens(NominalGradualParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(NominalGradualParser.COMMA, i);
		}
		public MethdefContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_methdef; }
	}

	public final MethdefContext methdef() throws RecognitionException {
		MethdefContext _localctx = new MethdefContext(_ctx, getState());
		enterRule(_localctx, 46, RULE_methdef);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(949);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << FINAL) | (1L << VIRTUAL) | (1L << OVERRIDE) | (1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
				{
				setState(947);
				_errHandler.sync(this);
				switch (_input.LA(1)) {
				case FINAL:
					{
					setState(943);
					((MethdefContext)_localctx).FINAL = match(FINAL);
					((MethdefContext)_localctx).f.add(((MethdefContext)_localctx).FINAL);
					}
					break;
				case PUBLIC:
				case PRIVATE:
				case PROTECTED:
				case INTERNAL:
				case INTERNAL_PROT:
					{
					setState(944);
					((MethdefContext)_localctx).visibility = visibility();
					((MethdefContext)_localctx).v.add(((MethdefContext)_localctx).visibility);
					}
					break;
				case VIRTUAL:
					{
					setState(945);
					((MethdefContext)_localctx).VIRTUAL = match(VIRTUAL);
					((MethdefContext)_localctx).virt.add(((MethdefContext)_localctx).VIRTUAL);
					}
					break;
				case OVERRIDE:
					{
					setState(946);
					((MethdefContext)_localctx).OVERRIDE = match(OVERRIDE);
					((MethdefContext)_localctx).o.add(((MethdefContext)_localctx).OVERRIDE);
					}
					break;
				default:
					throw new NoViableAltException(this);
				}
				}
				setState(951);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(952);
			((MethdefContext)_localctx).FUN = match(FUN);
			setState(953);
			((MethdefContext)_localctx).ident = ident();
			setState(954);
			match(LPAREN);
			setState(963);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==DYN || _la==LPAREN || _la==ID) {
				{
				setState(955);
				((MethdefContext)_localctx).argdecl = argdecl();
				((MethdefContext)_localctx).args.add(((MethdefContext)_localctx).argdecl);
				setState(960);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(956);
					match(COMMA);
					setState(957);
					((MethdefContext)_localctx).argdecl = argdecl();
					((MethdefContext)_localctx).args.add(((MethdefContext)_localctx).argdecl);
					}
					}
					setState(962);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(965);
			((MethdefContext)_localctx).RPAREN = match(RPAREN);
			setState(968);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==COLON) {
				{
				setState(966);
				match(COLON);
				setState(967);
				((MethdefContext)_localctx).dtype = dtype();
				((MethdefContext)_localctx).rets.add(((MethdefContext)_localctx).dtype);
				}
			}

			setState(970);
			((MethdefContext)_localctx).block = block();

					if(((MethdefContext)_localctx).f.Count()>1)
					{
						throw new ParseException("A method definition can have at most one finality marker!", new SourceLocs(((MethdefContext)_localctx).f.Select(f=>f.ToSourceSpan())));
					}
					if(((MethdefContext)_localctx).v.Count()>1)
					{
						throw new ParseException("A method definition can have at most one visibility modifier!", new SourceLocs(((MethdefContext)_localctx).v.Select(vn=>vn.v.Locs)));
					}
					if(((MethdefContext)_localctx).virt.Count()>1)
					{
						throw new ParseException("A method definition can have at most one virtual marker!", new SourceLocs(((MethdefContext)_localctx).virt.Select(v=>v.ToSourceSpan())));
					}
					if(((MethdefContext)_localctx).o.Count()>1)
					{
						throw new ParseException("A method definition can have at most one override marker!", new SourceLocs(((MethdefContext)_localctx).o.Select(o=>o.ToSourceSpan())));
					}
					((MethdefContext)_localctx).m = new MethodDef(((MethdefContext)_localctx).f.Count()>0, ((MethdefContext)_localctx).virt.Count()>0, ((MethdefContext)_localctx).o.Count()>0, ((MethdefContext)_localctx).v.Select(v=>v.v).SingleOrDefault(), ((MethdefContext)_localctx).ident.i, ((MethdefContext)_localctx).args.Select(a=>a.v), ((MethdefContext)_localctx).rets.Select(r=>r.t).SingleOrDefault()??TypeInterval.Default(((MethdefContext)_localctx).RPAREN.ToSourceSpan()), ((MethdefContext)_localctx).block.b, ((MethdefContext)_localctx).FUN.ToSourceSpan(((MethdefContext)_localctx).block.b.End));
				
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class StaticmethdefContext extends ParserRuleContext {
		public StaticMethodDef m;
		public VisibilityContext visibility;
		public List<VisibilityContext> v = new ArrayList<VisibilityContext>();
		public Token FUN;
		public IdentContext ident;
		public ArgdeclContext argdecl;
		public List<ArgdeclContext> args = new ArrayList<ArgdeclContext>();
		public Token RPAREN;
		public DtypeContext dtype;
		public List<DtypeContext> rets = new ArrayList<DtypeContext>();
		public BlockContext block;
		public TerminalNode STATIC() { return getToken(NominalGradualParser.STATIC, 0); }
		public TerminalNode FUN() { return getToken(NominalGradualParser.FUN, 0); }
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode LPAREN() { return getToken(NominalGradualParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(NominalGradualParser.RPAREN, 0); }
		public BlockContext block() {
			return getRuleContext(BlockContext.class,0);
		}
		public TerminalNode COLON() { return getToken(NominalGradualParser.COLON, 0); }
		public VisibilityContext visibility() {
			return getRuleContext(VisibilityContext.class,0);
		}
		public List<ArgdeclContext> argdecl() {
			return getRuleContexts(ArgdeclContext.class);
		}
		public ArgdeclContext argdecl(int i) {
			return getRuleContext(ArgdeclContext.class,i);
		}
		public DtypeContext dtype() {
			return getRuleContext(DtypeContext.class,0);
		}
		public List<TerminalNode> COMMA() { return getTokens(NominalGradualParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(NominalGradualParser.COMMA, i);
		}
		public StaticmethdefContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_staticmethdef; }
	}

	public final StaticmethdefContext staticmethdef() throws RecognitionException {
		StaticmethdefContext _localctx = new StaticmethdefContext(_ctx, getState());
		enterRule(_localctx, 48, RULE_staticmethdef);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(974);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << PUBLIC) | (1L << PRIVATE) | (1L << PROTECTED) | (1L << INTERNAL) | (1L << INTERNAL_PROT))) != 0)) {
				{
				setState(973);
				((StaticmethdefContext)_localctx).visibility = visibility();
				((StaticmethdefContext)_localctx).v.add(((StaticmethdefContext)_localctx).visibility);
				}
			}

			setState(976);
			match(STATIC);
			setState(977);
			((StaticmethdefContext)_localctx).FUN = match(FUN);
			setState(978);
			((StaticmethdefContext)_localctx).ident = ident();
			setState(979);
			match(LPAREN);
			setState(988);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==DYN || _la==LPAREN || _la==ID) {
				{
				setState(980);
				((StaticmethdefContext)_localctx).argdecl = argdecl();
				((StaticmethdefContext)_localctx).args.add(((StaticmethdefContext)_localctx).argdecl);
				setState(985);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(981);
					match(COMMA);
					setState(982);
					((StaticmethdefContext)_localctx).argdecl = argdecl();
					((StaticmethdefContext)_localctx).args.add(((StaticmethdefContext)_localctx).argdecl);
					}
					}
					setState(987);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(990);
			((StaticmethdefContext)_localctx).RPAREN = match(RPAREN);
			setState(993);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==COLON) {
				{
				setState(991);
				match(COLON);
				setState(992);
				((StaticmethdefContext)_localctx).dtype = dtype();
				((StaticmethdefContext)_localctx).rets.add(((StaticmethdefContext)_localctx).dtype);
				}
			}

			setState(995);
			((StaticmethdefContext)_localctx).block = block();
			((StaticmethdefContext)_localctx).m = new StaticMethodDef(((StaticmethdefContext)_localctx).v.SingleOrDefault().v??VisibilityNode.Private, ((StaticmethdefContext)_localctx).ident.i, ((StaticmethdefContext)_localctx).args.Select(a=>a.v), ((StaticmethdefContext)_localctx).rets.Select(r=>r.t).SingleOrDefault()??TypeInterval.Default(((StaticmethdefContext)_localctx).RPAREN.ToSourceSpan()), ((StaticmethdefContext)_localctx).block.b, ((StaticmethdefContext)_localctx).FUN.ToSourceSpan(((StaticmethdefContext)_localctx).block.b.End));
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class MethdeclContext extends ParserRuleContext {
		public MethodDecl m;
		public Token FUN;
		public IdentContext ident;
		public ArgdeclContext argdecl;
		public List<ArgdeclContext> args = new ArrayList<ArgdeclContext>();
		public Token RPAREN;
		public DtypeContext dtype;
		public List<DtypeContext> rets = new ArrayList<DtypeContext>();
		public Token SEMICOLON;
		public TerminalNode FUN() { return getToken(NominalGradualParser.FUN, 0); }
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode LPAREN() { return getToken(NominalGradualParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(NominalGradualParser.RPAREN, 0); }
		public TerminalNode SEMICOLON() { return getToken(NominalGradualParser.SEMICOLON, 0); }
		public TerminalNode COLON() { return getToken(NominalGradualParser.COLON, 0); }
		public List<ArgdeclContext> argdecl() {
			return getRuleContexts(ArgdeclContext.class);
		}
		public ArgdeclContext argdecl(int i) {
			return getRuleContext(ArgdeclContext.class,i);
		}
		public DtypeContext dtype() {
			return getRuleContext(DtypeContext.class,0);
		}
		public List<TerminalNode> COMMA() { return getTokens(NominalGradualParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(NominalGradualParser.COMMA, i);
		}
		public MethdeclContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_methdecl; }
	}

	public final MethdeclContext methdecl() throws RecognitionException {
		MethdeclContext _localctx = new MethdeclContext(_ctx, getState());
		enterRule(_localctx, 50, RULE_methdecl);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(998);
			((MethdeclContext)_localctx).FUN = match(FUN);
			setState(999);
			((MethdeclContext)_localctx).ident = ident();
			setState(1000);
			match(LPAREN);
			setState(1009);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==DYN || _la==LPAREN || _la==ID) {
				{
				setState(1001);
				((MethdeclContext)_localctx).argdecl = argdecl();
				((MethdeclContext)_localctx).args.add(((MethdeclContext)_localctx).argdecl);
				setState(1006);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(1002);
					match(COMMA);
					setState(1003);
					((MethdeclContext)_localctx).argdecl = argdecl();
					((MethdeclContext)_localctx).args.add(((MethdeclContext)_localctx).argdecl);
					}
					}
					setState(1008);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(1011);
			((MethdeclContext)_localctx).RPAREN = match(RPAREN);
			setState(1014);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==COLON) {
				{
				setState(1012);
				match(COLON);
				setState(1013);
				((MethdeclContext)_localctx).dtype = dtype();
				((MethdeclContext)_localctx).rets.add(((MethdeclContext)_localctx).dtype);
				}
			}

			setState(1016);
			((MethdeclContext)_localctx).SEMICOLON = match(SEMICOLON);
			((MethdeclContext)_localctx).m = new MethodDecl(VisibilityNode.Public, ((MethdeclContext)_localctx).ident.i, ((MethdeclContext)_localctx).args.Select(a=>a.v), ((MethdeclContext)_localctx).rets.Select(r=>r.t).SingleOrDefault()??TypeInterval.Default(((MethdeclContext)_localctx).RPAREN.ToSourceSpan()),((MethdeclContext)_localctx).FUN.ToSourceSpan(((MethdeclContext)_localctx).SEMICOLON));
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class TypeContext extends ParserRuleContext {
		public IType t;
		public TypeContext tq;
		public CtypeContext ctype;
		public TypelistContext tl;
		public Token ARROW;
		public TypeContext type;
		public List<TypeContext> ret = new ArrayList<TypeContext>();
		public Token RBRACKET;
		public Token QMARK;
		public CtypeContext ctype() {
			return getRuleContext(CtypeContext.class,0);
		}
		public TerminalNode ARROW() { return getToken(NominalGradualParser.ARROW, 0); }
		public TypelistContext typelist() {
			return getRuleContext(TypelistContext.class,0);
		}
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode LPAREN() { return getToken(NominalGradualParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(NominalGradualParser.RPAREN, 0); }
		public TerminalNode LBRACKET() { return getToken(NominalGradualParser.LBRACKET, 0); }
		public TerminalNode RBRACKET() { return getToken(NominalGradualParser.RBRACKET, 0); }
		public TerminalNode QMARK() { return getToken(NominalGradualParser.QMARK, 0); }
		public TypeContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_type; }
	}

	public final TypeContext type() throws RecognitionException {
		return type(0);
	}

	private TypeContext type(int _p) throws RecognitionException {
		ParserRuleContext _parentctx = _ctx;
		int _parentState = getState();
		TypeContext _localctx = new TypeContext(_ctx, _parentState);
		TypeContext _prevctx = _localctx;
		int _startState = 52;
		enterRecursionRule(_localctx, 52, RULE_type, _p);
		try {
			int _alt;
			enterOuterAlt(_localctx, 1);
			{
			setState(1035);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,116,_ctx) ) {
			case 1:
				{
				setState(1020);
				((TypeContext)_localctx).ctype = ctype();
				((TypeContext)_localctx).t =  ((TypeContext)_localctx).ctype.ct;
				}
				break;
			case 2:
				{
				setState(1023);
				((TypeContext)_localctx).tl = typelist();
				setState(1024);
				((TypeContext)_localctx).ARROW = match(ARROW);
				setState(1026);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,115,_ctx) ) {
				case 1:
					{
					setState(1025);
					((TypeContext)_localctx).type = type(0);
					((TypeContext)_localctx).ret.add(((TypeContext)_localctx).type);
					}
					break;
				}
				((TypeContext)_localctx).t = new FunType(((TypeContext)_localctx).tl.ts,((TypeContext)_localctx).ret.Select(r=>r.t),_localctx.start.ToSourceSpan(((TypeContext)_localctx).ret.Count()>0?((TypeContext)_localctx).ret.Single().t.End:((TypeContext)_localctx).ARROW.ToSourcePos()));
				}
				break;
			case 3:
				{
				setState(1030);
				match(LPAREN);
				setState(1031);
				((TypeContext)_localctx).tq = type(0);
				setState(1032);
				match(RPAREN);
				((TypeContext)_localctx).t = ((TypeContext)_localctx).tq.t;
				}
				break;
			}
			_ctx.stop = _input.LT(-1);
			setState(1046);
			_errHandler.sync(this);
			_alt = getInterpreter().adaptivePredict(_input,118,_ctx);
			while ( _alt!=2 && _alt!=org.antlr.v4.runtime.atn.ATN.INVALID_ALT_NUMBER ) {
				if ( _alt==1 ) {
					if ( _parseListeners!=null ) triggerExitRuleEvent();
					_prevctx = _localctx;
					{
					setState(1044);
					_errHandler.sync(this);
					switch ( getInterpreter().adaptivePredict(_input,117,_ctx) ) {
					case 1:
						{
						_localctx = new TypeContext(_parentctx, _parentState);
						_localctx.tq = _prevctx;
						_localctx.tq = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_type);
						setState(1037);
						if (!(precpred(_ctx, 4))) throw new FailedPredicateException(this, "precpred(_ctx, 4)");
						setState(1038);
						match(LBRACKET);
						setState(1039);
						((TypeContext)_localctx).RBRACKET = match(RBRACKET);
						((TypeContext)_localctx).t = new ArrayType(((TypeContext)_localctx).tq.t,((TypeContext)_localctx).tq.t.Start.SpanTo(((TypeContext)_localctx).RBRACKET));
						}
						break;
					case 2:
						{
						_localctx = new TypeContext(_parentctx, _parentState);
						_localctx.tq = _prevctx;
						_localctx.tq = _prevctx;
						pushNewRecursionContext(_localctx, _startState, RULE_type);
						setState(1041);
						if (!(precpred(_ctx, 2))) throw new FailedPredicateException(this, "precpred(_ctx, 2)");
						setState(1042);
						((TypeContext)_localctx).QMARK = match(QMARK);
						((TypeContext)_localctx).t = new MaybeType(((TypeContext)_localctx).tq.t,((TypeContext)_localctx).tq.t.Start.SpanTo(((TypeContext)_localctx).QMARK));
						}
						break;
					}
					} 
				}
				setState(1048);
				_errHandler.sync(this);
				_alt = getInterpreter().adaptivePredict(_input,118,_ctx);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			unrollRecursionContexts(_parentctx);
		}
		return _localctx;
	}

	public static class TypelistContext extends ParserRuleContext {
		public IEnumerable<IType> ts;
		public TypeContext type;
		public List<TypeContext> elems = new ArrayList<TypeContext>();
		public TerminalNode LPAREN() { return getToken(NominalGradualParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(NominalGradualParser.RPAREN, 0); }
		public List<TypeContext> type() {
			return getRuleContexts(TypeContext.class);
		}
		public TypeContext type(int i) {
			return getRuleContext(TypeContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(NominalGradualParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(NominalGradualParser.COMMA, i);
		}
		public TypelistContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_typelist; }
	}

	public final TypelistContext typelist() throws RecognitionException {
		TypelistContext _localctx = new TypelistContext(_ctx, getState());
		enterRule(_localctx, 54, RULE_typelist);
		int _la;
		try {
			setState(1064);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,120,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(1049);
				match(LPAREN);
				setState(1050);
				match(RPAREN);
				((TypelistContext)_localctx).ts = new List<IType>();
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(1052);
				match(LPAREN);
				setState(1053);
				((TypelistContext)_localctx).type = type(0);
				((TypelistContext)_localctx).elems.add(((TypelistContext)_localctx).type);
				setState(1058);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(1054);
					match(COMMA);
					setState(1055);
					((TypelistContext)_localctx).type = type(0);
					((TypelistContext)_localctx).elems.add(((TypelistContext)_localctx).type);
					}
					}
					setState(1060);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(1061);
				match(RPAREN);
				((TypelistContext)_localctx).ts = ((TypelistContext)_localctx).elems.Select(a=>a.t);
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class DtypeContext extends ParserRuleContext {
		public TypeInterval t;
		public TypeContext type;
		public Token DYN;
		public TypeContext d;
		public TypeContext s;
		public Token RANGLE;
		public Token RBRACKET;
		public Token BANG;
		public List<TypeContext> type() {
			return getRuleContexts(TypeContext.class);
		}
		public TypeContext type(int i) {
			return getRuleContext(TypeContext.class,i);
		}
		public TerminalNode DYN() { return getToken(NominalGradualParser.DYN, 0); }
		public TerminalNode LBRACKET() { return getToken(NominalGradualParser.LBRACKET, 0); }
		public TerminalNode RBRACKET() { return getToken(NominalGradualParser.RBRACKET, 0); }
		public TerminalNode LANGLE() { return getToken(NominalGradualParser.LANGLE, 0); }
		public TerminalNode RANGLE() { return getToken(NominalGradualParser.RANGLE, 0); }
		public TerminalNode BANG() { return getToken(NominalGradualParser.BANG, 0); }
		public DtypeContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_dtype; }
	}

	public final DtypeContext dtype() throws RecognitionException {
		DtypeContext _localctx = new DtypeContext(_ctx, getState());
		enterRule(_localctx, 56, RULE_dtype);
		try {
			setState(1096);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,121,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(1066);
				((DtypeContext)_localctx).type = type(0);
				((DtypeContext)_localctx).t = new TypeInterval(((DtypeContext)_localctx).type.t,((DtypeContext)_localctx).type.t,((DtypeContext)_localctx).type.t.Locs);
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(1069);
				((DtypeContext)_localctx).DYN = match(DYN);
				setState(1070);
				match(LBRACKET);
				setState(1071);
				((DtypeContext)_localctx).d = type(0);
				setState(1072);
				match(RBRACKET);
				setState(1073);
				match(LANGLE);
				setState(1074);
				((DtypeContext)_localctx).s = type(0);
				setState(1075);
				((DtypeContext)_localctx).RANGLE = match(RANGLE);
				((DtypeContext)_localctx).t = new TypeInterval(((DtypeContext)_localctx).d.t,((DtypeContext)_localctx).s.t,((DtypeContext)_localctx).DYN.ToSourceSpan(((DtypeContext)_localctx).RANGLE));
				}
				break;
			case 3:
				enterOuterAlt(_localctx, 3);
				{
				setState(1078);
				((DtypeContext)_localctx).DYN = match(DYN);
				setState(1079);
				match(LANGLE);
				setState(1080);
				((DtypeContext)_localctx).s = type(0);
				setState(1081);
				((DtypeContext)_localctx).RANGLE = match(RANGLE);
				((DtypeContext)_localctx).t = new TypeInterval(new BaseType.BotType(((DtypeContext)_localctx).DYN.ToSourceSpan()),((DtypeContext)_localctx).s.t,((DtypeContext)_localctx).DYN.ToSourceSpan(((DtypeContext)_localctx).RANGLE));
				}
				break;
			case 4:
				enterOuterAlt(_localctx, 4);
				{
				setState(1084);
				((DtypeContext)_localctx).DYN = match(DYN);
				setState(1085);
				match(LBRACKET);
				setState(1086);
				((DtypeContext)_localctx).d = type(0);
				setState(1087);
				((DtypeContext)_localctx).RBRACKET = match(RBRACKET);
				((DtypeContext)_localctx).t = new TypeInterval(((DtypeContext)_localctx).d.t,new BaseType.TopType(((DtypeContext)_localctx).DYN.ToSourceSpan()),((DtypeContext)_localctx).DYN.ToSourceSpan(((DtypeContext)_localctx).RBRACKET));
				}
				break;
			case 5:
				enterOuterAlt(_localctx, 5);
				{
				setState(1090);
				((DtypeContext)_localctx).DYN = match(DYN);
				((DtypeContext)_localctx).t = new TypeInterval(new BaseType.BotType(((DtypeContext)_localctx).DYN.ToSourceSpan()),new BaseType.TopType(((DtypeContext)_localctx).DYN.ToSourceSpan()),((DtypeContext)_localctx).DYN.ToSourceSpan());
				}
				break;
			case 6:
				enterOuterAlt(_localctx, 6);
				{
				setState(1092);
				((DtypeContext)_localctx).type = type(0);
				setState(1093);
				((DtypeContext)_localctx).BANG = match(BANG);
				((DtypeContext)_localctx).t = new TypeInterval(((DtypeContext)_localctx).type.t,new MaybeType(((DtypeContext)_localctx).type.t,((DtypeContext)_localctx).BANG.ToSourceSpan()),((DtypeContext)_localctx).type.t.Start.SpanTo(((DtypeContext)_localctx).BANG));
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgdeclContext extends ParserRuleContext {
		public VarDecl v;
		public DtypeContext dtype;
		public IdentContext ident;
		public DtypeContext dtype() {
			return getRuleContext(DtypeContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgdeclContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argdecl; }
	}

	public final ArgdeclContext argdecl() throws RecognitionException {
		ArgdeclContext _localctx = new ArgdeclContext(_ctx, getState());
		enterRule(_localctx, 58, RULE_argdecl);
		try {
			setState(1105);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,122,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(1098);
				((ArgdeclContext)_localctx).dtype = dtype();
				setState(1099);
				((ArgdeclContext)_localctx).ident = ident();
				((ArgdeclContext)_localctx).v = new VarDecl(((ArgdeclContext)_localctx).ident.i,((ArgdeclContext)_localctx).dtype.t,((ArgdeclContext)_localctx).dtype.t.Start.SpanTo(((ArgdeclContext)_localctx).ident.i.End));
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(1102);
				((ArgdeclContext)_localctx).ident = ident();
				((ArgdeclContext)_localctx).v = new VarDecl(((ArgdeclContext)_localctx).ident.i,new TypeInterval(new BaseType.BotType(((ArgdeclContext)_localctx).ident.i.Locs),new BaseType.TopType(((ArgdeclContext)_localctx).ident.i.Locs),((ArgdeclContext)_localctx).ident.i.Locs));
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public boolean sempred(RuleContext _localctx, int ruleIndex, int predIndex) {
		switch (ruleIndex) {
		case 6:
			return expr_sempred((ExprContext)_localctx, predIndex);
		case 26:
			return type_sempred((TypeContext)_localctx, predIndex);
		}
		return true;
	}
	private boolean expr_sempred(ExprContext _localctx, int predIndex) {
		switch (predIndex) {
		case 0:
			return precpred(_ctx, 18);
		case 1:
			return precpred(_ctx, 17);
		case 2:
			return precpred(_ctx, 16);
		case 3:
			return precpred(_ctx, 15);
		case 4:
			return precpred(_ctx, 14);
		case 5:
			return precpred(_ctx, 13);
		case 6:
			return precpred(_ctx, 12);
		case 7:
			return precpred(_ctx, 11);
		case 8:
			return precpred(_ctx, 10);
		case 9:
			return precpred(_ctx, 9);
		case 10:
			return precpred(_ctx, 8);
		case 11:
			return precpred(_ctx, 7);
		case 12:
			return precpred(_ctx, 6);
		case 13:
			return precpred(_ctx, 5);
		case 14:
			return precpred(_ctx, 4);
		case 15:
			return precpred(_ctx, 3);
		case 16:
			return precpred(_ctx, 2);
		case 17:
			return precpred(_ctx, 1);
		case 18:
			return precpred(_ctx, 28);
		case 19:
			return precpred(_ctx, 27);
		}
		return true;
	}
	private boolean type_sempred(TypeContext _localctx, int predIndex) {
		switch (predIndex) {
		case 20:
			return precpred(_ctx, 4);
		case 21:
			return precpred(_ctx, 2);
		}
		return true;
	}

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\3]\u0456\4\2\t\2\4"+
		"\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4\13\t"+
		"\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22\t\22"+
		"\4\23\t\23\4\24\t\24\4\25\t\25\4\26\t\26\4\27\t\27\4\30\t\30\4\31\t\31"+
		"\4\32\t\32\4\33\t\33\4\34\t\34\4\35\t\35\4\36\t\36\4\37\t\37\3\2\7\2@"+
		"\n\2\f\2\16\2C\13\2\3\2\3\2\3\2\7\2H\n\2\f\2\16\2K\13\2\3\2\3\2\3\3\3"+
		"\3\3\3\3\3\3\3\3\4\3\4\3\4\7\4W\n\4\f\4\16\4Z\13\4\3\4\3\4\3\4\3\5\3\5"+
		"\3\5\3\5\3\5\3\5\7\5e\n\5\f\5\16\5h\13\5\3\5\3\5\3\5\3\6\3\6\3\6\3\6\3"+
		"\6\3\6\3\6\3\6\3\6\3\6\5\6w\n\6\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3"+
		"\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\7\7\u0092"+
		"\n\7\f\7\16\7\u0095\13\7\5\7\u0097\n\7\3\7\5\7\u009a\n\7\3\7\3\7\3\7\3"+
		"\7\3\7\3\7\7\7\u00a2\n\7\f\7\16\7\u00a5\13\7\5\7\u00a7\n\7\3\7\3\7\3\7"+
		"\3\7\3\7\5\7\u00ae\n\7\3\b\3\b\3\b\3\b\3\b\3\b\3\b\7\b\u00b7\n\b\f\b\16"+
		"\b\u00ba\13\b\5\b\u00bc\n\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3"+
		"\b\3\b\3\b\3\b\7\b\u00cc\n\b\f\b\16\b\u00cf\13\b\5\b\u00d1\n\b\3\b\3\b"+
		"\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\7\b\u00e5"+
		"\n\b\f\b\16\b\u00e8\13\b\5\b\u00ea\n\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\7\b"+
		"\u00f3\n\b\f\b\16\b\u00f6\13\b\5\b\u00f8\n\b\3\b\3\b\3\b\3\b\3\b\3\b\3"+
		"\b\3\b\3\b\3\b\3\b\3\b\3\b\5\b\u0107\n\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3"+
		"\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b"+
		"\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3"+
		"\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b"+
		"\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3"+
		"\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b"+
		"\3\b\3\b\3\b\3\b\3\b\3\b\3\b\7\b\u016f\n\b\f\b\16\b\u0172\13\b\5\b\u0174"+
		"\n\b\3\b\3\b\3\b\7\b\u0179\n\b\f\b\16\b\u017c\13\b\3\t\3\t\3\t\3\t\3\t"+
		"\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3"+
		"\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\5\t\u019c\n\t\3\n\3\n\3\n\3\n\5\n\u01a2"+
		"\n\n\3\13\3\13\3\13\3\13\5\13\u01a8\n\13\3\f\3\f\3\f\3\f\5\f\u01ae\n\f"+
		"\3\r\3\r\3\r\5\r\u01b3\n\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\5\r\u01be"+
		"\n\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\5\r\u01c9\n\r\3\r\3\r\3\r\3\r"+
		"\3\r\3\r\3\r\3\r\3\r\5\r\u01d4\n\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r"+
		"\5\r\u01df\n\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r"+
		"\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3"+
		"\r\3\r\3\r\3\r\3\r\3\r\5\r\u0206\n\r\3\r\3\r\3\r\3\r\5\r\u020c\n\r\3\r"+
		"\3\r\3\r\3\r\5\r\u0212\n\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r"+
		"\3\r\3\r\7\r\u0221\n\r\f\r\16\r\u0224\13\r\3\r\3\r\5\r\u0228\n\r\3\r\3"+
		"\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\5\r\u0235\n\r\3\r\3\r\3\r\3\r\3"+
		"\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\5\r\u0249\n\r\3"+
		"\16\3\16\7\16\u024d\n\16\f\16\16\16\u0250\13\16\3\16\3\16\3\16\3\17\3"+
		"\17\3\17\3\20\3\20\3\20\3\21\3\21\7\21\u025d\n\21\f\21\16\21\u0260\13"+
		"\21\3\21\3\21\3\21\3\21\3\21\3\21\7\21\u0268\n\21\f\21\16\21\u026b\13"+
		"\21\5\21\u026d\n\21\3\21\3\21\3\21\7\21\u0272\n\21\f\21\16\21\u0275\13"+
		"\21\3\21\3\21\3\21\3\22\3\22\3\22\3\22\7\22\u027e\n\22\f\22\16\22\u0281"+
		"\13\22\3\22\3\22\3\22\3\22\5\22\u0287\n\22\3\22\3\22\3\22\3\22\7\22\u028d"+
		"\n\22\f\22\16\22\u0290\13\22\5\22\u0292\n\22\3\22\3\22\3\22\3\22\3\22"+
		"\3\22\3\22\3\22\3\22\7\22\u029d\n\22\f\22\16\22\u02a0\13\22\3\22\3\22"+
		"\3\22\3\23\5\23\u02a6\n\23\3\23\3\23\3\23\3\23\3\23\7\23\u02ad\n\23\f"+
		"\23\16\23\u02b0\13\23\5\23\u02b2\n\23\3\23\3\23\3\23\7\23\u02b7\n\23\f"+
		"\23\16\23\u02ba\13\23\3\23\3\23\3\23\3\23\3\23\7\23\u02c1\n\23\f\23\16"+
		"\23\u02c4\13\23\5\23\u02c6\n\23\3\23\3\23\3\23\7\23\u02cb\n\23\f\23\16"+
		"\23\u02ce\13\23\5\23\u02d0\n\23\3\23\3\23\3\23\3\24\5\24\u02d6\n\24\3"+
		"\24\5\24\u02d9\n\24\3\24\3\24\3\24\3\24\3\24\3\24\5\24\u02e1\n\24\3\24"+
		"\5\24\u02e4\n\24\3\24\3\24\3\24\3\24\3\24\5\24\u02eb\n\24\3\24\5\24\u02ee"+
		"\n\24\3\24\3\24\3\24\3\24\3\24\3\24\3\24\3\24\5\24\u02f8\n\24\3\24\5\24"+
		"\u02fb\n\24\3\24\3\24\3\24\3\24\3\24\3\24\5\24\u0303\n\24\3\25\5\25\u0306"+
		"\n\25\3\25\3\25\5\25\u030a\n\25\3\25\3\25\3\25\3\25\3\25\3\25\5\25\u0312"+
		"\n\25\3\25\3\25\5\25\u0316\n\25\3\25\3\25\3\25\3\25\3\25\5\25\u031d\n"+
		"\25\3\25\3\25\5\25\u0321\n\25\3\25\3\25\3\25\3\25\3\25\3\25\3\25\3\25"+
		"\5\25\u032b\n\25\3\25\3\25\5\25\u032f\n\25\3\25\3\25\3\25\3\25\3\25\3"+
		"\25\5\25\u0337\n\25\3\26\5\26\u033a\n\26\3\26\5\26\u033d\n\26\3\26\5\26"+
		"\u0340\n\26\3\26\3\26\3\26\3\26\3\26\3\26\3\26\3\26\7\26\u034a\n\26\f"+
		"\26\16\26\u034d\13\26\5\26\u034f\n\26\3\26\5\26\u0352\n\26\5\26\u0354"+
		"\n\26\3\26\3\26\7\26\u0358\n\26\f\26\16\26\u035b\13\26\3\26\3\26\3\26"+
		"\3\26\5\26\u0361\n\26\3\26\3\26\3\26\3\26\3\26\3\26\3\26\7\26\u036a\n"+
		"\26\f\26\16\26\u036d\13\26\5\26\u036f\n\26\3\26\3\26\3\26\3\26\3\26\3"+
		"\26\3\26\7\26\u0378\n\26\f\26\16\26\u037b\13\26\5\26\u037d\n\26\3\26\5"+
		"\26\u0380\n\26\5\26\u0382\n\26\3\26\3\26\7\26\u0386\n\26\f\26\16\26\u0389"+
		"\13\26\3\26\3\26\3\26\5\26\u038e\n\26\3\27\3\27\3\27\3\27\3\27\3\27\3"+
		"\30\3\30\3\30\3\30\3\30\3\30\7\30\u039c\n\30\f\30\16\30\u039f\13\30\5"+
		"\30\u03a1\n\30\3\30\3\30\3\30\5\30\u03a6\n\30\3\30\3\30\7\30\u03aa\n\30"+
		"\f\30\16\30\u03ad\13\30\3\30\3\30\3\30\3\31\3\31\3\31\3\31\7\31\u03b6"+
		"\n\31\f\31\16\31\u03b9\13\31\3\31\3\31\3\31\3\31\3\31\3\31\7\31\u03c1"+
		"\n\31\f\31\16\31\u03c4\13\31\5\31\u03c6\n\31\3\31\3\31\3\31\5\31\u03cb"+
		"\n\31\3\31\3\31\3\31\3\32\5\32\u03d1\n\32\3\32\3\32\3\32\3\32\3\32\3\32"+
		"\3\32\7\32\u03da\n\32\f\32\16\32\u03dd\13\32\5\32\u03df\n\32\3\32\3\32"+
		"\3\32\5\32\u03e4\n\32\3\32\3\32\3\32\3\33\3\33\3\33\3\33\3\33\3\33\7\33"+
		"\u03ef\n\33\f\33\16\33\u03f2\13\33\5\33\u03f4\n\33\3\33\3\33\3\33\5\33"+
		"\u03f9\n\33\3\33\3\33\3\33\3\34\3\34\3\34\3\34\3\34\3\34\3\34\5\34\u0405"+
		"\n\34\3\34\3\34\3\34\3\34\3\34\3\34\3\34\5\34\u040e\n\34\3\34\3\34\3\34"+
		"\3\34\3\34\3\34\3\34\7\34\u0417\n\34\f\34\16\34\u041a\13\34\3\35\3\35"+
		"\3\35\3\35\3\35\3\35\3\35\7\35\u0423\n\35\f\35\16\35\u0426\13\35\3\35"+
		"\3\35\3\35\5\35\u042b\n\35\3\36\3\36\3\36\3\36\3\36\3\36\3\36\3\36\3\36"+
		"\3\36\3\36\3\36\3\36\3\36\3\36\3\36\3\36\3\36\3\36\3\36\3\36\3\36\3\36"+
		"\3\36\3\36\3\36\3\36\3\36\3\36\3\36\5\36\u044b\n\36\3\37\3\37\3\37\3\37"+
		"\3\37\3\37\3\37\5\37\u0454\n\37\3\37\2\4\16\66 \2\4\6\b\n\f\16\20\22\24"+
		"\26\30\32\34\36 \"$&(*,.\60\62\64\668:<\2\2\2\u04fe\2A\3\2\2\2\4N\3\2"+
		"\2\2\6X\3\2\2\2\b^\3\2\2\2\nv\3\2\2\2\f\u00ad\3\2\2\2\16\u0106\3\2\2\2"+
		"\20\u019b\3\2\2\2\22\u01a1\3\2\2\2\24\u01a7\3\2\2\2\26\u01ad\3\2\2\2\30"+
		"\u0248\3\2\2\2\32\u024a\3\2\2\2\34\u0254\3\2\2\2\36\u0257\3\2\2\2 \u025e"+
		"\3\2\2\2\"\u027f\3\2\2\2$\u02a5\3\2\2\2&\u0302\3\2\2\2(\u0336\3\2\2\2"+
		"*\u038d\3\2\2\2,\u038f\3\2\2\2.\u0395\3\2\2\2\60\u03b7\3\2\2\2\62\u03d0"+
		"\3\2\2\2\64\u03e8\3\2\2\2\66\u040d\3\2\2\28\u042a\3\2\2\2:\u044a\3\2\2"+
		"\2<\u0453\3\2\2\2>@\5\4\3\2?>\3\2\2\2@C\3\2\2\2A?\3\2\2\2AB\3\2\2\2BI"+
		"\3\2\2\2CA\3\2\2\2DH\5\"\22\2EH\5 \21\2FH\5\b\5\2GD\3\2\2\2GE\3\2\2\2"+
		"GF\3\2\2\2HK\3\2\2\2IG\3\2\2\2IJ\3\2\2\2JL\3\2\2\2KI\3\2\2\2LM\b\2\1\2"+
		"M\3\3\2\2\2NO\7\b\2\2OP\5\6\4\2PQ\7\62\2\2QR\b\3\1\2R\5\3\2\2\2ST\5\34"+
		"\17\2TU\7\65\2\2UW\3\2\2\2VS\3\2\2\2WZ\3\2\2\2XV\3\2\2\2XY\3\2\2\2Y[\3"+
		"\2\2\2ZX\3\2\2\2[\\\5\34\17\2\\]\b\4\1\2]\7\3\2\2\2^_\7\r\2\2_`\5\6\4"+
		"\2`f\7<\2\2ae\5\"\22\2be\5 \21\2ce\5\b\5\2da\3\2\2\2db\3\2\2\2dc\3\2\2"+
		"\2eh\3\2\2\2fd\3\2\2\2fg\3\2\2\2gi\3\2\2\2hf\3\2\2\2ij\7=\2\2jk\b\5\1"+
		"\2k\t\3\2\2\2lm\7\34\2\2mw\b\6\1\2no\7\35\2\2ow\b\6\1\2pq\7\37\2\2qw\b"+
		"\6\1\2rs\7\36\2\2sw\b\6\1\2tu\7\33\2\2uw\b\6\1\2vl\3\2\2\2vn\3\2\2\2v"+
		"p\3\2\2\2vr\3\2\2\2vt\3\2\2\2w\13\3\2\2\2xy\7\3\2\2y\u00ae\b\7\1\2z{\7"+
		"\5\2\2{\u00ae\b\7\1\2|}\7G\2\2}~\7\5\2\2~\u00ae\b\7\1\2\177\u0080\7\4"+
		"\2\2\u0080\u00ae\b\7\1\2\u0081\u0082\7G\2\2\u0082\u0083\7\4\2\2\u0083"+
		"\u00ae\b\7\1\2\u0084\u0085\7\6\2\2\u0085\u00ae\b\7\1\2\u0086\u0087\7\7"+
		"\2\2\u0087\u00ae\b\7\1\2\u0088\u0089\7\26\2\2\u0089\u00ae\b\7\1\2\u008a"+
		"\u008b\5\36\20\2\u008b\u008c\7\63\2\2\u008c\u0099\5\34\17\2\u008d\u0096"+
		"\7:\2\2\u008e\u0093\5\f\7\2\u008f\u0090\7\67\2\2\u0090\u0092\5\f\7\2\u0091"+
		"\u008f\3\2\2\2\u0092\u0095\3\2\2\2\u0093\u0091\3\2\2\2\u0093\u0094\3\2"+
		"\2\2\u0094\u0097\3\2\2\2\u0095\u0093\3\2\2\2\u0096\u008e\3\2\2\2\u0096"+
		"\u0097\3\2\2\2\u0097\u0098\3\2\2\2\u0098\u009a\7;\2\2\u0099\u008d\3\2"+
		"\2\2\u0099\u009a\3\2\2\2\u009a\u009b\3\2\2\2\u009b\u009c\b\7\1\2\u009c"+
		"\u00ae\3\2\2\2\u009d\u00a6\7:\2\2\u009e\u00a3\5\f\7\2\u009f\u00a0\7\67"+
		"\2\2\u00a0\u00a2\5\f\7\2\u00a1\u009f\3\2\2\2\u00a2\u00a5\3\2\2\2\u00a3"+
		"\u00a1\3\2\2\2\u00a3\u00a4\3\2\2\2\u00a4\u00a7\3\2\2\2\u00a5\u00a3\3\2"+
		"\2\2\u00a6\u009e\3\2\2\2\u00a6\u00a7\3\2\2\2\u00a7\u00a8\3\2\2\2\u00a8"+
		"\u00a9\7;\2\2\u00a9\u00ae\b\7\1\2\u00aa\u00ab\5\34\17\2\u00ab\u00ac\b"+
		"\7\1\2\u00ac\u00ae\3\2\2\2\u00adx\3\2\2\2\u00adz\3\2\2\2\u00ad|\3\2\2"+
		"\2\u00ad\177\3\2\2\2\u00ad\u0081\3\2\2\2\u00ad\u0084\3\2\2\2\u00ad\u0086"+
		"\3\2\2\2\u00ad\u0088\3\2\2\2\u00ad\u008a\3\2\2\2\u00ad\u009d\3\2\2\2\u00ad"+
		"\u00aa\3\2\2\2\u00ae\r\3\2\2\2\u00af\u00b0\b\b\1\2\u00b0\u00b1\7/\2\2"+
		"\u00b1\u00b2\5\36\20\2\u00b2\u00bb\78\2\2\u00b3\u00b8\5\16\b\2\u00b4\u00b5"+
		"\7\67\2\2\u00b5\u00b7\5\16\b\2\u00b6\u00b4\3\2\2\2\u00b7\u00ba\3\2\2\2"+
		"\u00b8\u00b6\3\2\2\2\u00b8\u00b9\3\2\2\2\u00b9\u00bc\3\2\2\2\u00ba\u00b8"+
		"\3\2\2\2\u00bb\u00b3\3\2\2\2\u00bb\u00bc\3\2\2\2\u00bc\u00bd\3\2\2\2\u00bd"+
		"\u00be\79\2\2\u00be\u00bf\b\b\1\2\u00bf\u0107\3\2\2\2\u00c0\u00c1\78\2"+
		"\2\u00c1\u00c2\5:\36\2\u00c2\u00c3\79\2\2\u00c3\u00c4\5\16\b\37\u00c4"+
		"\u00c5\b\b\1\2\u00c5\u0107\3\2\2\2\u00c6\u00c7\5\34\17\2\u00c7\u00d0\7"+
		"8\2\2\u00c8\u00cd\5\16\b\2\u00c9\u00ca\7\67\2\2\u00ca\u00cc\5\16\b\2\u00cb"+
		"\u00c9\3\2\2\2\u00cc\u00cf\3\2\2\2\u00cd\u00cb\3\2\2\2\u00cd\u00ce\3\2"+
		"\2\2\u00ce\u00d1\3\2\2\2\u00cf\u00cd\3\2\2\2\u00d0\u00c8\3\2\2\2\u00d0"+
		"\u00d1\3\2\2\2\u00d1\u00d2\3\2\2\2\u00d2\u00d3\79\2\2\u00d3\u00d4\b\b"+
		"\1\2\u00d4\u0107\3\2\2\2\u00d5\u00d6\78\2\2\u00d6\u00d7\5\16\b\2\u00d7"+
		"\u00d8\79\2\2\u00d8\u00d9\b\b\1\2\u00d9\u0107\3\2\2\2\u00da\u00db\5\20"+
		"\t\2\u00db\u00dc\b\b\1\2\u00dc\u0107\3\2\2\2\u00dd\u00de\5\36\20\2\u00de"+
		"\u00df\7\63\2\2\u00df\u00e0\5\34\17\2\u00e0\u00e9\78\2\2\u00e1\u00e6\5"+
		"\16\b\2\u00e2\u00e3\7\67\2\2\u00e3\u00e5\5\16\b\2\u00e4\u00e2\3\2\2\2"+
		"\u00e5\u00e8\3\2\2\2\u00e6\u00e4\3\2\2\2\u00e6\u00e7\3\2\2\2\u00e7\u00ea"+
		"\3\2\2\2\u00e8\u00e6\3\2\2\2\u00e9\u00e1\3\2\2\2\u00e9\u00ea\3\2\2\2\u00ea"+
		"\u00eb\3\2\2\2\u00eb\u00ec\79\2\2\u00ec\u00ed\b\b\1\2\u00ed\u0107\3\2"+
		"\2\2\u00ee\u00f7\7<\2\2\u00ef\u00f4\5\16\b\2\u00f0\u00f1\7\67\2\2\u00f1"+
		"\u00f3\5\16\b\2\u00f2\u00f0\3\2\2\2\u00f3\u00f6\3\2\2\2\u00f4\u00f2\3"+
		"\2\2\2\u00f4\u00f5\3\2\2\2\u00f5\u00f8\3\2\2\2\u00f6\u00f4\3\2\2\2\u00f7"+
		"\u00ef\3\2\2\2\u00f7\u00f8\3\2\2\2\u00f8\u00f9\3\2\2\2\u00f9\u00fa\7="+
		"\2\2\u00fa\u0107\b\b\1\2\u00fb\u00fc\5\f\7\2\u00fc\u00fd\b\b\1\2\u00fd"+
		"\u0107\3\2\2\2\u00fe\u00ff\7B\2\2\u00ff\u0100\5\16\b\26\u0100\u0101\b"+
		"\b\1\2\u0101\u0107\3\2\2\2\u0102\u0103\7G\2\2\u0103\u0104\5\16\b\25\u0104"+
		"\u0105\b\b\1\2\u0105\u0107\3\2\2\2\u0106\u00af\3\2\2\2\u0106\u00c0\3\2"+
		"\2\2\u0106\u00c6\3\2\2\2\u0106\u00d5\3\2\2\2\u0106\u00da\3\2\2\2\u0106"+
		"\u00dd\3\2\2\2\u0106\u00ee\3\2\2\2\u0106\u00fb\3\2\2\2\u0106\u00fe\3\2"+
		"\2\2\u0106\u0102\3\2\2\2\u0107\u017a\3\2\2\2\u0108\u0109\f\24\2\2\u0109"+
		"\u010a\7J\2\2\u010a\u010b\5\16\b\25\u010b\u010c\b\b\1\2\u010c\u0179\3"+
		"\2\2\2\u010d\u010e\f\23\2\2\u010e\u010f\5\22\n\2\u010f\u0110\5\16\b\24"+
		"\u0110\u0111\b\b\1\2\u0111\u0179\3\2\2\2\u0112\u0113\f\22\2\2\u0113\u0114"+
		"\7K\2\2\u0114\u0115\5\16\b\23\u0115\u0116\b\b\1\2\u0116\u0179\3\2\2\2"+
		"\u0117\u0118\f\21\2\2\u0118\u0119\5\24\13\2\u0119\u011a\5\16\b\22\u011a"+
		"\u011b\b\b\1\2\u011b\u0179\3\2\2\2\u011c\u011d\f\20\2\2\u011d\u011e\5"+
		"\26\f\2\u011e\u011f\5\16\b\21\u011f\u0120\b\b\1\2\u0120\u0179\3\2\2\2"+
		"\u0121\u0122\f\17\2\2\u0122\u0123\7S\2\2\u0123\u0124\5\16\b\20\u0124\u0125"+
		"\b\b\1\2\u0125\u0179\3\2\2\2\u0126\u0127\f\16\2\2\u0127\u0128\7R\2\2\u0128"+
		"\u0129\5\16\b\17\u0129\u012a\b\b\1\2\u012a\u0179\3\2\2\2\u012b\u012c\f"+
		"\r\2\2\u012c\u012d\7T\2\2\u012d\u012e\5\16\b\16\u012e\u012f\b\b\1\2\u012f"+
		"\u0179\3\2\2\2\u0130\u0131\f\f\2\2\u0131\u0132\7E\2\2\u0132\u0133\5\16"+
		"\b\r\u0133\u0134\b\b\1\2\u0134\u0179\3\2\2\2\u0135\u0136\f\13\2\2\u0136"+
		"\u0137\7>\2\2\u0137\u0138\5\16\b\f\u0138\u0139\b\b\1\2\u0139\u0179\3\2"+
		"\2\2\u013a\u013b\f\n\2\2\u013b\u013c\7?\2\2\u013c\u013d\5\16\b\13\u013d"+
		"\u013e\b\b\1\2\u013e\u0179\3\2\2\2\u013f\u0140\f\t\2\2\u0140\u0141\7P"+
		"\2\2\u0141\u0142\5\16\b\t\u0142\u0143\b\b\1\2\u0143\u0179\3\2\2\2\u0144"+
		"\u0145\f\b\2\2\u0145\u0146\7Q\2\2\u0146\u0147\5\16\b\b\u0147\u0148\b\b"+
		"\1\2\u0148\u0179\3\2\2\2\u0149\u014a\f\7\2\2\u014a\u014b\7M\2\2\u014b"+
		"\u014c\5\16\b\7\u014c\u014d\b\b\1\2\u014d\u0179\3\2\2\2\u014e\u014f\f"+
		"\6\2\2\u014f\u0150\7N\2\2\u0150\u0151\5\16\b\6\u0151\u0152\b\b\1\2\u0152"+
		"\u0179\3\2\2\2\u0153\u0154\f\5\2\2\u0154\u0155\7C\2\2\u0155\u0156\5\16"+
		"\b\5\u0156\u0157\b\b\1\2\u0157\u0179\3\2\2\2\u0158\u0159\f\4\2\2\u0159"+
		"\u015a\7D\2\2\u015a\u015b\5\16\b\4\u015b\u015c\b\b\1\2\u015c\u0179\3\2"+
		"\2\2\u015d\u015e\f\3\2\2\u015e\u015f\7O\2\2\u015f\u0160\5\16\b\3\u0160"+
		"\u0161\b\b\1\2\u0161\u0179\3\2\2\2\u0162\u0163\f\36\2\2\u0163\u0164\7"+
		"\65\2\2\u0164\u0165\5\34\17\2\u0165\u0166\b\b\1\2\u0166\u0179\3\2\2\2"+
		"\u0167\u0168\f\35\2\2\u0168\u0169\7\65\2\2\u0169\u016a\5\34\17\2\u016a"+
		"\u0173\78\2\2\u016b\u0170\5\16\b\2\u016c\u016d\7\67\2\2\u016d\u016f\5"+
		"\16\b\2\u016e\u016c\3\2\2\2\u016f\u0172\3\2\2\2\u0170\u016e\3\2\2\2\u0170"+
		"\u0171\3\2\2\2\u0171\u0174\3\2\2\2\u0172\u0170\3\2\2\2\u0173\u016b\3\2"+
		"\2\2\u0173\u0174\3\2\2\2\u0174\u0175\3\2\2\2\u0175\u0176\79\2\2\u0176"+
		"\u0177\b\b\1\2\u0177\u0179\3\2\2\2\u0178\u0108\3\2\2\2\u0178\u010d\3\2"+
		"\2\2\u0178\u0112\3\2\2\2\u0178\u0117\3\2\2\2\u0178\u011c\3\2\2\2\u0178"+
		"\u0121\3\2\2\2\u0178\u0126\3\2\2\2\u0178\u012b\3\2\2\2\u0178\u0130\3\2"+
		"\2\2\u0178\u0135\3\2\2\2\u0178\u013a\3\2\2\2\u0178\u013f\3\2\2\2\u0178"+
		"\u0144\3\2\2\2\u0178\u0149\3\2\2\2\u0178\u014e\3\2\2\2\u0178\u0153\3\2"+
		"\2\2\u0178\u0158\3\2\2\2\u0178\u015d\3\2\2\2\u0178\u0162\3\2\2\2\u0178"+
		"\u0167\3\2\2\2\u0179\u017c\3\2\2\2\u017a\u0178\3\2\2\2\u017a\u017b\3\2"+
		"\2\2\u017b\17\3\2\2\2\u017c\u017a\3\2\2\2\u017d\u017e\7:\2\2\u017e\u017f"+
		"\7\66\2\2\u017f\u0180\5\16\b\2\u0180\u0181\7;\2\2\u0181\u0182\b\t\1\2"+
		"\u0182\u019c\3\2\2\2\u0183\u0184\7:\2\2\u0184\u0185\5\16\b\2\u0185\u0186"+
		"\7\66\2\2\u0186\u0187\5\16\b\2\u0187\u0188\7;\2\2\u0188\u0189\b\t\1\2"+
		"\u0189\u019c\3\2\2\2\u018a\u018b\7:\2\2\u018b\u018c\7\66\2\2\u018c\u018d"+
		"\5\16\b\2\u018d\u018e\7\64\2\2\u018e\u018f\5\16\b\2\u018f\u0190\7;\2\2"+
		"\u0190\u0191\b\t\1\2\u0191\u019c\3\2\2\2\u0192\u0193\7:\2\2\u0193\u0194"+
		"\5\16\b\2\u0194\u0195\7\66\2\2\u0195\u0196\5\16\b\2\u0196\u0197\7\64\2"+
		"\2\u0197\u0198\5\16\b\2\u0198\u0199\7;\2\2\u0199\u019a\b\t\1\2\u019a\u019c"+
		"\3\2\2\2\u019b\u017d\3\2\2\2\u019b\u0183\3\2\2\2\u019b\u018a\3\2\2\2\u019b"+
		"\u0192\3\2\2\2\u019c\21\3\2\2\2\u019d\u019e\7I\2\2\u019e\u01a2\b\n\1\2"+
		"\u019f\u01a0\7H\2\2\u01a0\u01a2\b\n\1\2\u01a1\u019d\3\2\2\2\u01a1\u019f"+
		"\3\2\2\2\u01a2\23\3\2\2\2\u01a3\u01a4\7F\2\2\u01a4\u01a8\b\13\1\2\u01a5"+
		"\u01a6\7G\2\2\u01a6\u01a8\b\13\1\2\u01a7\u01a3\3\2\2\2\u01a7\u01a5\3\2"+
		"\2\2\u01a8\25\3\2\2\2\u01a9\u01aa\7Y\2\2\u01aa\u01ae\b\f\1\2\u01ab\u01ac"+
		"\7Z\2\2\u01ac\u01ae\b\f\1\2\u01ad\u01a9\3\2\2\2\u01ad\u01ab\3\2\2\2\u01ae"+
		"\27\3\2\2\2\u01af\u01b0\5\16\b\2\u01b0\u01b1\7\65\2\2\u01b1\u01b3\3\2"+
		"\2\2\u01b2\u01af\3\2\2\2\u01b2\u01b3\3\2\2\2\u01b3\u01b4\3\2\2\2\u01b4"+
		"\u01b5\5\34\17\2\u01b5\u01b6\7L\2\2\u01b6\u01b7\5\16\b\2\u01b7\u01b8\7"+
		"\62\2\2\u01b8\u01b9\b\r\1\2\u01b9\u0249\3\2\2\2\u01ba\u01bb\5\16\b\2\u01bb"+
		"\u01bc\7\65\2\2\u01bc\u01be\3\2\2\2\u01bd\u01ba\3\2\2\2\u01bd\u01be\3"+
		"\2\2\2\u01be\u01bf\3\2\2\2\u01bf\u01c0\5\34\17\2\u01c0\u01c1\7U\2\2\u01c1"+
		"\u01c2\5\16\b\2\u01c2\u01c3\7\62\2\2\u01c3\u01c4\b\r\1\2\u01c4\u0249\3"+
		"\2\2\2\u01c5\u01c6\5\16\b\2\u01c6\u01c7\7\65\2\2\u01c7\u01c9\3\2\2\2\u01c8"+
		"\u01c5\3\2\2\2\u01c8\u01c9\3\2\2\2\u01c9\u01ca\3\2\2\2\u01ca\u01cb\5\34"+
		"\17\2\u01cb\u01cc\7V\2\2\u01cc\u01cd\5\16\b\2\u01cd\u01ce\7\62\2\2\u01ce"+
		"\u01cf\b\r\1\2\u01cf\u0249\3\2\2\2\u01d0\u01d1\5\16\b\2\u01d1\u01d2\7"+
		"\65\2\2\u01d2\u01d4\3\2\2\2\u01d3\u01d0\3\2\2\2\u01d3\u01d4\3\2\2\2\u01d4"+
		"\u01d5\3\2\2\2\u01d5\u01d6\5\34\17\2\u01d6\u01d7\7W\2\2\u01d7\u01d8\5"+
		"\16\b\2\u01d8\u01d9\7\62\2\2\u01d9\u01da\b\r\1\2\u01da\u0249\3\2\2\2\u01db"+
		"\u01dc\5\16\b\2\u01dc\u01dd\7\65\2\2\u01dd\u01df\3\2\2\2\u01de\u01db\3"+
		"\2\2\2\u01de\u01df\3\2\2\2\u01df\u01e0\3\2\2\2\u01e0\u01e1\5\34\17\2\u01e1"+
		"\u01e2\7X\2\2\u01e2\u01e3\5\16\b\2\u01e3\u01e4\7\62\2\2\u01e4\u01e5\b"+
		"\r\1\2\u01e5\u0249\3\2\2\2\u01e6\u01e7\5:\36\2\u01e7\u01e8\5\34\17\2\u01e8"+
		"\u01e9\7L\2\2\u01e9\u01ea\5\16\b\2\u01ea\u01eb\7\62\2\2\u01eb\u01ec\b"+
		"\r\1\2\u01ec\u0249\3\2\2\2\u01ed\u01ee\5:\36\2\u01ee\u01ef\5\34\17\2\u01ef"+
		"\u01f0\7\62\2\2\u01f0\u01f1\b\r\1\2\u01f1\u0249\3\2\2\2\u01f2\u01f3\5"+
		"\16\b\2\u01f3\u01f4\7\62\2\2\u01f4\u01f5\b\r\1\2\u01f5\u0249\3\2\2\2\u01f6"+
		"\u01f7\7\60\2\2\u01f7\u01f8\78\2\2\u01f8\u01f9\7\3\2\2\u01f9\u01fa\79"+
		"\2\2\u01fa\u01fb\7\62\2\2\u01fb\u0249\b\r\1\2\u01fc\u01fd\7\61\2\2\u01fd"+
		"\u01fe\78\2\2\u01fe\u01ff\5\16\b\2\u01ff\u0200\79\2\2\u0200\u0201\7\62"+
		"\2\2\u0201\u0202\b\r\1\2\u0202\u0249\3\2\2\2\u0203\u0205\7*\2\2\u0204"+
		"\u0206\7\5\2\2\u0205\u0204\3\2\2\2\u0205\u0206\3\2\2\2\u0206\u0207\3\2"+
		"\2\2\u0207\u0208\7\62\2\2\u0208\u0249\b\r\1\2\u0209\u020b\7+\2\2\u020a"+
		"\u020c\7\5\2\2\u020b\u020a\3\2\2\2\u020b\u020c\3\2\2\2\u020c\u020d\3\2"+
		"\2\2\u020d\u020e\7\62\2\2\u020e\u0249\b\r\1\2\u020f\u0211\7)\2\2\u0210"+
		"\u0212\5\16\b\2\u0211\u0210\3\2\2\2\u0211\u0212\3\2\2\2\u0212\u0213\3"+
		"\2\2\2\u0213\u0214\7\62\2\2\u0214\u0249\b\r\1\2\u0215\u0216\7\"\2\2\u0216"+
		"\u0217\78\2\2\u0217\u0218\5\16\b\2\u0218\u0219\79\2\2\u0219\u0222\5\32"+
		"\16\2\u021a\u021b\7$\2\2\u021b\u021c\78\2\2\u021c\u021d\5\16\b\2\u021d"+
		"\u021e\79\2\2\u021e\u021f\5\32\16\2\u021f\u0221\3\2\2\2\u0220\u021a\3"+
		"\2\2\2\u0221\u0224\3\2\2\2\u0222\u0220\3\2\2\2\u0222\u0223\3\2\2\2\u0223"+
		"\u0227\3\2\2\2\u0224\u0222\3\2\2\2\u0225\u0226\7#\2\2\u0226\u0228\5\32"+
		"\16\2\u0227\u0225\3\2\2\2\u0227\u0228\3\2\2\2\u0228\u0229\3\2\2\2\u0229"+
		"\u022a\b\r\1\2\u022a\u0249\3\2\2\2\u022b\u022c\7\"\2\2\u022c\u022d\78"+
		"\2\2\u022d\u022e\5\34\17\2\u022e\u022f\7\64\2\2\u022f\u0230\5\66\34\2"+
		"\u0230\u0231\79\2\2\u0231\u0234\5\32\16\2\u0232\u0233\7#\2\2\u0233\u0235"+
		"\5\32\16\2\u0234\u0232\3\2\2\2\u0234\u0235\3\2\2\2\u0235\u0236\3\2\2\2"+
		"\u0236\u0237\b\r\1\2\u0237\u0249\3\2\2\2\u0238\u0239\7%\2\2\u0239\u023a"+
		"\78\2\2\u023a\u023b\5\16\b\2\u023b\u023c\79\2\2\u023c\u023d\5\32\16\2"+
		"\u023d\u023e\b\r\1\2\u023e\u0249\3\2\2\2\u023f\u0240\7\'\2\2\u0240\u0241"+
		"\78\2\2\u0241\u0242\5<\37\2\u0242\u0243\7(\2\2\u0243\u0244\5\16\b\2\u0244"+
		"\u0245\79\2\2\u0245\u0246\5\32\16\2\u0246\u0247\b\r\1\2\u0247\u0249\3"+
		"\2\2\2\u0248\u01b2\3\2\2\2\u0248\u01bd\3\2\2\2\u0248\u01c8\3\2\2\2\u0248"+
		"\u01d3\3\2\2\2\u0248\u01de\3\2\2\2\u0248\u01e6\3\2\2\2\u0248\u01ed\3\2"+
		"\2\2\u0248\u01f2\3\2\2\2\u0248\u01f6\3\2\2\2\u0248\u01fc\3\2\2\2\u0248"+
		"\u0203\3\2\2\2\u0248\u0209\3\2\2\2\u0248\u020f\3\2\2\2\u0248\u0215\3\2"+
		"\2\2\u0248\u022b\3\2\2\2\u0248\u0238\3\2\2\2\u0248\u023f\3\2\2\2\u0249"+
		"\31\3\2\2\2\u024a\u024e\7<\2\2\u024b\u024d\5\30\r\2\u024c\u024b\3\2\2"+
		"\2\u024d\u0250\3\2\2\2\u024e\u024c\3\2\2\2\u024e\u024f\3\2\2\2\u024f\u0251"+
		"\3\2\2\2\u0250\u024e\3\2\2\2\u0251\u0252\7=\2\2\u0252\u0253\b\16\1\2\u0253"+
		"\33\3\2\2\2\u0254\u0255\7[\2\2\u0255\u0256\b\17\1\2\u0256\35\3\2\2\2\u0257"+
		"\u0258\5\6\4\2\u0258\u0259\b\20\1\2\u0259\37\3\2\2\2\u025a\u025d\5\n\6"+
		"\2\u025b\u025d\7\n\2\2\u025c\u025a\3\2\2\2\u025c\u025b\3\2\2\2\u025d\u0260"+
		"\3\2\2\2\u025e\u025c\3\2\2\2\u025e\u025f\3\2\2\2\u025f\u0261\3\2\2\2\u0260"+
		"\u025e\3\2\2\2\u0261\u0262\7\16\2\2\u0262\u026c\5\34\17\2\u0263\u0264"+
		"\7\21\2\2\u0264\u0265\5\36\20\2\u0265\u0269\7\67\2\2\u0266\u0268\5\36"+
		"\20\2\u0267\u0266\3\2\2\2\u0268\u026b\3\2\2\2\u0269\u0267\3\2\2\2\u0269"+
		"\u026a\3\2\2\2\u026a\u026d\3\2\2\2\u026b\u0269\3\2\2\2\u026c\u0263\3\2"+
		"\2\2\u026c\u026d\3\2\2\2\u026d\u026e\3\2\2\2\u026e\u0273\7<\2\2\u026f"+
		"\u0272\5\64\33\2\u0270\u0272\5 \21\2\u0271\u026f\3\2\2\2\u0271\u0270\3"+
		"\2\2\2\u0272\u0275\3\2\2\2\u0273\u0271\3\2\2\2\u0273\u0274\3\2\2\2\u0274"+
		"\u0276\3\2\2\2\u0275\u0273\3\2\2\2\u0276\u0277\7=\2\2\u0277\u0278\b\21"+
		"\1\2\u0278!\3\2\2\2\u0279\u027e\7\25\2\2\u027a\u027e\5\n\6\2\u027b\u027e"+
		"\7\t\2\2\u027c\u027e\7\n\2\2\u027d\u0279\3\2\2\2\u027d\u027a\3\2\2\2\u027d"+
		"\u027b\3\2\2\2\u027d\u027c\3\2\2\2\u027e\u0281\3\2\2\2\u027f\u027d\3\2"+
		"\2\2\u027f\u0280\3\2\2\2\u0280\u0282\3\2\2\2\u0281\u027f\3\2\2\2\u0282"+
		"\u0283\7\17\2\2\u0283\u0286\5\34\17\2\u0284\u0285\7\21\2\2\u0285\u0287"+
		"\5\36\20\2\u0286\u0284\3\2\2\2\u0286\u0287\3\2\2\2\u0287\u0291\3\2\2\2"+
		"\u0288\u0289\7\22\2\2\u0289\u028e\5\36\20\2\u028a\u028b\7\67\2\2\u028b"+
		"\u028d\5\36\20\2\u028c\u028a\3\2\2\2\u028d\u0290\3\2\2\2\u028e\u028c\3"+
		"\2\2\2\u028e\u028f\3\2\2\2\u028f\u0292\3\2\2\2\u0290\u028e\3\2\2\2\u0291"+
		"\u0288\3\2\2\2\u0291\u0292\3\2\2\2\u0292\u0293\3\2\2\2\u0293\u029e\7<"+
		"\2\2\u0294\u029d\5\60\31\2\u0295\u029d\5&\24\2\u0296\u029d\5$\23\2\u0297"+
		"\u029d\5\62\32\2\u0298\u029d\5(\25\2\u0299\u029d\5\"\22\2\u029a\u029d"+
		"\5 \21\2\u029b\u029d\5*\26\2\u029c\u0294\3\2\2\2\u029c\u0295\3\2\2\2\u029c"+
		"\u0296\3\2\2\2\u029c\u0297\3\2\2\2\u029c\u0298\3\2\2\2\u029c\u0299\3\2"+
		"\2\2\u029c\u029a\3\2\2\2\u029c\u029b\3\2\2\2\u029d\u02a0\3\2\2\2\u029e"+
		"\u029c\3\2\2\2\u029e\u029f\3\2\2\2\u029f\u02a1\3\2\2\2\u02a0\u029e\3\2"+
		"\2\2\u02a1\u02a2\7=\2\2\u02a2\u02a3\b\22\1\2\u02a3#\3\2\2\2\u02a4\u02a6"+
		"\5\n\6\2\u02a5\u02a4\3\2\2\2\u02a5\u02a6\3\2\2\2\u02a6\u02a7\3\2\2\2\u02a7"+
		"\u02a8\7.\2\2\u02a8\u02b1\78\2\2\u02a9\u02ae\5<\37\2\u02aa\u02ab\7\67"+
		"\2\2\u02ab\u02ad\5<\37\2\u02ac\u02aa\3\2\2\2\u02ad\u02b0\3\2\2\2\u02ae"+
		"\u02ac\3\2\2\2\u02ae\u02af\3\2\2\2\u02af\u02b2\3\2\2\2\u02b0\u02ae\3\2"+
		"\2\2\u02b1\u02a9\3\2\2\2\u02b1\u02b2\3\2\2\2\u02b2\u02b3\3\2\2\2\u02b3"+
		"\u02b4\79\2\2\u02b4\u02b8\7<\2\2\u02b5\u02b7\5\30\r\2\u02b6\u02b5\3\2"+
		"\2\2\u02b7\u02ba\3\2\2\2\u02b8\u02b6\3\2\2\2\u02b8\u02b9\3\2\2\2\u02b9"+
		"\u02cf\3\2\2\2\u02ba\u02b8\3\2\2\2\u02bb\u02bc\7-\2\2\u02bc\u02c5\78\2"+
		"\2\u02bd\u02c2\5\16\b\2\u02be\u02bf\7\67\2\2\u02bf\u02c1\5\16\b\2\u02c0"+
		"\u02be\3\2\2\2\u02c1\u02c4\3\2\2\2\u02c2\u02c0\3\2\2\2\u02c2\u02c3\3\2"+
		"\2\2\u02c3\u02c6\3\2\2\2\u02c4\u02c2\3\2\2\2\u02c5\u02bd\3\2\2\2\u02c5"+
		"\u02c6\3\2\2\2\u02c6\u02c7\3\2\2\2\u02c7\u02c8\79\2\2\u02c8\u02cc\7\62"+
		"\2\2\u02c9\u02cb\5\30\r\2\u02ca\u02c9\3\2\2\2\u02cb\u02ce\3\2\2\2\u02cc"+
		"\u02ca\3\2\2\2\u02cc\u02cd\3\2\2\2\u02cd\u02d0\3\2\2\2\u02ce\u02cc\3\2"+
		"\2\2\u02cf\u02bb\3\2\2\2\u02cf\u02d0\3\2\2\2\u02d0\u02d1\3\2\2\2\u02d1"+
		"\u02d2\7=\2\2\u02d2\u02d3\b\23\1\2\u02d3%\3\2\2\2\u02d4\u02d6\5\n\6\2"+
		"\u02d5\u02d4\3\2\2\2\u02d5\u02d6\3\2\2\2\u02d6\u02d8\3\2\2\2\u02d7\u02d9"+
		"\7\f\2\2\u02d8\u02d7\3\2\2\2\u02d8\u02d9\3\2\2\2\u02d9\u02da\3\2\2\2\u02da"+
		"\u02db\5:\36\2\u02db\u02dc\5\34\17\2\u02dc\u02dd\7\62\2\2\u02dd\u02de"+
		"\b\24\1\2\u02de\u0303\3\2\2\2\u02df\u02e1\5\n\6\2\u02e0\u02df\3\2\2\2"+
		"\u02e0\u02e1\3\2\2\2\u02e1\u02e3\3\2\2\2\u02e2\u02e4\7\f\2\2\u02e3\u02e2"+
		"\3\2\2\2\u02e3\u02e4\3\2\2\2\u02e4\u02e5\3\2\2\2\u02e5\u02e6\5\34\17\2"+
		"\u02e6\u02e7\7\62\2\2\u02e7\u02e8\b\24\1\2\u02e8\u0303\3\2\2\2\u02e9\u02eb"+
		"\5\n\6\2\u02ea\u02e9\3\2\2\2\u02ea\u02eb\3\2\2\2\u02eb\u02ed\3\2\2\2\u02ec"+
		"\u02ee\7\f\2\2\u02ed\u02ec\3\2\2\2\u02ed\u02ee\3\2\2\2\u02ee\u02ef\3\2"+
		"\2\2\u02ef\u02f0\5:\36\2\u02f0\u02f1\5\34\17\2\u02f1\u02f2\7L\2\2\u02f2"+
		"\u02f3\5\16\b\2\u02f3\u02f4\7\62\2\2\u02f4\u02f5\b\24\1\2\u02f5\u0303"+
		"\3\2\2\2\u02f6\u02f8\5\n\6\2\u02f7\u02f6\3\2\2\2\u02f7\u02f8\3\2\2\2\u02f8"+
		"\u02fa\3\2\2\2\u02f9\u02fb\7\f\2\2\u02fa\u02f9\3\2\2\2\u02fa\u02fb\3\2"+
		"\2\2\u02fb\u02fc\3\2\2\2\u02fc\u02fd\5\34\17\2\u02fd\u02fe\7L\2\2\u02fe"+
		"\u02ff\5\16\b\2\u02ff\u0300\7\62\2\2\u0300\u0301\b\24\1\2\u0301\u0303"+
		"\3\2\2\2\u0302\u02d5\3\2\2\2\u0302\u02e0\3\2\2\2\u0302\u02ea\3\2\2\2\u0302"+
		"\u02f7\3\2\2\2\u0303\'\3\2\2\2\u0304\u0306\5\n\6\2\u0305\u0304\3\2\2\2"+
		"\u0305\u0306\3\2\2\2\u0306\u0307\3\2\2\2\u0307\u0309\7\24\2\2\u0308\u030a"+
		"\7\f\2\2\u0309\u0308\3\2\2\2\u0309\u030a\3\2\2\2\u030a\u030b\3\2\2\2\u030b"+
		"\u030c\5:\36\2\u030c\u030d\5\34\17\2\u030d\u030e\7\62\2\2\u030e\u030f"+
		"\b\25\1\2\u030f\u0337\3\2\2\2\u0310\u0312\5\n\6\2\u0311\u0310\3\2\2\2"+
		"\u0311\u0312\3\2\2\2\u0312\u0313\3\2\2\2\u0313\u0315\7\24\2\2\u0314\u0316"+
		"\7\f\2\2\u0315\u0314\3\2\2\2\u0315\u0316\3\2\2\2\u0316\u0317\3\2\2\2\u0317"+
		"\u0318\5\34\17\2\u0318\u0319\7\62\2\2\u0319\u031a\b\25\1\2\u031a\u0337"+
		"\3\2\2\2\u031b\u031d\5\n\6\2\u031c\u031b\3\2\2\2\u031c\u031d\3\2\2\2\u031d"+
		"\u031e\3\2\2\2\u031e\u0320\7\24\2\2\u031f\u0321\7\f\2\2\u0320\u031f\3"+
		"\2\2\2\u0320\u0321\3\2\2\2\u0321\u0322\3\2\2\2\u0322\u0323\5:\36\2\u0323"+
		"\u0324\5\34\17\2\u0324\u0325\7L\2\2\u0325\u0326\5\f\7\2\u0326\u0327\7"+
		"\62\2\2\u0327\u0328\b\25\1\2\u0328\u0337\3\2\2\2\u0329\u032b\5\n\6\2\u032a"+
		"\u0329\3\2\2\2\u032a\u032b\3\2\2\2\u032b\u032c\3\2\2\2\u032c\u032e\7\24"+
		"\2\2\u032d\u032f\7\f\2\2\u032e\u032d\3\2\2\2\u032e\u032f\3\2\2\2\u032f"+
		"\u0330\3\2\2\2\u0330\u0331\5\34\17\2\u0331\u0332\7L\2\2\u0332\u0333\5"+
		"\f\7\2\u0333\u0334\7\62\2\2\u0334\u0335\b\25\1\2\u0335\u0337\3\2\2\2\u0336"+
		"\u0305\3\2\2\2\u0336\u0311\3\2\2\2\u0336\u031c\3\2\2\2\u0336\u032a\3\2"+
		"\2\2\u0337)\3\2\2\2\u0338\u033a\5\n\6\2\u0339\u0338\3\2\2\2\u0339\u033a"+
		"\3\2\2\2\u033a\u033c\3\2\2\2\u033b\u033d\7\32\2\2\u033c\u033b\3\2\2\2"+
		"\u033c\u033d\3\2\2\2\u033d\u033f\3\2\2\2\u033e\u0340\7\31\2\2\u033f\u033e"+
		"\3\2\2\2\u033f\u0340\3\2\2\2\u0340\u0341\3\2\2\2\u0341\u0342\7\20\2\2"+
		"\u0342\u0353\5\34\17\2\u0343\u0344\7\21\2\2\u0344\u0351\5\34\17\2\u0345"+
		"\u034e\78\2\2\u0346\u034b\5\f\7\2\u0347\u0348\7\67\2\2\u0348\u034a\5\f"+
		"\7\2\u0349\u0347\3\2\2\2\u034a\u034d\3\2\2\2\u034b\u0349\3\2\2\2\u034b"+
		"\u034c\3\2\2\2\u034c\u034f\3\2\2\2\u034d\u034b\3\2\2\2\u034e\u0346\3\2"+
		"\2\2\u034e\u034f\3\2\2\2\u034f\u0350\3\2\2\2\u0350\u0352\79\2\2\u0351"+
		"\u0345\3\2\2\2\u0351\u0352\3\2\2\2\u0352\u0354\3\2\2\2\u0353\u0343\3\2"+
		"\2\2\u0353\u0354\3\2\2\2\u0354\u0355\3\2\2\2\u0355\u0359\7<\2\2\u0356"+
		"\u0358\5,\27\2\u0357\u0356\3\2\2\2\u0358\u035b\3\2\2\2\u0359\u0357\3\2"+
		"\2\2\u0359\u035a\3\2\2\2\u035a\u035c\3\2\2\2\u035b\u0359\3\2\2\2\u035c"+
		"\u035d\7=\2\2\u035d\u035e\b\26\1\2\u035e\u038e\3\2\2\2\u035f\u0361\5\n"+
		"\6\2\u0360\u035f\3\2\2\2\u0360\u0361\3\2\2\2\u0361\u0362\3\2\2\2\u0362"+
		"\u0363\7\31\2\2\u0363\u0364\7\20\2\2\u0364\u0365\5\34\17\2\u0365\u036e"+
		"\78\2\2\u0366\u036b\5<\37\2\u0367\u0368\7\67\2\2\u0368\u036a\5<\37\2\u0369"+
		"\u0367\3\2\2\2\u036a\u036d\3\2\2\2\u036b\u0369\3\2\2\2\u036b\u036c\3\2"+
		"\2\2\u036c\u036f\3\2\2\2\u036d\u036b\3\2\2\2\u036e\u0366\3\2\2\2\u036e"+
		"\u036f\3\2\2\2\u036f\u0370\3\2\2\2\u0370\u0381\79\2\2\u0371\u0372\7\21"+
		"\2\2\u0372\u037f\5\34\17\2\u0373\u037c\78\2\2\u0374\u0379\5\f\7\2\u0375"+
		"\u0376\7\67\2\2\u0376\u0378\5\f\7\2\u0377\u0375\3\2\2\2\u0378\u037b\3"+
		"\2\2\2\u0379\u0377\3\2\2\2\u0379\u037a\3\2\2\2\u037a\u037d\3\2\2\2\u037b"+
		"\u0379\3\2\2\2\u037c\u0374\3\2\2\2\u037c\u037d\3\2\2\2\u037d\u037e\3\2"+
		"\2\2\u037e\u0380\79\2\2\u037f\u0373\3\2\2\2\u037f\u0380\3\2\2\2\u0380"+
		"\u0382\3\2\2\2\u0381\u0371\3\2\2\2\u0381\u0382\3\2\2\2\u0382\u0383\3\2"+
		"\2\2\u0383\u0387\7<\2\2\u0384\u0386\5,\27\2\u0385\u0384\3\2\2\2\u0386"+
		"\u0389\3\2\2\2\u0387\u0385\3\2\2\2\u0387\u0388\3\2\2\2\u0388\u038a\3\2"+
		"\2\2\u0389\u0387\3\2\2\2\u038a\u038b\7=\2\2\u038b\u038c\b\26\1\2\u038c"+
		"\u038e\3\2\2\2\u038d\u0339\3\2\2\2\u038d\u0360\3\2\2\2\u038e+\3\2\2\2"+
		"\u038f\u0390\5\34\17\2\u0390\u0391\7L\2\2\u0391\u0392\5\f\7\2\u0392\u0393"+
		"\7\62\2\2\u0393\u0394\b\27\1\2\u0394-\3\2\2\2\u0395\u0396\7,\2\2\u0396"+
		"\u0397\5\34\17\2\u0397\u03a0\78\2\2\u0398\u0399\5<\37\2\u0399\u039d\7"+
		"\67\2\2\u039a\u039c\5<\37\2\u039b\u039a\3\2\2\2\u039c\u039f\3\2\2\2\u039d"+
		"\u039b\3\2\2\2\u039d\u039e\3\2\2\2\u039e\u03a1\3\2\2\2\u039f\u039d\3\2"+
		"\2\2\u03a0\u0398\3\2\2\2\u03a0\u03a1\3\2\2\2\u03a1\u03a2\3\2\2\2\u03a2"+
		"\u03a5\79\2\2\u03a3\u03a4\7\64\2\2\u03a4\u03a6\5:\36\2\u03a5\u03a3\3\2"+
		"\2\2\u03a5\u03a6\3\2\2\2\u03a6\u03a7\3\2\2\2\u03a7\u03ab\7<\2\2\u03a8"+
		"\u03aa\5\30\r\2\u03a9\u03a8\3\2\2\2\u03aa\u03ad\3\2\2\2\u03ab\u03a9\3"+
		"\2\2\2\u03ab\u03ac\3\2\2\2\u03ac\u03ae\3\2\2\2\u03ad\u03ab\3\2\2\2\u03ae"+
		"\u03af\7=\2\2\u03af\u03b0\b\30\1\2\u03b0/\3\2\2\2\u03b1\u03b6\7\25\2\2"+
		"\u03b2\u03b6\5\n\6\2\u03b3\u03b6\7\27\2\2\u03b4\u03b6\7\30\2\2\u03b5\u03b1"+
		"\3\2\2\2\u03b5\u03b2\3\2\2\2\u03b5\u03b3\3\2\2\2\u03b5\u03b4\3\2\2\2\u03b6"+
		"\u03b9\3\2\2\2\u03b7\u03b5\3\2\2\2\u03b7\u03b8\3\2\2\2\u03b8\u03ba\3\2"+
		"\2\2\u03b9\u03b7\3\2\2\2\u03ba\u03bb\7,\2\2\u03bb\u03bc\5\34\17\2\u03bc"+
		"\u03c5\78\2\2\u03bd\u03c2\5<\37\2\u03be\u03bf\7\67\2\2\u03bf\u03c1\5<"+
		"\37\2\u03c0\u03be\3\2\2\2\u03c1\u03c4\3\2\2\2\u03c2\u03c0\3\2\2\2\u03c2"+
		"\u03c3\3\2\2\2\u03c3\u03c6\3\2\2\2\u03c4\u03c2\3\2\2\2\u03c5\u03bd\3\2"+
		"\2\2\u03c5\u03c6\3\2\2\2\u03c6\u03c7\3\2\2\2\u03c7\u03ca\79\2\2\u03c8"+
		"\u03c9\7\64\2\2\u03c9\u03cb\5:\36\2\u03ca\u03c8\3\2\2\2\u03ca\u03cb\3"+
		"\2\2\2\u03cb\u03cc\3\2\2\2\u03cc\u03cd\5\32\16\2\u03cd\u03ce\b\31\1\2"+
		"\u03ce\61\3\2\2\2\u03cf\u03d1\5\n\6\2\u03d0\u03cf\3\2\2\2\u03d0\u03d1"+
		"\3\2\2\2\u03d1\u03d2\3\2\2\2\u03d2\u03d3\7\24\2\2\u03d3\u03d4\7,\2\2\u03d4"+
		"\u03d5\5\34\17\2\u03d5\u03de\78\2\2\u03d6\u03db\5<\37\2\u03d7\u03d8\7"+
		"\67\2\2\u03d8\u03da\5<\37\2\u03d9\u03d7\3\2\2\2\u03da\u03dd\3\2\2\2\u03db"+
		"\u03d9\3\2\2\2\u03db\u03dc\3\2\2\2\u03dc\u03df\3\2\2\2\u03dd\u03db\3\2"+
		"\2\2\u03de\u03d6\3\2\2\2\u03de\u03df\3\2\2\2\u03df\u03e0\3\2\2\2\u03e0"+
		"\u03e3\79\2\2\u03e1\u03e2\7\64\2\2\u03e2\u03e4\5:\36\2\u03e3\u03e1\3\2"+
		"\2\2\u03e3\u03e4\3\2\2\2\u03e4\u03e5\3\2\2\2\u03e5\u03e6\5\32\16\2\u03e6"+
		"\u03e7\b\32\1\2\u03e7\63\3\2\2\2\u03e8\u03e9\7,\2\2\u03e9\u03ea\5\34\17"+
		"\2\u03ea\u03f3\78\2\2\u03eb\u03f0\5<\37\2\u03ec\u03ed\7\67\2\2\u03ed\u03ef"+
		"\5<\37\2\u03ee\u03ec\3\2\2\2\u03ef\u03f2\3\2\2\2\u03f0\u03ee\3\2\2\2\u03f0"+
		"\u03f1\3\2\2\2\u03f1\u03f4\3\2\2\2\u03f2\u03f0\3\2\2\2\u03f3\u03eb\3\2"+
		"\2\2\u03f3\u03f4\3\2\2\2\u03f4\u03f5\3\2\2\2\u03f5\u03f8\79\2\2\u03f6"+
		"\u03f7\7\64\2\2\u03f7\u03f9\5:\36\2\u03f8\u03f6\3\2\2\2\u03f8\u03f9\3"+
		"\2\2\2\u03f9\u03fa\3\2\2\2\u03fa\u03fb\7\62\2\2\u03fb\u03fc\b\33\1\2\u03fc"+
		"\65\3\2\2\2\u03fd\u03fe\b\34\1\2\u03fe\u03ff\5\36\20\2\u03ff\u0400\b\34"+
		"\1\2\u0400\u040e\3\2\2\2\u0401\u0402\58\35\2\u0402\u0404\7@\2\2\u0403"+
		"\u0405\5\66\34\2\u0404\u0403\3\2\2\2\u0404\u0405\3\2\2\2\u0405\u0406\3"+
		"\2\2\2\u0406\u0407\b\34\1\2\u0407\u040e\3\2\2\2\u0408\u0409\78\2\2\u0409"+
		"\u040a\5\66\34\2\u040a\u040b\79\2\2\u040b\u040c\b\34\1\2\u040c\u040e\3"+
		"\2\2\2\u040d\u03fd\3\2\2\2\u040d\u0401\3\2\2\2\u040d\u0408\3\2\2\2\u040e"+
		"\u0418\3\2\2\2\u040f\u0410\f\6\2\2\u0410\u0411\7:\2\2\u0411\u0412\7;\2"+
		"\2\u0412\u0417\b\34\1\2\u0413\u0414\f\4\2\2\u0414\u0415\7A\2\2\u0415\u0417"+
		"\b\34\1\2\u0416\u040f\3\2\2\2\u0416\u0413\3\2\2\2\u0417\u041a\3\2\2\2"+
		"\u0418\u0416\3\2\2\2\u0418\u0419\3\2\2\2\u0419\67\3\2\2\2\u041a\u0418"+
		"\3\2\2\2\u041b\u041c\78\2\2\u041c\u041d\79\2\2\u041d\u042b\b\35\1\2\u041e"+
		"\u041f\78\2\2\u041f\u0424\5\66\34\2\u0420\u0421\7\67\2\2\u0421\u0423\5"+
		"\66\34\2\u0422\u0420\3\2\2\2\u0423\u0426\3\2\2\2\u0424\u0422\3\2\2\2\u0424"+
		"\u0425\3\2\2\2\u0425\u0427\3\2\2\2\u0426\u0424\3\2\2\2\u0427\u0428\79"+
		"\2\2\u0428\u0429\b\35\1\2\u0429\u042b\3\2\2\2\u042a\u041b\3\2\2\2\u042a"+
		"\u041e\3\2\2\2\u042b9\3\2\2\2\u042c\u042d\5\66\34\2\u042d\u042e\b\36\1"+
		"\2\u042e\u044b\3\2\2\2\u042f\u0430\7\23\2\2\u0430\u0431\7:\2\2\u0431\u0432"+
		"\5\66\34\2\u0432\u0433\7;\2\2\u0433\u0434\7>\2\2\u0434\u0435\5\66\34\2"+
		"\u0435\u0436\7?\2\2\u0436\u0437\b\36\1\2\u0437\u044b\3\2\2\2\u0438\u0439"+
		"\7\23\2\2\u0439\u043a\7>\2\2\u043a\u043b\5\66\34\2\u043b\u043c\7?\2\2"+
		"\u043c\u043d\b\36\1\2\u043d\u044b\3\2\2\2\u043e\u043f\7\23\2\2\u043f\u0440"+
		"\7:\2\2\u0440\u0441\5\66\34\2\u0441\u0442\7;\2\2\u0442\u0443\b\36\1\2"+
		"\u0443\u044b\3\2\2\2\u0444\u0445\7\23\2\2\u0445\u044b\b\36\1\2\u0446\u0447"+
		"\5\66\34\2\u0447\u0448\7B\2\2\u0448\u0449\b\36\1\2\u0449\u044b\3\2\2\2"+
		"\u044a\u042c\3\2\2\2\u044a\u042f\3\2\2\2\u044a\u0438\3\2\2\2\u044a\u043e"+
		"\3\2\2\2\u044a\u0444\3\2\2\2\u044a\u0446\3\2\2\2\u044b;\3\2\2\2\u044c"+
		"\u044d\5:\36\2\u044d\u044e\5\34\17\2\u044e\u044f\b\37\1\2\u044f\u0454"+
		"\3\2\2\2\u0450\u0451\5\34\17\2\u0451\u0452\b\37\1\2\u0452\u0454\3\2\2"+
		"\2\u0453\u044c\3\2\2\2\u0453\u0450\3\2\2\2\u0454=\3\2\2\2}AGIXdfv\u0093"+
		"\u0096\u0099\u00a3\u00a6\u00ad\u00b8\u00bb\u00cd\u00d0\u00e6\u00e9\u00f4"+
		"\u00f7\u0106\u0170\u0173\u0178\u017a\u019b\u01a1\u01a7\u01ad\u01b2\u01bd"+
		"\u01c8\u01d3\u01de\u0205\u020b\u0211\u0222\u0227\u0234\u0248\u024e\u025c"+
		"\u025e\u0269\u026c\u0271\u0273\u027d\u027f\u0286\u028e\u0291\u029c\u029e"+
		"\u02a5\u02ae\u02b1\u02b8\u02c2\u02c5\u02cc\u02cf\u02d5\u02d8\u02e0\u02e3"+
		"\u02ea\u02ed\u02f7\u02fa\u0302\u0305\u0309\u0311\u0315\u031c\u0320\u032a"+
		"\u032e\u0336\u0339\u033c\u033f\u034b\u034e\u0351\u0353\u0359\u0360\u036b"+
		"\u036e\u0379\u037c\u037f\u0381\u0387\u038d\u039d\u03a0\u03a5\u03ab\u03b5"+
		"\u03b7\u03c2\u03c5\u03ca\u03d0\u03db\u03de\u03e3\u03f0\u03f3\u03f8\u0404"+
		"\u040d\u0416\u0418\u0424\u042a\u044a\u0453";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}