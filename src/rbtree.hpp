/**rbtree.hpp
 *
 * Interface for the left-leaning red black tree template class.
 */

#ifndef RBTREE_H
#define RBTREE_H

#include <cstdint>
#include <string>
#include <vector>
#include <initializer_list>
#include <functional>
#include <utility>

template <typename key_t, typename value_t, typename Compare = std::less<key_t>>
class RedBlackTree
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
        // Static keyword in nested class. Is it static for just the inner
        // class or static for both classes.
        static TreeNode *NIL;
        const static bool RED = true;
        const static bool BLACK = false;

        std::pair<key_t, value_t> p;
        TreeNode *left;
        TreeNode *right;
        size_t sz;
        bool color;

        TreeNode(std::pair<key_t, value_t> pair, bool c)
            : p(pair), left(NIL), right(NIL), sz(1), color(c) {}
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

    // Tree rotation
    TreeNode *rotateLeft(TreeNode *node);
    TreeNode *rotateRight(TreeNode *node);

    // Modify coloring
    TreeNode *leanLeft(TreeNode *node);
    TreeNode *leanRight(TreeNode *node);
    void flipColor(TreeNode *node);

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

    TreeNode *_insert(TreeNode *node, const std::pair<key_t, value_t> &pair);
    TreeNode *_eraseMin(TreeNode *node);
    TreeNode *_erase(TreeNode *node, const key_t &key);

public:
    /**
     * Constructors
     */

    RedBlackTree();
    RedBlackTree(const std::initializer_list<std::pair<key_t, value_t>> &init);
    RedBlackTree(const Compare &comp);
    RedBlackTree(const Compare &comp, const std::initializer_list<std::pair<key_t, value_t>> &init);
    RedBlackTree(const RedBlackTree &that);

    /**
     * Utilities
     */

    size_t size() const;
    bool empty() const;
    bool operator==(const RedBlackTree &that) const;
    bool operator!=(const RedBlackTree &that) const;

    /**
     * Search
     */

    value_t at(const key_t &key) const;
    const value_t &operator[](const key_t &key) const;
    bool contains(const key_t &key) const;

    /**
     * Symbol table operations
     */

    int rank(const key_t &key) const;
    key_t min() const;
    key_t max() const;

    key_t floor(const key_t &key);
    key_t ceiling(const key_t &key);
    key_t rankSelect(int rank);

    /**
     * Range search
     */
    std::vector<key_t> keys();                                    // TODO
    std::vector<key_t> keys(const key_t &low, const key_t &high); // TODO

    /**
     * Insertion
     */

    void insert(const std::pair<key_t, value_t> &pair);
    value_t &operator[](const key_t &key);

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
    ~RedBlackTree();
};

#include "rbtree.ipp"

#endif /*RBTREE_H*/