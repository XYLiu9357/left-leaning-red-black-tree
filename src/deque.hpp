/**deque.hpp
 *
 * The STL container deque is too powerful and bulky for this application.
 * deque.hpp contains a minimalistic linked-list deque made solely for the
 * purpose of supporting left-leaning red-black tree operations.
 */

#ifndef DEQUE
#define DEQUE

#include <stdexcept>

template <typename T>
class Deque
{
private:
    struct Node
    {
        T value;
        Node *prev;
        Node *next;

        Node(const T &val) : value(val), prev(nullptr), next(nullptr) {}
    };

    Node *head;
    Node *tail;
    size_t sz;

public:
    // Constructor
    Deque() : head(nullptr), tail(nullptr), sz(0) {}

    // Destructor
    ~Deque()
    {
        clear();
    }

    // Check if the deque is empty
    bool empty() const
    {
        return sz == 0;
    }

    // Clear the deque
    void clear()
    {
        while (!empty())
            pop_front();
    }

    // Return the number of items in the deque
    size_t size() const
    {
        return sz;
    }

    T &front() const
    {
        if (empty())
            throw std::out_of_range("Attempted access on an empty queue");
        return head->value;
    }

    // Add the item to the front
    void push_front(const T &item)
    {
        Node *newHead = new Node(item);
        if (sz == 0)
        {
            head = newHead;
            tail = newHead;
        }
        else
        {
            newHead->next = head;
            head->prev = newHead;
            head = newHead;
        }

        sz++;
    }

    // Add the item to the back
    void push_back(const T &item)
    {
        Node *newTail = new Node(item);
        if (sz == 0)
        {
            head = newTail;
            tail = newTail;
        }
        else
        {
            newTail->prev = tail;
            tail->next = newTail;
            tail = newTail;
        }

        sz++;
    }

    // Remove and return the item from the front
    T pop_front()
    {
        if (empty())
            throw std::out_of_range("Attempt to call pop_front on empty queue.");

        Node *oldHead = head;
        T item = head->value;

        if (sz == 1)
        {
            head = nullptr;
            tail = nullptr;
        }
        else
        {
            head = head->next;
            head->prev = nullptr;
        }

        delete oldHead;
        sz--;

        return item;
    }

    // Remove and return the item from the back
    T pop_back()
    {
        if (empty())
            throw std::out_of_range("Attempt to call pop_back on empty queue.");

        Node *oldTail = tail;
        T item = tail->value;

        if (sz == 1)
        {
            head = nullptr;
            tail = nullptr;
        }
        else
        {
            tail = tail->prev;
            tail->next = nullptr;
        }

        delete oldTail;
        sz--;

        return item;
    }
};

#endif /*DEQUE*/