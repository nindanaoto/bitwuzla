(set-logic QF_BV)
(set-info :status sat)
(declare-const v0 (_ BitVec 3))
(assert (and (= (bvmul v0 v0) (_ bv4 3)) (not (bvumulo v0 v0))))
(check-sat)