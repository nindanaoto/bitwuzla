#include "bitvector_node.h"
#include "bzlals.h"
#include "test.h"

namespace bzlals {
namespace test {

class TestBzlaLs : public TestBvNodeCommon
{
 protected:
  void SetUp() override
  {
    TestBvNodeCommon::SetUp();

    d_bzlals.reset(new BzlaLs(100));

    d_two4  = BitVector(4, "0010");
    d_for4  = BitVector(4, "0100");
    d_fiv4  = BitVector(4, "0101");
    d_six4  = BitVector(4, "0110");
    d_sev4  = BitVector(4, "0111");
    d_ten4  = BitVector(4, "1010");
    d_ele4  = BitVector(4, "1011");
    d_egt4  = BitVector(4, "1000");
    d_twe4  = BitVector(4, "1100");
    d_thi4  = BitVector(4, "1101");
    d_zero4 = BitVector::mk_zero(4);
    d_zero1 = BitVector::mk_zero(1);
    d_ones4 = BitVector::mk_ones(4);
    d_one4  = BitVector::mk_one(4);
    d_one1  = BitVector::mk_one(1);

    d_c1 = d_bzlals->mk_node(d_ten4, BitVectorDomain(d_ten4));
    d_v1 = d_bzlals->mk_node(TEST_BW);
    d_v2 = d_bzlals->mk_node(TEST_BW);
    d_v3 = d_bzlals->mk_node(TEST_BW);

    d_bzlals->set_assignment(d_v2, d_ones4);
    d_bzlals->set_assignment(d_v3, d_six4);

    // v1 + c1
    d_v1pc1 =
        d_bzlals->mk_node(BzlaLs::OperatorKind::ADD, TEST_BW, {d_v1, d_c1});
    // (v1 + c1) * v2
    d_v1pc1mv2 =
        d_bzlals->mk_node(BzlaLs::OperatorKind::MUL, TEST_BW, {d_v1pc1, d_v2});
    // v1 + v2
    d_v1pv2 =
        d_bzlals->mk_node(BzlaLs::OperatorKind::ADD, TEST_BW, {d_v1, d_v2});
    // (v1 + v2) & v2
    d_v1pv2av2 =
        d_bzlals->mk_node(BzlaLs::OperatorKind::AND, TEST_BW, {d_v1pv2, d_v2});

    // v1[0:0]
    d_v1e = d_bzlals->mk_indexed_node(
        BzlaLs::OperatorKind::EXTRACT, 1, d_v1, {0, 0});
    // v3[0:0]
    d_v3e = d_bzlals->mk_indexed_node(
        BzlaLs::OperatorKind::EXTRACT, 1, d_v3, {0, 0});
    // v1[0:0] / v3[0:0]
    d_v1edv3e =
        d_bzlals->mk_node(BzlaLs::OperatorKind::UDIV, 1, {d_v1e, d_v3e});
    // sext(v1[0:0] / v3[0:0], 3)
    d_v1edv3e_ext = d_bzlals->mk_indexed_node(
        BzlaLs::OperatorKind::SEXT, TEST_BW, d_v1edv3e, {3});

    // v3 << c1
    d_v3sc1 =
        d_bzlals->mk_node(BzlaLs::OperatorKind::SHL, TEST_BW, {d_v3, d_c1});
    // (v3 << c1) + v3
    d_v3sc1pv3 =
        d_bzlals->mk_node(BzlaLs::OperatorKind::ADD, TEST_BW, {d_v3sc1, d_v3});
    // ((v3 << c1) + v3) + v1
    d_v3sc1pv3pv1 = d_bzlals->mk_node(
        BzlaLs::OperatorKind::ADD, TEST_BW, {d_v3sc1pv3, d_v1});

    // root1: (v1 + c1) + v2 < (v1 + v2) & v2
    d_root1 = d_bzlals->mk_node(
        BzlaLs::OperatorKind::ULT, 1, {d_v1pc1mv2, d_v1pv2av2});
    // root2: sext(v1[0:0] + v3[0:0], 3) = ((v3 + c1) + v3) + v1
    d_root2 = d_bzlals->mk_node(
        BzlaLs::OperatorKind::EQ, 1, {d_v1edv3e_ext, d_v3sc1pv3pv1});
  }

