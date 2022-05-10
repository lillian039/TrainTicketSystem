/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

struct my_true_type {};
struct my_false_type {};

template<typename T>
struct my_type_traits {
    using difference_type = typename T::difference_type;
    using value_type = typename T::value_type;
    using pointer = typename T::pointer;
    using reference = typename T::reference;
    using iterator_category = typename T::iterator_category;
    using iterator_assignable = typename T::iterator_assignable;
};

template <
    class Key,
    class T,
    class Compare = std::less<Key>
    > class map {
public:
    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::map as value_type by typedef.
     */
    typedef pair<const Key, T> value_type;
private:
    struct RedBlackNode {
        int size;
        value_type *makeUp;
        RedBlackNode *father;
        RedBlackNode *son[2];
        int colour;
        RedBlackNode() {
            makeUp = (value_type *)malloc(sizeof(value_type));
            size = colour = 2;
            father = son[0] = son[1] = nullptr;
        }
        RedBlackNode(const value_type &v, RedBlackNode *f = nullptr) {
            makeUp = (value_type *)malloc(sizeof(value_type));
            new (makeUp) value_type(v);
            size = 1;
            colour = 0;
            father = f;
            son[0] = son[1] = nullptr;
        }
        RedBlackNode(const RedBlackNode &other, RedBlackNode *f = nullptr) {
            makeUp = (value_type *)malloc(sizeof(value_type));
            new (makeUp) value_type(*other.makeUp);
            father = f;
            son[0] = son[1] = nullptr;
            size = other.size;
            colour = other.colour;
        }
        ~RedBlackNode() {
            if (colour < 2)
                makeUp->~value_type();

            free(makeUp);
        }
    };
    RedBlackNode *root;
    RedBlackNode *endptr;
    void change(RedBlackNode *x, RedBlackNode *y) {
        if (x->father)
            x->father->son[identify(x)] = y;

        if (y->father)
            y->father->son[identify(y)] = x;

        std::swap(x->father, y->father);
        std::swap(x->son[0], y->son[0]);
        std::swap(x->son[1], y->son[1]);

        if (x->son[0])
            x->son[0]->father = x;

        if (x->son[1])
            x->son[1]->father = x;

        if (y->son[0])
            y->son[0]->father = y;

        if (y->son[1])
            y->son[1]->father = y;

        std::swap(x->colour, y->colour);
        std::swap(x->size, y->size);

        if (x == root)
            root = y;
        else if (y == root)
            root = x;
    }
    void findNxt(RedBlackNode *&node) const {
        if (node->son[1]) {
            node = node->son[1];

            while (node->son[0])
                node = node->son[0];
        } else {
            while (node->father && identify(node))
                node = node->father;

            node = node->father;
        }

        if (node == nullptr)
            node = endptr;
    }
    void findPre(RedBlackNode *&node) const {
        if (node->colour > 1) {
            node = root;

            if (node)
                while (node->son[1])
                    node = node->son[1];
        } else {
            if (node->son[0]) {
                node = node->son[0];

                while (node->son[1])
                    node = node->son[1];
            } else {
                while (node->father && !identify(node))
                    node = node->father;

                node = node->father;
            }
        }
    }
    void findKey(RedBlackNode *&node, const Key &k) const {
        while (node) {
            if (Compare()(node->makeUp->first, k))
                node = node->son[1];
            else if (Compare()(k, node->makeUp->first))
                node = node->son[0];
            else
                break;
        }
    }
    void findMin(RedBlackNode *&node) const {
        while (node->son[0])
            node = node->son[0];
    }
    int identify(RedBlackNode *x) const {
        return x->father->son[1] == x;
    }
    void build(RedBlackNode *&x, RedBlackNode *y, RedBlackNode *f) {
        if (y == nullptr)
            return;

        x = new RedBlackNode(*y, f);
        build(x->son[0], y->son[0], x);
        build(x->son[1], y->son[1], x);
    }
    void makeEmpty(RedBlackNode *x) {
        if (!x)
            return;

        makeEmpty(x->son[0]);
        makeEmpty(x->son[1]);
        delete x;
    }
    void up(RedBlackNode *x) {
        x->size = 1;

        if (x->son[0])
            x->size += x->son[0]->size;

        if (x->son[1])
            x->size += x->son[1]->size;
    }
    void rotate(RedBlackNode *x, int xWhich) {
        RedBlackNode *y = x->son[xWhich];

        if (x->father) {
            int fWhich = identify(x);
            x->father->son[fWhich] = y;
        }

        y->father = x->father;
        x->son[xWhich] = y->son[!xWhich];

        if (y->son[!xWhich])
            y->son[!xWhich]->father = x;

        y->son[!xWhich] = x;
        x->father = y;
        up(x);
        up(y);

        if (x == root)
            root = y;
    }
    RedBlackNode *solveDoubleRed(RedBlackNode *x, int xWhich) {
        RedBlackNode *father = x->father;
        int fWhich = identify(x);
        RedBlackNode *uncle = father->son[!fWhich];

        if (uncle && !uncle->colour) {
            father->colour = 0;
            x->colour = 1;
            uncle->colour = 1;
            return father;
        }

        if (xWhich == fWhich) {
            rotate(father, fWhich);
            x->colour = 1;
            father->colour = 0;
            return x;
        }

        rotate(x, xWhich);
        return x;
    }
    void removeReBalance(RedBlackNode *x) {
        while (x->father) {
            int fWhich = identify(x);
            RedBlackNode *father = x->father;
            RedBlackNode *uncle = father->son[!fWhich];

            if (uncle && !uncle->colour) {
                rotate(father, !fWhich);
                uncle->colour = 1;
                father->colour = 0;
                continue;
            }

            int many = 0;

            if (uncle) {
                if (uncle->son[0])
                    many += (!uncle->son[0]->colour);

                if (uncle->son[1])
                    many += (!uncle->son[1]->colour);
            }

            if (many == 0) {
                if (uncle)
                    uncle->colour = 0;

                if (father->colour)
                    x = x->father;
                else {
                    father->colour = 1;
                    break;
                }
            }

            if (many == 1) {
                if (uncle->son[fWhich] && !uncle->son[fWhich]->colour) {
                    rotate(uncle, fWhich);
                    rotate(father, !fWhich);
                    father->colour = father->son[fWhich]->colour;
                    father->son[fWhich]->colour = 1;
                    break;
                }

                rotate(father, !fWhich);
                father->colour = father->colour;
                father->colour = 1;
                uncle->son[!fWhich]->colour = 1;
                break;
            }

            if (many == 2) {
                RedBlackNode *newFather = uncle->son[fWhich];
                rotate(uncle, fWhich);
                rotate(father, !fWhich);
                newFather->colour = father->colour;
                father->colour = 1;
                break;
            }
        }
    }
public:
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
    class const_iterator;
    class iterator {
        friend class map;
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
        RedBlackNode *node;
        map *where;
    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::value_type is the type that the
        // iterator points to.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type = std::ptrdiff_t;
        using value_type = map::value_type;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::output_iterator_tag;
        using iterator_assignable = my_true_type;
        // If you are interested in type_traits, toy_traits_test provides a place to
        // practice. But the method used in that test is old and rarely used, so you
        // may explore on your own.
        // Notice: you may add some code in here and class const_iterator and namespace sjtu to implement toy_traits_test,
        // this part is only for bonus.


        iterator(RedBlackNode *n = nullptr, map *w = nullptr) {
            // TODO
            node = n;
            where = w;
        }
        iterator(const iterator &other) {
            // TODO
            node = other.node;
            where = other.where;
        }
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            iterator it = *this;

            if (node->colour > 1)
                throw invalid_iterator();

            where->findNxt(node);
            return it;
        }
        /**
         * TODO ++iter
         */
        iterator &operator++() {
            if (node->colour > 1)
                throw invalid_iterator();

            where->findNxt(node);
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            iterator it = *this;
            RedBlackNode *ptr = node;
            where->findPre(ptr);

            if (ptr == nullptr)
                throw invalid_iterator();

            node = ptr;
            return it;
        }
        /**
         * TODO --iter
         */
        iterator &operator--() {
            RedBlackNode *ptr = node;
            where->findPre(ptr);

            if (ptr == nullptr)
                throw invalid_iterator();

            node = ptr;
            return *this;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        value_type &operator*() const {
            if (node->colour > 1)
                throw invalid_iterator();

            return *(node->makeUp);
        }
        bool operator==(const iterator &rhs) const {
            return node == rhs.node;
        }
        bool operator==(const const_iterator &rhs) const {
            return node == rhs.node;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return node != rhs.node;
        }
        bool operator!=(const const_iterator &rhs) const {
            return node != rhs.node;
        }

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
         */
        value_type *operator->() const noexcept {
            if (node->colour > 1)
                throw invalid_iterator();

            return node->makeUp;
        }
    };
    class const_iterator {
        // it should has similar member method as iterator.
        //  and it should be able to construct from an iterator.
        friend class map;
    private:
        // data members.
        RedBlackNode *node;
        const map *where;
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = map::value_type;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::output_iterator_tag;
        using iterator_assignable = my_false_type;

        const_iterator(RedBlackNode *n = nullptr, const map *w = nullptr) {
            // TODO
            node = n;
            where = w;
        }
        const_iterator(const const_iterator &other) {
            // TODO
            node = other.node;
            where = other.where;
        }
        const_iterator(const iterator &other) {
            // TODO
            node = other.node;
            where = other.where;
        }
        // And other methods in iterator.
        // And other methods in iterator.
        // And other methods in iterator.
        const_iterator operator++(int) {
            const_iterator it = *this;

            if (node->colour > 1)
                throw invalid_iterator();

            where->findNxt(node);
            return it;
        }
        /**
         * TODO ++iter
         */
        const_iterator &operator++() {
            if (node->colour > 1)
                throw invalid_iterator();

            where->findNxt(node);
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            const_iterator it = *this;
            RedBlackNode *ptr = node;
            where->findPre(ptr);

            if (ptr == nullptr)
                throw invalid_iterator();

            node = ptr;
            return it;
        }
        /**
         * TODO --iter
         */
        const_iterator &operator--() {
            RedBlackNode *ptr = node;
            where->findPre(ptr);

            if (ptr == nullptr)
                throw invalid_iterator();

            node = ptr;
            return *this;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        const value_type &operator*() const {
            if (node->colour > 1)
                throw invalid_iterator();

            return *(node->makeUp);
        }
        bool operator==(const iterator &rhs) const {
            return node == rhs.node;
        }
        bool operator==(const const_iterator &rhs) const {
            return node == rhs.node;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return node != rhs.node;
        }
        bool operator!=(const const_iterator &rhs) const {
            return node != rhs.node;
        }

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
         */
        const value_type *operator->() const noexcept {
            if (node->colour > 1)
                throw invalid_iterator();

            return node->makeUp;
        }
    };
    /**
     * TODO two constructors
     */
    map() {
        endptr = new RedBlackNode();
        root = nullptr;
    }
    map(const map &other) {
        endptr = new RedBlackNode();
        build(root = nullptr, other.root, nullptr);
    }
    /**
     * TODO assignment operator
     */
    map &operator=(const map &other) {
        if (this == &other)
            return *this;

        makeEmpty(root);
        build(root = nullptr, other.root, nullptr);
        return *this;
    }
    /**
     * TODO Destructors
     */
    ~map() {
        makeEmpty(root);
        root = nullptr;
        delete endptr;
    }
    /**
     * TODO
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key equivalent to key.
     * If no such element exists, an exception of type `index_out_of_bound'
     */
    T &at(const Key &key) {
        RedBlackNode *node = root;
        findKey(node, key);

        if (node)
            return node->makeUp->second;

        throw index_out_of_bound();
    }
    const T &at(const Key &key) const {
        RedBlackNode *node = root;
        findKey(node, key);

        if (node)
            return node->makeUp->second;

        throw index_out_of_bound();
    }
    /**
     * TODO
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to key,
     *   performing an insertion if such key does not already exist.
     */
    T &operator[](const Key &key) {
        RedBlackNode *node = root;
        findKey(node, key);

        if (node)
            return node->makeUp->second;

        return insert(value_type(key, T())).first->second;
    }
    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T &operator[](const Key &key) const {
        RedBlackNode *node = root;
        findKey(node, key);

        if (node)
            return node->makeUp->second;

        throw index_out_of_bound();
    }
    /**
     * return a iterator to the beginning
     */
    iterator begin() {
        RedBlackNode *node = root;

        if (!root)
            node = endptr;
        else
            findMin(node);

        return iterator(node, this);
    }
    const_iterator cbegin() const {
        RedBlackNode *node = root;

        if (!root)
            node = endptr;
        else
            findMin(node);

        return const_iterator(node, this);
    }
    /**
     * return a iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() {
        return iterator(endptr, this);
    }
    const_iterator cend() const {
        return const_iterator(endptr, this);
    }
    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const {
        return root == nullptr;
    }
    /**
     * returns the number of elements.
     */
    size_t size() const {
        if (root)
            return root->size;

        return 0;
    }
    /**
     * clears the contents
     */
    void clear() {
        makeEmpty(root);
        root = nullptr;
    }
    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    pair<iterator, bool> insert(const value_type &value) {
        if (!root) {
            root = new RedBlackNode(value);
            root->colour = 1;
            return pair<iterator, bool>(iterator(root, this), 1);
        }

        RedBlackNode *x = root;

        while (x) {
            if (Compare()(x->makeUp->first, value.first))
                x = x->son[1];
            else if (Compare()(value.first, x->makeUp->first))
                x = x->son[0];
            else
                return pair<iterator, bool>(iterator(x, this), 0);
        }

        x = root;
        RedBlackNode *y = nullptr;

        while (x) {
            x->size++, y = x;
            x = x->son[Compare()(x->makeUp->first, value.first)];
        }

        x = new RedBlackNode(value, y);
        y->son[Compare()(y->makeUp->first, value.first)] = x;
        RedBlackNode *z = x;

        while (y && y->father) {
            if (!x->colour && !y->colour)
                x = solveDoubleRed(y, identify(x)), y = x->father;
            else
                break;
        }

        root->colour = 1;
        return pair<iterator, bool>(iterator(z, this), 1);
    }
    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos) {
        if (pos == end() || pos.where != this)
            throw index_out_of_bound();

        RedBlackNode *x = root;
        Key k = pos->first;

        while (x) {
            if (Compare()(x->makeUp->first, k))
                x = x->son[1];
            else if (Compare()(k, x->makeUp->first))
                x = x->son[0];
            else
                break;
        }

        if (!x)
            throw index_out_of_bound();

        x = root;

        while (x) {
            x->size--;

            if (Compare()(x->makeUp->first, k))
                x = x->son[1];
            else if (Compare()(k, x->makeUp->first))
                x = x->son[0];
            else
                break;
        }

        if (x->son[0] && x->son[1]) {
            RedBlackNode *y = x->son[1];

            while (y->son[0])
                y->size--, y = y->son[0];

            y->size--;
            change(x, y);
        }

        if (x->son[0] || x->son[1]) {
            int xWhich = (x->son[0] == nullptr);

            if (x->father)
                x->son[xWhich]->father = x->father,
                                x->father->son[identify(x)] = x->son[xWhich],
                                        up(x->father);
            else
                root = x->son[xWhich], root->father = nullptr;

            x->son[xWhich]->colour = 1;
        } else {
            if (x->colour)
                removeReBalance(x);

            if (x->father)
                x->father->son[identify(x)] = nullptr,
                                              up(x->father);
            else
                root = nullptr;
        }

        delete x;
        x = nullptr;
    }
    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const {
        RedBlackNode *node = root;
        findKey(node, key);

        if (node)
            return 1;

        return 0;
    }
    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key &key) {
        RedBlackNode *node = root;
        findKey(node, key);

        if (node)
            return iterator(node, this);

        return end();
    }
    const_iterator find(const Key &key) const {
        RedBlackNode *node = root;
        findKey(node, key);

        if (node)
            return const_iterator(node, this);

        return cend();
    }
};

}

#endif
