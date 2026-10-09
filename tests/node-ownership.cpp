#include <cassert>
#include <string>
#include <type_traits>

// Include the actual teaching file while keeping its demonstration separate.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main course_demonstration_main
#include COURSE_SOURCE
#undef main
#pragma GCC diagnostic pop

template <typename Collection> void checkOwnerPolicy() {
    static_assert(std::is_default_constructible_v<Collection>);
    static_assert(std::is_destructible_v<Collection>);
    static_assert(!std::is_copy_constructible_v<Collection>);
    static_assert(!std::is_copy_assignable_v<Collection>);
    static_assert(!std::is_move_constructible_v<Collection>);
    static_assert(!std::is_move_assignable_v<Collection>);
}

#if OWNER_KIND == 1
void exercise() {
    checkOwnerPolicy<SinglyLinkedList<int>>();
    checkOwnerPolicy<SinglyLinkedList<std::string>>();
    for (int round = 0; round < 32; ++round) {
        SinglyLinkedList<int> list;
        SinglyLinkedList<int> independent;
        assert(list.size() == 0);
        independent.insertHead(-1);
        for (int value = 0; value < 32; ++value) list.insertTail(value);
        assert(list.size() == 32);
        for (int value = 0; value < 32; ++value) assert(list.at(value) == value);
        for (int value = 0; value < 32; value += 2) assert(list.remove(value));
        assert(!list.remove(100));
        assert(list.size() == 16);
        for (int index = 0; index < 16; ++index) assert(list.at(index) == 2 * index + 1);
        assert(independent.size() == 1 && independent.at(0) == -1);
        list.clear();
        list.clear();
        assert(list.size() == 0 && list.toString().empty());
        for (int index : {-1, 0}) {
            bool rejected = false;
            try { (void)list.at(index); }
            catch (const std::out_of_range&) { rejected = true; }
            assert(rejected);
        }
        // Destruction of a populated object must release the final node.
        list.insertHead(round);
    }
}
#elif OWNER_KIND == 2
void exercise() {
    checkOwnerPolicy<BinarySearchTree>();
    for (int round = 0; round < 32; ++round) {
        BinarySearchTree tree;
        assert(tree.levelOrder() == "empty");
        for (int value = 0; value < 64; ++value) {
            assert(tree.add(value));
            assert(!tree.add(value));
        }
        assert(tree.levelOrder().starts_with("0 1 2 "));
#if OWNER_REFERENCE
        for (int value = 0; value < 64; ++value) assert(tree.remove(value));
        assert(!tree.remove(100));
        assert(tree.levelOrder() == "empty");
        tree.add(round);
#endif
        // The starter's removal TODO remains untouched.
    }
}
#elif OWNER_KIND == 3
void exercise() {
#if OWNER_REFERENCE
    using Tree = AvlTree;
#else
    using Tree = AvlTreeStarter;
#endif
    checkOwnerPolicy<Tree>();
    for (int round = 0; round < 32; ++round) {
        Tree tree;
        assert(tree.levelOrder() == "empty");
        for (int value : {30, 20, 10}) tree.add(value);
#if OWNER_REFERENCE
        assert(tree.levelOrder() == "20(h=2) 10(h=1) 30(h=1) ");
        tree.remove(20);
        assert(tree.levelOrder() == "10(h=2) 30(h=1) ");
        tree.remove(10);
        tree.remove(30);
        assert(tree.levelOrder() == "empty");
        tree.add(round);
#else
        assert(tree.levelOrder() == "30(h=3) 20(h=2) 10(h=1) ");
        // The starter's balancing TODO remains untouched.
#endif
    }
}
#else
template <typename Collection> void exerciseSet() {
    checkOwnerPolicy<Collection>();
    for (int round = 0; round < 32; ++round) {
        Collection values;
        for (int value = -32; value < 32; ++value) assert(!values.contains(value));
        for (int value = -32; value < 32; ++value) {
            values.add(value);
            values.add(value);
        }
        for (int value = -40; value < 40; ++value)
            assert(values.contains(value) == (value >= -32 && value < 32));
    }
}
void exercise() {
    exerciseSet<LinkedListSet>();
    exerciseSet<BinarySearchTreeSet>();
    exerciseSet<AvlSet>();
}
#endif

int main() {
    exercise();
    std::cout << "Single-owner policy and independent lifetimes passed\n";
}
