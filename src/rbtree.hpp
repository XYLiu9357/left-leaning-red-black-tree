/**rbtree.hpp
 *
 * Interface for the left-leaning red black tree template class.
 */

#ifndef RBTREE_H
#define RBTREE_H

#include <cstdint>
#include <string>
#include <initializer_list>
#include <functional>
#include <utility>

using ComparisonResult = std::int8_t;
constexpr ComparisonResult LESS_THAN = -1;
constexpr ComparisonResult EQUAL_TO = 0;
constexpr ComparisonResult GREATER_THAN = 1;

template <typename key_t, typename value_t, typename Compare = std::less<key_t>>
class RedBlackTree
{
private:
    /**
     * TreeNode
     */
    class TreeNode
    {
    public:
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

    TreeNode *root;
    Compare comparator;

    ComparisonResult comp(const key_t &k1, const key_t &k2) const;
    size_t size(TreeNode *node) const;

    /**
     * Maintain left-leaning red-black scheme
     */
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

    size_t size() const;
    size_t rank(const key_t &key) const;
    bool empty() const;

    /**
     * Search
     */
    value_t at(const key_t &key) const;
    const value_t &operator[](const key_t &key) const;
    bool contains(const key_t &key) const;
    key_t min() const;

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