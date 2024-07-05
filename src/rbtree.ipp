/**rbtree.ipp
 *
 * Implementation of the left-leaning red black tree template class.
 */

#include <cstdint>
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
ComparisonResult RedBlackTree<key_t, value_t, Compare>::comp(const key_t &k1, const key_t &k2) const
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
bool RedBlackTree<key_t, value_t, Compare>::empty() const
{
    return root == TreeNode::NIL;
}

template <typename key_t, typename value_t, typename Compare>
size_t RedBlackTree<key_t, value_t, Compare>::size(TreeNode *node) const
{
    return node == TreeNode::NIL ? 0 : node->sz;
}

template <typename key_t, typename value_t, typename Compare>
size_t RedBlackTree<key_t, value_t, Compare>::size() const
{
    return size(root);
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
    TreeNode *queryNode = _at(root, key);

    if (queryNode == TreeNode::NIL)
        throw std::out_of_range("Query key not found");

    value_t queryValue = queryNode->p.second;
    return queryValue;
}

template <typename key_t, typename value_t, typename Compare>
const value_t &RedBlackTree<key_t, value_t, Compare>::operator[](const key_t &key) const
{
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

template <typename key_t, typename value_t, typename Compare>
key_t RedBlackTree<key_t, value_t, Compare>::min() const
{
    TreeNode *cur = root;
    while (cur->left != TreeNode::NIL)
        cur = cur->left;

    return cur->p.first;
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
    node->sz = 1 + size(node->left) + size(node->right);
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
    node->sz = 1 + size(node->left) + size(node->right);
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

    node->sz = 1 + size(node->left) + size(node->right);
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

    node->sz = 1 + size(node->left) + size(node->right);
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
        throw std::out_of_range("Cannot erase from empty container");
    if (!contains(key))
        throw std::out_of_range("Key is not in the container");

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
        throw std::out_of_range("Cannot serialize empty tree");

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