  /**
   * Create a mapping from nodes to their parents to compare against the
   * mapping created internally on node creation.
   */
  BzlaLs::ParentsMap get_expected_parents();
  /**
   * Get a reference of the parents map of the BzlaLs object.
   * Note: BzlaLs::d_parents is private and only the main test class has
   *       access to it.
   */
  const BzlaLs::ParentsMap& get_parents() { return d_bzlals->d_parents; }

  /**
   * Wrapper for BzlaLs::update_cone().
   * Note: BzlaLs::update_cone() is private and only the main test class has
   *       access to it.
   */
  void update_cone(uint32_t id, const BitVector& assignment);

  std::unique_ptr<BzlaLs> d_bzlals;

  BitVector d_two4, d_for4, d_fiv4, d_six4, d_sev4;
  BitVector d_egt4, d_ten4, d_ele4, d_twe4, d_thi4;
  BitVector d_zero4, d_zero1, d_ones4, d_one4, d_one1;

  uint32_t d_c1, d_v1, d_v2, d_v3;
  uint32_t d_v1pc1, d_v1pc1mv2;
  uint32_t d_v1pv2, d_v1pv2av2;
  uint32_t d_v1e, d_v3e;
  uint32_t d_v1edv3e, d_v1edv3e_ext;
  uint32_t d_v3sc1, d_v3sc1pv3;
  uint32_t d_v3sc1pv3pv1;
  uint32_t d_root1, d_root2;
};

BzlaLs::ParentsMap
TestBzlaLs::get_expected_parents()
{
  BzlaLs::ParentsMap parents;
  std::vector<uint32_t> to_visit = {d_root1, d_root2};
  while (!to_visit.empty())
  {
    uint32_t cur_id = to_visit.back();
    to_visit.pop_back();
    if (parents.find(cur_id) == parents.end())
    {
      parents[cur_id] = {};
    }
    for (uint32_t i = 0; i < d_bzlals->get_arity(cur_id); ++i)
    {
      uint32_t child_id = d_bzlals->get_child(cur_id, i);
      if (parents.find(child_id) == parents.end())
      {
        parents[child_id] = {};
      }
      if (parents.at(child_id).find(cur_id) == parents.at(child_id).end())
      {
        parents.at(child_id).insert(cur_id);
      }
      to_visit.push_back(child_id);
    }
  }
  return parents;
}

void
TestBzlaLs::update_cone(uint32_t id, const BitVector& assignment)
{
  d_bzlals->update_cone(d_bzlals->get_node(id), assignment);
}

TEST_F(TestBzlaLs, parents)
{
  d_bzlals->register_root(d_root1);
  d_bzlals->register_root(d_root2);

  const BzlaLs::ParentsMap& parents   = get_parents();
  BzlaLs::ParentsMap parents_expected = get_expected_parents();

  {
    const std::unordered_set<uint32_t>& p = parents.at(d_c1);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_c1);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 2);
    ASSERT_TRUE(p.find(d_v1pc1) != p.end());
    ASSERT_TRUE(p.find(d_v3sc1) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p = parents.at(d_v1);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v1);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 4);
    ASSERT_TRUE(p.find(d_v1pc1) != p.end());
    ASSERT_TRUE(p.find(d_v1pv2) != p.end());
    ASSERT_TRUE(p.find(d_v3sc1pv3pv1) != p.end());
    ASSERT_TRUE(p.find(d_v1e) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p = parents.at(d_v2);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v2);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 3);
    ASSERT_TRUE(p.find(d_v1pc1mv2) != p.end());
    ASSERT_TRUE(p.find(d_v1pv2) != p.end());
    ASSERT_TRUE(p.find(d_v1pv2av2) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p = parents.at(d_v3);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v3);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 3);
    ASSERT_TRUE(p.find(d_v3sc1) != p.end());
    ASSERT_TRUE(p.find(d_v3sc1pv3) != p.end());
    ASSERT_TRUE(p.find(d_v3e) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p = parents.at(d_v1pc1);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v1pc1);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 1);
    ASSERT_TRUE(p.find(d_v1pc1mv2) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p  = parents.at(d_v1pc1mv2);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v1pc1mv2);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 1);
    ASSERT_TRUE(p.find(d_root1) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p = parents.at(d_v1pv2);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v1pv2);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 1);
    ASSERT_TRUE(p.find(d_v1pv2av2) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p = parents.at(d_v1pv2av2);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v1pv2av2);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 1);
    ASSERT_TRUE(p.find(d_root1) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p = parents.at(d_v1e);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v1e);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 1);
    ASSERT_TRUE(p.find(d_v1edv3e) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p = parents.at(d_v3e);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v3e);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 1);
    ASSERT_TRUE(p.find(d_v1edv3e) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p  = parents.at(d_v1edv3e);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v1edv3e);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 1);
    ASSERT_TRUE(p.find(d_v1edv3e_ext) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p  = parents.at(d_v1edv3e_ext);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v1edv3e_ext);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 1);
    ASSERT_TRUE(p.find(d_root2) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p  = parents.at(d_v3sc1);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v3sc1);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 1);
    ASSERT_TRUE(p.find(d_v3sc1pv3) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p  = parents.at(d_v3sc1pv3);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v3sc1pv3);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 1);
    ASSERT_TRUE(p.find(d_v3sc1pv3pv1) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p  = parents.at(d_v3sc1pv3pv1);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_v3sc1pv3pv1);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 1);
    ASSERT_TRUE(p.find(d_root2) != p.end());
  }
  {
    const std::unordered_set<uint32_t>& p = parents.at(d_root1);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_root1);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 0);
  }
  {
    const std::unordered_set<uint32_t>& p = parents.at(d_root2);
    const std::unordered_set<uint32_t>& pe = parents_expected.at(d_root2);
    ASSERT_EQ(p, pe);
    ASSERT_TRUE(p.size() == 0);
  }
}

