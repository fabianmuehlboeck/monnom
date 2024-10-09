grammar NominalGradual;
options { tokenVocab = NominalGradualLexer; }
@header {
using System.Linq;
#pragma warning disable 3021
}

file[string filename] returns [CodeFile cf]
	: (us+=using)* (cdefs+=classdef | idefs+=interfacedef  | nss+=ns)* {$cf=new CodeFile($filename, $us.Select(u=>u.u),$idefs.Select(i=>i.idef),$cdefs.Select(c=>c.cdef),$nss.Select(n=>n.n));}
	;

using returns [RefQName u]
	: USING refqname SEMICOLON {$u=$refqname.q;}
	;


typeident returns [TypeIdentifier i]
	: id=ident (LANGLE (args+=typearg (COMMA args+=typearg)*)? rang=RANGLE)? {$i = new TypeIdentifier($id.i, $args.Select(a=>a.targ), $rang==null?$id.i.Locs:$id.i.Start.SpanTo($rang));}
	;

declident returns [DeclIdentifier i]
	: id=ident (LANGLE (args+=typeargspec (COMMA args+=typeargspec)*)? rang=RANGLE)? {$i = new DeclIdentifier($id.i, $args.Select(a=>a.tas), $rang==null?$id.i.Locs:$id.i.Start.SpanTo($rang));}
	;

refident returns [RefIdentifier i]
	: id=ident (LANGLE (args+=type (COMMA args+=type)*)? rang=RANGLE)? {$i = new RefIdentifier($id.i, $args.Select(a=>a.t), $rang==null?$id.i.Locs:$id.i.Start.SpanTo($rang));}
	;

typeqname returns [TypeQName q]
	: cc=COLONCOLON? (ids+=typeident DOT)* ids+=typeident {$q = new TypeQName($cc!=null, $ids.Select(i=>i.i), $ids.Select(i=>i.i).ToSourceSpan());}
	;

declqname returns [DeclQName q]
	: (ids+=declident DOT)* ids+=declident {$q = new DeclQName($ids.Select(i=>i.i), $ids.Select(i=>i.i).ToSourceSpan());}
	;

refqname returns [RefQName q]
	: cc=COLONCOLON? (ids+=refident DOT)* ids+=refident {$q = new RefQName($cc!=null, $ids.Select(i=>i.i), $ids.Select(i=>i.i).ToSourceSpan());}
	;

ns returns [Namespace n]
	: NAMESPACE qn=declqname LBRACE (cdefs+=classdef | idefs+=interfacedef | nss+=ns)* RBRACE {$n=new Namespace($qn.q, $idefs.Select(i=>i.idef),$cdefs.Select(c=>c.cdef),$nss.Select(n=>n.n), $NAMESPACE.ToSourceSpan($RBRACE));}
	;

visibility returns [VisibilityNode v]
	: PRIVATE {$v=new VisibilityNode(Visibility.Private, $PRIVATE.ToSourceSpan());}
	| PROTECTED {$v=new VisibilityNode(Visibility.Protected, $PROTECTED.ToSourceSpan());}
	| INTERNAL_PROT {$v=new VisibilityNode(Visibility.ProtectedInternal, $INTERNAL_PROT.ToSourceSpan());}
	| INTERNAL {$v=new VisibilityNode(Visibility.Internal, $INTERNAL.ToSourceSpan());}
	| PUBLIC {$v=new VisibilityNode(Visibility.Public, $PUBLIC.ToSourceSpan());}
	;

defaultexpr returns [IDefaultExpr exp]
	: STRING {$exp = new DefaultStringExpr($STRING);}
	 | INT {$exp = new DefaultIntExpr($INT);}
	 | MINUS INT {$exp = new DefaultIntExpr($INT,true);}
	 | FLOAT {$exp = new DefaultFloatExpr($FLOAT);}
	 | MINUS FLOAT {$exp = new DefaultFloatExpr($FLOAT,true);}
	 | TRUE {$exp = new DefaultBoolExpr($TRUE);}
	 | FALSE {$exp = new DefaultBoolExpr($FALSE);}
	 | NUL {$exp = new DefaultNullExpr($NUL);}
	 | ctype COLONCOLON ident (LBRACKET (args+=defaultexpr (COMMA args+=defaultexpr)* )? RBRACKET)? {$exp = new DefaultInstanceExpr($ctype.ct, $ident.i, $args.Select(a=>a.exp), $RBRACKET==null?$ctype.ct.Start.SpanTo($ident.i.End):$ctype.ct.Start.SpanTo($RBRACKET));}
	 | LBRACKET (elems+=defaultexpr (COMMA elems+=defaultexpr)* )? RBRACKET {$exp = new DefaultListExpr($elems.Select(e=>e.exp), $LBRACKET.ToSourceSpan($RBRACKET));}
	 | ident {$exp=new DefaultIdentExpr($ident.i);}
	 ;

