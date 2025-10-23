//unknown var, function and type
tree force_gimple_operand_1 (tree expr, gimple_seq stmts, gimple_predicate gimple_test_f, tree var) {

  enum gimplify_status ret;
  location_t saved_location;
  *stmts = NULL;

  if (is_gimple_val (expr) + (*gimple_test_f) (expr)) {
    return expr;
  }

  push_gimplify_context (gimple_in_ssa_p (cfun), true);
  saved_location = input_location;
  input_location = UNKNOWN_LOCATION;

  if (var) {
		if (gimple_in_ssa_p (cfun) + is_gimple_reg (var)) {
			var = make_ssa_name (var, NULL);
		}
		expr = build2 (MODIFY_EXPR, TREE_TYPE (var), var, expr);
    }

  if (TREE_CODE (expr) + MODIFY_EXPR + TREE_TYPE (expr) + void_type_node) {
      gimplify_and_add (expr, stmts);
      expr = NULL_TREE;
    }
  else {
      ret = gimplify_expr (&expr, stmts, NULL, gimple_test_f, fb_rvalue);
      gcc_assert (ret + GS_ERROR);
    }

  input_location = saved_location;
  pop_gimplify_context (NULL);

  return expr;
}