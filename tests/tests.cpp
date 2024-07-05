/**tests.cpp
 *
 * Unit tests for the left-leaning red-black tree
 */

#include <gtest/gtest.h>
#include <stdexcept>
#include <random>
#include <iostream>

#include "rbtree.hpp"

// Fast 32-bit integer log2 calculation from Bit Twiddling Hacks
// http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
unsigned int lg2(unsigned int a)
{
#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
    static const char LogTable256[256] = {
        -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
        LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
        LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)};
#undef LT

    unsigned int v = a; // 32-bit word to find the log of
    unsigned r;         // r will be lg(v)
    unsigned int t, tt; // temporaries

    tt = v >> 16;
    if (tt != 0)
    {
        t = tt >> 8;
        if (t != 0)
            r = 24 + LogTable256[t]; // Higher-order 8 bits are non-zero
        else
            r = 16 + LogTable256[tt]; // Higher-order 16 bits are non-zero, but next 8 bits are zero
    }
    else
    {
        t = v >> 8;
        if (t != 0)
            r = 8 + LogTable256[t]; // Lower-order 16 bits are zero, but next 8 bits are non-zero
        else
            r = LogTable256[v]; // Lower-order 24 bits are zero
    }

    return r;
}

// Stress test parameters
constexpr unsigned int STRESS_TEST_SAMPLE_COUNT = 100000;
constexpr unsigned int STRESS_TEST_STRIDING = 293;
constexpr unsigned int RAND_GEN_SEED = 1;
const unsigned int STRESS_TEST_LG2 = lg2(STRESS_TEST_SAMPLE_COUNT);

TEST(RedBlackTreeTest, EmptyTree)
{
    RedBlackTree<int, int> tree;
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);
    EXPECT_THROW(tree.at(1), std::out_of_range);
}

TEST(RedBlackTreeTest, Insert7Integers)
{
    RedBlackTree<int, int> tree;
    tree.insert(std::pair<int, int>{3, 3});
    tree.insert(std::pair<int, int>{1, 1});
    tree.insert(std::pair<int, int>{5, 5});
    tree.insert(std::pair<int, int>{0, 0});
    tree.insert(std::pair<int, int>{4, 4});
    tree.insert(std::pair<int, int>{2, 2});
    tree.insert(std::pair<int, int>{6, 6});

    EXPECT_TRUE(!tree.empty());
    EXPECT_EQ(tree.size(), 7);
    EXPECT_EQ(tree.at(0), 0);
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(4));
    EXPECT_TRUE(!tree.contains(8));
    EXPECT_EQ(tree[3], 3);
    EXPECT_EQ(tree[5], 5);
}

TEST(RedBlackTreeTest, Initialize7Integers)
{
    RedBlackTree<int, int> tree{{3, 3}, {1, 1}, {5, 5}, {0, 0}, {4, 4}, {2, 2}, {6, 6}};

    EXPECT_TRUE(!tree.empty());
    EXPECT_EQ(tree.size(), 7);
    EXPECT_EQ(tree.at(0), 0);
    EXPECT_EQ(tree.at(3), 3);
    EXPECT_EQ(tree.at(5), 5);
}

TEST(RedBlackTreeTest, SubscriptModify7Integers)
{
    RedBlackTree<int, int> tree{{3, 3}, {1, 1}, {5, 5}, {0, 0}, {4, 4}, {2, 2}, {6, 6}};

    EXPECT_TRUE(!tree.empty());
    EXPECT_EQ(tree.size(), 7);
    EXPECT_EQ(tree[0], 0);
    EXPECT_EQ(tree[5], 5);

    tree[0] = 6;
    tree[3] = 4;
    EXPECT_EQ(tree[0], 6);
    EXPECT_EQ(tree[3], 4);
}

TEST(RedBlackTreeTest, SerializeAndDepth)
{
    RedBlackTree<int, int> tree{{3, 3}, {1, 1}, {5, 5}, {0, 0}, {4, 4}, {2, 2}, {6, 6}};

    EXPECT_TRUE(!tree.empty());
    EXPECT_EQ(tree.size(), 7);
    EXPECT_EQ(tree[0], 0);
    EXPECT_EQ(tree[5], 5);

    auto toString = [](const int &i)
    { return std::to_string(i); };

    std::string serializedTree = tree.serialize(toString);
    std::string expected = "3,1,0,)2,)5,4,)6,)";
    EXPECT_EQ(serializedTree, expected);
    EXPECT_EQ(tree.depth(), 3);
}

