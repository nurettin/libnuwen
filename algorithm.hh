// Copyright Stephan T. Lavavej, http://nuwen.net .
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://boost.org/LICENSE_1_0.txt .

#ifndef PHAM_ALGORITHM_HH
#define PHAM_ALGORITHM_HH

#include "compiler.hh"

#ifdef NUWEN_PLATFORM_MSVC
    #pragma once
#endif

#include "external_begin.hh"
    #include <algorithm>
    #include <iterator>
    #include <list>
    #include <boost/mem_fn.hpp>
#include "external_end.hh"

namespace nuwen {
    template <typename InIt, typename OutIt, typename Pred> OutIt copy_if(InIt first, InIt last, OutIt result, Pred pred);

    template <typename C> C&            universal_sort(C& container);
    template <typename T> std::list<T>& universal_sort(std::list<T>& l);

    template <typename C, typename F> C&            universal_sort(C& container,    F comparator);
    template <typename T, typename F> std::list<T>& universal_sort(std::list<T>& l, F comparator);

    template <typename C>             C universal_sort_copy(const C& container);
    template <typename C, typename F> C universal_sort_copy(const C& container, F comparator);

    template <typename C> C&            universal_stable_sort(C& container);
    template <typename T> std::list<T>& universal_stable_sort(std::list<T>& l);

    template <typename C, typename F> C&            universal_stable_sort(C& container,    F comparator);
    template <typename T, typename F> std::list<T>& universal_stable_sort(std::list<T>& l, F comparator);

    template <typename C>             C universal_stable_sort_copy(const C& container);
    template <typename C, typename F> C universal_stable_sort_copy(const C& container, F comparator);

    template <typename C, typename T> C& nuke     (      C& container, const T& value);
    template <typename C, typename T> C  nuke_copy(const C& container, const T& value);

    template <typename C, typename P> C& nuke_if     (      C& container, P pred);
    template <typename C, typename P> C  nuke_copy_if(const C& container, P pred);

    template <typename C>                         C& nuke_dupes(C& container);
    template <typename C, typename F, typename B> C& nuke_dupes(C& container, F comparator, B binary_pred);

    template <typename C>                         C nuke_dupes_copy(const C& container);
    template <typename C, typename F, typename B> C nuke_dupes_copy(const C& container, F comparator, B binary_pred);

    template <typename C>                         C& stable_nuke_dupes(C& container);
    template <typename C, typename F, typename B> C& stable_nuke_dupes(C& container, F comparator, B binary_pred);

    template <typename C>                         C stable_nuke_dupes_copy(const C& container);
    template <typename C, typename F, typename B> C stable_nuke_dupes_copy(const C& container, F comparator, B binary_pred);


    template <typename InIt, typename OutIt, typename MemR, typename MemT>
        OutIt copy_if(InIt first, InIt last, OutIt result, MemR (MemT::* pred)() const);

    template <typename C, typename MemR, typename MemT>
        C& nuke_if(C& container, MemR (MemT::* pred)() const);

    template <typename C, typename MemR, typename MemT>
        C nuke_copy_if(const C& container, MemR (MemT::* pred)() const);

    template <typename InIt, typename MemR, typename MemT>
        void for_each(InIt first, InIt last, MemR (MemT::* fxn)());

    template <typename InIt, typename MemR, typename MemT>
        InIt find_if(InIt first, InIt last, MemR (MemT::* pred)() const);

    template <typename InIt, typename MemR, typename MemT>
        typename std::iterator_traits<InIt>::difference_type
        count_if(InIt first, InIt last, MemR (MemT::* pred)() const);

    template <typename InIt, typename OutIt, typename MemR, typename MemT>
        OutIt transform(InIt first, InIt last, OutIt result, MemR (MemT::* op)() const);

    template <typename FwdIt, typename MemR, typename MemT, typename T>
        void replace_if(FwdIt first, FwdIt last, MemR (MemT::* pred)() const, const T& new_value);

    template <typename It, typename OutIt, typename MemR, typename MemT, typename T>
        OutIt replace_copy_if(It first, It last, OutIt result, MemR (MemT::* pred)() const, const T& new_value);

    template <typename BiIt, typename MemR, typename MemT>
        BiIt partition(BiIt first, BiIt last, MemR (MemT::* pred)() const);

    template <typename BiIt, typename MemR, typename MemT>
        BiIt stable_partition(BiIt first, BiIt last, MemR (MemT::* pred)() const);
}

template <typename InIt, typename OutIt, typename Pred> OutIt nuwen::copy_if(InIt first, const InIt last, OutIt result, const Pred pred) {
    while (first != last) {
        if (pred(*first)) {
            *result++ = *first++;
        } else {
            ++first;
        }
    }

    return result;
}

template <typename C> C& nuwen::universal_sort(C& container) {
    std::sort(container.begin(), container.end());
    return container;
}

template <typename T> std::list<T>& nuwen::universal_sort(std::list<T>& l) {
    l.sort();
    return l;
}

template <typename C, typename F> C& nuwen::universal_sort(C& container, const F comparator) {
    std::sort(container.begin(), container.end(), comparator);
    return container;
}

template <typename T, typename F> std::list<T>& nuwen::universal_sort(std::list<T>& l, const F comparator) {
    l.sort(comparator);
    return l;
}

template <typename C> C nuwen::universal_sort_copy(const C& container) {
    C ret(container);
    return universal_sort(ret);
}

template <typename C, typename F> C nuwen::universal_sort_copy(const C& container, const F comparator) {
    C ret(container);
    return universal_sort(ret, comparator);
}

template <typename C> C& nuwen::universal_stable_sort(C& container) {
    std::stable_sort(container.begin(), container.end());
    return container;
}

