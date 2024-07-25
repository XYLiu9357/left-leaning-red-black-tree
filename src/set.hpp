/**set.hpp
 *
 * Interface for an ordered set container implemented as a
 * left-leaning red black tree template class.
 */

#ifndef RBSET_H
#define RBSET_H

#include <cstdint>
#include <functional>
#include <initializer_list>
#include <string>

#include "deque.hpp"

template <typename key_t, typename Compare = std::less<key_t>>
class Set
{
private:
    using ComparisonResult = std::int8_t;
    constexpr static ComparisonResult LESS_THAN = -1;
    constexpr static ComparisonResult EQUAL_TO = 0;
    constexpr static ComparisonResult GREATER_THAN = 1;

    /**
     * TreeNode
     */

    class TreeNode
    {
    public:
        const static bool RED = true;
        const static bool BLACK = false;

        key_t key;
        TreeNode *left;
        TreeNode *right;
        size_t sz;
        bool color;

        TreeNode(key_t k, bool c)
            : key(k), left(nullptr), right(nullptr), sz(1), color(c) {}
    };

    // Tree attributes
    TreeNode *root;
    Compare comparator;

    // Utilities
    ComparisonResult comp(const key_t &k1, const key_t &k2) const;
    size_t nodeSize(TreeNode *node) const;
    bool treeEqual(TreeNode *node1, TreeNode *node2) const;

    // Recursive deep copy
    TreeNode *copyTree(TreeNode const *node);

    // Tree rotation & coloring
    bool isRed(TreeNode *node);
    TreeNode *rotateLeft(TreeNode *node);
    TreeNode *rotateRight(TreeNode *node);
    void flipColors(TreeNode *node);

    // Fixup during insertion
    TreeNode *rbFix(TreeNode *node);

    // Deletion 2-node fixups
    TreeNode *moveRedLeft(TreeNode *node);
    TreeNode *moveRedRight(TreeNode *node);

    // Recursive helpers
    TreeNode *_at(TreeNode *node, const key_t &key) const;
    int _rank(TreeNode *node, const key_t &key) const;
    TreeNode *_floor(TreeNode *node, const key_t &key) const;
    TreeNode *_ceiling(TreeNode *node, const key_t &key) const;
    const key_t &_rankSelect(TreeNode *node, int rank) const;

    TreeNode *_insert(TreeNode *node, const key_t &key);
    TreeNode *_eraseMin(TreeNode *node);
    TreeNode *_erase(TreeNode *node, const key_t &key);

public:
    /**
     * Constructors
     */

    Set();
    Set(const std::initializer_list<key_t> &init);
    Set(const Set &that); // Deep copy

    /**
     * Utilities
     */

    size_t size() const;
    bool empty() const;

    Set &operator=(const Set &that); // Deep copy
    bool operator==(const Set &that) const;
    bool operator!=(const Set &that) const;

    /**
     * Search
     */

    bool contains(const key_t &key) const;

    /**
     * Ordered set operations
     */

    int rank(const key_t &key) const;
    key_t min() const;
    key_t max() const;

    key_t floor(const key_t &key);
    key_t ceiling(const key_t &key);
    key_t rankSelect(int rank);

    /**
     * Insertion
     */

    void insert(const key_t &key);

    /**
     * Deletion
     */

    void erase(const key_t &key);

    /**
     * Tree processing
     */

    std::string serialize(const std::function<std::string(const key_t &)> &objToString, const std::string &delim = ",", const std::string &nilStr = ")") const;
    size_t depth() const; // BFS: ~2n node accesses

    /**
     * Delete tree
     */

    void _deleteTree(TreeNode *node);
    ~Set();

    /**
     * Inorder iterator
     */
private:
    class Iterator
    {
    public:
        Deque<TreeNode *> nodeStack;

        Iterator(const Set<key_t, Compare> &tree);
        key_t &operator*();
        bool operator==(Iterator that) const;
        bool operator!=(Iterator that) const;
        void operator++();
    };

public:
    Iterator begin() const;
    Iterator end() const;
    Iterator find(const key_t &key) const;
};

#include "set.ipp"

#endif /*RBSET_H*/