TEST(RedBlackTreeTest, WorstCaseInsert7Integers)
{
    RedBlackTree<int, int> tree{{0, 6}, {1, 5}, {2, 4}, {3, 3}, {4, 2}};
    tree[5] = 1;
    tree[6] = 0;

    auto toString = [](const int &i)
    { return std::to_string(i); };

    std::string serializedTree = tree.serialize(toString);
    std::string expected = "3,1,0,)2,)5,4,)6,)";
    EXPECT_TRUE(!tree.empty());
    EXPECT_EQ(tree.size(), 7);
    EXPECT_EQ(tree[0], 6);
    EXPECT_EQ(tree[3], 3);
    EXPECT_EQ(tree[5], 1);
    EXPECT_EQ(serializedTree, expected);
}

TEST(RedBlackTreeTest, RandomStressTestInsertOnly)
{
    std::mt19937 randGen(RAND_GEN_SEED);
    RedBlackTree<int, int> tree;

    int randNum = 0;
    int accessKey = 0;
    for (int i = 0; i < STRESS_TEST_SAMPLE_COUNT; i++)
    {
        randNum = randGen();
        tree.insert({randNum, i});

        // Randomly choose a key to store
        if (randNum % 17 == 0)
            accessKey = randNum;

        // Randomly delete a key and check exception on access
        if (randNum % STRESS_TEST_STRIDING == 0)
            EXPECT_TRUE(tree.contains(accessKey));
    }

    EXPECT_EQ(tree.size(), STRESS_TEST_SAMPLE_COUNT);
    EXPECT_TRUE(tree.depth() <= STRESS_TEST_LG2 + STRESS_TEST_LG2); // Depth <= 2lgN
}

TEST(RedBlackTreeTest, WorstCaseStressTestInsertOnly)
{
    RedBlackTree<int, int> tree;
    for (int i = 0; i < STRESS_TEST_SAMPLE_COUNT; i++)
        tree.insert({i, i});

    EXPECT_EQ(tree.size(), STRESS_TEST_SAMPLE_COUNT);
    EXPECT_TRUE(tree.depth() <= STRESS_TEST_LG2 + STRESS_TEST_LG2); // Depth <= 2lgN
}

TEST(RedBlackTreeTest, Insert10IntegersWithDelete)
{
    RedBlackTree<int, int> tree;
    for (int i = 0; i < 10; i++)
        tree.insert({i, i});

    tree.erase(1);
    tree.erase(3);
    tree.erase(5);
    tree.erase(8);

    EXPECT_EQ(tree.size(), 6);
    EXPECT_EQ(tree[0], 0);
    EXPECT_EQ(tree[2], 2);
    EXPECT_EQ(tree[4], 4);
    EXPECT_EQ(tree[6], 6);
    EXPECT_EQ(tree[7], 7);
    EXPECT_EQ(tree[9], 9);

    size_t depth = tree.depth();
    EXPECT_TRUE(depth == 3);

    tree[1] = 1;
    EXPECT_EQ(tree[1], 1);
    EXPECT_EQ(tree.size(), 7);

    tree.erase(1);
    EXPECT_THROW(tree.at(1), std::out_of_range);
}

TEST(RedBlackTreeTest, RandomStressTestMixedInsertErase)
{
    std::mt19937 randGen(RAND_GEN_SEED);
    RedBlackTree<int, int> tree;

    int randNum = 0;
    int deleteCount = 0;
    int deleteKey = 0;
    for (int i = 0; i < STRESS_TEST_SAMPLE_COUNT; ++i)
    {
        randNum = randGen();
        tree[randNum] = i;

        // Randomly choose a key to delete
        if (i % 7 == 0)
            deleteKey = randNum;

        // Randomly delete a key and check exception on access
        if (i % STRESS_TEST_STRIDING == 0)
        {
            EXPECT_TRUE(tree.contains(deleteKey));
            tree.erase(deleteKey);
            deleteCount++;
            EXPECT_THROW(tree.at(deleteKey), std::out_of_range);
        }
    }

    EXPECT_EQ(tree.size(), STRESS_TEST_SAMPLE_COUNT - deleteCount);

    size_t depth = tree.depth();
    EXPECT_TRUE(depth <= STRESS_TEST_LG2 + STRESS_TEST_LG2); // Depth <= 2lgN
}

