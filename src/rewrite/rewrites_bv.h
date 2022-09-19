#ifndef BZLA_REWRITE_REWRITES_BV_H_INCLUDED
#define BZLA_REWRITE_REWRITES_BV_H_INCLUDED

#include "rewrite/rewriter.h"

namespace bzla {

/* bvadd -------------------------------------------------------------------- */

// const_binary_bv_exp
// special_const_lhs_binary_exp
// special_const_rhs_binary_exp
// bool_add
// mult_add
// not_add
// bcond_add
// urem_add
// neg_add
// zero_add
// const_lhs_add
// const_rhs_add
// const_neg_lhs_add
// const_neg_rhs_add
// push_ite_add
// sll_add
// mul_add

/* bvand -------------------------------------------------------------------- */

// const_binary_bv_exp
template <>
Node RewriteRule<RewriteRuleKind::BV_AND_EVAL>::_apply(Rewriter& rewriter,
                                                       const Node& node);
// special_const_lhs_binary_exp
// special_const_rhs_binary_exp
// idem1_and
// contr1_and
// contr2_and
// idem2_and
// comm_and
// bool_xnor_and
// resol1_and
// resol2_and
// lt_false_and
// lt_and
// contr_rec_and
// subsum1_and
// subst1_and
// subst2_and
// subsum2_and
// subst3_and
// subst4_and
// contr3_and
// idem3_and
// const1_and
// const2_and
// concat_and

/* bvashr ------------------------------------------------------------------- */

/* bvconcat ----------------------------------------------------------------- */

// const_binary_bv_exp
// special_const_lhs_binary_exp
// special_const_rhs_binary_exp
// const_concat
// slice_concat
// and_lhs_concat
// and_rhs_concat

/* bvextract ---------------------------------------------------------------- */

// full_slice
// const_slice
// slice_slice
// concat_lower_slice
// concat_upper_slice
// concat_rec_upper_slice
// concat_rec_lower_slice
// concat_rec_slice
// and_slice
// bcond_slice
// zero_lower_slice

/* bvmul -------------------------------------------------------------------- */

// const_binary_bv_exp
// special_const_lhs_binary_exp
// special_const_rhs_binary_exp
// bool_mul
#if 0
// this increases mul nodes in the general case
// bcond_mul
#endif
// const_lhs_mul
// const_rhs_mul
// const_mul
// push_ite_mul
// sll_mul
// neg_mul
// ones_mul

/* bvshl -------------------------------------------------------------------- */

// const_binary_bv_exp
// special_const_lhs_binary_exp
// special_const_rhs_binary_exp
// const_sll

/* bvshr -------------------------------------------------------------------- */

// const_binary_bv_exp
// special_const_lhs_binary_exp
// special_const_rhs_binary_exp
// const_srl
// same_srl
// not_same_srl

/* bvslt -------------------------------------------------------------------- */

// const_binary_bv_exp
// special_const_lhs_binary_exp
// special_const_rhs_binary_exp
// false_lt
// bool_slt
// concat_lower_slt
// bcond_slt

/* bvudiv ------------------------------------------------------------------- */

// const_binary_bv_exp
// special_const_lhs_binary_exp
// special_const_rhs_binary_exp
// bool_udiv
// power2_udiv
// one_udiv
// bcond_udiv

/* bvult -------------------------------------------------------------------- */

// const_binary_bv_exp
// special_const_lhs_binary_exp
// special_const_rhs_binary_exp
// false_lt
// bool_ult
// concat_upper_ult
// concat_lower_ult
// bcond_ult

/* bvurem ------------------------------------------------------------------- */

// const_binary_bv_exp
// special_const_lhs_binary_exp
// special_const_rhs_binary_exp
// bool_urem
// zero_urem

}  // namespace bzla

#endif