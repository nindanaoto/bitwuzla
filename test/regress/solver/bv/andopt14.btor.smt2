(set-logic QF_BV)
(set-info :status unsat)
(declare-const v0 (_ BitVec 8))
(declare-const v1 (_ BitVec 8))
(assert (= #b1 (ite (distinct (bvand (bvnot (bvand v0 v1)) v1) (bvand (bvnot v0) v1)) #b1 #b0)))
(check-sat)