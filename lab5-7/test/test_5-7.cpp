#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

class TimerManager {
public:
    void Start() {
        running = true;
        start_time = std::chrono::steady_clock::now();
    }

    long ElapsedMs() const {
        if (!running) return 0;
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
    }

    void Stop() {
        running = false;
    }

private:
    bool running = false;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
};


class TreeManager {
public:
    TreeManager() {
        CreateNode(-1, -1);
    }

    bool CreateNode(int child_id, int parent_id) {
        if (nodes.find(child_id) != nodes.end()) return false;
        if (parent_id != -1 && nodes.find(parent_id) == nodes.end()) return false;

        NodeInfo info;
        info.id = child_id;
        info.parent = parent_id;
        nodes[child_id] = info;
        if (parent_id != -1) {
            nodes[parent_id].children.insert(child_id);
        }

        return true;
    }

    bool RemoveNode(int id) {
        if (nodes.find(id) == nodes.end()) return false;
        RemoveSubtree(id);
        return true;
    }

    bool IsNodeAccessible(int id) const {
        return nodes.find(id) != nodes.end();
    }

private:
    struct NodeInfo {
        int id;
        int parent;
        std::unordered_set<int> children;
    };

    std::unordered_map<int, NodeInfo> nodes;

    void RemoveSubtree(int id) {
        for (auto c : nodes[id].children) {
            RemoveSubtree(c);
        }
        int parent_id = nodes[id].parent;
        if (parent_id != -1) {
            nodes[parent_id].children.erase(id);
        }
        nodes.erase(id);
    }
};


TEST(TimerManagerTest, StartStopCheck) {
    TimerManager tm;
    tm.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    long elapsed = tm.ElapsedMs();
    EXPECT_GE(elapsed, 50);
    tm.Stop();
    EXPECT_EQ(tm.ElapsedMs(), 0);
}

TEST(TimerManagerTest, NoStartCheck) {
    TimerManager tm;
    EXPECT_EQ(tm.ElapsedMs(), 0);
    tm.Stop(); 
    EXPECT_EQ(tm.ElapsedMs(), 0);
}


TEST(TreeManagerTest, CreateRemoveCheck) {
    TreeManager tm;
    EXPECT_TRUE(tm.IsNodeAccessible(-1));

    EXPECT_TRUE(tm.CreateNode(10, -1));
    EXPECT_TRUE(tm.IsNodeAccessible(10));

    EXPECT_TRUE(tm.CreateNode(11, 10));
    EXPECT_TRUE(tm.IsNodeAccessible(11));

    EXPECT_TRUE(tm.RemoveNode(10));
    EXPECT_FALSE(tm.IsNodeAccessible(10));
    EXPECT_FALSE(tm.IsNodeAccessible(11));
}

TEST(TreeManagerTest, FailCases) {
    TreeManager tm;
    EXPECT_FALSE(tm.CreateNode(-1, -1));
    EXPECT_FALSE(tm.CreateNode(100, 9999));
    EXPECT_FALSE(tm.RemoveNode(9999));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}