TEST_F(TestBzlaLs, update_cone)
{
  std::map<uint32_t, BitVector> ass_init = {
      {d_c1, d_ten4},            // 1010
      {d_v1, d_zero4},           // 0000
      {d_v2, d_ones4},           // 1111
      {d_v3, d_six4},            // 0110
      {d_v1pc1, d_ten4},         // 0000 + 1010 = 1010
      {d_v1pc1mv2, d_six4},      // 1010 * 1111 = 0110
      {d_v1pv2, d_ones4},        // 0000 + 1111 = 1111
      {d_v1pv2av2, d_ones4},     // 1111 & 1111 = 1111
      {d_v1e, d_zero1},          // 0000[0:0] = 0
      {d_v3e, d_zero1},          // 0110[0:0] = 0
      {d_v1edv3e, d_one1},       // 0 / 0 = 1
      {d_v1edv3e_ext, d_ones4},  // sext(1, 3) = 1111
      {d_v3sc1, d_zero4},        // 0110 << 1010 = 0000
      {d_v3sc1pv3, d_six4},      // 0000 + 0110 = 0110
      {d_v3sc1pv3pv1, d_six4},   // 0110 + 0000 = 0110
      {d_root1, d_zero1},        // 0110 <s 1111 = 0
      {d_root2, d_zero1},        // 0110 == 0000 = 0
  };

  for (auto& p : ass_init)
  {
    ASSERT_EQ(d_bzlals->get_assignment(p.first).compare(p.second), 0);
  }

  /* v1 -> 0001 */
  update_cone(d_v1, d_one4);
  std::map<uint32_t, BitVector> ass_up_v1 = {
      {d_c1, d_ten4},            // 1010
      {d_v1, d_one4},            // 0001
      {d_v2, d_ones4},           // 1111
      {d_v3, d_six4},            // 0110
      {d_v1pc1, d_ele4},         // 0001 + 1010 = 1011
      {d_v1pc1mv2, d_fiv4},      // 1011 * 1111 = 0101
      {d_v1pv2, d_zero4},        // 0001 + 1111 = 0000
      {d_v1pv2av2, d_zero4},     // 0000 & 1111 = 0000
      {d_v1e, d_one1},           // 0001[0:0] = 1
      {d_v3e, d_zero1},          // 0110[0:0] = 0 (not in COI)
      {d_v1edv3e, d_one1},       // 1 / 0 = 1
      {d_v1edv3e_ext, d_ones4},  // sext(1, 3) = 1111
      {d_v3sc1, d_zero4},        // 0110 << 1010 = 0000 (not in COI)
      {d_v3sc1pv3, d_six4},      // 0000 + 0110 = 0110  (not in COI)
      {d_v3sc1pv3pv1, d_sev4},   // 0110 + 0001 = 0111
      {d_root1, d_zero1},        // 0101 <s 0000 = 0
      {d_root2, d_zero1},        // 0111 == 1111 = 0
  };
  for (auto& p : ass_up_v1)
  {
    ASSERT_EQ(d_bzlals->get_assignment(p.first).compare(p.second), 0);
  }

  /* v2 -> 0111 */
  update_cone(d_v2, d_sev4);
  std::map<uint32_t, BitVector> ass_up_v2 = {
      {d_c1, d_ten4},            // 1010
      {d_v1, d_one4},            // 0001
      {d_v2, d_sev4},            // 0111
      {d_v3, d_six4},            // 0110
      {d_v1pc1, d_ele4},         // 0001 + 1010 = 1011 (not in COI)
      {d_v1pc1mv2, d_thi4},      // 1011 * 0111 = 1101
      {d_v1pv2, d_egt4},         // 0001 + 0111 = 1000
      {d_v1pv2av2, d_zero4},     // 1000 & 0111 = 0000
      {d_v1e, d_one1},           // 0001[0:0] = 1 (not in COI)
      {d_v3e, d_zero1},          // 0110[0:0] = 0 (not in COI)
      {d_v1edv3e, d_one1},       // 1 / 0 = 1 (not in COI)
      {d_v1edv3e_ext, d_ones4},  // sext(1, 3) = 1111 (not in COI)
      {d_v3sc1, d_zero4},        // 0110 << 1010 = 0000 (not in COI)
      {d_v3sc1pv3, d_six4},      // 0000 + 0110 = 0110  (not in COI)
      {d_v3sc1pv3pv1, d_sev4},   // 0110 + 0001 = 0111 (not in COI)
      {d_root1, d_one1},         // 1101 <s 0000 = 1
      {d_root2, d_zero1},        // 0111 == 1111 = 0 (not in COI)
  };
  for (auto& p : ass_up_v2)
  {
    ASSERT_EQ(d_bzlals->get_assignment(p.first).compare(p.second), 0);
  }

  /* v3 -> 0001 */
  update_cone(d_v3, d_one4);
  std::map<uint32_t, BitVector> ass_up_v3 = {
      {d_c1, d_ten4},            // 1010
      {d_v1, d_one4},            // 0001
      {d_v2, d_sev4},            // 0111
      {d_v3, d_one4},            // 0001
      {d_v1pc1, d_ele4},         // 0001 + 1010 = 1011 (not in COI)
      {d_v1pc1mv2, d_thi4},      // 1011 * 0111 = 1101 (not in COI)
      {d_v1pv2, d_egt4},         // 0001 + 0111 = 1000 (not in COI)
      {d_v1pv2av2, d_zero4},     // 1000 & 0111 = 0000 (not in COI)
      {d_v1e, d_one1},           // 0001[0:0] = 1 (not in COI)
      {d_v3e, d_one1},           // 0001[0:0] = 1
      {d_v1edv3e, d_one1},       // 1 / 1 = 1
      {d_v1edv3e_ext, d_ones4},  // sext(1, 3) = 1111
      {d_v3sc1, d_zero4},        // 0001 << 1010 = 0000
      {d_v3sc1pv3, d_one4},      // 0000 + 0001 = 0001
      {d_v3sc1pv3pv1, d_two4},   // 0001 + 0001 = 0010
      {d_root1, d_one1},         // 1101 <s 0000 = 1 (not in COI)
      {d_root2, d_zero1},        // 0010 == 1111 = 0
  };
  for (auto& p : ass_up_v3)
  {
    ASSERT_EQ(d_bzlals->get_assignment(p.first).compare(p.second), 0);
  }
}

}  // namespace test
}  // namespace bzlals