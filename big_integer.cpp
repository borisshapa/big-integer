#include "big_integer.h"

#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <iostream>

big_integer::big_integer() : number(1), sign(false) {
    number[0] = 0;
}

big_integer::big_integer(big_integer const &other) : number(other.number), sign(other.sign) {}

big_integer::big_integer(int a) : number(1), sign(a < 0) {
    number[0] = (a == INT32_MIN) ? (uint32_t) a : abs(a);
}

big_integer::big_integer(uint32_t a) : number(1), sign(false) {
    number[0] = a;
}

big_integer::big_integer(std::string const &str) : number(1) {
    if (str.empty())
        return;
    number[0] = 0;
    sign = false;
    for (size_t i = (str[0] == '-'); i < str.size(); i++) {
        *this *= 10;
        *this += str[i] - '0';
    }
    sign = (*this != 0 && str[0] == '-');
}

big_integer::~big_integer() = default;


big_integer &big_integer::operator=(big_integer const &other) {
    sign = other.sign;
    number = other.number;
    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    return *this = *this + rhs;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    return *this = *this - rhs;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    return *this = *this * rhs;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    return *this = *this / rhs;
}

big_integer &big_integer::operator/=(uint32_t rhs) {
    return *this = *this / rhs;
}

big_integer &big_integer::operator/=(int rhs) {
    return *this = *this / rhs;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return *this = *this % rhs;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    return *this = *this & rhs;
}


big_integer &big_integer::operator|=(big_integer const &rhs) {
    return *this = *this | rhs;
}


big_integer &big_integer::operator^=(big_integer const &rhs) {
    return *this = *this ^ rhs;
}

big_integer &big_integer::operator<<=(int rhs) {
    return *this = *this << rhs;
}

big_integer &big_integer::operator>>=(int rhs) {
    return *this = *this >> rhs;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    if (this->number.size() == 1 && this->number[0] == 0)
        return big_integer(*this);
    big_integer tmp(*this);
    tmp.sign = !tmp.sign;
    return tmp;
}

big_integer big_integer::operator~() const {
    return -(*this + 1);
}

big_integer &big_integer::operator++() {
    return *this += 1;
}

big_integer big_integer::operator++(int) {
    big_integer res = *this;
    ++*this;
    return res;
}

big_integer &big_integer::operator--() {
    return *this -= 1;
}

big_integer big_integer::operator--(int) {
    big_integer res = *this;
    --*this;
    return res;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return a.sign == b.sign && a.number == b.number;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign)
        return a.sign;
    if (a.sign) {
        if (a.number.size() != b.number.size()) {
            return a.number.size() > b.number.size();
        } else {
            for (ptrdiff_t i = a.number.size() - 1; i >= 0; i--) {
                if (a.number[i] > b.number[i])
                    return true;
            }
            return false;
        }
    } else {
        if (a.number.size() != b.number.size()) {
            return a.number.size() < b.number.size();
        } else {
            for (ptrdiff_t i = a.number.size() - 1; i >= 0; i--) {
                if (a.number[i] < b.number[i]) {
                    return true;
                }
                if (a.number[i] > b.number[i]) {
                    return false;
                }
            }
            return false;
        }
    }
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

std::string to_string(big_integer const &a) {
    std::string res;
    big_integer tmp(a);
    if (tmp == 0)
        res = "0";
    while (tmp != 0) {
        uint64_t digit = 0;
        for (ptrdiff_t i = tmp.number.size() - 1; i >= 0; i--) {
            digit = ((digit << 32u) + tmp.number[i]) % 10;
        }
        res += std::to_string(digit);
        tmp /= 10;
    }
    if (a.sign)
        res += '-';
    std::reverse(res.begin(), res.end());
    return res;
}

big_integer abs(big_integer a) {
    a.sign = false;
    return a;
}


void normalize(big_integer &a) {
    while (a.number.size() > 1 && a.number.back() == 0)
        a.number.pop_back();
    if (a.number.size() == 1 && a.number[0] == 0)
        a.sign = false;
}