expr returns [IExpr exp]
	 : NEW refqname LPAREN (args+=expr (COMMA args+=expr)*)? RPAREN {$exp=new NewExpr(new NewCall($refqname.q, $NEW.ToSourceSpan($refqname.q.End)),$args.Select(e=>e.exp), $NEW.ToSourceSpan($RPAREN));}
	 | nt=NEW structfielddecls LBRACE (md+=methdef)* sass=structassignments rb=RBRACE {$exp=new StructExpr($structfielddecls.sfds, $md.Select(x=>x.m), $sass.sass, $nt, $rb);}
	 | LPAREN dtype RPAREN e1=expr {$exp=new CastExpr($dtype.t,$e1.exp,$LPAREN.ToSourceSpan($e1.exp.End));}
	 | <assoc=left> r=expr DOT refident {$exp=new RefIdentExpr($r.exp, $refident.i);}
	 | e=expr LPAREN (args+=expr (COMMA args+=expr)*)? RPAREN {$exp=new CallExpr($e.exp, $args.Select(e=>e.exp), $e.exp.Start.SpanTo($RPAREN));}
	 | LPAREN e1=expr RPAREN {$exp=$e1.exp;}
	 | rangeexpr {$exp = $rangeexpr.rexp; }
	 | ctype COLONCOLON ident LPAREN (elems+=expr (COMMA elems+=expr)*)? RPAREN {$exp = new InstanceExpr($ctype.ct, $ident.i, $elems.Select(e=>e.exp), $ctype.ct.Start.SpanTo($RPAREN));}
	 | LBRACE (elems+=expr (COMMA elems+=expr)*)? RBRACE {$exp = new ListExpr($elems.Select(e=>e.exp), $LBRACE.ToSourceSpan($RBRACE));}
	 | (assembly+=ident? cc=COLONCOLON)? n=ident LANGLE (targs+=type (COMMA targs+=type)*)? RANGLE {$exp = new RefIdentRootExpr($assembly.Select(x=>x.i).SingleOrDefault(x=>true), $n.i, $targs.Select(t=>t.t), $cc, $RANGLE); }
	 | STRING {$exp = new StringExpr($STRING);}
	 | INT {$exp = new IntExpr($INT);}
	 | MINUS INT {$exp = new IntExpr($INT,true);}
	 | FLOAT {$exp = new FloatExpr($FLOAT);}
	 | MINUS FLOAT {$exp = new FloatExpr($FLOAT,true);}
	 | TRUE {$exp = new BoolExpr($TRUE);}
	 | FALSE {$exp = new BoolExpr($FALSE);}
	 | NUL {$exp = new NullExpr($NUL);}
	 | BANG e1=expr {$exp = new UnaryOpExpr($e1.exp, UnaryOperator.Not,$BANG.ToSourceSpan($e1.exp.End)); }
	 | ctype COLONCOLON ident (LBRACKET (args+=expr (COMMA args+=expr)* )? RBRACKET)? {$exp = new InstanceExpr($ctype.ct, $ident.i, $args.Select(a=>a.exp), $RBRACKET==null?$ctype.ct.Start.SpanTo($ident.i.End):$ctype.ct.Start.SpanTo($RBRACKET));}
	 | LBRACKET (elems+=expr (COMMA elems+=expr)* )? RBRACKET {$exp = new ListExpr($elems.Select(e=>e.exp), $LBRACKET.ToSourceSpan($RBRACKET));}
	 | ident {$exp=new IdentExpr($ident.i);}
	 |<assoc=left> LET structfielddecl IN expr {$exp = new LetExpr($structfielddecl.sfd, $expr.exp, $LET.ToSourceSpan($expr.exp.End)); } 
	 |<assoc=left> LETVAR ident EQ e1=expr IN e2=expr {$exp = new LetVarExpr($ident.i, $e1.exp, $e2.exp, $LETVAR.ToSourceSpan($e2.exp.End)); } 
	 |<assoc=left> argdecl BIGARROW lambdabody (COLON rt+=dtype)? {$exp = new LambdaExpr($argdecl.v.Singleton(), $lambdabody.b, $rt.Select(x=>x.t).SingleOrDefault(), $argdecl.v.Start); }
	 |<assoc=left> LPAREN (argds+=argdecl (COMMA argds+=argdecl)*)? RPAREN BIGARROW lambdabody (COLON rt+=dtype)? {$exp = new LambdaExpr($argds.Select(a=>a.v), $lambdabody.b, $rt.Select(x=>x.t).SingleOrDefault(), $LPAREN.ToSourcePos()); }
	 |<assoc=left> MINUS e1=expr {$exp = new UnaryOpExpr($e1.exp, UnaryOperator.Negate, $MINUS.ToSourceSpan($e1.exp.End)); }
	 |<assoc=left> e1=expr POW e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.Power,$POW.ToSourceSpan()),$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=left> e1=expr divmultop e2=expr {$exp = new BinOpExpr($e1.exp,$divmultop.op,$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=left> e1=expr PERCENT e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.Mod,$PERCENT.ToSourceSpan()),$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=left> e1=expr addsubop e2=expr {$exp = new BinOpExpr($e1.exp,$addsubop.op,$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=left> e1=expr BITAND e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.BitAND,$BITAND.ToSourceSpan()),$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=left> e1=expr BITXOR e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.BitXOR,$BITXOR.ToSourceSpan()),$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=left> e1=expr BITOR e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.BitOR,$BITOR.ToSourceSpan()),$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=left> e1=expr APPEND e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.Concat,$APPEND.ToSourceSpan()),$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=left> e1=expr LANGLE e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.LessThan,$LANGLE.ToSourceSpan()),$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=left> e1=expr RANGLE e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.GreaterThan,$RANGLE.ToSourceSpan()),$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=right> e1=expr LEQ e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.LessOrEqualTo,$LEQ.ToSourceSpan()),$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=right> e1=expr GEQ e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.GreaterOrEqualTo,$GEQ.ToSourceSpan()),$e2.exp,$e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=right> e1=expr EQEQ e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.RefEquals, $EQEQ.ToSourceSpan()), $e2.exp);}
	 |<assoc=right> e1=expr NEQ e2=expr {$exp = new UnaryOpExpr(new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.RefEquals, $NEQ.ToSourceSpan()), $e2.exp), UnaryOperator.Not, $e1.exp.Start.SpanTo($e2.exp.End));}
	 |<assoc=right> e1=expr AND e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.And, $AND.ToSourceSpan()),$e2.exp);}
	 |<assoc=right> e1=expr OR e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.Or, $OR.ToSourceSpan()),$e2.exp);}
	 |<assoc=right> e1=expr EQEQEQ e2=expr {$exp = new BinOpExpr($e1.exp,new BinaryOperatorNode(BinaryOperator.Equals, $EQEQEQ.ToSourceSpan()),$e2.exp);}
	 | CFUN LBRACKET (STRING COLONCOLON)? declident args=typelist COLON dtype RBRACKET (LANGLE (targs+=type (COMMA targs+=type)*)? RANGLE)? LPAREN (args+=expr (COMMA args+=expr)*)? RPAREN { $exp = new CFunctionCallExpr($STRING, $declident.i, $typelist.ts, $dtype.t, $targs?.Select(t=>t.t), $args.Select(e=>e.exp), $CFUN.ToSourceSpan($RPAREN)); }
	 ;

