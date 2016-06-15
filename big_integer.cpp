#include "big_integer.h"
#include <bits/stdc++.h>

typedef uint_fast32_t usi;
typedef int_fast64_t ll;

static const uint_fast32_t base = (uint_fast32_t) (1 << 31) - 1;
static const int_fast64_t actualBase = (int_fast64_t) base + 1;
static const int basepow = 31;

static inline bool afterMultSignValidation(bool thissign, bool xsign) {
    if (thissign == xsign) {
        thissign = true;
    } else thissign = false;
    return thissign;
}


static inline void extracode(big_integer &a) {
    for (size_t i = 0; i < a.number.size(); i++) {
        a.number[i] = (~(a.number[i]) & base);
    }
    a.sign = true;
    a += 1;
}

static inline void normalcode(big_integer &a) {
    a -= 1;
    a.sign = false;
    for (size_t i = 0; i < a.number.size(); i++) {
        a.number[i] = (~(a.number[i]) & base);
    }
}

static void abstractLogicOperation(big_integer &a, big_integer b,
                                   uint_fast32_t (*logicFunc)(uint_fast32_t x, uint_fast32_t y),
                                   bool (*check)(bool x, bool y)) {
    bool asign = a.sign;
    bool bsign = b.sign;
    if (!asign) {
        extracode(a);
    }
    if (!b.sign) {
        extracode(b);
    }
    for (size_t i = 0; i < a.number.size(); i++) {
        a.number[i] = logicFunc(a.number[i], (i < b.number.size() ? b.number[i] : 0));
    }
    if (check(!asign, !bsign)) {
        normalcode(a);
    }
}

static bool cmpPosSigns(big_integer const &a, big_integer const &b) { //if a == b return false; if a > b return true;
    if (a.number.size() > b.number.size()) {
        return true;
    } else if (a.number.size() < b.number.size()) {
        return false;
    } else {
        for (int i = (int) a.number.size() - 1; i >= 0; i--) {
            if (a.number[i] > b.number[i]) {
                return true;
            }
            if (a.number[i] < b.number[i]) {
                return false;
            }
        }
    }
    return false;
}

big_integer::big_integer() {
    number.clear();
    number.push_back(0);
    sign = true;
}

big_integer::big_integer(big_integer const &other) {
    number = other.number;
    sign = other.sign;
}

big_integer::big_integer(int_fast64_t a) {
    number.clear();
    sign = (a >= 0);
    //a = std::abs(a);
    if (a == 0) {
        number.push_back(0);
    }
    while (a != 0) {
        number.push_back((usi) (std::abs((a % actualBase))));
        a /= actualBase;
    }
}

big_integer::big_integer(std::string const &str) {
    std::string s = str;
    number.clear();
    sign = str[0] != '-';
    if (!sign) {
        s = str.substr(1, str.length() - 1);
    }
    for (size_t i = 0; i < s.length(); i++) {
        *this *= 10;
        if (sign) *this += (s[i] - '0');
        else *this -= (s[i] - '0');
    }
}

big_integer::~big_integer() {
    number.clear();
}

big_integer &big_integer::operator=(big_integer const &other) {
    this->number.resize(other.number.size());
    //memcpy(&this->number[0], &other.number[0], other.number.size() * sizeof(usi));
    //number = other.number;
    for (size_t i = 0; i < other.number.size(); i++) {
        this->number[i] = other.number[i];
    }
    sign = other.sign;
    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    if (sign == rhs.sign) {
        usi carry = 0;
        ll cur = 0;
        for (size_t i = 0; i < std::max(number.size(), rhs.number.size()) || carry; ++i) {
            if (i == number.size())
                number.push_back(0);
            cur = (ll) number[i] + (ll) carry + (ll) (i < rhs.number.size() ? rhs.number[i] : 0);
            number[i] = (usi) (cur % actualBase);
            carry = (usi) (cur / actualBase);
        }
    } else {
        big_integer posThis = *this;
        posThis.sign = true;
        big_integer posRhs = rhs;
        posRhs.sign = true;
        bool saveSign;
        if (posThis > posRhs) {
            saveSign = sign;
        } else {
            saveSign = !sign;
        }
        posThis -= posRhs;
        *this = posThis;
        sign = saveSign;
    }
    if (this->number.size() == 0 || (this->number.size() == 1 && this->number[0] == 0)) {
        this->sign = true;
    }
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    big_integer a = *this;
    a.sign = true;
    big_integer b = rhs;
    b.sign = true;
    if (this->sign == rhs.sign) {
        if (a < b) {
            std::swap(a, b);
        }
        ll carry = 0;
        ll cur = 0;
        for (size_t i = 0; i < b.number.size() || carry; ++i) {
            cur = (ll) a.number[i];
            cur -= carry + (ll) (i < b.number.size() ? b.number[i] : 0);
            carry = (cur < 0);
            if (carry) cur += actualBase;
            a.number[i] = (usi) cur;
        }
        while (a.number.size() > 1 && a.number.back() == 0)
            a.number.pop_back();
        if (cmpPosSigns(*this, rhs)) {
            sign = this->sign;
        } else {
            sign = !this->sign;
        }
    } else {
        a += b;
    }
    this->number = a.number;
    if (this->number.size() == 0 || (this->number.size() == 1 && this->number[0] == 0)) {
        this->sign = true;
    }
    return *this;
}

