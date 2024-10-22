/**tests.cpp
 *
 * Unit tests for the left-leaning red-black tree
 */

#include <gtest/gtest.h>
#include <stdexcept>
#include <random>
#include <iostream>

#include "map.hpp"
#include "set.hpp"

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

/**
 * Note: the pseudorandom generator starts producing repeated keys
 * if STRESS_TEST_SAMPLE_COUNT > 100,000, which invalidate some of
 * the unit tests.
 */

// Stress test parameters
constexpr unsigned int STRESS_TEST_SAMPLE_COUNT = 100000;
constexpr unsigned int STRESS_TEST_STRIDING = 293;
constexpr unsigned int RAND_GEN_SEED = 1;
const unsigned int STRESS_TEST_LG2 = lg2(STRESS_TEST_SAMPLE_COUNT);

/**
 * Tree operations: search, insert, delete, copy tree
 */

TEST(MapOperations, EmptyTree)
{
    Map<int, int> tree;
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);
    EXPECT_THROW(tree.at(1), std::out_of_range);
    EXPECT_THROW(tree.erase(1), std::out_of_range);
    EXPECT_THROW(tree.rankSelect(1), std::out_of_range);

    auto toString = [](int k)
    { return std::to_string(k); };
    EXPECT_THROW(tree.serialize(toString), std::out_of_range);
    EXPECT_EQ(tree.depth(), 0);
}