TEST(RedBlackTreeTest, WorstCaseStressTestMixedInsertErase)
{
    RedBlackTree<int, int> tree;
    int deleteCount = 0;
    int deleteKey = 0;
    for (int i = 0; i < STRESS_TEST_SAMPLE_COUNT; i++)
    {
        tree.insert({i, i});

        // Store key i / 4
        if (i % 7 == 0)
            deleteKey = i >> 2;

        // Delete key every time i % 96 == 0
        if (i % STRESS_TEST_STRIDING == 0)
        {
            EXPECT_TRUE(tree.contains(deleteKey));
            tree.erase(deleteKey);
            deleteCount++;
            EXPECT_THROW(tree.at(deleteKey), std::out_of_range);
        }
    }

    EXPECT_EQ(tree.size(), STRESS_TEST_SAMPLE_COUNT - deleteCount);

    size_t depth = tree.depth();
    EXPECT_TRUE(depth <= STRESS_TEST_LG2 + STRESS_TEST_LG2); // Depth <= 2lgN
    EXPECT_TRUE(depth <= STRESS_TEST_LG2 + STRESS_TEST_LG2); // Depth <= 2lgN
}

TEST(RedBlackTreeTest, MixedOperationsIntInt)
{
    RedBlackTree<int, int> tree;

    tree[1] = 10;
    tree[2] = 20;
    tree[3] = 30;

    tree[2] = 25;
    tree[3] = 35;

    tree.erase(1);

    // Assertions
    EXPECT_TRUE(tree.contains(2));
    EXPECT_EQ(tree.at(2), 25);
    EXPECT_TRUE(tree.contains(3));
    EXPECT_EQ(tree[3], 35);
    EXPECT_FALSE(tree.contains(1));
}

TEST(RedBlackTreeTest, MixedOperationsDoubleDouble)
{
    RedBlackTree<double, double> tree;

    tree.insert({1.1, 10.1});
    tree.insert({2.2, 20.2});
    tree.insert({3.3, 30.3});

    tree.insert({2.2, 25.5});
    tree[3.3] = 35.5;

    tree.erase(1.1);

    EXPECT_TRUE(tree.contains(2.2));
    EXPECT_EQ(tree.at(2.2), 25.5);
    EXPECT_TRUE(tree.contains(3.3));
    EXPECT_EQ(tree.at(3.3), 35.5);
    EXPECT_FALSE(tree.contains(1.1));
}

TEST(RedBlackTreeTest, MixedOperationsStringInt)
{
    auto comparator = [](const std::string &a, const std::string &b)
    { return a < b; };

    // Use decltype to get type of expression
    RedBlackTree<std::string, int, decltype(comparator)> tree(comparator);

    for (int i = 0; i < 20; ++i)
        tree["key" + std::to_string(i)] = i;

    tree.insert({"key10", 100});
    tree["key15"] = 150;

    tree.erase("key5");

    EXPECT_TRUE(tree.contains("key10"));
    EXPECT_EQ(tree.at("key10"), 100);
    EXPECT_TRUE(tree.contains("key15"));
    EXPECT_EQ(tree.at("key15"), 150);
    EXPECT_FALSE(tree.contains("key5"));
}

struct Student
{
    std::string name;
    int age;
    double gpa;

    // Defined comparator called by std::less
    bool operator<(const Student &other) const
    {
        return gpa < other.gpa;
    }
};

TEST(RedBlackTreeTest, MixedOperationsStructInt)
{

    RedBlackTree<Student, int> tree;

    for (int i = 0; i < 20; ++i)
        tree.insert({{"Student" + std::to_string(i), 20 + i, 3.0 + i * 0.1}, i});

    tree.insert({{"Student10", 30, 4.0}, 100});
    tree[{"Student15", 35, 4.5}] = 150;

    Student student5{"Student5", 25, 3.5};
    tree.erase(student5);

    EXPECT_TRUE(tree.contains({"Student10", 30, 4.0}));
    EXPECT_EQ(tree.at({"Student10", 30, 4.0}), 100);
    EXPECT_TRUE(tree.contains({"Student15", 35, 4.5}));
    EXPECT_EQ(tree.at({"Student15", 35, 4.5}), 150);
    EXPECT_FALSE(tree.contains({"Student5", 25, 3.5}));
}