//static usi _ans[(1 << 64)];
static big_integer _ans;

static big_integer mult(const big_integer &b, uint_fast32_t x) {
    ll carry = 0;
    big_integer a = b;
    a.number.push_back(0);
    for (size_t i = 0; i < a.number.size() || carry; i++) {
        ll cur = carry + (ll) a.number[i] * x;
        a.number[i] = (usi) (cur % actualBase);
        carry = usi(cur / actualBase);
    }
    while (a.number.size() > 1 && a.number.back() == 0)
        a.number.pop_back();
    return a;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    if (rhs.number.size() == 1) {
        *this = mult(*this, rhs.number[0]);
        sign = afterMultSignValidation(sign, rhs.sign);
        return *this;
    }
    //_ans.number.resize(number.size() + rhs.number.size());
    if (number.size() + rhs.number.size() > _ans.number.size()) _ans.number.resize(number.size() + rhs.number.size());
    memset(&_ans.number[0], 0, _ans.number.size() * sizeof(usi));
    for (size_t i = 0; i < number.size(); i++)
        for (size_t j = 0, carry = 0; j < rhs.number.size() || carry; j++) {
            ll cur = (ll) _ans.number[i + j] + (ll) number[i] * (ll) (j < rhs.number.size() ? rhs.number[j] : 0) +
                     carry;
            _ans.number[i + j] = usi((cur % actualBase));
            carry = usi((cur / actualBase));
        }
//    number.resize(_ans.number.size());
//    memcpy(&number[0], &_ans.number[0], sizeof(usi) *_ans.number.size());
    number = _ans.number;
    while (number.size() > 1 && number.back() == 0)
        number.pop_back();
    sign = afterMultSignValidation(sign, rhs.sign);
    return *this;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    if (rhs.number.size() == 1 && (rhs.number[0] <= usi(std::numeric_limits<int_fast32_t>::max()))) {
        *this /= rhs.number[0];
        this->sign = afterMultSignValidation(sign, rhs.sign);
        return *this;
    }
    std::vector<usi> ans;
    ans.resize(this->number.size() - rhs.number.size() + 1);
    big_integer curValue(*this);
    curValue.sign = true;
    big_integer now(0);
    big_integer divider(rhs);
    divider.sign = true;
    for (size_t i = 0; i < ans.size(); i++) {
        ll l = 0, r = base + 1;
        while (r - l > 1) {
            ll m = (l + r) / 2;
            now = mult(divider, usi(m));
            if (ans.size() - i - 1 > 0) {
                std::reverse(now.number.begin(), now.number.end());
                for (size_t j = 0; j < ans.size() - i - 1; j++) {
                    now.number.push_back(0);
                }
                std::reverse(now.number.begin(), now.number.end());
            }
            if (now <= curValue) {
                l = m;
            } else {
                r = m;
            }
        }
        now = l * divider;
        if (now.number.size() < curValue.number.size()) {
            std::reverse(now.number.begin(), now.number.end());
            while (now.number.size() < curValue.number.size()) {
                now.number.push_back(0);
            }
            std::reverse(now.number.begin(), now.number.end());
        }
        if (now > curValue) {
            now.number.erase(now.number.begin());
        }
        curValue -= now;
        ans[i] = usi(l);
    }
    std::reverse(ans.begin(), ans.end());
    while (ans.size() > 1 && ans.back() == 0) {
        ans.pop_back();
    }
    this->number = ans;
    this->sign = afterMultSignValidation(sign, rhs.sign);
    if (ans.size() == 0 || (ans.size() == 1 && ans[0] == 0)) {
        this->sign = true;
    }
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    *this -= rhs * (*this / rhs);
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    abstractLogicOperation(*this, big_integer(rhs), [](usi a, usi b) -> usi { return a & b; },
                           [](bool x, bool y) -> bool { return x & y; });
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    abstractLogicOperation(*this, big_integer(rhs), [](usi a, usi b) -> usi { return a | b; },
                           [](bool x, bool y) -> bool { return x | y; });
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    abstractLogicOperation(*this, big_integer(rhs), [](usi a, usi b) -> usi { return a ^ b; },
                           [](bool x, bool y) -> bool { return x | y; });
    return *this;
}

big_integer &big_integer::operator<<=(int rhs) {
    std::reverse(this->number.begin(), this->number.end());
    for (int i = 0; i < rhs / basepow; i++) {
        this->number.push_back(0);
    }
    std::reverse(this->number.begin(), this->number.end());
    for (int i = 0; i < rhs % basepow; i++) {
        *this *= 2;
    }
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    std::reverse(this->number.begin(), this->number.end());
    bool thissigne = sign;
    for (int i = 0; i < rhs / basepow; i++) {
        this->number.pop_back();
    }
    std::reverse(this->number.begin(), this->number.end());
    for (int i = 0; i < rhs % basepow; i++) {
        *this /= 2;
    }
    if (!thissigne) {
        *this -= 1;
    }
    return *this;
}

