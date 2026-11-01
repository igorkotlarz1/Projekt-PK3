#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

// Generic binary search tree container. T needs operator<, operator> and operator==.
// Nodes own their children through std::unique_ptr, so memory is released automatically.
template<typename T>
class Node
{
public:
    T data;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    explicit Node(T value) : data(std::move(value)), left(nullptr), right(nullptr) {}
};

template<typename T>
class BinaryTree
{
private:
    std::unique_ptr<Node<T>> root;
    int counter;

    // Insertion / deletion
    bool add_node(std::unique_ptr<Node<T>>& current, const T& value)
    {
        if (!current)
        {
            current = std::make_unique<Node<T>>(value);
            return true;
        }
        if (value < current->data)
            return add_node(current->left, value);
        if (value > current->data)
            return add_node(current->right, value);
        return false; // duplicate key, nothing inserted
    }

    std::unique_ptr<Node<T>> delete_node(std::unique_ptr<Node<T>> current, const T& value, bool& removed)
    {
        if (!current)
            return nullptr;

        if (value < current->data)
            current->left = delete_node(std::move(current->left), value, removed);
        else if (value > current->data)
            current->right = delete_node(std::move(current->right), value, removed);
        else
        {
            removed = true;
            if (!current->left)          // no left child (or a leaf)
                return std::move(current->right);
            if (!current->right)         // no right child
                return std::move(current->left);

            // two children: replace with the in-order successor (smallest in the right subtree)
            Node<T>* successor = find_min(current->right.get());
            current->data = successor->data;
            bool unused = false;
            current->right = delete_node(std::move(current->right), current->data, unused);
        }
        return current;
    }

    static Node<T>* find_min(Node<T>* current)
    {
        while (current->left)
            current = current->left.get();
        return current;
    }

    // Lookup
    Node<T>* find_node(Node<T>* current, const T& value) const
    {
        if (!current || current->data == value)
            return current;
        if (value < current->data)
            return find_node(current->left.get(), value);
        return find_node(current->right.get(), value);
    }

    // Traversals
    void in_order_traversal(const std::unique_ptr<Node<T>>& current, bool ascending) const
    {
        if (!current)
            return;
        in_order_traversal(ascending ? current->left : current->right, ascending);
        std::cout << current->data << std::endl;
        in_order_traversal(ascending ? current->right : current->left, ascending);
    }

    void in_order_func(const std::unique_ptr<Node<T>>& current, const std::function<void(const T&)>& function) const
    {
        if (!current)
            return;
        in_order_func(current->left, function);
        function(current->data);
        in_order_func(current->right, function);
    }

    static void median_first(const std::vector<const T*>& sorted, int lo, int hi,
                             const std::function<void(const T&)>& function)
    {
        if (lo > hi)
            return;
        int mid = lo + (hi - lo) / 2;
        function(*sorted[mid]);
        median_first(sorted, lo, mid - 1, function);
        median_first(sorted, mid + 1, hi, function);
    }

    int height_of(const std::unique_ptr<Node<T>>& current) const
    {
        if (!current)
            return 0;
        return 1 + std::max(height_of(current->left), height_of(current->right));
    }

    // Helpers
    std::unique_ptr<Node<T>> copy_node(const std::unique_ptr<Node<T>>& current)
    {
        if (!current)
            return nullptr;
        auto new_node = std::make_unique<Node<T>>(current->data);
        new_node->left = copy_node(current->left);
        new_node->right = copy_node(current->right);
        return new_node;
    }

public:
    BinaryTree() : root(nullptr), counter(0) {}

    // Move constructor
    BinaryTree(BinaryTree&& other) noexcept : root(std::move(other.root)), counter(other.counter)
    {
        other.counter = 0;
    }

    // Copy constructor (deep copy)
    BinaryTree(const BinaryTree& other) : root(copy_node(other.root)), counter(other.counter) {}

    // Move assignment
    BinaryTree& operator=(BinaryTree&& other) noexcept
    {
        if (this != &other)
        {
            root = std::move(other.root);
            counter = other.counter;
            other.counter = 0;
        }
        return *this;
    }

    // Insertion / deletion. Both return whether the tree changed.
    bool add(const T& value)
    {
        bool inserted = add_node(root, value);
        if (inserted)
            counter++;
        return inserted;
    }

    bool delete_(const T& value)
    {
        bool removed = false;
        root = delete_node(std::move(root), value, removed);
        if (removed)
            counter--;
        return removed;
    }

    // Lookup: pointer to the node, or nullptr
    Node<T>* find(const T& value) const
    {
        return find_node(root.get(), value);
    }

    // Traversals
    void in_order(const std::function<void(const T&)>& function) const
    {
        in_order_func(root, function);
    }

    void print(bool ascending) const
    {
        in_order_traversal(root, ascending);
    }

    // Visits elements median-first (sorted middle, then each half recursively).
    // Re-inserting elements in this order rebuilds a balanced tree, which is
    // why the library saves its files this way instead of alphabetically.
    void balanced_order(const std::function<void(const T&)>& function) const
    {
        std::vector<const T*> sorted;
        sorted.reserve(counter);
        in_order([&sorted](const T& value) { sorted.push_back(&value); });
        median_first(sorted, 0, static_cast<int>(sorted.size()) - 1, function);
    }

    // Extras
    void copy_to_tree(const T& value, BinaryTree<T>& other_tree) const
    {
        if (Node<T>* node = find(value))
            other_tree.add(node->data);
    }

    int getCounter() const { return counter; }
    int height() const { return height_of(root); }
    bool is_empty() const { return counter == 0; }
};
