/*
 * libherdstat -- util/container_base.hh
 * $Id: container_base.hh 223 2006-01-12 11:40:28Z ka0ttic $
 * Copyright (c) 2005 Aaron Walker <ka0ttic@gentoo.org>
 *
 * This file is part of libherdstat.
 *
 * libherdstat is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * libherdstat is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * libherdstat; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 325, Boston, MA  02111-1257  USA
 */

#ifndef _HAVE_CONTAINER_BASE_HH
#define _HAVE_CONTAINER_BASE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vector>
#include <map>
#include <set>
#include <functional>

/**
 * @file herdstat/util/container_base.hh
 * @brief Defines the ContainerBase base class template.
 */

    /**
     * @class ContainerBase container_base.hh herdstat/util/container_base.hh
     * @brief Base for classes who should act like standard containers.
     *
     * @section overview Overview
     *
     * The ContainerBase family of class templates are meant to be used as base
     * classes for objects that want to mimic the interface of standard
     * containers.  Why not just inherit from the standard container itself you
     * may ask?  Because standard containers were not designed to be base
     * classes and thus do not declare their destructors virtual.  Using these
     * templates work around this because they don't inherit from the containers
     * but instead just wrap them.
     *
     * None of these classes are meant to be used standalone, thus
     * the reason why their constructors are declared protected.
     *
     * @section usage Usage
     *
     * To use the ContainerBase family of class templates, simply inherit from
     * the appropriate ContainerBase derivative.  For example, if your class
     * needs to mimic a std::vector<std::string>, you'd have it inherit from
     * VectorBase<std::string>.
     *
     * There may be some cases in which the underlying container itself is
     * needed (iterators won't do).  Using an insert iterator inside the class
     * that is inheriting from a ContainerBase derivative, is one example.  In
     * these cases, you can use the container() member function, which returns a
     * reference to the underlying container.  See the example below to see it
     * in action.
     *
     * @section example Example
     *
     * Below is a simple example of using VectorBase to mimic a std::vector:
     *
@code
class Foo : public herdstat::util::VectorBase<std::string>
{
    public:
        Foo(const container_type& c)
            : herdstat::util::VectorBase<std::string>(c) { }

        bool is_foo() const;
        void bar();

    private:
        template <typename InputIterator>
        void fill_container(InputIterator first, InputIterator last);
};

template <typename InputIterator>
void
Foo::fill_container(InputIterator first, InputIterator last)
{
    std::transform(first, last,
        std::back_inserter(this->container()),
        std::bind2nd(std::plus<std::string>(), "!"));
}
...
Foo v;
v.push_back("foo");
v.push_back("bar");
v.insert(v.end(),
    std::istream_iterator<std::string>(stream),
    std::istream_iterator<std::string>());
@endcode
     *
     */

    template <typename C>
    class ContainerBase
    {
        public:
            /// underlying container type.
            typedef C container_type;
            ///@{
            /// container_type type fowards
            typedef typename container_type::iterator iterator;
            typedef typename container_type::const_iterator const_iterator;
            typedef typename container_type::reverse_iterator reverse_iterator;
            typedef typename container_type::const_reverse_iterator const_reverse_iterator;
            typedef typename container_type::reference reference;
            typedef typename container_type::const_reference const_reference;
            typedef typename container_type::value_type value_type;
            typedef typename container_type::size_type size_type;
            typedef typename container_type::difference_type difference_type;
            ///@}

            /// Destructor.
            virtual ~ContainerBase();

            //@{
            /// Operations common among all standard containers.

            inline container_type& operator= (const container_type& c)
            { this->swap(c); }

            inline iterator begin() { return _c.begin(); }
            inline const_iterator begin() const { return _c.begin(); }
            inline iterator end() { return _c.end(); }
            inline const_iterator end() const { return _c.end(); }
            inline reverse_iterator rbegin() { return _c.rbegin(); }
            inline const_reverse_iterator rbegin() const { return _c.rbegin(); }
            inline reverse_iterator rend() { return _c.rend(); }
            inline const_reverse_iterator rend() const { return _c.rend(); }

            inline size_type size() const { return _c.size(); }
            inline size_type max_size() const { return _c.max_size(); }

            inline bool empty() const { return _c.empty(); }
            inline void clear() { _c.clear(); }
            inline void swap(const container_type& c) { _c.swap(c); }
            //@}
    
        protected:
            /// Default constructor.
            ContainerBase();

            /** Constructor.
             * @param c const reference to container_type.
             */
            explicit ContainerBase(const container_type& c);

            /** Constructor.  Initialize with range [begin,end).
             * @param begin Beginning iterator.
             * @param end Ending iterator.
             */
            template <typename In>
            ContainerBase(In begin, In end);

            //@{
            /// For derived classes to use when the container itself is needed.
            inline container_type& container() { return _c; }
            inline const container_type& container() const { return _c; }
            //@}

        private:
            container_type _c;
    };

    template <typename T>
    ContainerBase<T>::ContainerBase()
        : _c()
    {
    }

    template <typename T>
    ContainerBase<T>::ContainerBase(const container_type& c)
        : _c(c)
    {
    }

    template <typename T>
    template <typename In>
    ContainerBase<T>::ContainerBase(In begin, In end)
        : _c(begin, end)
    {
    }

    template <typename T>
    ContainerBase<T>::~ContainerBase()
    {
    }

    /**
     * @class VectorBase container_base.hh herdstat/util/container_base.hh
     * @brief Base class template for classes acting like vector<T>.
     * @see ContainerBase documentation.
     */

    template <typename T>
    class VectorBase : public ContainerBase<std::vector<T> >
    {
        public:
            typedef ContainerBase<std::vector<T> > base_type;
            typedef typename base_type::container_type container_type;
            typedef typename container_type::size_type size_type;
            typedef typename container_type::difference_type difference_type;
            typedef typename container_type::value_type value_type;
            typedef typename container_type::iterator iterator;
            typedef typename container_type::const_iterator const_iterator;

            virtual ~VectorBase();

            template <typename In>
            inline void assign(In begin, In end)
            { this->container().assign(begin, end); }

            inline void assign(size_type n, const value_type& v)
            { this->container().assign(n, v); }

            inline void resize(size_type sz, value_type c = value_type())
            { this->container().resize(sz, c); }

            inline size_type capacity() const { return this->container().capacity(); }

            inline void reserve(size_type n) { this->container().reserve(n); }

            inline value_type& operator[](size_type n)
            { return (this->container())[n]; }
            inline const value_type& operator[](size_type n) const
            { return (this->container())[n]; }
            inline const value_type& at(size_type n) { return this->container().at(n); }

            inline value_type& front() { return this->container().front(); }
            inline const value_type& front() const { return this->container().front(); }
            inline value_type& back() { return this->container().back(); }
            inline const value_type& back() const { return this->container().back(); }

            inline void push_back(const value_type& v)
            { this->container().push_back(v); }
            inline void pop_back() { this->container().pop_back(); }

            inline iterator insert(iterator pos, const value_type& v)
            { return this->container().insert(pos, v); }

            inline void insert(iterator pos, size_type n, const value_type& v)
            { this->container().insert(pos, n, v); }

            inline iterator erase(iterator pos) { return this->container().erase(pos); }
            inline iterator erase(iterator begin, iterator end)
            { return this->container().erase(begin, end); }

            template <typename In>
            inline void insert(iterator pos, In begin, In end)
            { this->container().insert(pos, begin, end); }

        protected:
            VectorBase();
            VectorBase(const container_type& c);
            template <typename In>
            VectorBase(In begin, In end);
    };

    template <typename T>
    VectorBase<T>::VectorBase()
    {
    }

    template <typename T>
    VectorBase<T>::VectorBase(const container_type& c)
        : base_type(c)
    {
    }
    
    template <typename T>
    template <typename In>
    VectorBase<T>::VectorBase(In begin, In end)
        : base_type(begin, end)
    {
    }

    template <typename T>
    VectorBase<T>::~VectorBase()
    {
    }

    /**
     * @class SetBase container_base.hh herdstat/util/container_base.hh
     * @brief Base class template for classes that act like a set<T>.
     * @see ContainerBase documentation.
     */

    template <typename T, typename Compare = std::less<T> >
    class SetBase : public ContainerBase<std::set<T, Compare> >
    {
        public:
            typedef ContainerBase<std::set<T> > base_type;
            typedef typename base_type::container_type container_type;
            typedef typename container_type::size_type size_type;
            typedef typename container_type::difference_type difference_type;
            typedef typename container_type::value_type value_type;
            typedef typename container_type::iterator iterator;
            typedef typename container_type::const_iterator const_iterator;
            typedef value_type key_type;
            typedef Compare key_compare;
            typedef Compare value_compare;

            virtual ~SetBase();

            inline std::pair<iterator, bool> insert(const value_type& v)
            { return this->container().insert(v); }
            inline iterator insert(iterator hintpos, const value_type& v)
            { return this->container().insert(hintpos, v); }
            template <typename In>
            inline void insert(In begin, In end)
            { this->container().insert(begin, end); }

            inline void erase(iterator pos) { this->container().erase(pos); }
            inline size_type erase(const key_type& k)
            { return this->container().erase(k); }
            inline void erase(iterator begin, iterator end)
            { this->container().erase(begin, end); }

            inline key_compare key_comp() const
            { return this->container().key_comp(); }
            inline value_compare value_comp() const
            { return this->container().value_comp(); }

            inline iterator find(const key_type& k)
            { return this->container().find(k); }
            inline const_iterator find(const key_type& k) const
            { return this->container().find(k); }
            inline size_type count(const key_type& k) const
            { return this->container().count(k); }

            inline iterator lower_bound(const key_type& k) const
            { return this->container().lower_bound(k); }
            inline iterator upper_bound(const key_type& k) const
            { return this->container().upper_bound(k); }
            inline std::pair<iterator, iterator> equal_range(const key_type& k) const
            { return this->container().equal_range(k); }

        protected:
            SetBase();
            SetBase(const container_type& c);
            template <class In>
            SetBase(In begin, In end);
    };

    template <typename T, typename Compare>
    SetBase<T, Compare>::SetBase()
    {
    }

    template <typename T, typename Compare>
    SetBase<T, Compare>::SetBase(const container_type& c)
        : base_type(c)
    {
    }
    
    template <typename T, typename Compare>
    template <class In>
    SetBase<T, Compare>::SetBase(In begin, In end)
        : base_type(begin, end)
    {
    }

    template <typename T, typename Compare>
    SetBase<T, Compare>::~SetBase()
    {
    }

    /**
     * @class MapBase container_base.hh herdstat/util/container_base.hh
     * @brief Base class template for classes that act like a map<K,V>.
     * @see ContainerBase documentation.
     */

    template <typename K, typename V, typename Compare = std::less<K> >
    class MapBase : public ContainerBase<std::map<K, V, Compare> >
    {
        public:
            typedef ContainerBase<std::map<K, V, Compare> > base_type;
            typedef typename base_type::container_type container_type;
            typedef typename container_type::size_type size_type;
            typedef typename container_type::difference_type difference_type;
            typedef typename container_type::value_type value_type;
            typedef typename container_type::iterator iterator;
            typedef typename container_type::const_iterator const_iterator;
            typedef K key_type;
            typedef V mapped_type;
            typedef Compare key_compare;
            typedef typename container_type::value_compare value_compare;

            virtual ~MapBase();

            inline mapped_type& operator[](const key_type& k)
            { return (this->container())[k]; }

            inline std::pair<iterator, bool> insert(const value_type& v)
            { return this->container().insert(v); }
            inline iterator insert(iterator hintpos, const value_type& v)
            { return this->container().insert(hintpos, v); }
            template <typename In>
            inline void insert(In begin, In end)
            { this->container().insert(begin, end); }

            inline void erase(iterator pos) { this->container().erase(pos); }
            inline size_type erase(const key_type& k)
            { return this->container().erase(k); }
            inline void erase(iterator begin, iterator end)
            { this->container().erase(begin, end); }

            inline key_compare key_comp() const
            { return this->container().key_comp(); }
            inline value_compare value_comp() const
            { return this->container().value_comp(); }

            inline iterator find(const key_type& k)
            { return this->container().find(k); }
            inline const_iterator find(const key_type& k) const
            { return this->container().find(k); }

            inline size_type count(const key_type& k) const
            { return this->container().count(k); }

            inline iterator lower_bound(const key_type& k)
            { return this->container().lower_bound(k); }
            inline const_iterator lower_bound(const key_type& k) const
            { return this->container().lower_bound(k); }
            inline iterator upper_bound(const key_type& k)
            { return this->container().upper_bound(k); }
            inline const_iterator upper_bound(const key_type& k) const
            { return this->container().upper_bound(k); }
            inline std::pair<iterator, iterator> equal_range(const key_type& k)
            { return this->container().equal_range(k); }
            inline std::pair<const_iterator, const_iterator>
            equal_range(const key_type& k) const
            { return this->container().equal_range(k); }

        protected:
            MapBase();
            MapBase(const container_type& c);
            template <typename In>
            MapBase(In begin, In end);
    };

    template <typename K, typename V, typename Compare>
    MapBase<K,V,Compare>::MapBase()
    {
    }
    
    template <typename K, typename V, typename Compare>
    MapBase<K,V,Compare>::MapBase(const container_type& c) 
        : base_type(c)
    {
    }
    
    template <typename K, typename V, typename Compare>
    template <typename In>
    MapBase<K,V,Compare>::MapBase(In begin, In end)
        : base_type(begin, end)
    {
    }

    template <typename K, typename V, typename Compare>
    MapBase<K,V,Compare>::~MapBase()
    {
    }

#endif /* _HAVE_CONTAINER_BASE_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
