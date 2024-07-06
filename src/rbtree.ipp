/**rbtree.ipp
 *
 * Implementation of the left-leaning red black tree template class.
 */

#ifndef RBTREE_I
#define RBTREE_I

#include <cstdint>
#include <deque>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

// Custom comparator
template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::ComparisonResult RedBlackTree<key_t, value_t, Compare>::comp(const key_t &k1, const key_t &k2) const
{
    if (comparator(k1, k2))
        return LESS_THAN;
    else if (comparator(k2, k1))
        return GREATER_THAN;
    else
        return EQUAL_TO;
}

/**
 * Constructors
 */

template <typename key_t, typename value_t, typename Compare>
RedBlackTree<key_t, value_t, Compare>::RedBlackTree()
    : root(nullptr), comparator(Compare()) {}

template <typename key_t, typename value_t, typename Compare>
RedBlackTree<key_t, value_t, Compare>::RedBlackTree(const std::initializer_list<std::pair<key_t, value_t>> &init)
    : root(nullptr), comparator(Compare())
{
    for (std::pair<key_t, value_t> pair : init)
        insert(pair);
}

template <typename key_t, typename value_t, typename Compare>
RedBlackTree<key_t, value_t, Compare>::RedBlackTree(const Compare &comp)
    : root(nullptr), comparator(comp) {}