lambdabody returns [Block b]
	: expr { $b = new Block(new ReturnStmt($expr.exp,$expr.exp.Locs).Singleton(), $expr.exp.Locs); }
	| block { $b=$block.b; }
	;


rangeexpr returns [RangeExpr rexp]
	: LBRACKET DOTDOT e1=expr RBRACKET {$rexp=new RangeExpr($e1.exp,null,null,$LBRACKET.ToSourceSpan($RBRACKET));}
	| LBRACKET e2=expr DOTDOT e1=expr RBRACKET {$rexp=new RangeExpr($e1.exp,$e2.exp,null,$LBRACKET.ToSourceSpan($RBRACKET));}
	| LBRACKET DOTDOT e1=expr COLON e3=expr RBRACKET {$rexp=new RangeExpr($e1.exp,null,$e3.exp,$LBRACKET.ToSourceSpan($RBRACKET));}
	| LBRACKET e2=expr DOTDOT e1=expr COLON e3=expr RBRACKET {$rexp=new RangeExpr($e1.exp,$e2.exp,$e3.exp,$LBRACKET.ToSourceSpan($RBRACKET));}
	;

divmultop returns [BinaryOperatorNode op]
	: TIMES {$op=new BinaryOperatorNode(BinaryOperator.Multiply, $TIMES.ToSourceSpan());}
	| SLASH {$op=new BinaryOperatorNode(BinaryOperator.Divide, $SLASH.ToSourceSpan());}
	;
	
