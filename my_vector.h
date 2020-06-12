#ifndef BIGINT_VECTOR_H
#define BIGINT_VECTOR_H

#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

template <typename T>
struct vector {
    vector();

    vector(size_t);

    vector(size_t, T);

    vector(vector<T> const &);

    vector &operator=(vector<T> const &);

    size_t size() const;

    T back() const;

    T const &operator[](size_t) const;

    bool operator==(vector<T> const &) const;

    T &operator[](size_t);

    void pop_back();

    void push_back(T);

    void resize(size_t);

    ~vector();

    void swap(vector &);

private:
    bool is_small = true;

    union {
        std::shared_ptr<std::vector<T>> big_obj;
        T small_obj;
    };

    void inv();

};

template<typename T>
vector<T>::vector() : vector(1, 0) {}

template<typename T>
vector<T>::vector(size_t size) : vector(size, 0) {}

template<typename T>
vector<T>::vector(size_t size, T elem) {
    is_small = (size == 1);
    if (is_small) {
        small_obj = elem;
    } else {
        new(&big_obj) std::shared_ptr<std::vector<T>>(new std::vector<T>(size, elem));
    }
}

template<typename T>
vector<T>::vector(vector<T> const &other) {
    is_small = other.is_small;
    if (is_small) {
        small_obj = other.small_obj;
    } else {
        new(&big_obj) std::shared_ptr<std::vector<T>>(other.big_obj);
    }
}

template<typename T>
size_t vector<T>::size() const {
    return (is_small ? 1 : big_obj.get()->size());
}

template<typename T>
vector<T> &vector<T>::operator=(vector<T> const &other) {
    if (!is_small) {
        if (!other.is_small) {
            big_obj = other.big_obj;
        } else {
            big_obj.~shared_ptr();
            small_obj = other.small_obj;
        }
    } else {
        if (!other.is_small) {
            new(&big_obj) std::shared_ptr<std::vector<T>>(other.big_obj);
        } else {
            small_obj = other.small_obj;
        }
    }
    is_small = other.is_small;
    return *this;
}

template<typename T>
T vector<T>::back() const {
    if (is_small) return small_obj;
    return big_obj.get()->back();
}

template<typename T>
T const &vector<T>::operator[](size_t pos) const {
    return (is_small ? small_obj : (*big_obj.get())[pos]);
}

template<typename T>
bool vector<T>::operator==(vector<T> const &other) const {
    if (is_small != other.is_small) return false;
    if (is_small) return small_obj == other.small_obj;

    return *big_obj.get() == *other.big_obj.get();
}

template<typename T>
T &vector<T>::operator[](size_t i) {
    if (is_small) return small_obj;
    inv();
    return (*big_obj.get())[i];
}

template<typename T>
void vector<T>::pop_back() {
    if (is_small) {
        small_obj = 0;
        return;
    }
    inv();
    if (big_obj.get()->size() == 2) {
        is_small = true;
        T tmp = (*big_obj.get())[0];
        big_obj.reset();
        small_obj = tmp;
        return;
    }
    big_obj.get()->pop_back();
}

template<typename T>
void vector<T>::push_back(T x) {
    if (is_small) {
        is_small = false;
        new(&big_obj) std::shared_ptr<std::vector<T>>(new std::vector<T>(1, small_obj));
        big_obj.get()->push_back(x);
    } else {
        inv();
        big_obj.get()->push_back(x);
    }
}

template<typename T>
void vector<T>::resize(size_t size) {
    if (size > 1) {
        if (!is_small) {
            (*big_obj.get()).resize(size);
        } else {
            vector tmp(size);
            tmp[0] = small_obj;
            swap(tmp);
            is_small = false;
        }
    }
}


template<typename T>
void vector<T>::inv() {
    if (!big_obj.unique()) {
        big_obj.reset(new std::vector<T>(*big_obj.get()));
    }
}

template<typename T>
vector<T>::~vector() {
    if (!is_small)
        big_obj.~shared_ptr();
}

template<typename T>
void vector<T>::swap(vector<T> &other) {
    vector tmp(other);
    other = *this;
    *this = tmp;
}
#endif //BIGINT_VECTOR_H