// The Great Computer Language Shootout
// http://shootout.alioth.debian.org/
//
// contributed by David Hedbor
// modified by Sjoerd Visscher


$ir_obj_def_const(this, "ToggleContract", (function() {
   var c = $ir_contract_for("object");
   var cb = $ir_contract_for("boolean");
   c = $ir_contract_oblige_member(c, "start_state", cb);
   $ir_contract_freeze(c);
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


var n = 150000000; //arguments[0];
var i;

var toggle = new Toggle(true);
for (i = 0; i < 5; i++) {
  toggle.activate();
  print(toggle.value() ? "true" : "false");
}
for (i = 0; i < n; i++) {
  toggle = new Toggle(true);
}

print("");

var ntoggle = new NthToggle(true, 3);
for (i = 0; i < 8; i++) {
  ntoggle.activate();
  print((ntoggle.value()) ? "true" : "false");
}
for (i = 0; i < n; i++) {
   ntoggle = new NthToggle(true, 3);
}