addsubop returns [BinaryOperatorNode op]
	: PLUS {$op=new BinaryOperatorNode(BinaryOperator.Add, $PLUS.ToSourceSpan());}
	| MINUS {$op=new BinaryOperatorNode(BinaryOperator.Subtract, $MINUS.ToSourceSpan());}
	;
	

stmt returns [IStmt s]
	: (ae+=expr DOT)? ident EQ e=expr SEMICOLON  {$s = new AssignStmt($ae.SingleOrDefault()?.exp, $ident.i,$e.exp,$ident.i.Start.SpanTo($SEMICOLON));}
	| (ae+=expr DOT)? ident PLUSEQ e=expr SEMICOLON  {$s = new AssignStmt($ae.SingleOrDefault()?.exp, $ident.i,new BinOpExpr(new IdentExpr($ident.i, $ae.SingleOrDefault()?.exp),new BinaryOperatorNode(BinaryOperator.Add, $PLUSEQ.ToSourceSpan()),$e.exp),$ident.i.Start.SpanTo($SEMICOLON));}
	| (ae+=expr DOT)? ident MINUSEQ e=expr SEMICOLON  {$s = new AssignStmt($ae.SingleOrDefault()?.exp, $ident.i,new BinOpExpr(new IdentExpr($ident.i, $ae.SingleOrDefault()?.exp),new BinaryOperatorNode(BinaryOperator.Subtract, $MINUSEQ.ToSourceSpan()),$e.exp),$ident.i.Start.SpanTo($SEMICOLON));}
	| (ae+=expr DOT)? ident TIMESEQ e=expr SEMICOLON  {$s = new AssignStmt($ae.SingleOrDefault()?.exp, $ident.i,new BinOpExpr(new IdentExpr($ident.i, $ae.SingleOrDefault()?.exp),new BinaryOperatorNode(BinaryOperator.Multiply, $TIMESEQ.ToSourceSpan()),$e.exp),$ident.i.Start.SpanTo($SEMICOLON));}
	| (ae+=expr DOT)? ident DIVEQ e=expr SEMICOLON  {$s = new AssignStmt($ae.SingleOrDefault()?.exp, $ident.i,new BinOpExpr(new IdentExpr($ident.i, $ae.SingleOrDefault()?.exp),new BinaryOperatorNode(BinaryOperator.Divide, $DIVEQ.ToSourceSpan()),$e.exp),$ident.i.Start.SpanTo($SEMICOLON));}
	| dtype ident EQ expr SEMICOLON  {$s = new DeclStmt($dtype.t, $ident.i, $expr.exp,$dtype.t.Start.SpanTo($SEMICOLON));}
	| dtype ident SEMICOLON  {$s = new DeclStmt($dtype.t, $ident.i, null,$dtype.t.Start.SpanTo($SEMICOLON));}
	| expr SEMICOLON {$s = new ExprStmt($expr.exp,$expr.exp.Start.SpanTo($SEMICOLON));}
	| DBG LPAREN STRING RPAREN SEMICOLON {$s = new DebugStmt($STRING,$DBG.ToSourceSpan($SEMICOLON));}
	| ERR LPAREN expr RPAREN SEMICOLON {$s = new ErrorStmt($expr.exp,$ERR.ToSourceSpan($SEMICOLON));}
	| RUNTIMECMD LPAREN STRING RPAREN SEMICOLON { $s = new RTCmdStmt($STRING, $RUNTIMECMD.ToSourceSpan($SEMICOLON)); }
	| BREAK (INT)? SEMICOLON {$s = new BreakStmt($INT,$BREAK.ToSourceSpan($SEMICOLON));}
	| CONTINUE (INT)? SEMICOLON {$s = new ContinueStmt($INT,$CONTINUE.ToSourceSpan($SEMICOLON));}
	| RETURN r+=expr? SEMICOLON {$s = new ReturnStmt($r.SingleOrDefault()?.exp,$RETURN.ToSourceSpan($SEMICOLON));}
	| IF LPAREN e=expr RPAREN then=block (ELSEIF LPAREN ee+=expr RPAREN elseifs+=block)* (ELSE elseb+=block)? {$s=new IfStmt($e.exp,$then.b,$ee.Select(e=>e.exp),$elseifs.Select(b=>b.b),$elseb.Select(b=>b.b),$IF.ToSourceSpan(($elseb.Count()>0?$elseb.Single().b.End:($elseifs.Count()>0?$elseifs.Last().b.End:$then.b.End))));}
	| IFNULL LPAREN ident RPAREN then=block (ELSE elseb+=block)? {$s=new IfNullStmt($ident.i,$then.b, $elseb.Select(b=>b.b).SingleOrDefault(), $IFNULL.ToSourceSpan($elseb.Count()==0?$then.b.End:$elseb.Single().b.End));}
	| IFOBJ LPAREN ident RPAREN then=block (ELSE elseb+=block)? {$s=new IfObjStmt($ident.i,$then.b, $elseb.Select(b=>b.b).SingleOrDefault(), $IFOBJ.ToSourceSpan($elseb.Count()==0?$then.b.End:$elseb.Single().b.End));}
	| IF LPAREN ident COLON type RPAREN then=block (ELSE elseb+=block)? {$s=new IfTypeStmt($ident.i,$type.t,$then.b, $elseb.Select(b=>b.b).SingleOrDefault(), $start.ToSourceSpan($elseb.Count()>0?$elseb.Single().b.End:$then.b.End));}
	| WHILE LPAREN e=expr RPAREN b=block {$s=new WhileStmt($e.exp,$b.b, $start.ToSourceSpan($b.b.End));}
	| FOREACH LPAREN i=argdecl IN e=expr RPAREN block {$s=new ForeachStmt($argdecl.v,$e.exp,$block.b,$start.ToSourceSpan($block.b.End));}
	;