big_integer operator+(big_integer a, big_integer const &b) {
    if (a.sign && b.sign)
        return -(abs(a) + abs(b));
    if (a.sign && !b.sign)
        return abs(b) - abs(a);
    if (!a.sign && b.sign)
        return abs(a) - abs(b);

    uint32_t carry = 0;
    for (size_t i = 0; i < std::max(a.number.size(), b.number.size()) || carry > 0; i++) {
        if (i == a.number.size()) {
            a.number.push_back(0);
        }
        uint64_t sum = (uint64_t) a.number[i] + carry + (i < b.number.size() ? b.number[i] : 0);
        a.number[i] = (uint32_t) sum;
        carry = sum >> 32u;
    }
    return a;
}

big_integer operator-(big_integer a, big_integer const &b) {
    uint32_t carry = 0;
    if (a.sign && b.sign)
        return abs(b) - abs(a);
    if (a.sign && !b.sign)
        return -(abs(a) + abs(b));
    if (!a.sign && b.sign)
        return abs(a) + abs(b);
    if (a < b) {
        return -(abs(b) - abs(a));
    }
    for (size_t i = 0; i < b.number.size() || carry > 0; i++) {
        uint64_t diff = (uint64_t) a.number[i] - (carry + (i < b.number.size() ? b.number[i] : 0));
        a.number[i] = (uint32_t) diff;
        carry = diff >> 63u;
    }
    normalize(a);
    return a;
}

big_integer operator*(big_integer a, big_integer const &b) {
    big_integer res;
    if (a.sign != b.sign)
        res.sign = true;
    res.number.resize(a.number.size() + b.number.size());
    for (size_t i = 0; i < a.number.size(); i++) {
        for (size_t j = 0, carry = 0; j < b.number.size() || carry > 0; j++) {
            uint64_t mul = res.number[i + j] + (uint64_t) a.number[i] * (j < b.number.size() ? b.number[j] : 0) + carry;
            res.number[i + j] = (uint32_t) mul;
            carry = mul >> 32u;
        }
    }
    normalize(res);
    return res;
}

big_integer operator/(big_integer a, uint32_t b) {
    uint32_t carry = 0;
    for (ptrdiff_t i = a.number.size() - 1; i >= 0; i--) {
        uint64_t tmp = (uint64_t(carry) << 32u) + a.number[i];
        a.number[i] = tmp / b;
        carry = tmp % b;
    }
    normalize(a);
    return a;
}

big_integer operator/(big_integer a, int b) {
    big_integer abs_a = abs(a);
    uint32_t abs_b = (b == INT32_MIN) ? (uint32_t) b : abs(b);
    big_integer res = abs_a / abs_b;
    res.sign = (a.sign != (b < 0));
    normalize(res);
    return res;
}


big_integer operator/(big_integer a, big_integer const &b) {
    big_integer res;
    big_integer abs_a = abs(a);
    big_integer abs_b = abs(b);
    if (a.sign != b.sign)
        return -(abs_a / abs_b);
    if (abs_a.number.size() < abs_b.number.size()) {
        res = 0;
    } else if (abs_b.number.size() == 1) {
        res = abs_a / abs_b.number[0];
    } else {
        uint32_t f = uint32_t((1ull << 32u) / ((uint64_t) (abs_b.number.back() + 1)));
        big_integer norm_a = abs_a * f;
        big_integer norm_b = abs_b * f;
        int n = norm_b.number.size();
        int m = norm_a.number.size() - norm_b.number.size();
        res.number.resize(m + 1);
        if (norm_a >= (norm_b << 32 * m)) {
            res.number[m] = 1;
            norm_a -= (norm_b << 32 * m);
        } else {
            res.number[m] = 0;
        }
        for (ptrdiff_t i = m - 1; i >= 0; i--) {
            while (size_t(n + i) >= norm_a.number.size())
                norm_a.number.push_back(0);
            uint64_t q64 = (((uint64_t) norm_a.number[n + i] << 32u) + norm_a.number[n + i - 1]) / norm_b.number[n - 1];
            uint32_t q = UINT32_MAX;
            if (q64 < UINT32_MAX)
                q = q64;
            big_integer big_q(q);
            norm_a -= ((big_q * norm_b) << uint(32 * i));

            while (norm_a < 0) {
                q--;
                norm_a += (norm_b << (32 * i));
            }
            res.number[i] = q;
        }
    }
    normalize(res);
    return res;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a - (a / b) * b;
}

