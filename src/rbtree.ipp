/**rbtree.ipp
 *
 * Implementation of the left-leaning red black tree template class.
 */

#include <cstdint>
#include <vector>
#include <deque>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

// Define NIL node
template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::TreeNode::NIL = nullptr;

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
    : comparator(Compare())
{
    if (TreeNode::NIL == nullptr)
    {
        // Construct NIL node
        TreeNode::NIL = new TreeNode(std::pair<key_t, value_t>{key_t{}, value_t{}}, TreeNode::BLACK);
        if (!TreeNode::NIL)
            throw std::bad_alloc();

        TreeNode::NIL->left = TreeNode::NIL;
        TreeNode::NIL->right = TreeNode::NIL;
        TreeNode::NIL->sz = 0;
    }

    root = TreeNode::NIL;
}
template <typename key_t, typename value_t, typename Compare>
RedBlackTree<key_t, value_t, Compare>::RedBlackTree(const std::initializer_list<std::pair<key_t, value_t>> &init)
{
    if (TreeNode::NIL == nullptr)
    {
        // Construct NIL node
        TreeNode::NIL = new TreeNode(std::pair<key_t, value_t>{key_t{}, value_t{}}, TreeNode::BLACK);
        if (!TreeNode::NIL)
            throw std::bad_alloc();
        TreeNode::NIL->left = TreeNode::NIL;
        TreeNode::NIL->right = TreeNode::NIL;
        TreeNode::NIL->sz = 0;
    }

    root = TreeNode::NIL;
    for (std::pair<key_t, value_t> pair : init)
        insert(pair);
}

template <typename key_t, typename value_t, typename Compare>
RedBlackTree<key_t, value_t, Compare>::RedBlackTree(const Compare &comp)
    : comparator(comp)
{
    if (TreeNode::NIL == nullptr)
    {
        // Construct NIL node
        TreeNode::NIL = new TreeNode(std::pair<key_t, value_t>{key_t{}, value_t{}}, TreeNode::BLACK);
        if (!TreeNode::NIL)
            throw std::bad_alloc();

        TreeNode::NIL->left = TreeNode::NIL;
        TreeNode::NIL->right = TreeNode::NIL;
        TreeNode::NIL->sz = 0;
    }

    root = TreeNode::NIL;
}