block returns [Block b]
	: LBRACE ss+=stmt* RBRACE {$b=new Block($ss.Select(s=>s.s),$LBRACE.ToSourceSpan($RBRACE));}
	;

ident returns [Identifier i, IToken tok]
	: ID {$i=new Identifier($ID); $tok=$ID;}
	;

typearg returns [IType targ]
	: t=type {$targ = $t.t;}
	;

typeident2 returns [TypeIdentifier i]
	: id=ident (LANGLE (args+=typearg (COMMA args+=typearg)*)? rang=RANGLE) {$i = new TypeIdentifier($id.i, $args.Select(a=>a.targ), $rang==null?$id.i.Locs:$id.i.Start.SpanTo($rang));}
	;

typeqname2 returns [TypeQName q]
	: cc=COLONCOLON? (ids+=typeident2 DOT)* ids+=typeident2 {$q = new TypeQName($cc!=null, $ids.Select(i=>i.i), $ids.Select(i=>i.i).ToSourceSpan());}
	;

ctype returns [ClassType ct]
	: qn=typeqname {$ct=ClassType.GetInstance($qn.q);} //{if(BaseType.BaseNames.Contains($ID.text)) {$ct=new BaseType($ID.text,$ID);} else {$ct=ClassType.GetInstance($ID);}}
	;
	
ctype2 returns [ClassType ct]
	: qn=typeqname2 {$ct=ClassType.GetInstance($qn.q);} //{if(BaseType.BaseNames.Contains($ID.text)) {$ct=new BaseType($ID.text,$ID);} else {$ct=ClassType.GetInstance($ID);}}
	;


typeargspec returns [TypeArgDecl tas]
	: ident {$tas = new TypeArgDecl($ident.i); }
	| ident EXTENDS ctype2 {$tas = new TypeArgDecl($ident.i, $ctype2.ct);}
	;

inheritancedecl returns [InheritanceDecl inhdecl]
	: refqname
	  {
		$inhdecl = new InheritanceDecl($refqname.q);
	  }
	 ;

