// Unit tests for BinaryTree (plain asserts, no framework needed).
#undef NDEBUG  // keep asserts active in every build type
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../src/binary_tree.hpp"
#include "../src/movie.hpp"

namespace
{
    std::vector<int> to_vector(const BinaryTree<int>& tree)
    {
        std::vector<int> values;
        tree.in_order([&values](const int& v) { values.push_back(v); });
        return values;
    }

    void test_insert_find_and_order()
    {
        BinaryTree<int> tree;
        for (int v : {50, 30, 70, 20, 40, 60, 80})
            assert(tree.add(v));
        assert(!tree.add(40));                       // duplicates are rejected...
        assert(tree.getCounter() == 7);              // ...and not counted
        assert((to_vector(tree) == std::vector<int>{20, 30, 40, 50, 60, 70, 80}));
        assert(tree.find(60) && tree.find(60)->data == 60);
        assert(tree.find(65) == nullptr);
        assert(tree.height() == 3);
    }

    void test_delete_all_cases()
    {
        BinaryTree<int> tree;
        for (int v : {50, 30, 70, 20, 40, 60, 80, 65})
            tree.add(v);
        assert(tree.delete_(20));                    // leaf
        assert(tree.delete_(60));                    // one child (65)
        assert(tree.delete_(50));                    // two children, root -> successor 65
        assert(!tree.delete_(999));                  // missing key does not change the count
        assert(tree.getCounter() == 5);
        assert((to_vector(tree) == std::vector<int>{30, 40, 65, 70, 80}));
    }

    void test_copy_and_move()
    {
        BinaryTree<int> original;
        for (int v : {2, 1, 3})
            original.add(v);

        BinaryTree<int> copy(original);              // deep copy
        copy.add(4);
        assert(original.getCounter() == 3 && copy.getCounter() == 4);

        BinaryTree<int> moved(std::move(copy));
        assert(moved.getCounter() == 4 && copy.getCounter() == 0 && copy.is_empty());

        BinaryTree<int> assigned;
        assigned = std::move(moved);
        assert(assigned.getCounter() == 4 && moved.is_empty());
        assert((to_vector(assigned) == std::vector<int>{1, 2, 3, 4}));
    }

    void test_balanced_order_rebuilds_balanced_tree()
    {
        BinaryTree<int> chain;                       // sorted inserts -> degenerate list
        for (int v = 1; v <= 15; ++v)
            chain.add(v);
        assert(chain.height() == 15);

        BinaryTree<int> rebuilt;                     // re-insert median-first
        chain.balanced_order([&rebuilt](const int& v) { rebuilt.add(v); });
        assert(rebuilt.height() == 4);               // perfect tree for 15 elements
        assert(to_vector(rebuilt) == to_vector(chain));
    }

    void test_movies_are_keyed_by_title()
    {
        BinaryTree<Movie> movies;
        movies.add(Movie("Inception", "Christopher Nolan", 2010, 8.8, "Sci-Fi"));
        movies.add(Movie("Fight Club", "David Fincher", 1999, 8.8, "Drama"));
        assert(!movies.add(Movie("Inception", "someone else", 2000, 1.0, "Drama")));
        Node<Movie>* found = movies.find(Movie("Inception"));
        assert(found && found->data.getDirector() == "Christopher Nolan");
        assert(movies.delete_(Movie("Fight Club")) && movies.getCounter() == 1);
    }
}

int main()
{
    test_insert_find_and_order();
    test_delete_all_cases();
    test_copy_and_move();
    test_balanced_order_rebuilds_balanced_tree();
    test_movies_are_keyed_by_title();
    std::cout << "All BinaryTree tests passed" << std::endl;
    return 0;
}
