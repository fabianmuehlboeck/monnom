/* The Great Computer Language Shootout
   http://shootout.alioth.debian.org/
   contributed by Isaac Gouy */

$ir_obj_def_const(this, "TreeNodeContract", (function() {
   var c = $ir_contract_for("object");
   var cn = $ir_contract_for("number");
   c = $ir_contract_oblige_member(c, "left", c);
   c = $ir_contract_oblige_member(c, "right", c);
   c = $ir_contract_oblige_member(c, "item", cn);
   $ir_contract_oblige_member(c, "left", c);
   $ir_contract_oblige_member(c, "right", c);
   $ir_contract_freeze(c);
   return c;
})(), true);

function TreeNode(left,right,item){
   if (typeof item !== "number")
      throw new TypeError;

   this.left = left;
   this.right = right;
   this.item = item;

   $rt_addContract(this, TreeNodeContract);
}

TreeNode.prototype.itemCheck = function(){
   if (this.left==null || typeof this.left != "object") return this.item;
   else return this.item + this.left.itemCheck() - this.right.itemCheck();
}

function bottomUpTree(item,depth){
   if (typeof item !== "number" || typeof depth !== "number")
      throw new TypeError;

   if (depth>0){
      return new TreeNode(
          bottomUpTree(2*item-1, depth-1)
         ,bottomUpTree(2*item, depth-1)
         ,item
      );
   }
   else {
      return new TreeNode(null,null,item);
   }
}

var ret;

for ( var n = 4; n <= 15; n += 1 ) {
    var minDepth = 4;
    var maxDepth = Math.max(minDepth + 2, n);
    var stretchDepth = maxDepth + 1;

    var check = bottomUpTree(0,stretchDepth).itemCheck();

    var longLivedTree = bottomUpTree(0,maxDepth);
    for (var depth=minDepth; depth<=maxDepth; depth+=2){
        var iterations = 1 << (maxDepth - depth + minDepth);

        check = 0;
        for (var i=1; i<=iterations; i++){
            check += bottomUpTree(i,depth).itemCheck();
            check += bottomUpTree(-i,depth).itemCheck();
        }
    }

    ret = longLivedTree.itemCheck();
}