big_integer big_integer::operator+() const {
    big_integer a = *this;
    a.sign = true;
    return a;

}

big_integer big_integer::operator-() const {
    big_integer a = *this;
    a.sign = true;
    if (a != 0) {
        a.sign = !sign;
    }
    return a;
}

big_integer big_integer::operator~() const {
    return (-*this) - 1;
}

big_integer &big_integer::operator++() {
    return *this += 1;

}

big_integer big_integer::operator++(int a) {
    return *this += 1;

}

big_integer &big_integer::operator--() {
    return *this -= 1;

}

big_integer big_integer::operator--(int) {
    return *this -= 1;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator*(big_integer a, int_fast32_t const x) {
    return a *= x;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator/(big_integer a, int_fast32_t const x) {
    return a /= x;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator%(big_integer a, int_fast32_t const x) {
    return a %= x;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const &a, big_integer const &b) {
    if (a.sign == b.sign) {
        if (a.number.size() == b.number.size()) {
            for (int i = 0; i < (int) a.number.size(); i++) {
                if (a.number[i] != b.number[i]) return false;
            }
            return true;
        } else return false;
    } else return false;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    return !(a >= b);
}

bool operator>(big_integer const &a, big_integer const &b) {
    if (a.sign == b.sign) {
        if (!a.sign) {
            big_integer pb = b;
            pb.sign = true;
            big_integer pa = a;
            pa.sign = true;
            return cmpPosSigns(pb, pa);
        } else {
            return cmpPosSigns(a, b);
        }
    } else {
        return a.sign;
    }
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return (a > b) || (a == b);
}

std::string to_string(big_integer const &a) {
    int beg = 0;
    std::string s;
    if (!a.sign) {
        s += '-';
        beg++;
    }
    big_integer b = a;
    b.sign = true;
    big_integer cur = 0;
    std::vector<usi> ans;
    if (b == 0) ans.push_back(0);
    while (b != 0) {
        cur = b % 10;
        ans.push_back(cur.number[0]);
        b /= 10;
    }
    std::reverse(ans.begin(), ans.end());
    for (size_t i = 0; i < ans.size(); i++) {
        s += std::to_string(ans[i]);
    }
    return s;
}

big_integer &big_integer::operator/=(int_fast32_t const x) {
    usi carry = 0;
    int_fast64_t xx = (int_fast64_t) x;
    uint_fast32_t y = (usi) std::abs(xx);
    if (*this == 0) return *this;
    bool xsign = (x >= 0);
    for (int i = (int) this->number.size() - 1; i >= 0; --i) {
        ll cur = (ll) this->number[i] + (ll) carry * actualBase;
        this->number[i] = usi(cur / y);
        carry = usi(cur % y);
    }
    while (this->number.size() > 1 && this->number.back() == 0)
        this->number.pop_back();
    this->sign = afterMultSignValidation(this->sign, xsign);
    if (number.size() == 0 || (number.size() == 1 && number[0] == 0)) {
        this->sign = true;
    }
    return *this;
}

big_integer big_integer::operator%=(int_fast32_t const x) {
    usi carry = 0;
    int_fast64_t xx = (int_fast64_t) x;
    uint_fast32_t y = (usi) std::abs(xx);
    if (*this == 0) return *this;
    bool xsign = (x >= 0);
    for (int i = (int) this->number.size() - 1; i >= 0; --i) {
        ll cur = (ll) this->number[i] + (ll) carry * actualBase;
        this->number[i] = usi(cur / y);
        carry = usi(cur % y);
    }
    this->sign = afterMultSignValidation(this->sign, xsign);
    big_integer newthis = carry;
    newthis.sign = this->sign;
    return newthis;
}

big_integer &big_integer::operator*=(int_fast32_t const x) {
    if (*this == 0 || x == 0) {
        *this = 0;
        return *this;
    }
    bool xsign = (x == std::abs(x));
    this->sign = afterMultSignValidation(this->sign, xsign);
    this->number = mult(*this, usi(std::abs(x))).number;
    return *this;
}

big_integer &big_integer::operator=(int_fast64_t other) {
    big_integer a(other);
    *this = a;
    return *this;
}


std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}

void operator>>(std::istream &s, big_integer &a) {
    std::string str;
    s >> str;
    a = big_integer(str);
}

//int main() {
//    big_integer p;
//    big_integer q;
//    std::cout << clock() / 1000.0 << std::endl;
//    freopen("tests.in", "r", stdin);
//    std::cin >> p;
//    std::cin >> q;
//
//    const int N = 100;
//    for (int i = 0; i < N; i++) {
//        p *= q;
//    }
//    for (int i = 0; i < N; i++) {
//        p += p;
//    }
//    for (int i = 0; i < N; i++) {
//        p /= 2;
//    }
//    for (int i = 0; i < N; i++) {
//        p /= q;
//    }
//
//    std::cout << p << std::endl;
//    std::cout << clock() / 1000.0 << std::endl;
//    return 0;
//}