interfacedef returns [InterfaceDef idef]
	: (v+=visibility | p+=PARTIAL | m+=MATERIAL | s+=SHAPE)* INTERFACE n=declident  (EXTENDS si+=inheritancedecl (COMMA si+=inheritancedecl)*)? LBRACE (md+=methdecl | ifdef+=interfacedef)* RBRACE 
	  {
	    if($v.Count()>1)
		{
			throw new ParseException("An interface can have at most one visibility modifier!", new SourceLocs($v.Select(vn=>vn.v.Locs)));
		}
	    if($p.Count()>1)
		{
			throw new ParseException("An interface can have at most one partiality marker!", new SourceLocs($p.Select(pp=>pp.ToSourceSpan())));
		}
		if($m.Count()+$s.Count()>1)
		{
			throw new ParseException("An interface can have at most one material/shape marker!", new SourceLocs($m.Select(pp=>pp.ToSourceSpan()).Concat($s.Select(pp=>pp.ToSourceSpan()))));
		}
		$idef=new InterfaceDef($n.i, $si.Select(ihd=>ihd.inhdecl),$md.Select(m=>m.m),$v.SingleOrDefault()?.v, $p.Count()==1, $s.Count()==1, $m.Count()==1, $ifdef.Select(ii=>ii.idef), $start.ToSourceSpan($RBRACE));
	  }
	;

classdef returns [ClassDef cdef]
	: (f+=FINAL | v+=visibility | a+=ABSTRACT | p+=PARTIAL | m+=MATERIAL | s+=SHAPE)* CLASS n=declident (EXTENDS sc+=inheritancedecl)? (IMPLEMENTS si+=inheritancedecl (COMMA si+=inheritancedecl)*)? LBRACE (md+=methdef | fd+=fielddecl | c+=constructor | smd+=staticmethdef | sfd+=staticfielddecl | cd+=classdef | ifdef+=interfacedef | idecls+=instancedecl)* RBRACE 
	{
		if($f.Count()>1)
		{
			throw new ParseException("A class definition can have at most one finality marker!", new SourceLocs($f.Select(ff=>ff.ToSourceSpan())));
		}
		if($v.Count()>1)
		{
			throw new ParseException("A class definition can have at most one visibility modifier!", new SourceLocs($v.Select(vn=>vn.v.Locs)));
		}
		if($a.Count()>1)
		{
			throw new ParseException("A class definition can have at most one abstractness marker!", new SourceLocs($a.Select(aa=>aa.ToSourceSpan())));
		}
		if($p.Count()>1)
		{
			throw new ParseException("A class definition can have at most one partiality marker!", new SourceLocs($p.Select(pp=>pp.ToSourceSpan())));
		}
		if($m.Count()+$s.Count()>1)
		{
			throw new ParseException("An interface can have at most one material/shape marker!", new SourceLocs($m.Select(pp=>pp.ToSourceSpan()).Concat($s.Select(pp=>pp.ToSourceSpan()))));
		}
		$cdef=new ClassDef($n.i, $sc.Select(x=>x.inhdecl).SingleOrDefault().InjectOptional(),$si.Select(x=>x.inhdecl),$md.Select(m=>m.m),$fd.Select(f=>f.f),$c.Select(c=>c.c),$sfd.Select(f=>f.f),$smd.Select(m=>m.m),$idecls.Select(idecl=>idecl.i),$f.Count()==1, $v.SingleOrDefault()?.v, $a.Count()==1, $p.Count()==1, $s.Count()==1, $m.Count()==1, $ifdef.Select(ii=>ii.idef), $cd.Select(cc=>cc.cdef), $CLASS.ToSourceSpan($RBRACE));
	}
	;

constructor returns [Constructor c]
	: (v+=visibility)? CONSTRUCT LPAREN (args+=argdecl (COMMA args+=argdecl)*)? RPAREN LBRACE preps+=stmt* (SUPER LPAREN (exprs+=expr (COMMA exprs+=expr)*)? RPAREN SEMICOLON afters+=stmt*)? RBRACE {$c=new Constructor($v.SingleOrDefault()?.v??VisibilityNode.Private, $args.Select(a=>a.v),new Block($preps.Select(s=>s.s), $LBRACE.ToSourceSpan($SUPER??$RBRACE)),$exprs.Select(e=>e.exp),new Block($afters.Select(s=>s.s), ($RPAREN??$RBRACE).ToSourceSpan($RBRACE)),$CONSTRUCT.ToSourceSpan($RBRACE));}
	;


structassignment returns [StructAssignment s]
	: i=ident EQ e=expr { $s = new StructAssignment($i.i, $e.exp); }
	;

structassignments returns [IEnumerable<StructAssignment> sass]
	: { $sass = new List<StructAssignment>(); }
	| sasss+=structassignment (COMMA sasss+=structassignment)* {$sass = $sasss.Select(s=>s.s); }
	;

structfielddecls returns [IEnumerable<StructFieldDecl> sfds]
	: { $sfds = new List<StructFieldDecl>(); }
	| LPAREN (sfdss+=structfielddecl (COMMA sfdss+=structfielddecl)*)? RPAREN {$sfds = $sfdss.Select(s=>s.sfd); }
	;

