
#include <vector>

#include "globals.h"
#include "gtest/gtest.h"
#include <cstdlib>

#include "handlers/PositionHandler.h"

class PositionHandlerTest : public ::testing::Test {
protected:
    // Helper function to generate a random float in a given range
    float RandomFloat(float min, float max) {
        return  min + static_cast<float>(rand()) / 
                (static_cast<float>(RAND_MAX / (max - min)));
    }

    glm::vec3 RandomVec3(float min, float max) {
        return { RandomFloat(min,  max), RandomFloat(min,  max), RandomFloat(min,  max) };
    }

};

TEST_F(PositionHandlerTest, AddAndGetPositionsTest) {
    PositionHandler position_handler;  // Use the mock for testing

    std::vector<PositionHandler::Box> boxes;

    // Seed the random number generator
    srand(static_cast<unsigned>(time(0)));

    for (int i = 0; i < 100; ++i) {
        PositionHandler::Box mock_box;
        mock_box.pos = RandomVec3(-10, 10);
        mock_box.dims = RandomVec3(-10, 10);
        // use dir to store movement vector
        mock_box.dir = RandomVec3(-10, 10);

        position_handler.Add(i, mock_box);
        position_handler.Move(i, mock_box.dir);

        boxes.push_back(mock_box);
    }
    
    for (int i = 0; i < 100; ++i) {
        PositionHandler::Box retrieved_box = position_handler.GetEntityBox(i);
        auto& old_box = boxes[i];
        old_box.pos += old_box.dir;
        
        EXPECT_NEAR(retrieved_box.pos.x, old_box.pos.x, 0.001);
        EXPECT_NEAR(retrieved_box.pos.y, old_box.pos.y, 0.001);
        EXPECT_NEAR(retrieved_box.pos.z, old_box.pos.z, 0.001);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
