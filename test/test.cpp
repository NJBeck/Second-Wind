
#include <vector>
#include <random>
#include <algorithm>

#include "gtest/gtest.h"

#include "globals.h"

using std::vector, std::uniform_real_distribution, std::uniform_int_distribution, std::default_random_engine, std::is_sorted;

namespace {

    class PosTest : public ::testing::Test {
        // generate num randomly distributed entities with positions
        // test that they are properly sorted
    protected:
        PosTest() {
            _entities.reserve(num);
            for (unsigned i = 0; i < num; ++i) { _entities.emplace_back(entity()); }
            uniform_real_distribution<double> unifDouble(lower, upper);
            default_random_engine re;

            for (auto& ent: _entities) {
                Pos entPos = { unifDouble(re), unifDouble(re) };

                globals::posHandler.add(ent.handle, entPos);
            }
        }
        vector<entity> _entities;
    public:
        unsigned int num = 10000;
        double lower = -10000.0;
        double upper = 10000.0;
    };

    TEST_F(PosTest, sorted) {
        // get the entities sorted by decreasing Y value in range
        vector<unsigned long> sortedEntities = globals::posHandler.InRange(lower, upper, lower, upper);
        // put their y position values into a vector to test if they are in fact sorted
        vector<Pos> sortedPos;
        sortedPos.reserve(num);
        for (auto& ent : sortedEntities) {
            sortedPos.push_back(globals::posHandler.GetPos(ent));
        }
        EXPECT_TRUE(is_sorted(sortedPos.begin(), sortedPos.end(), [](Pos const& lhs, Pos const& rhs) { return lhs.yPos > rhs.yPos; }));
    }

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}