uint32_t and_(uint32_t a, uint32_t b) {
    return a & b;
}

uint32_t or_(uint32_t a, uint32_t b) {
    return a | b;
}

uint32_t xor_(uint32_t a, uint32_t b) {
    return a ^ b;
}

big_integer bit_operation(big_integer a, big_integer const &b, func bit_op) {
    big_integer res;
    res.number.resize(std::max(a.number.size(), b.number.size()));
    big_integer inv_a = a, inv_b = b;
    if (a.sign) {
        inv_a = a + 1;
        for (size_t i = 0; i < inv_a.number.size(); i++)
            inv_a.number[i] = ~inv_a.number[i];
    }
    if (b.sign) {
        inv_b = b + 1;
        for (size_t i = 0; i < inv_b.number.size(); i++)
            inv_b.number[i] = ~inv_b.number[i];
    }

    for (size_t i = 0; i < std::max(inv_a.number.size(), inv_b.number.size()); i++) {
        uint32_t arg_a = (i < inv_a.number.size()) ? inv_a.number[i] : ((inv_a.sign) ? UINT32_MAX : 0),
                arg_b = (i < inv_b.number.size()) ? inv_b.number[i] : ((inv_b.sign) ? UINT32_MAX : 0);
        res.number[i] = (*bit_op)(arg_a, arg_b);
    }
    res.sign = (*bit_op)(a.sign, b.sign);
    if (res.sign) {
        for (size_t i = 0; i < std::max(a.number.size(), b.number.size()); i++)
            res.number[i] = ~res.number[i];
        res--;
    }
    return res;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return bit_operation(a, b, and_);
}

big_integer operator|(big_integer a, big_integer const &b) {
    return bit_operation(a, b, or_);
}

big_integer operator^(big_integer a, big_integer const &b) {
    return bit_operation(a, b, xor_);
}

big_integer operator<<(big_integer a, int b) {
    size_t cnt_zero = (size_t) b >> 5u;
    size_t shift = (size_t) b & 31u;
    uint32_t carry = 0;
    for (size_t i = 0; i < a.number.size(); i++) {
        uint64_t tmp = (((uint64_t) a.number[i] << shift) | carry);
        a.number[i] = (uint32_t) tmp;
        carry = (tmp >> 32u);
    }

    if (carry > 0)
        a.number.push_back(carry);

    a.number.resize(a.number.size() + cnt_zero);

    for (ptrdiff_t i = a.number.size() - 1; i >= 0; i--) {
        if ((size_t) i >= cnt_zero)
            a.number[i] = a.number[i - cnt_zero];
        else
            a.number[i] = 0;
    }
    return a;
}

big_integer operator>>(big_integer a, int b) {
    size_t cnt_zero = ((size_t) b >> 5u);
    size_t shift = ((size_t) b & 31u);
    uint32_t carry = 0;
    for (size_t i = 0; i < a.number.size() - cnt_zero; i++) {
        a.number[i] = a.number[i + cnt_zero];
    }
    for (size_t i = 0; i < cnt_zero; i++)
        a.number.pop_back();
    for (ptrdiff_t i = a.number.size() - 1; i >= 0; i--) {
        uint64_t tmp = ((uint64_t) a.number[i] << (32 - shift));
        a.number[i] = ((tmp >> 32u) | carry);
        carry = (uint32_t) tmp;
    }
    normalize(a);
    if (a.sign)
        a--;
    return a;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}