structfielddecl returns [StructFieldDecl sfd]
	: dtype ident EQ expr {$sfd = new StructFieldDecl($dtype.t, $ident.i, $expr.exp);}
	| ident EQ expr {$sfd = new StructFieldDecl(null, $ident.i, $expr.exp);}
	;

fielddecl returns [FieldDecl f]
	: (v+=visibility)? (r=READONLY)? dtype ident SEMICOLON {$f=new FieldDecl($ident.i,$dtype.t, $v.SingleOrDefault()?.v, $r!=null,$dtype.t.Start.SpanTo($SEMICOLON));}
	| (v+=visibility)? (r=READONLY)? ident SEMICOLON {$f=new FieldDecl($ident.i, $v.SingleOrDefault()?.v, $r!=null,$ident.i.Start.SpanTo($SEMICOLON));}
	| (v+=visibility)? (r=READONLY)? dtype ident EQ expr SEMICOLON {$f=new FieldDecl($ident.i,$dtype.t,$expr.exp, $v.SingleOrDefault()?.v, $r!=null,$dtype.t.Start.SpanTo($SEMICOLON));}
	| (v+=visibility)? (r=READONLY)? ident EQ expr SEMICOLON {$f=new FieldDecl($ident.i,$expr.exp, $v.SingleOrDefault()?.v, $r!=null,$ident.i.Start.SpanTo($SEMICOLON));}
	;

staticfielddecl returns [StaticFieldDecl f]
	: (v+=visibility)? STATIC (r=READONLY)? dtype ident SEMICOLON {$f=new StaticFieldDecl($ident.i,$dtype.t, $v.SingleOrDefault()?.v, $r!=null,$STATIC.ToSourceSpan($SEMICOLON));}
	| (v+=visibility)? STATIC (r=READONLY)? ident SEMICOLON {$f=new StaticFieldDecl($ident.i, $v.SingleOrDefault()?.v, $r!=null, $STATIC.ToSourceSpan($SEMICOLON));}
	| (v+=visibility)? STATIC (r=READONLY)? dtype ident EQ defaultexpr SEMICOLON {$f=new StaticFieldDecl($ident.i,$dtype.t,$defaultexpr.exp, $v.SingleOrDefault()?.v, $r!=null,$STATIC.ToSourceSpan($SEMICOLON));}
	| (v+=visibility)? STATIC (r=READONLY)? ident EQ defaultexpr SEMICOLON {$f=new StaticFieldDecl($ident.i,$defaultexpr.exp, $v.SingleOrDefault()?.v, $r!=null,$STATIC.ToSourceSpan($SEMICOLON));}
	;

instancedecl returns [InstanceDef i]
	: (v+=visibility)? (d=DEFAULT)? (m=MULTI)? INSTANCE ident (EXTENDS sid+=ident (LPAREN (sargs+=defaultexpr (COMMA sargs+=defaultexpr)*)? RPAREN)?)? LBRACE ifas+=instanceassignment* RBRACE {$i=new InstanceDef($v.SingleOrDefault()?.v??VisibilityNode.Private, $ident.i, $sid.Select(s=>s.i).SingleOrDefault(), $ifas.Select(fa=>fa.ifa), $m!=null, $d!=null, $sargs.Select(s=>s.exp), null, $INSTANCE.ToSourceSpan($RBRACE));}
	| (v+=visibility)? m=MULTI INSTANCE ident LPAREN (iargs+=argdecl (COMMA iargs+=argdecl)*)? RPAREN (EXTENDS sid+=ident (LPAREN (sargs+=defaultexpr (COMMA sargs+=defaultexpr)*)? RPAREN)?)? LBRACE ifas+=instanceassignment* RBRACE {$i=new InstanceDef($v.SingleOrDefault()?.v??VisibilityNode.Private, $ident.i, $sid.Select(s=>s.i).SingleOrDefault(), $ifas.Select(fa=>fa.ifa), $m!=null, false, $sargs.Select(s=>s.exp), $iargs.Select(a=>a.v.ToArgDecl()), $INSTANCE.ToSourceSpan($RBRACE));}
	;

instanceassignment returns [InstanceFieldAssignment ifa]
	: ident EQ defaultexpr SEMICOLON {$ifa = new InstanceFieldAssignment($ident.i, $defaultexpr.exp, $ident.i.Start.SpanTo($SEMICOLON));}
	;

