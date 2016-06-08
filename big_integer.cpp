#include "big_integer.h"

#include <bits/stdc++.h>

typedef uint_fast32_t usi;
typedef int_fast64_t ll;

big_integer::big_integer() {
    number.clear();
    number.push_back(0);
    sign = true;
}

big_integer::big_integer(big_integer const &other) {
    number = other.number;
    sign = other.sign;
}

big_integer::big_integer(uint_fast64_t a) {
    number.clear();
    sign = a >= 0;
    while (a > 0) {
        number.push_back((usi) (a % base));
        a /= base;
    }
}

big_integer::big_integer(std::string const &str) {
    std::string s = str;
    number.clear();
    sign = str[0] != '-';
    if (!sign) {
        s = str.substr(1, str.length() - 1);
    }
    *this = 0;
    for (int i = 0; i < s.length(); i++) {
        *this *= 10;
        *this += (s[i] - '0');
    }
}

big_integer::~big_integer() {
    number.clear();
}

big_integer &big_integer::operator=(big_integer const &other) {
    number = other.number;
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
            number[i] = (usi) (cur % base);
            carry = (usi) (cur / base);
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
            if (carry) cur += base;
            a.number[i] = (usi) cur;
        }
        while (a.number.size() > 1 && a.number.back() == 0)
            a.number.pop_back();
        if (*this < rhs) {
            *this = a;
            sign = false;
        } else {
            *this = a;
            sign = true;
        }
    } else {
        if (sign) {
            *this += b;
        } else {
            a += b;
            *this = a;
            this->sign = false;
        }
    }
    return *this;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer ans;
    ans.number.resize(number.size() + rhs.number.size());
    for (size_t i = 0; i < number.size(); ++i)
        for (ll j = 0, carry = 0; j < (int) rhs.number.size() || carry; ++j) {
            ll cur = (ll) ans.number[i + j] + (ll) number[i] * (ll) (j < (usi) rhs.number.size() ? rhs.number[j] : 0) + carry;
            ans.number[i + j] = usi((cur % base));
            carry = usi((cur / base));

        }
    while (ans.number.size() > 1 && ans.number.back() == 0)
        ans.number.pop_back();
    if (sign != rhs.sign) ans.sign = false;
    *this = ans;
    return *this;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    if (rhs == 1) {
        return *this;
    }
    if (sign && rhs.sign && *this < rhs) {
        *this = 0;
    } else if (!sign && !rhs.sign && *this > rhs) {
        *this = 0;
    } else {
        big_integer curValue = *this;
        curValue.sign = true;
        big_integer divider = rhs;
        divider.sign = true;
        std::vector<usi> ans;
        while (curValue.number.size() > 0) {
            big_integer pref;
            //take pref = min {pref >= divider} || pref = 0
            std::vector<usi> digits;
            int it = (int) curValue.number.size() - 1;
            if (curValue.number[it] == 0) {
                digits.push_back(0);
                pref.number = digits;
            }
            else {
                while (digits.size() < divider.number.size()) {
                    digits.push_back(curValue.number[it--]);
                }
                std::reverse(digits.begin(), digits.end());
                pref.number = digits;
                if (pref < divider) {
                    if (it < 0) break;
                    std::reverse(digits.begin(), digits.end()); //
                    digits.push_back(curValue.number[it]);
                    std::reverse(digits.begin(), digits.end());
                    pref.number = digits;
                }
            }
            while (pref.number.size() > 1 && pref.number.back() == 0)
                pref.number.pop_back();
            //take l = max {k: k*divider <= pref}
            ll l = 0, r = (ll) base + 1;
            while (r - l > 1) {
                usi m = usi((l + r) / 2);
                if (m * divider <= pref) {
                    l = m;
                } else {
                    r = m;
                }
            }
            //finally
            ans.push_back(l);
            usi save = (usi) pref.number.size();
            pref -= l * divider;
            int d = save - (int) pref.number.size();
            for (int i = 0; i < d; i++) {
                curValue.number.pop_back();
            }
            it = (int) curValue.number.size() - 1;
            for (int i = (int) pref.number.size() - 1; i >= 0; i--) {
                curValue.number[it--] = pref.number[i];
            }
            if (curValue.number.back() == 0)
                curValue.number.pop_back();
        }
        std::reverse(ans.begin(), ans.end());
        this->number = ans;
        if (sign != rhs.sign) {
            this->sign = false;
        } else {
            this->sign = true;
        }
    }
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    *this -= rhs * (*this / rhs);
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    return *this;

}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    return *this;

}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    return *this;

}

big_integer &big_integer::operator<<=(int rhs) {
    return *this;

}

big_integer &big_integer::operator>>=(int rhs) {
    return *this;

}

big_integer big_integer::operator+() const {
    big_integer a = *this;
    a.sign = true;
    return a;

}

big_integer big_integer::operator-() const {
    big_integer a = *this;
    a.sign = !sign;
    return a;

}

big_integer big_integer::operator~() const {
    return *this;

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

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
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
            for (long long i = 0; i < a.number.size(); i++) {
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
    std::string ans = "";
    if (!a.sign) {
        ans += '-';
    }
    big_integer b = a;
    big_integer cur = 0;
    big_integer ten = 10;
    while (b > 0) {
        cur = b % ten;
        ans += std::to_string(cur.number[0]);
        b /= ten;
    }
    for (ll i = 0; i < ans.length() / 2; i++) {
        std::swap(ans[i], ans[ans.length() - i - 1]);
    }
    return ans;
}

bool cmpPosSigns(big_integer const &a, big_integer const &b) {
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

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}

void operator>>(std::istream &s, big_integer &a) {
    std::string str;
    s >> str;
    a = big_integer(str);
}

int main() {
    big_integer p;
    big_integer q;

    std::cout << clock() / 1000000.0 << std::endl;
    freopen("tests.in", "r", stdin);
    std::cin >> p;
    std::cin >> q;

    for (int i = 0; i < 100; i++) {
        p *= q;
    }
    for (int i = 0; i < 100; i++) {
        p += p;
    }
    for (int i = 0; i < 100; i++) {
        p /= 2;
    }
    for (int i = 0; i < 100; i++) {
        p /= q;
    }
    std::cout << p << std::endl;
    std::cout << clock() / 1000000.0 << std::endl;
    return 0;
}