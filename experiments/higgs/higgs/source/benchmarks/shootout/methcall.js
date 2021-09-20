// The Great Computer Language Shootout
// http://shootout.alioth.debian.org/
//
// contributed by David Hedbor
// modified by Sjoerd Visscher
// contracts added by Gregor Richards


$ir_obj_def_const(this, "ToggleContract", (function() {
   var c, c2;
   var co = $ir_contract_for("object");
   var cb = $ir_contract_for("boolean");
   var cf = $ir_contract_for("function");
   var cfb = $ir_contract_oblige_return(cf, cb);
   c = $ir_contract_oblige_member(co, "state", cb);
   c = $ir_contract_oblige_member(c, "value", cfb);
   c = $ir_contract_oblige_member(c, "activate", co);
   c2 = $ir_contract_oblige_return(cf, c);
   $ir_contract_oblige_member(c, "activate", c2);
   $ir_contract_freeze(c);
   $ir_contract_freeze(cfb);
   $ir_contract_freeze(c2);
   return c;
})(), true);

function Toggle(start_state) {
   if (typeof start_state !== "boolean" && typeof start_state !== "undefined")
      throw new TypeError;
   this.state = start_state;
}

Toggle.prototype.value = function() {
   $rt_addContract(this, ToggleContract);
   return this.state;
}

Toggle.prototype.activate = function() {
   $rt_addContract(this, ToggleContract);
   this.state = !this.state;
   return this;
}


$ir_obj_def_const(this, "NthToggleContract", (function() {
   var c = ToggleContract;
   var cn = $ir_contract_for("number");
   c = $ir_contract_oblige_member(c, "count_max", cn);
   c = $ir_contract_oblige_member(c, "count", cn);
   $ir_contract_freeze(c);
   return c;
})(), true);

function NthToggle (start_state, max_counter) {
   Toggle.call(this, start_state);
   if (typeof max_counter !== "number")
       throw new TypeError;
   this.count_max = max_counter;
   this.count = 0;
}

NthToggle.prototype = new Toggle;

NthToggle.prototype.activate = function() {
   $rt_addContract(this, NthToggleContract);
   if (++this.count >= this.count_max) {
     this.state = !this.state;
     this.count = 0;
   }
   return this;
}

var n = 500000000; //arguments[0];
var i;
var val = true;
var toggle = new Toggle(val);
for (i=0; i<n; i++) {
  val = toggle.activate().value();
}
print(toggle.value() ? "true" : "false");

val = true;
var ntoggle = new NthToggle(val, 3);
for (i=0; i<n; i++) {
  val = ntoggle.activate().value();
}
print(ntoggle.value() ? "true" : "false");
