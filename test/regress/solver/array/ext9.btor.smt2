(set-logic QF_ABV)
(set-info :status unsat)
(declare-const a0 (Array (_ BitVec 8) (_ BitVec 8) ))
(declare-const v0 (_ BitVec 8))
(declare-const v1 (_ BitVec 8))
(declare-const v2 (_ BitVec 8))
(declare-const v3 (_ BitVec 8))
(declare-const v4 (_ BitVec 8))
(declare-const v5 (_ BitVec 8))
(assert (= #b1 (bvand (bvnot (ite (= (store (store a0 v0 v1) v2 v3) (store (store a0 v0 v1) v4 v5)) #b1 #b0)) (bvand (ite (= v2 v4) #b1 #b0) (ite (= v3 v5) #b1 #b0)))))
(check-sat)