/**
 * implement a container like std::linked_hashmap
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
/**
 * In linked_hashmap, iteration ordering is differ from map,
 * which is the order in which keys were inserted into the map.
 * You should maintain a doubly-linked list running through all
 * of its entries to keep the correct iteration order.
 *
 * Note that insertion order is not affected if a key is re-inserted
 * into the map.
 */

    template<
            class Key,
            class T,
            class Hash = std::hash<Key>,
            class Equal = std::equal_to<Key>
    >
    class linked_hashmap {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::linked_hashmap as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
    private:
        struct node {
            value_type *makeUp;
            node *pre, *nxt, *next;

            node() {
                makeUp = nullptr;
                pre = nullptr;
                nxt = nullptr;
                next = nullptr;
            }

            node(const value_type &v) {
                makeUp = new value_type(v);
                pre = nullptr;
                nxt = nullptr;
                next = nullptr;
            }

            ~node() {
                if (makeUp)
                    delete makeUp;
            }
        };

        node *head, *tail, **arr;
        int capacity, n;

        node *Insert(const value_type &value) {
            n++;

            if (n > capacity * 0.75)
                doublespace();

            node *ptr = new node(value);
            ptr->pre = tail->pre;
            tail->pre->nxt = ptr;
            ptr->nxt = tail;
            tail->pre = ptr;
            int mod = (Hash()(value.first)) & (capacity - 1);
            ptr->next = arr[mod]->next;
            arr[mod]->next = ptr;
            return ptr;
        }

        void doublespace() {
            for (int i = 0; i < capacity; i++)
                delete arr[i];

            capacity <<= 1;
            delete[] arr;
            arr = new node *[capacity];

            for (int i = 0; i < capacity; i++)
                arr[i] = new node;

            node *ptr = head->nxt;

            while (ptr != tail) {
                int mod = (Hash()(ptr->makeUp->first)) & (capacity - 1);
                ptr->next = arr[mod]->next;
                arr[mod]->next = ptr;
                ptr = ptr->nxt;
            }
        }

        void halfspace() {
            for (int i = 0; i < capacity; i++)
                delete arr[i];

            capacity >>= 1;
            delete[] arr;
            arr = new node *[capacity];

            for (int i = 0; i < capacity; i++)
                arr[i] = new node;

            node *ptr = head->nxt;

            while (ptr != tail) {
                int mod = (Hash()(ptr->makeUp->first)) & (capacity - 1);
                ptr->next = arr[mod]->next;
                arr[mod]->next = ptr;
                ptr = ptr->nxt;
            }
        }

    public:
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = linked_hashmap.begin(); --it;
         *       or it = linked_hashmap.end(); ++end();
         */
        class const_iterator;

        class iterator {
            friend class linked_hashmap;

        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            node *ptr;
            linked_hashmap *where;
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
            using value_type = linked_hashmap::value_type;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = std::output_iterator_tag;


            iterator(node *p = nullptr, linked_hashmap *w = nullptr) {
                ptr = p;
                where = w;
                // TODO
            }

            iterator(const iterator &other) {
                // TODO
                ptr = other.ptr;
                where = other.where;
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator it = *this;
                operator++();
                return it;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                if (*this == where->end())
                    throw invalid_iterator();

                ptr = ptr->nxt;
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator it = *this;
                operator--();
                return it;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                if (*this == where->begin())
                    throw invalid_iterator();

                ptr = ptr->pre;
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                if (*this == where->end())
                    throw invalid_iterator();

                return *(ptr->makeUp);
            }

            bool operator==(const iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator==(const const_iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return ptr != rhs.ptr;
            }

            bool operator!=(const const_iterator &rhs) const {
                return ptr != rhs.ptr;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                return ptr->makeUp;
            }
        };

        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
            friend class linked_hashmap;

        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            node *ptr;
            const linked_hashmap *where;
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
            using value_type = linked_hashmap::value_type;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = std::output_iterator_tag;


            const_iterator(node *p = nullptr, const linked_hashmap *w = nullptr) {
                ptr = p;
                where = w;
                // TODO
            }

            const_iterator(const const_iterator &other) {
                // TODO
                ptr = other.ptr;
                where = other.where;
            }

            const_iterator(const iterator &other) {
                // TODO
                ptr = other.ptr;
                where = other.where;
            }

            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                const_iterator it = *this;
                operator++();
                return it;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                if (*this == where->cend())
                    throw invalid_iterator();

                ptr = ptr->nxt;
                return *this;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                const_iterator it = *this;
                operator--();
                return it;
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                if (*this == where->cbegin())
                    throw invalid_iterator();

                ptr = ptr->pre;
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            const value_type &operator*() const {
                if (*this == where->cend())
                    throw invalid_iterator();

                return *(ptr->makeUp);
            }

            bool operator==(const iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator==(const const_iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return ptr != rhs.ptr;
            }

            bool operator!=(const const_iterator &rhs) const {
                return ptr != rhs.ptr;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            const value_type *operator->() const noexcept {
                return ptr->makeUp;
            }
        };

        /**
         * TODO two constructors
         */
        linked_hashmap() {
            capacity = 16;
            n = 0;
            arr = new node *[capacity];

            for (int i = 0; i < capacity; i++)
                arr[i] = new node;

            head = new node;
            tail = new node;
            head->nxt = tail;
            tail->pre = head;
        }

        linked_hashmap(const linked_hashmap &other) {
            capacity = other.capacity;
            n = 0;
            head = new node;
            tail = new node;
            head->nxt = tail;
            tail->pre = head;
            arr = new node *[capacity];

            for (int i = 0; i < capacity; i++)
                arr[i] = new node;

            node *ptr = other.head->nxt;

            while (ptr != other.tail)
                Insert(*(ptr->makeUp)), ptr = ptr->nxt;
        }

        /**
         * TODO assignment operator
         */
        linked_hashmap &operator=(const linked_hashmap &other) {
            if (this == &other)
                return *this;

            clear();

            for (int i = 0; i < capacity; i++)
                delete arr[i];

            capacity = other.capacity;
            n = 0;
            head->nxt = tail;
            tail->pre = head;
            delete[] arr;
            arr = new node *[capacity];

            for (int i = 0; i < capacity; i++)
                arr[i] = new node;

            node *ptr = other.head->nxt;

            while (ptr != other.tail)
                Insert(*(ptr->makeUp)), ptr = ptr->nxt;

            return *this;
        }

        /**
         * TODO Destructors
         */
        ~linked_hashmap() {
            clear();
            delete head;
            delete tail;

            for (int i = 0; i < capacity; i++)
                delete arr[i];

            delete[] arr;
        }

        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {
            int mod = (Hash()(key)) & (capacity - 1);

            for (node *ptr = arr[mod]->next; ptr; ptr = ptr->next)
                if (Equal()(key, ptr->makeUp->first))
                    return ptr->makeUp->second;

            throw index_out_of_bound();
        }

        const T &at(const Key &key) const {
            int mod = (Hash()(key)) & (capacity - 1);

            for (node *ptr = arr[mod]->next; ptr; ptr = ptr->next)
                if (Equal()(key, ptr->makeUp->first))
                    return ptr->makeUp->second;

            throw index_out_of_bound();
        }

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            int mod = (Hash()(key)) & (capacity - 1);

            for (node *ptr = arr[mod]->next; ptr; ptr = ptr->next)
                if (Equal()(key, ptr->makeUp->first))
                    return ptr->makeUp->second;

            return insert(value_type(key, T())).first->second;
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            int mod = (Hash()(key)) & (capacity - 1);

            for (node *ptr = arr[mod]->next; ptr; ptr = ptr->next)
                if (Equal()(key, ptr->makeUp->first))
                    return ptr->makeUp->second;

            throw index_out_of_bound();
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return iterator(head->nxt, this);
        }

        const_iterator cbegin() const {
            return const_iterator(head->nxt, this);
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(tail, this);
        }

        const_iterator cend() const {
            return const_iterator(tail, this);
        }

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return n == 0;
        }

        /**
         * returns the number of elements.
         */
        size_t size() const {
            return n;
        }

        /**
         * clears the contents
         */
        void clear() {
            while (head->nxt != tail) {
                node *ptr = head->nxt;
                ptr->nxt->pre = head;
                head->nxt = ptr->nxt;
                delete ptr;
            }

            for (int i = 0; i < capacity; i++)
                delete arr[i];

            n = 0;
            capacity = 16;
            delete[] arr;
            arr = new node *[capacity];

            for (int i = 0; i < capacity; i++)
                arr[i] = new node;
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            int mod = (Hash()(value.first)) & (capacity - 1);

            for (node *ptr = arr[mod]->next; ptr; ptr = ptr->next)
                if (Equal()(value.first, ptr->makeUp->first))
                    return pair<iterator, bool>(iterator(ptr, this), 0);

            return pair<iterator, bool>(iterator(Insert(value), this), 1);
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos == end() || pos.where != this)
                throw index_out_of_bound();

            int mod = (Hash()(pos->first)) & (capacity - 1);

            for (node *lst = arr[mod], *ptr = lst->next; ptr; lst = ptr, ptr = ptr->next)
                if (Equal()(ptr->makeUp->first, pos->first)) {
                    lst->next = ptr->next;
                    ptr->pre->nxt = ptr->nxt;
                    ptr->nxt->pre = ptr->pre;
                    delete ptr;
                    break;
                }

            n--;

            if (capacity > 16 && n < capacity / 4)
                halfspace();
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         */
        size_t count(const Key &key) const {
            int mod = (Hash()(key)) & (capacity - 1);

            for (node *ptr = arr[mod]->next; ptr; ptr = ptr->next)
                if (Equal()(key, ptr->makeUp->first))
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
            int mod = (Hash()(key)) & (capacity - 1);

            for (node *ptr = arr[mod]->next; ptr; ptr = ptr->next)
                if (Equal()(key, ptr->makeUp->first))
                    return iterator(ptr, this);

            return end();
        }

        const_iterator find(const Key &key) const {
            int mod = (Hash()(key)) & (capacity - 1);

            for (node *ptr = arr[mod]->next; ptr; ptr = ptr->next)
                if (Equal()(key, ptr->makeUp->first))
                    return const_iterator(ptr, this);

            return cend();
        }
    };

}

#endif
