#ifndef BIGINT_BIG_INTEGER_H
#define BIGINT_BIG_INTEGER_H

#include <iosfwd>
#include <vector>
#include <cstdint>

struct big_integer {
    std::vector<uint_fast32_t> number;
    bool sign;

    big_integer();

    big_integer(big_integer const &other);

    big_integer(int_fast64_t a);

    explicit big_integer(std::string const &str);

    ~big_integer();

    big_integer &operator=(big_integer const &other);

    big_integer &operator+=(big_integer const &rhs);

    big_integer &operator-=(big_integer const &rhs);

    big_integer &operator*=(big_integer const &rhs);

    big_integer &operator*=(int_fast32_t const x);

    big_integer &operator/=(big_integer const &rhs);

    big_integer &operator/=(int_fast32_t const x);

    big_integer &operator%=(big_integer const &rhs);

    big_integer operator%=(int_fast32_t const x);

    big_integer &operator&=(big_integer const &rhs);

    big_integer &operator|=(big_integer const &rhs);

    big_integer &operator^=(big_integer const &rhs);

    big_integer &operator<<=(int rhs);

    big_integer &operator>>=(int rhs);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer &operator++();

    big_integer operator++(int);

    big_integer &operator--();

    big_integer operator--(int);

    big_integer mult(uint_fast32_t x);

    friend bool operator==(big_integer const &a, big_integer const &b);

    friend bool operator!=(big_integer const &a, big_integer const &b);

    friend bool operator<(big_integer const &a, big_integer const &b);

    friend bool operator>(big_integer const &a, big_integer const &b);

    friend bool operator<=(big_integer const &a, big_integer const &b);

    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend bool cmpPosSigns(big_integer const &a, big_integer const &b); //if a == b return false; if a > b return true;

    friend std::string to_string(big_integer const &a);

    big_integer &extracode();

    big_integer &normalcode();

private:
    const uint_fast32_t base = (uint_fast32_t) (1 << 31) - 1;
    const uint_fast64_t actualBase = (uint_fast64_t) base + 1;
    const int basepow = 31;

    void afterMultSignValidation(bool xsign);

};

big_integer operator+(big_integer a, big_integer const &b);

big_integer operator-(big_integer a, big_integer const &b);

big_integer operator*(big_integer a, big_integer const &b);

big_integer operator*(big_integer a, int_fast32_t const x);

big_integer operator/(big_integer a, big_integer const &b);

big_integer operator/(big_integer a, int_fast32_t const x);

big_integer operator%(big_integer a, big_integer const &b);

big_integer operator%(big_integer a, int_fast32_t const x);

big_integer operator&(big_integer a, big_integer const &b);

big_integer operator|(big_integer a, big_integer const &b);

big_integer operator^(big_integer a, big_integer const &b);

big_integer operator<<(big_integer a, int b);

big_integer operator>>(big_integer a, int b);

void abstractLogicOperation(big_integer &a, big_integer b,
                            uint_fast32_t (*logicFunc)(uint_fast32_t x, uint_fast32_t y),
                            bool (*check)(bool x, bool y));

bool operator==(big_integer const &a, big_integer const &b);

bool operator!=(big_integer const &a, big_integer const &b);

bool operator<(big_integer const &a, big_integer const &b);

bool operator>(big_integer const &a, big_integer const &b);

bool operator<=(big_integer const &a, big_integer const &b);

bool operator>=(big_integer const &a, big_integer const &b);

bool cmpPosSigns(big_integer const &a, big_integer const &b); //if a == b return false;

std::string to_string(big_integer const &a);

std::ostream &operator<<(std::ostream &s, big_integer const &a);

void operator>>(std::ostream &s, big_integer &a);

#endif //BIGINT_BIG_INTEGER_H