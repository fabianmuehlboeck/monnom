// The Great Computer Language Shootout
// http://shootout.alioth.debian.org/
//
// contributed by David Hedbor
// modified by Isaac Gouy

var SIZE=30;

$ir_obj_def_const(this, "MContract", (function() {
  var c1 = $ir_contract_for("object");
  var cn = $ir_contract_for("number");
  c1 = $ir_contract_oblige_array(c1, cn);
  $ir_contract_freeze(c1);
  var c = $ir_contract_for("object");
  c = $ir_contract_oblige_array(c, c1);
  $ir_contract_freeze(c);
  return c;
}()), true);

function mkmatrix(rows, cols) {
  if (typeof rows !== "number" || typeof cols !== "number")
    throw new TypeError;
  var i, j, count = 1;
  var m = new Array(rows);
  for (i = 0; i < rows; i++) {
    m[i] = new Array(cols);
    for (j = 0; j < cols; j++) {
      m[i][j] = count++;
    }
  }
  return m;
}

function mmult(rows, cols,  m1, m2, m3) {
  if (typeof rows !== "number" || typeof cols !== "number")
    throw new TypeError;
  $rt_addContract(m1, MContract);
  $rt_addContract(m2, MContract);
  $rt_addContract(m3, MContract);
  var i, j, k, val;
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      val = 0;
      for (k = 0; k < cols; k++) {
	val += m1[i][k] * m2[k][j];
      }
      m3[i][j] = val;
    }
  }
  return m3;
}

var n = 120; //arguments[0];
var i;
var m1 = mkmatrix(SIZE, SIZE);
var m2 = mkmatrix(SIZE, SIZE);
var mm = mkmatrix(SIZE, SIZE);

for (i = 0; i < n; i++) {
  mmult(SIZE, SIZE, m1, m2, mm);
}
print(mm[0][0], mm[2][3], mm[3][2], mm[4][4]);