template <typename key_t, typename value_t, typename Compare>
RedBlackTree<key_t, value_t, Compare>::RedBlackTree(const Compare &comp, const std::initializer_list<std::pair<key_t, value_t>> &init)
    : comparator(comp)
{
    if (TreeNode::NIL == nullptr)
    {
        // Construct NIL node
        TreeNode::NIL = new TreeNode(std::pair<key_t, value_t>{key_t{}, value_t{}}, TreeNode::BLACK);
        if (!TreeNode::NIL)
            throw std::bad_alloc();

        TreeNode::NIL->left = TreeNode::NIL;
        TreeNode::NIL->right = TreeNode::NIL;
        TreeNode::NIL->sz = 0;
    }

    root = TreeNode::NIL;
    for (std::pair<key_t, value_t> pair : init)
        insert(pair);
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::copyTree(TreeNode const *node)
{
    TreeNode *curNode = new TreeNode(node->p, node->color);
    curNode->sz = node->sz;
    curNode->left = (node->left == TreeNode::NIL) ? TreeNode::NIL : copyTree(node->left);
    curNode->right = (node->right == TreeNode::NIL) ? TreeNode::NIL : copyTree(node->right);
    return curNode;
}

template <typename key_t, typename value_t, typename Compare>
RedBlackTree<key_t, value_t, Compare>::RedBlackTree(const RedBlackTree &that)
{
    if (that.empty())
        return TreeNode::NIL;

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
    return root == TreeNode::NIL;
}

template <typename key_t, typename value_t, typename Compare>
size_t RedBlackTree<key_t, value_t, Compare>::nodeSize(TreeNode *node) const
{
    return node == TreeNode::NIL ? 0 : node->sz;
}

template <typename key_t, typename value_t, typename Compare>
size_t RedBlackTree<key_t, value_t, Compare>::size() const
{
    return nodeSize(root);
}

template <typename key_t, typename value_t, typename Compare>
bool RedBlackTree<key_t, value_t, Compare>::treeEqual(TreeNode *node1, TreeNode *node2) const
{
    if (node1 == TreeNode::NIL && node2 == TreeNode::NIL)
        return true;
    else if (node1 == TreeNode::NIL)
        return false;
    else if (node2 == TreeNode::NIL)
        return false;

    bool nodeEquality = node1->p == node2->p && node1->sz == node2->sz && node1->color == node2->color;
    return nodeEquality && treeEqual(node1->left, node2->left) && treeEqual(node1->right, node2->right);
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
    if (node == TreeNode::NIL)
        return TreeNode::NIL;

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
    if (queryNode == TreeNode::NIL)
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
    if (queryNode == TreeNode::NIL)
        throw std::out_of_range("Query key not found");

    const value_t &queryRef = queryNode->p.second;
    return queryRef;
}

template <typename key_t, typename value_t, typename Compare>
bool RedBlackTree<key_t, value_t, Compare>::contains(const key_t &key) const
{
    TreeNode *queryNode = _at(root, key);
    return queryNode != TreeNode::NIL;
}

/**
 * Symbol table operations
 */

template <typename key_t, typename value_t, typename Compare>
int RedBlackTree<key_t, value_t, Compare>::_rank(TreeNode *node, const key_t &key) const
{
    if (node == TreeNode::NIL)
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
    while (cur->left != TreeNode::NIL)
        cur = cur->left;

    return cur->p.first;
}

template <typename key_t, typename value_t, typename Compare>
key_t RedBlackTree<key_t, value_t, Compare>::max() const
{
    if (empty())
        throw std::out_of_range("Invalid call to max() with empty container");

    TreeNode *cur = root;
    while (cur->right != TreeNode::NIL)
        cur = cur->right;

    return cur->p.first;
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::_floor(TreeNode *node, const key_t &key) const
{
    if (node == TreeNode::NIL)
        return node;

    ComparisonResult cmp = comp(key, node->p.first);
    if (cmp == EQUAL_TO)
        return node;
    else if (cmp == LESS_THAN)
        return _floor(node->left, key);

    // Query key is greater than node->key
    TreeNode *rightFloor = _floor(node->right, key);
    if (rightFloor == TreeNode::NIL)
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
    if (queryNode == TreeNode::NIL)
        throw std::out_of_range("Argument to floor() is too small");
    else
        return queryNode->p.first;
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::_ceiling(TreeNode *node, const key_t &key) const
{
    if (node == TreeNode::NIL)
        return node;

    ComparisonResult cmp = comp(key, node->p.first);
    if (cmp == EQUAL_TO)
        return node;
    else if (cmp == GREATER_THAN)
        return _ceiling(node->right, key);

    // Query key is less than node->key
    TreeNode *leftCeiling = _ceiling(node->left, key);
    if (leftCeiling == TreeNode::NIL)
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
    if (queryNode == TreeNode::NIL)
        throw std::out_of_range("Argument to ceiling() is too large");
    else
        return queryNode->p.first;
}

template <typename key_t, typename value_t, typename Compare>
const key_t &RedBlackTree<key_t, value_t, Compare>::_rankSelect(TreeNode *node, int rank) const
{
    if (node == TreeNode::NIL)
        throw std::logic_error("Rank select did not find key matching query rank");

    size_t leftSize = node->left->sz;
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
 * Range search
 */

std::vector<key_t> keys()
{
}

std::vector<key_t> keys(const key_t &low, const key_t &high)
{
}

/**
 * Red-black scheme helpers
 */

// Tree rotation
template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::rotateLeft(TreeNode *node)
{
    TreeNode *newNode = node->right;
    node->right = newNode->left;
    newNode->left = node;

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

    // Size update
    newNode->sz = node->sz;
    node->sz = 1 + nodeSize(node->left) + nodeSize(node->right);
    return newNode;
}

// Modify coloring
template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::leanLeft(TreeNode *node)
{
    TreeNode *newNode = rotateLeft(node);
    newNode->color = newNode->left->color;
    newNode->left->color = TreeNode::RED;
    return newNode;
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::leanRight(TreeNode *node)
{
    TreeNode *newNode = rotateRight(node);
    newNode->color = newNode->right->color;
    newNode->right->color = TreeNode::RED;
    return newNode;
}

template <typename key_t, typename value_t, typename Compare>
void RedBlackTree<key_t, value_t, Compare>::flipColor(TreeNode *node)
{
    node->color = !node->color;
    node->left->color = !node->left->color;
    node->right->color = !node->right->color;
}

// Fixup during insertion
template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::rbFix(TreeNode *node)
{
    if (node->right->color == TreeNode::RED && node->left->color == TreeNode::BLACK)
        node = leanLeft(node);
    if (node->left->color == TreeNode::RED && node->left->left->color == TreeNode::RED)
        node = leanRight(node);
    if (node->left->color == TreeNode::RED && node->right->color == TreeNode::RED)
        flipColor(node);

    node->sz = 1 + nodeSize(node->left) + nodeSize(node->right);
    return node;
}

// Deletion 2-node fixups
template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::moveRedLeft(TreeNode *node)
{
    flipColor(node);
    if (node->right->left->color == TreeNode::RED)
    {
        node->right = leanRight(node->right);
        node = leanLeft(node);
        flipColor(node);
    }

    return node;
}

template <typename key_t, typename value_t, typename Compare>
typename RedBlackTree<key_t, value_t, Compare>::TreeNode *RedBlackTree<key_t, value_t, Compare>::moveRedRight(TreeNode *node)
{
    flipColor(node);
    if (node->left->left->color == TreeNode::RED)
    {
        node = leanRight(node);
        flipColor(node);
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
    if (node == TreeNode::NIL)
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
    if (queryNode == TreeNode::NIL)
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
    if (node->left == TreeNode::NIL)
    {
        delete node;
        return TreeNode::NIL;
    }

    // Fix 2-node if necessary
    if (node->left->color == TreeNode::BLACK && node->left->left->color == TreeNode::BLACK)
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
        if (node->left->color == TreeNode::BLACK && node->left->left->color == TreeNode::BLACK)
            node = moveRedLeft(node);

        node->left = _erase(node->left, key);
    }
    else
    {
        if (node->left->color == TreeNode::RED)
            node = leanRight(node);

        // Simple case: leaf node deletion
        if (comp(key, node->p.first) == EQUAL_TO && node->right == TreeNode::NIL)
        {
            delete node;
            return TreeNode::NIL;
        }

        // Push red right if two node
        if (node->right->color == TreeNode::BLACK && node->right->left->color == TreeNode::BLACK)
            node = moveRedRight(node);

        // Complex case: branch node deletion
        if (comp(key, node->p.first) == EQUAL_TO)
        {
            auto subtreeMin = [](TreeNode *start)
            {
                TreeNode *cur = start;
                while (cur->left != TreeNode::NIL)
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
    if (root == TreeNode::NIL)
        throw std::out_of_range("Invalid erase from empty container");
    if (!contains(key))
        throw std::out_of_range("Erase query key not found");

    if (root->left->color == TreeNode::BLACK && root->right->color == TreeNode::BLACK)
        root->color = TreeNode::RED;

    root = _erase(root, key);
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
        if (curRight != TreeNode::NIL)
            nodeStack.push_back(curRight);
        if (curLeft != TreeNode::NIL)
            nodeStack.push_back(curLeft);

        if (curLeft == TreeNode::NIL && curRight == TreeNode::NIL)
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

        if (cur->left != TreeNode::NIL)
        {
            nodeQueue.push_back(cur->left);
            depthQueue.push_back(curDepth + 1);
        }
        if (cur->right != TreeNode::NIL)
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
    if (node == TreeNode::NIL)
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
