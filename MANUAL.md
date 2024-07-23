# LLRB Ordered Symbol Table API

This document contains a full collection of descriptions for all public functions in the `Map` class. The files [rbtree.hpp](src/rbtree.hpp) and [rbtree.ipp](src/rbtree.ipp) contains the interface and implementation of the container, and [deque.hpp](src/deque.hpp) contains a light-weight double-ended queue for some of `Map`'s operations. The operations support in `Set` are identical to `Map`, except that the symbol table lookup operations are removed.

For more general information about the class, see [README.md](README.md).

## Constructors

`Map<key_t, value_t>()`: Default constructor. Returns empty `Map` with specified key and value type. Both key and value types must have default constructors. Comparator is set to default: `std::less`.

`Map<key_t, value_t>(const std::initializer_list<pair<key_t, value_t>>& init)`: Inserts all key-value pairs represented by `std::pair<key_t, value_t>` in the provided initializer list to `Map` and return the container object.

`Map<key_t, value_t, Compare>()`: Constructor with custom comparator. Returns empty `Map` with specified custom comparator. Create a `struct` or `class` and define `operator()` as the custom comparator. Note that the comparator should define a <a href="https://en.wikipedia.org/wiki/Total_order#Strict_and_non-strict_total_orders">total order</a>. A code sample is shown below.

```
struct Comparator
{
    bool operator()(const key_t &lhs, const value_t &rhs) const
    {
        // Define a total order based on key type
        return lhs.attribute < rhs.attribute;
    }
};
```

`Map<key_t, value_t, Compare>(const std::initializer_list<std::pair<key_t, value_t>>& init)`: Constructor with custom comparator and initializer list. Combines the functionality of the two constructors above.

`Map(const Map& that)`: Copy constructor. Make a **deep copy** of the container. If custom classes are used for keys or values, then their copy constructors are called.

## Container Utilities

`size()`: Returns the number of elements in the tree as `size_t`.

`empty()`: Checks if the tree is empty. Returns `true` if empty.

`operator=`: Make a **deep copy** of the container and assign to current container.

`operator==`, `operator!=`: Equality and inequality comparison operators. Two `Map` are equal if and only if all the key-value pairs and the comparator used are the same. For custom classes, `operator==` must be defined. **Compare two `Map` with different `key_t` and `value_T` will result in undefined behavior.**

## Search

`at(const key_t& key)`: Returns the value associated with the given key.

`operator[](const key_t& key)`: Returns a reference to the value associated with the given key.

`contains(const key_t& key)`: Checks if the tree contains the given key.

`begin()`, `end()`: Returns iterators for in-order traversal.

`find(const key_t& key)`: Returns an iterator to the element with the given key.

## Ordering Statistics

`rank(const key_t& key)`: Returns the rank of the given key. _Rank_ is defines as the number of keys present in the container that are strictly less than the given key.

`min()`, `max()`: Returns the smallest and largest keys in the container.

`floor(const key_t& key)`: Returns the largest key less than or equal to the given key.

`ceiling(const key_t& key)`: Returns the smallest key greater than or equal to the given key.

`rankSelect(int rank)`: Returns the key with the given rank.

## Insertion

`insert(const std::pair<key_t, value_t>& pair)`: Inserts the key-value pair into the tree.

`operator[]`: Allows insertion and access using subscript notation.

## Deletion

`erase(const key_t& key)`: Deletes the node with the given key.

## Tree processing

`serialize(const std::function<string(const key_t&)>& objToString, const std::string& delim = ",", const std::string& nilStr = ")")`: Serializes the tree into a string format using custom serialization function. A function `objToString` must be provided to represent the `key_t` type object as a string. `delim` specifies the delimiter used. `nilStr` specifies the string to represent NIL node (null). By default, the delimiter is `,` and `)` represents NIL. A code sample is shown below.

```
Map<int, int> tree{{0, 6}, {1, 5}, {2, 4}, {3, 3}, {4, 2}, {5, 1}, {6, 0}};

auto toString = [](const int &i)
{ return std::to_string(i); };

std::string serializedTree = tree.serialize(toString);
// Output: 3,1,0,)2,)5,4,)6,)
```

`depth()`: Returns the depth of the tree. An empty tree has depth 0.

## Iterator Methods

`operator*()`: Dereferences the iterator to access the current element, which is `std::pair<key_t, value_t>`.

`operator->()`: Accesses the current element through pointer, which is `*std::pair<key_t, value_t>`.

`operator==`, `operator!=`: Equality and inequality comparison operators. Two iterators are equal if they represent the same object. **Comparing iterators from different container objects result in undefined behavior.**

`operator++()`: Advances the iterator to the next element in in-order traversal (prefix increment).

## Destructor

`~Map()`: Destructor to delete the tree and free memory.