TEST(MapOperations, Insert7Integers)
{
    Map<int, int> tree;
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

TEST(MapOperations, Initialize7Integers)
{
    Map<int, int> tree{{3, 3}, {1, 1}, {5, 5}, {0, 0}, {4, 4}, {2, 2}, {6, 6}};

    EXPECT_TRUE(!tree.empty());
    EXPECT_EQ(tree.size(), 7);
    EXPECT_EQ(tree.at(0), 0);
    EXPECT_EQ(tree.at(3), 3);
    EXPECT_EQ(tree.at(5), 5);
}

TEST(MapOperations, SubscriptModify7Integers)
{
    Map<int, int> tree{{3, 3}, {1, 1}, {5, 5}, {0, 0}, {4, 4}, {2, 2}, {6, 6}};

    EXPECT_TRUE(!tree.empty());
    EXPECT_EQ(tree.size(), 7);
    EXPECT_EQ(tree[0], 0);
    EXPECT_EQ(tree[5], 5);

    tree[0] = 6;
    tree[3] = 4;
    EXPECT_EQ(tree[0], 6);
    EXPECT_EQ(tree[3], 4);
}

TEST(MapOperations, SerializeAndDepth)
{
    Map<int, int> tree{{3, 3}, {1, 1}, {5, 5}, {0, 0}, {4, 4}, {2, 2}, {6, 6}};

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

TEST(MapOperations, WorstCaseInsert7Integers)
{
    Map<int, int> tree{{0, 6}, {1, 5}, {2, 4}, {3, 3}, {4, 2}};
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

TEST(MapOperations, RandomStressTestInsertOnly)
{
    std::mt19937 randGen(RAND_GEN_SEED);
    Map<int, int> tree;

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

TEST(MapOperations, WorstCaseStressTestInsertOnly)
{
    Map<int, int> tree;
    for (int i = 0; i < STRESS_TEST_SAMPLE_COUNT; i++)
        tree.insert({i, i});

    EXPECT_EQ(tree.size(), STRESS_TEST_SAMPLE_COUNT);
    EXPECT_TRUE(tree.depth() <= STRESS_TEST_LG2 + STRESS_TEST_LG2); // Depth <= 2lgN
}

TEST(MapOperations, Insert10IntegersWithDelete)
{
    Map<int, int> tree;
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

TEST(MapOperations, RandomStressTestMixedInsertErase)
{
    std::mt19937 randGen(RAND_GEN_SEED);
    Map<int, int> tree;

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

TEST(MapOperations, WorstCaseStressTestMixedInsertErase)
{
    Map<int, int> tree;
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

TEST(MapOperations, MixedOperationsIntInt)
{
    Map<int, int> tree;

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

TEST(MapOperations, MixedOperationsDoubleDouble)
{
    Map<double, double> tree;

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

TEST(MapOperations, MixedOperationsStringInt)
{
    struct Comparator
    {
        // Lexicographic comparison
        bool operator()(const std::string &lhs, const std::string &rhs) const { return lhs < rhs; }
    };

    Map<std::string, int, Comparator> tree;

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

    bool operator==(const Student &other) const
    {
        return name == other.name && age == other.age && gpa == other.gpa;
    }
};

TEST(MapOperations, MixedOperationsStructInt)
{
    Map<Student, int> tree;

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

TEST(MapOperations, TreeDeepCopyStructInt)
{
    Map<Student, int> tree1;

    for (int i = 0; i < 20; ++i)
        tree1.insert({{"Student" + std::to_string(i), 20 + i, 3.0 + i * 0.1}, i});

    Map<Student, int> tree2 = tree1;

    EXPECT_EQ(tree1, tree2);

    tree1.insert({{"Student10", 30, 4.0}, 100});
    tree1[{"Student15", 35, 4.5}] = 150;
    tree2.insert({{"Student10", 30, 4.0}, 100});
    tree2[{"Student15", 35, 4.5}] = 150;
    EXPECT_EQ(tree1, tree2);

    Student student5{"Student5", 25, 3.5};
    tree1.erase(student5);
    EXPECT_TRUE(tree1 != tree2);
}

TEST(MapOperations, TreeDeepCopyStressTest)
{
    Map<int, int> tree1;
    Map<int, int> tree2;
    for (int i = 0; i < STRESS_TEST_SAMPLE_COUNT; i++)
        tree1.insert({i, i});

    EXPECT_EQ(tree1.size(), STRESS_TEST_SAMPLE_COUNT);
    EXPECT_TRUE(tree1.depth() <= STRESS_TEST_LG2 + STRESS_TEST_LG2); // Depth <= 2lgN

    tree2 = tree1; // Deep copy made
    EXPECT_EQ(tree1, tree2);

    while (tree1.size() > 0)
        tree1.erase(tree1.max());

    EXPECT_TRUE(tree1.empty());
    EXPECT_TRUE(!tree2.empty());
}

/**
 * Symbol table operations
 */

TEST(MapSymbolTableOps, MinMaxRankIntInt)
{
    Map<int, int> tree;
    tree[5] = 50;
    tree[3] = 30;
    tree[8] = 80;
    tree[1] = 10;
    tree[4] = 40;
    tree[7] = 70;
    tree[10] = 100;
    tree[2] = 20;
    tree[6] = 60;
    tree[9] = 90;

    EXPECT_EQ(tree.rank(5), 4);
    EXPECT_EQ(tree.rank(3), 2);
    EXPECT_EQ(tree.rank(8), 7);
    EXPECT_EQ(tree.rank(1), 0);
    EXPECT_EQ(tree.rank(-1), 0);
    EXPECT_EQ(tree.rank(11), 10); // All keys are strictly less than 11

    EXPECT_EQ(tree.min(), 1);
    EXPECT_EQ(tree.max(), 10);
}

TEST(MapSymbolTableOps, FloorCeilingSelectLongString)
{
    Map<long, std::string> tree;
    tree[20] = "twenty";
    tree[18] = "eighteen";
    tree[16] = "sixteen";
    tree[2] = "two";
    tree[4] = "four";
    tree[8] = "eight";
    tree[10] = "ten";
    tree[12] = "twelve";
    tree[6] = "six";
    tree[14] = "fourteen";

    EXPECT_EQ(tree.floor(5), 4);
    EXPECT_EQ(tree.floor(6), 6);
    EXPECT_EQ(tree.floor(11), 10);
    EXPECT_THROW(tree.floor(1), std::out_of_range);

    EXPECT_EQ(tree.ceiling(0), 2);
    EXPECT_EQ(tree.ceiling(15), 16);
    EXPECT_EQ(tree.ceiling(6), 6);
    EXPECT_THROW(tree.ceiling(21), std::out_of_range);

    EXPECT_EQ(tree.rankSelect(0), 2);
    EXPECT_EQ(tree.rankSelect(3), 8);
    EXPECT_EQ(tree.rankSelect(9), 20);
    EXPECT_THROW(tree.rankSelect(11), std::out_of_range);

    EXPECT_EQ(tree[8], "eight");
    EXPECT_EQ(tree[10], "ten");
    EXPECT_EQ(tree[14], "fourteen");
}

TEST(MapSymbolTableOps, IteratorIntInt)
{
    Map<int, int> tree1;
    Map<int, int> tree2;

    for (int i = 0; i < 20; i++)
        tree1[i] = 20 - i;

    for (std::pair<int, int> p : tree1)
        tree2.insert(p);

    for (int i = 0; i < 20; i++)
        tree1.erase(19 - i);

    EXPECT_TRUE(tree1.empty());
    EXPECT_TRUE(tree1.size() == 0);
    EXPECT_TRUE(tree2.size() == 20);

    int counter = 0;
    for (std::pair<int, int> p : tree2)
    {
        EXPECT_EQ(p.first, counter);
        EXPECT_EQ(p.second, 20 - p.first);
        EXPECT_EQ(tree2.find(counter)->second, 20 - counter);
        counter++;
    }
}

/**
 * Symbol table operations stress test
 */

struct KeyStruct
{
    std::string strField;
    int intField;
    double doubleField;

    KeyStruct() {}
    KeyStruct(const std::string &str, int i, double d) : strField(str), intField(i), doubleField(d) {}
};

class ValueClass
{
public:
    std::vector<int> vecField;
    std::string strField;
    int intField;
    double doubleField;

    ValueClass() {}
    ValueClass(const std::vector<int> &vec, const std::string &str, int i, double d)
        : vecField(vec), strField(str), intField(i), doubleField(d) {}
};

// Comparator
struct KeyStructComparator
{
    bool operator()(const KeyStruct &lhs, const KeyStruct &rhs) const
    {
        // Sort based on integer field
        if (lhs.intField != rhs.intField)
            return lhs.intField < rhs.intField;
        if (lhs.strField != rhs.strField)
            return lhs.strField < rhs.strField;
        return lhs.doubleField < rhs.doubleField;
    }
};

// Test suite definition
class MapSymbolTableOpsStressTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        // Insert a large number of key-value pairs into the tree
        for (int i = 0; i < STRESS_TEST_SAMPLE_COUNT; ++i)
        {
            KeyStruct key("Key" + std::to_string(i), i, static_cast<double>(i));
            ValueClass value({i, i + 1, i + 2}, "Value" + std::to_string(i), i, static_cast<double>(i));
            tree[key] = value;
        }
    }

    Map<KeyStruct, ValueClass, KeyStructComparator> tree;
};

TEST_F(MapSymbolTableOpsStressTest, DepthCheck)
{
    EXPECT_TRUE(tree.depth() <= STRESS_TEST_LG2 + STRESS_TEST_LG2); // Depth <= 2lgN
}

TEST_F(MapSymbolTableOpsStressTest, RankOperation)
{
    for (int i = 0; i < STRESS_TEST_SAMPLE_COUNT; ++i)
    {
        KeyStruct key("Key" + std::to_string(i), i, static_cast<double>(i));
        EXPECT_EQ(tree.rank(key), i);
    }
}

TEST_F(MapSymbolTableOpsStressTest, MinOperation)
{
    EXPECT_EQ(tree.min().intField, 0);
}

TEST_F(MapSymbolTableOpsStressTest, MaxOperation)
{
    EXPECT_EQ(tree.max().intField, STRESS_TEST_SAMPLE_COUNT - 1);
}

TEST_F(MapSymbolTableOpsStressTest, FloorOperation)
{
    KeyStruct key("Key5000", 5000, 5000.0);
    EXPECT_EQ(tree.floor(key).intField, 5000);
}

TEST_F(MapSymbolTableOpsStressTest, CeilingOperation)
{
    KeyStruct key("Key5000", 5000, 5000.0);
    EXPECT_EQ(tree.ceiling(key).intField, 5000);
}

TEST_F(MapSymbolTableOpsStressTest, RankSelectOperation)
{
    EXPECT_EQ(tree.rankSelect(5000).intField, 5000);
}

TEST_F(MapSymbolTableOpsStressTest, ForEachIteration)
{
    int counter = 0;
    for (auto &pair : tree)
    {
        EXPECT_EQ(pair.first.intField, counter);
        EXPECT_EQ(pair.second.intField, counter);
        ++counter;
    }
}

TEST_F(MapSymbolTableOpsStressTest, FindOperation)
{
    for (int i = 0; i < STRESS_TEST_SAMPLE_COUNT; ++i)
    {
        KeyStruct key("Key" + std::to_string(i), i, static_cast<double>(i));
        auto it = tree.find(key);
        ASSERT_NE(it, tree.end()); // Catch nasty test failures
        EXPECT_EQ(it->first.intField, i);
        EXPECT_EQ(it->second.intField, i);
    }
}

TEST(SetOperations, Insert7Integers)
{
    Set<int> tree;
    tree.insert(3);
    tree.insert(1);
    tree.insert(5);
    tree.insert(0);
    tree.insert(4);
    tree.insert(2);
    tree.insert(6);

    EXPECT_TRUE(!tree.empty());
    EXPECT_EQ(tree.size(), 7);
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(4));
    EXPECT_TRUE(!tree.contains(8));
}

TEST(SetOperations, RandomStressTestMixedInsertErase)
{
    std::mt19937 randGen(RAND_GEN_SEED);
    Set<int> set;

    int randNum = 0;
    int deleteCount = 0;
    int deleteKey = 0;
    for (int i = 0; i < STRESS_TEST_SAMPLE_COUNT; ++i)
    {
        randNum = randGen();
        set.insert(randNum);

        // Randomly choose a key to delete
        if (i % 7 == 0)
            deleteKey = randNum;

        // Randomly delete a key and check exception on access
        if (i % STRESS_TEST_STRIDING == 0)
        {
            EXPECT_TRUE(set.contains(deleteKey));
            set.erase(deleteKey);
            deleteCount++;
        }
    }

    EXPECT_EQ(set.size(), STRESS_TEST_SAMPLE_COUNT - deleteCount);

    size_t depth = set.depth();
    EXPECT_TRUE(depth <= STRESS_TEST_LG2 + STRESS_TEST_LG2); // Depth <= 2lgN
}

TEST(SetOperations, WorstCaseStressTestMixedInsertErase)
{
    Set<int> set;
    int deleteCount = 0;
    int deleteKey = 0;
    for (int i = 0; i < STRESS_TEST_SAMPLE_COUNT; i++)
    {
        set.insert(i);

        // Store key i / 4
        if (i % 7 == 0)
            deleteKey = i >> 2;

        // Delete key every time i % 96 == 0
        if (i % STRESS_TEST_STRIDING == 0)
        {
            EXPECT_TRUE(set.contains(deleteKey));
            set.erase(deleteKey);
            deleteCount++;
        }
    }

    EXPECT_EQ(set.size(), STRESS_TEST_SAMPLE_COUNT - deleteCount);

    size_t depth = set.depth();
    EXPECT_TRUE(depth <= STRESS_TEST_LG2 + STRESS_TEST_LG2); // Depth <= 2lgN
    EXPECT_TRUE(depth <= STRESS_TEST_LG2 + STRESS_TEST_LG2); // Depth <= 2lgN
}

TEST(SetOperations, MixedOperationsStructInt)
{
    Set<Student> set;

    for (int i = 0; i < 20; ++i)
        set.insert({"Student" + std::to_string(i), 20 + i, 3.0 + i * 0.1});

    set.insert({"Student10", 30, 4.0});
    Student student5{"Student5", 25, 3.5};
    set.erase(student5);

    EXPECT_TRUE(set.contains({"Student10", 30, 4.0}));
    EXPECT_TRUE(set.contains({"Student15", 35, 4.5}));
    EXPECT_FALSE(set.contains({"Student5", 25, 3.5}));
}

TEST(SetOperations, IteratorInt)
{
    Set<int> s1;
    Set<int> s2;

    for (int i = 0; i < 20; i++)
        s1.insert(i);

    for (int val : s1)
        s2.insert(val);

    for (int i = 0; i < 20; i++)
        s1.erase(19 - i);

    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s1.size() == 0);
    EXPECT_TRUE(s2.size() == 20);

    int counter = 0;
    for (int val : s2)
    {
        EXPECT_EQ(val, counter);
        EXPECT_EQ(*(s2.find(counter)), counter);
        counter++;
    }
}