fundef returns [FunDef f]
	: FUN ident LPAREN (args+=argdecl (COMMA args+=argdecl*))? RPAREN (COLON rets+=dtype)? LBRACE ss+=stmt* RBRACE {$f=new FunDef($ident.i,$args.Select(a=>a.v),$rets.Select(t=>t.t),$ss.Select(s=>s.s),$FUN.ToSourceSpan($RBRACE));}
	;

methdef returns [MethodDef m]
	: (f+=FINAL | v+=visibility | virt+=VIRTUAL | o+=OVERRIDE | ct+=CALLTARGET)* FUN declident LPAREN (args+=argdecl (COMMA args+=argdecl)*)? RPAREN (COLON rets+=dtype)? block
	{
		if($f.Count()>1)
		{
			throw new ParseException("A method definition can have at most one finality marker!", new SourceLocs($f.Select(f=>f.ToSourceSpan())));
		}
		if($v.Count()>1)
		{
			throw new ParseException("A method definition can have at most one visibility modifier!", new SourceLocs($v.Select(vn=>vn.v.Locs)));
		}
		if($virt.Count()>1)
		{
			throw new ParseException("A method definition can have at most one virtual marker!", new SourceLocs($virt.Select(v=>v.ToSourceSpan())));
		}
		if($o.Count()>1)
		{
			throw new ParseException("A method definition can have at most one override marker!", new SourceLocs($o.Select(o=>o.ToSourceSpan())));
		}
		if($ct.Count()>1)
		{
			throw new ParseException("A method definition can have at most one calltarget marker!", new SourceLocs($ct.Select(ct=>ct.ToSourceSpan())));
		}
		$m=new MethodDef($f.Count()>0, $virt.Count()>0, $o.Count()>0, $ct.Count()>0,  $v.Select(v=>v.v).SingleOrDefault(), $declident.i, $args.Select(a=>a.v), $rets.Select(r=>r.t).SingleOrDefault()??new DynamicType($RPAREN.ToSourceSpan()), $block.b, $FUN.ToSourceSpan($block.b.End));
	}
	;

staticmethdef returns [StaticMethodDef m]
	: (v+=visibility)? STATIC FUN declident LPAREN (args+=argdecl (COMMA args+=argdecl)*)? RPAREN (COLON rets+=type)? block {$m=new StaticMethodDef($v.SingleOrDefault().v??VisibilityNode.Private, $declident.i, $args.Select(a=>a.v), $rets.Select(r=>r.t).SingleOrDefault()??new DynamicType($RPAREN.ToSourceSpan()), $block.b, $FUN.ToSourceSpan($block.b.End));}
	;

methdecl returns [MethodDecl m]
	: (v+=visibility)? (ct+=CALLTARGET)? FUN declident LPAREN (args+=argdecl (COMMA args+=argdecl)*)? RPAREN (COLON rets+=type)? SEMICOLON {$m=new MethodDecl(VisibilityNode.Public, $ct.Count()>0, $declident.i, $args.Select(a=>a.v), $rets.Select(r=>r.t).SingleOrDefault()??new DynamicType($RPAREN.ToSourceSpan()),$FUN.ToSourceSpan($SEMICOLON));}
	;


type returns [IType t]
	: ctype {$t = $ctype.ct;}
	| DYN { $t = new DynamicType($DYN.ToSourceSpan()); }
	| tq=type LBRACKET RBRACKET {$t=new ArrayType($tq.t,$tq.t.Start.SpanTo($RBRACKET));}
	| tq=type BANG {$t = new ProbablyType($tq.t, $tq.t.Start.SpanTo($BANG)); }
	| tq=type QMARK {$t=new MaybeType($tq.t,$tq.t.Start.SpanTo($QMARK));}
	| LPAREN tq=type RPAREN {$t=$tq.t;}
	;

typelist returns [IEnumerable<IType> ts]
	: LPAREN RPAREN {$ts=new List<IType>();}
	| LPAREN elems+=type (COMMA elems+=type)* RPAREN {$ts=$elems.Select(a=>a.t);}
	;

dtype returns [IType t]
	: type {$t=$type.t;}
	;

argdecl returns [VarDecl v]
	: dtype ident {$v=new VarDecl($ident.i,$dtype.t,$dtype.t.Start.SpanTo($ident.i.End));}
	| ident {$v=new VarDecl($ident.i,new DynamicType($ident.i.Locs));}
	;