template <typename key_t, typename value_t, typename Compare>
RedBlackTree<key_t, value_t, Compare>::RedBlackTree(const Compare &comp, const std::initializer_list<std::pair<key_t, value_t>> &init)
    : root(nullptr), comparator(comp)
{
    for (std::pair<key_t, value_t> pair : init)
        insert(pair);
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::copyTree(TreeNode const *node)
{
    if (node == nullptr)
        return nullptr;

    // Deep copy if a copy constructor is specified
    key_t keyCopy = node->p.first;
    value_t valCopy = node->p.second;

    TreeNode *curNode = new TreeNode(std::pair<key_t, value_t>(keyCopy, valCopy), node->color);
    curNode->sz = node->sz;
    curNode->left = copyTree(node->left);
    curNode->right = copyTree(node->right);
    return curNode;
}

template <typename key_t, typename value_t, typename Compare>
RedBlackTree<key_t, value_t, Compare>::RedBlackTree(const RedBlackTree &that)
{
    TreeNode *newRoot = copyTree(that.root);
    this->root = newRoot;
    this->comparator = that.comparator;
}

/**
 * Utilities
 */

template <typename key_t, typename value_t, typename Compare>
bool RedBlackTree<key_t, value_t, Compare>::empty() const
{
    return root == nullptr;
}

template <typename key_t, typename value_t, typename Compare>
size_t RedBlackTree<key_t, value_t, Compare>::nodeSize(TreeNode *node) const
{
    return node == nullptr ? 0 : node->sz;
}

template <typename key_t, typename value_t, typename Compare>
size_t RedBlackTree<key_t, value_t, Compare>::size() const
{
    return nodeSize(root);
}

template <typename key_t, typename value_t, typename Compare>
bool RedBlackTree<key_t, value_t, Compare>::treeEqual(TreeNode *node1, TreeNode *node2) const
{
    if (node1 == nullptr && node2 == nullptr)
        return true;
    else if (node1 == nullptr)
        return false;
    else if (node2 == nullptr)
        return false;

    bool nodeEquality = node1->p == node2->p && node1->sz == node2->sz && node1->color == node2->color;
    return nodeEquality && treeEqual(node1->left, node2->left) && treeEqual(node1->right, node2->right);
}
template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::RedBlackTree &RedBlackTree<key_t, value_t, Compare>::operator=(const RedBlackTree &that)
{
    // Copy and swap
    RedBlackTree<key_t, value_t> temp(that);
    std::swap(this->root, temp.root);
    std::swap(this->comparator, temp.comparator);

    return *this;
}

template <typename key_t, typename value_t, typename Compare>
bool RedBlackTree<key_t, value_t, Compare>::operator==(const RedBlackTree &that) const
{
    return treeEqual(this->root, that.root);
}

template <typename key_t, typename value_t, typename Compare>
bool RedBlackTree<key_t, value_t, Compare>::operator!=(const RedBlackTree &that) const
{
    return !treeEqual(this->root, that.root);
}

/**
 * Search
 */

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::_at(TreeNode *node, const key_t &key) const
{
    if (node == nullptr)
        return nullptr;

    if (comp(key, node->p.first) == LESS_THAN)
        return _at(node->left, key);
    else if (comp(key, node->p.first) == GREATER_THAN)
        return _at(node->right, key);
    else
        return node;
}

template <typename key_t, typename value_t, typename Compare>
value_t RedBlackTree<key_t, value_t, Compare>::at(const key_t &key) const
{
    if (empty())
        throw std::out_of_range("Invalid search in empty container");

    TreeNode *queryNode = _at(root, key);
    if (queryNode == nullptr)
        throw std::out_of_range("Query key not found");

    value_t queryValue = queryNode->p.second;
    return queryValue;
}

template <typename key_t, typename value_t, typename Compare>
const value_t &RedBlackTree<key_t, value_t, Compare>::operator[](const key_t &key) const
{
    if (empty())
        throw std::out_of_range("Invalid search in empty container");

    TreeNode *queryNode = _at(root, key);
    if (queryNode == nullptr)
        throw std::out_of_range("Query key not found");

    const value_t &queryRef = queryNode->p.second;
    return queryRef;
}

template <typename key_t, typename value_t, typename Compare>
bool RedBlackTree<key_t, value_t, Compare>::contains(const key_t &key) const
{
    TreeNode *queryNode = _at(root, key);
    return queryNode != nullptr;
}

/**
 * Ordered symbol table operations
 */

template <typename key_t, typename value_t, typename Compare>
int RedBlackTree<key_t, value_t, Compare>::_rank(TreeNode *node, const key_t &key) const
{
    if (node == nullptr)
        return 0;

    ComparisonResult cmp = comp(key, node->p.first);
    if (cmp == LESS_THAN)
        return _rank(node->left, key);
    else if (cmp == GREATER_THAN)
        return nodeSize(node->left) + 1 + _rank(node->right, key);
    else
        return nodeSize(node->left);
}

template <typename key_t, typename value_t, typename Compare>
int RedBlackTree<key_t, value_t, Compare>::rank(const key_t &key) const
{
    if (empty())
        throw std::out_of_range("Invalid rank query with empty container");
    return _rank(root, key);
}

template <typename key_t, typename value_t, typename Compare>
key_t RedBlackTree<key_t, value_t, Compare>::min() const
{
    if (empty())
        throw std::out_of_range("Invalid call to min() with empty container");

    TreeNode *cur = root;
    while (cur->left != nullptr)
        cur = cur->left;

    return cur->p.first;
}

template <typename key_t, typename value_t, typename Compare>
key_t RedBlackTree<key_t, value_t, Compare>::max() const
{
    if (empty())
        throw std::out_of_range("Invalid call to max() with empty container");

    TreeNode *cur = root;
    while (cur->right != nullptr)
        cur = cur->right;

    return cur->p.first;
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::_floor(TreeNode *node, const key_t &key) const
{
    if (node == nullptr)
        return nullptr;

    ComparisonResult cmp = comp(key, node->p.first);
    if (cmp == EQUAL_TO)
        return node;
    else if (cmp == LESS_THAN)
        return _floor(node->left, key);

    // Query key is greater than node->key
    TreeNode *rightFloor = _floor(node->right, key);
    if (rightFloor == nullptr)
        return node;
    else
        return rightFloor;
}

template <typename key_t, typename value_t, typename Compare>
key_t RedBlackTree<key_t, value_t, Compare>::floor(const key_t &key)
{
    if (empty())
        throw std::out_of_range("Invalid call to floor() with empty container");

    TreeNode *queryNode = _floor(root, key);
    if (queryNode == nullptr)
        throw std::out_of_range("Argument to floor() is too small");
    else
        return queryNode->p.first;
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::_ceiling(TreeNode *node, const key_t &key) const
{
    if (node == nullptr)
        return node;

    ComparisonResult cmp = comp(key, node->p.first);
    if (cmp == EQUAL_TO)
        return node;
    else if (cmp == GREATER_THAN)
        return _ceiling(node->right, key);

    // Query key is less than node->key
    TreeNode *leftCeiling = _ceiling(node->left, key);
    if (leftCeiling == nullptr)
        return node;
    else
        return leftCeiling;
}

template <typename key_t, typename value_t, typename Compare>
key_t RedBlackTree<key_t, value_t, Compare>::ceiling(const key_t &key)
{
    if (empty())
        throw std::out_of_range("Invalid call to ceiling() with empty container");

    TreeNode *queryNode = _ceiling(root, key);
    if (queryNode == nullptr)
        throw std::out_of_range("Argument to ceiling() is too large");
    else
        return queryNode->p.first;
}

template <typename key_t, typename value_t, typename Compare>
const key_t &RedBlackTree<key_t, value_t, Compare>::_rankSelect(TreeNode *node, int rank) const
{
    if (node == nullptr)
        throw std::logic_error("Rank select did not find key matching query rank");

    size_t leftSize = nodeSize(node->left);
    if (rank < leftSize)
        return _rankSelect(node->left, rank);
    else if (rank > leftSize)
        return _rankSelect(node->right, rank - leftSize - 1);
    else
        return node->p.first;
}

template <typename key_t, typename value_t, typename Compare>
key_t RedBlackTree<key_t, value_t, Compare>::rankSelect(int rank)
{
    if (empty())
        throw std::out_of_range("Invalid call to rankSelect() with empty container");
    if (rank < 0 || rank >= size())
        throw std::out_of_range("Argument to rankSelect() is invalid");

    key_t queryKey = _rankSelect(root, rank); // Defensive copy
    return queryKey;
}

/**
 * Red-black scheme helpers
 */

// Tree rotation & coloring
template <typename key_t, typename value_t, typename Compare>
bool RedBlackTree<key_t, value_t, Compare>::isRed(TreeNode *node)
{
    if (node == nullptr)
        return TreeNode::BLACK;
    else
        return node->color;
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::rotateLeft(TreeNode *node)
{
    TreeNode *newNode = node->right;
    node->right = newNode->left;
    newNode->left = node;

    // Enforce color
    newNode->color = newNode->left->color;
    newNode->left->color = TreeNode::RED;

    // Size update
    newNode->sz = node->sz;
    node->sz = 1 + nodeSize(node->left) + nodeSize(node->right);
    return newNode;
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::rotateRight(TreeNode *node)
{
    TreeNode *newNode = node->left;
    node->left = newNode->right;
    newNode->right = node;

    // Enforce color
    newNode->color = newNode->right->color;
    newNode->right->color = TreeNode::RED;

    // Size update
    newNode->sz = node->sz;
    node->sz = 1 + nodeSize(node->left) + nodeSize(node->right);
    return newNode;
}

template <typename key_t, typename value_t, typename Compare>
void RedBlackTree<key_t, value_t, Compare>::flipColors(TreeNode *node)
{
    node->color = !node->color;
    node->left->color = !node->left->color;
    node->right->color = !node->right->color;
}

// Fixup during insertion
template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::rbFix(TreeNode *node)
{
    if (isRed(node->right) && !isRed(node->left))
        node = rotateLeft(node);
    if (isRed(node->left) && isRed(node->left->left))
        node = rotateRight(node);
    if (isRed(node->left) && isRed(node->right))
        flipColors(node);

    node->sz = 1 + nodeSize(node->left) + nodeSize(node->right);
    return node;
}

// Deletion 2-node fixups
template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::moveRedLeft(TreeNode *node)
{
    flipColors(node);
    if (isRed(node->right->left))
    {
        node->right = rotateRight(node->right);
        node = rotateLeft(node);
        flipColors(node);
    }

    return node;
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::moveRedRight(TreeNode *node)
{
    flipColors(node);
    if (isRed(node->left->left))
    {
        node = rotateRight(node);
        flipColors(node);
    }

    return node;
}

/**
 * Insertion
 */

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::_insert(TreeNode *node, const std::pair<key_t, value_t> &pair)
{
    // Recursive insertion
    if (node == nullptr)
    {
        TreeNode *newNode = new TreeNode(pair, TreeNode::RED);
        if (!newNode)
            throw std::bad_alloc();
        return newNode;
    }

    ComparisonResult cmp = comp(pair.first, node->p.first);
    if (cmp == LESS_THAN)
        node->left = _insert(node->left, pair);
    else if (cmp == GREATER_THAN)
        node->right = _insert(node->right, pair);
    else
        node->p.second = pair.second;

    // Maintain red-black scheme
    node = rbFix(node);

    node->sz = 1 + nodeSize(node->left) + nodeSize(node->right);
    return node;
}

template <typename key_t, typename value_t, typename Compare>
void RedBlackTree<key_t, value_t, Compare>::insert(const std::pair<key_t, value_t> &pair)
{
    root = _insert(root, pair);
    root->color = TreeNode::BLACK;
}

template <typename key_t, typename value_t, typename Compare>
value_t &RedBlackTree<key_t, value_t, Compare>::operator[](const key_t &key)
{
    TreeNode *queryNode = _at(root, key);
    if (queryNode == nullptr)
    {
        root = _insert(root, {key, value_t{}});
        queryNode = _at(root, key);
    }

    value_t &queryRef = queryNode->p.second;
    return queryRef;
}

/**
 * Deletion
 */

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::_eraseMin(TreeNode *node)
{
    if (node->left == nullptr)
    {
        delete node;
        return nullptr;
    }

    // Fix 2-node if necessary
    if (!isRed(node->left) && !isRed(node->left->left))
        node = moveRedLeft(node);

    node->left = _eraseMin(node->left);

    // Backtrack clean up transformation
    return rbFix(node);
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::_erase(TreeNode *node, const key_t &key)
{
    if (comp(key, node->p.first) == LESS_THAN)
    {
        // Push red link right if 2-node
        if (!isRed(node->left) && !isRed(node->left->left))
            node = moveRedLeft(node);

        node->left = _erase(node->left, key);
    }
    else
    {
        if (isRed(node->left))
            node = rotateRight(node);

        // Simple case: leaf node deletion
        if (comp(key, node->p.first) == EQUAL_TO && node->right == nullptr)
        {
            delete node;
            return nullptr;
        }

        // Push red right if two black nodes
        if (!isRed(node->right) && !isRed(node->right->left))
            node = moveRedRight(node);

        // Complex case: branch node deletion
        if (comp(key, node->p.first) == EQUAL_TO)
        {
            auto subtreeMin = [](TreeNode *start)
            {
                TreeNode *cur = start;
                while (cur->left != nullptr)
                    cur = cur->left;
                return cur->p;
            };
            node->p = subtreeMin(node->right);
            node->right = _eraseMin(node->right);
        }
        else
            node->right = _erase(node->right, key);
    }

    // Backtrack clean up transformation
    return rbFix(node);
}

template <typename key_t, typename value_t, typename Compare>
void RedBlackTree<key_t, value_t, Compare>::erase(const key_t &key)
{
    if (root == nullptr)
        throw std::out_of_range("Invalid erase from empty container");
    if (!contains(key))
        throw std::out_of_range("Erase query key not found");

    if (!isRed(root->left) && !isRed(root->right))
        root->color = TreeNode::RED;

    root = _erase(root, key);

    if (!empty())
        root->color = TreeNode::BLACK;
}

/**
 * Tree processing
 */
template <typename key_t, typename value_t, typename Compare>
std::string RedBlackTree<key_t, value_t, Compare>::serialize(const std::function<std::string(const key_t &)> &objToString, const std::string &delim, const std::string &nilStr) const
{
    if (empty())
        throw std::out_of_range("Invalid serialization of empty container");

    // Reserve space: there will be 3 / 8 * size nil nodes on average.
    // Reserve 1 additional byte for the null character.
    std::string serializedTree;
    size_t sizeDiv8 = size() >> 3;
    size_t reserveSize = size() + (sizeDiv8 + sizeDiv8 + sizeDiv8) + 1;

    // Set upper bound to avoid buffer overflow
    size_t requestSize = reserveSize < 512 ? reserveSize : 512;
    serializedTree.reserve(requestSize);

    // Preorder DFS to serialize tree
    std::deque<const TreeNode *> nodeStack;
    TreeNode const *cur;
    TreeNode const *curLeft;
    TreeNode const *curRight;
    nodeStack.push_back(root);

    while (nodeStack.size() > 0)
    {
        cur = nodeStack.back();
        nodeStack.pop_back();
        curLeft = cur->left;
        curRight = cur->right;

        serializedTree += objToString(cur->p.first) + delim;
        if (curRight != nullptr)
            nodeStack.push_back(curRight);
        if (curLeft != nullptr)
            nodeStack.push_back(curLeft);

        if (curLeft == nullptr && curRight == nullptr)
            serializedTree += nilStr;
    }

    return serializedTree;
}

template <typename key_t, typename value_t, typename Compare>
size_t RedBlackTree<key_t, value_t, Compare>::depth() const
{
    if (empty())
        throw std::out_of_range("Invalid depth query with empty container");

    std::deque<TreeNode *> nodeQueue;
    std::deque<size_t> depthQueue;

    nodeQueue.push_back(root);
    depthQueue.push_back(1);
    size_t maxDepth = 0;

    // Use BFS to find depth of the tree
    while (!nodeQueue.empty())
    {
        TreeNode *cur = nodeQueue.front();
        size_t curDepth = depthQueue.front();
        nodeQueue.pop_front();
        depthQueue.pop_front();

        maxDepth = curDepth > maxDepth ? curDepth : maxDepth;

        if (cur->left != nullptr)
        {
            nodeQueue.push_back(cur->left);
            depthQueue.push_back(curDepth + 1);
        }
        if (cur->right != nullptr)
        {
            nodeQueue.push_back(cur->right);
            depthQueue.push_back(curDepth + 1);
        }
    }

    return maxDepth;
}

/**
 * Delete Tree
 */
template <typename key_t, typename value_t, typename Compare>
void RedBlackTree<key_t, value_t, Compare>::_deleteTree(TreeNode *node)
{
    if (node == nullptr)
        return;

    _deleteTree(node->left);
    _deleteTree(node->right);
    delete node;
}

template <typename key_t, typename value_t, typename Compare>
RedBlackTree<key_t, value_t, Compare>::~RedBlackTree()
{
    _deleteTree(root);
}

#endif /*RBTREE_I*/