template <typename T> std::list<T>& nuwen::universal_stable_sort(std::list<T>& l) {
    l.sort();
    return l;
}

template <typename C, typename F> C& nuwen::universal_stable_sort(C& container, const F comparator) {
    std::stable_sort(container.begin(), container.end(), comparator);
    return container;
}

template <typename T, typename F> std::list<T>& nuwen::universal_stable_sort(std::list<T>& l, const F comparator) {
    l.sort(comparator);
    return l;
}

template <typename C> C nuwen::universal_stable_sort_copy(const C& container) {
    C ret(container);
    return universal_stable_sort(ret);
}

template <typename C, typename F> C nuwen::universal_stable_sort_copy(const C& container, const F comparator) {
    C ret(container);
    return universal_stable_sort(ret, comparator);
}

template <typename C, typename T> C& nuwen::nuke(C& container, const T& value) {
    container.erase(std::remove(container.begin(), container.end(), value), container.end());
    return container;
}

template <typename C, typename T> C nuwen::nuke_copy(const C& container, const T& value) {
    C ret(container);
    return nuke(ret, value);
}

template <typename C, typename P> C& nuwen::nuke_if(C& container, const P pred) {
    container.erase(std::remove_if(container.begin(), container.end(), pred), container.end());
    return container;
}

template <typename C, typename P> C nuwen::nuke_copy_if(const C& container, const P pred) {
    C ret(container);
    return nuke_if(ret, pred);
}

template <typename C> C& nuwen::nuke_dupes(C& container) {
    universal_sort(container);
    container.erase(std::unique(container.begin(), container.end()), container.end());
    return container;
}

template <typename C, typename F, typename B> C& nuwen::nuke_dupes(C& container, const F comparator, const B binary_pred) {
    universal_sort(container, comparator);
    container.erase(std::unique(container.begin(), container.end(), binary_pred), container.end());
    return container;
}

template <typename C> C nuwen::nuke_dupes_copy(const C& container) {
    C ret(container);
    return nuke_dupes(ret);
}

template <typename C, typename F, typename B> C nuwen::nuke_dupes_copy(const C& container, const F comparator, const B binary_pred) {
    C ret(container);
    return nuke_dupes(ret, comparator, binary_pred);
}

template <typename C> C& nuwen::stable_nuke_dupes(C& container) {
    universal_stable_sort(container);
    container.erase(std::unique(container.begin(), container.end()), container.end());
    return container;
}

template <typename C, typename F, typename B> C& nuwen::stable_nuke_dupes(C& container, const F comparator, const B binary_pred) {
    universal_stable_sort(container, comparator);
    container.erase(std::unique(container.begin(), container.end(), binary_pred), container.end());
    return container;
}

template <typename C> C nuwen::stable_nuke_dupes_copy(const C& container) {
    C ret(container);
    return stable_nuke_dupes(ret);
}

template <typename C, typename F, typename B> C nuwen::stable_nuke_dupes_copy(const C& container, const F comparator, const B binary_pred) {
    C ret(container);
    return stable_nuke_dupes(ret, comparator, binary_pred);
}


template <typename InIt, typename OutIt, typename MemR, typename MemT>
    OutIt nuwen::copy_if(InIt first, InIt last, OutIt result, MemR (MemT::* const pred)() const) {

    return copy_if(first, last, result, boost::mem_fn(pred));
}

template <typename C, typename MemR, typename MemT>
    C& nuwen::nuke_if(C& container, MemR (MemT::* const pred)() const) {

    return nuke_if(container, boost::mem_fn(pred));
}

template <typename C, typename MemR, typename MemT>
    C nuwen::nuke_copy_if(const C& container, MemR (MemT::* const pred)() const) {

    return nuke_copy_if(container, boost::mem_fn(pred));
}

template <typename InIt, typename MemR, typename MemT>
    void nuwen::for_each(InIt first, InIt last, MemR (MemT::* const fxn)()) {

    std::for_each(first, last, boost::mem_fn(fxn));
}

template <typename InIt, typename MemR, typename MemT>
    InIt nuwen::find_if(InIt first, InIt last, MemR (MemT::* const pred)() const) {

    return std::find_if(first, last, boost::mem_fn(pred));
}

template <typename InIt, typename MemR, typename MemT>
    typename std::iterator_traits<InIt>::difference_type
    nuwen::count_if(InIt first, InIt last, MemR (MemT::* const pred)() const) {

    return std::count_if(first, last, boost::mem_fn(pred));
}

template <typename InIt, typename OutIt, typename MemR, typename MemT>
    OutIt nuwen::transform(InIt first, InIt last, OutIt result, MemR (MemT::* const op)() const) {

    return std::transform(first, last, result, boost::mem_fn(op));
}

template <typename FwdIt, typename MemR, typename MemT, typename T>
    void nuwen::replace_if(FwdIt first, FwdIt last, MemR (MemT::* const pred)() const, const T& new_value) {

    std::replace_if(first, last, boost::mem_fn(pred), new_value);
}

template <typename It, typename OutIt, typename MemR, typename MemT, typename T>
    OutIt nuwen::replace_copy_if(It first, It last, OutIt result, MemR (MemT::* const pred)() const, const T& new_value) {

    return std::replace_copy_if(first, last, result, boost::mem_fn(pred), new_value);
}

template <typename BiIt, typename MemR, typename MemT>
    BiIt nuwen::partition(BiIt first, BiIt last, MemR (MemT::* const pred)() const) {

    return std::partition(first, last, boost::mem_fn(pred));
}

template <typename BiIt, typename MemR, typename MemT>
    BiIt nuwen::stable_partition(BiIt first, BiIt last, MemR (MemT::* const pred)() const) {

    return std::stable_partition(first, last, boost::mem_fn(pred));
}

#endif // Idempotency
