#include <vector>
#include <string>
#include <cassert>
#include <exception>




const uint32_t base = 10000;

class BigInteger {
public:
    BigInteger(int value);
    BigInteger() : BigInteger(0) {};
    BigInteger(const BigInteger& other) = default;
    BigInteger(BigInteger&& other) = default;
    BigInteger& operator=(const BigInteger &other) & = default;
    BigInteger& operator=(BigInteger&& other) & noexcept = default;

    BigInteger abs() const;

    BigInteger& operator+=(const BigInteger& other) &;
    BigInteger& operator-=(const BigInteger& other) &;
    friend BigInteger operator+(const BigInteger& first, const BigInteger& second);
    friend BigInteger operator-(const BigInteger& first, const BigInteger& second);

    BigInteger& operator*=(const BigInteger& other) &;
    BigInteger& operator/=(const BigInteger& other) &;
    BigInteger& operator%=(const BigInteger& other) &;
    friend BigInteger operator*(const BigInteger& first, const BigInteger& second);
    friend BigInteger operator/(const BigInteger& first, const BigInteger& second);
    friend BigInteger operator%(const BigInteger& first, const BigInteger& second);

    friend bool operator<(const BigInteger& first, const BigInteger& second);
    friend bool operator>(const BigInteger& first, const BigInteger& second);
    friend bool operator<=(const BigInteger& first, const BigInteger& second);
    friend bool operator>=(const BigInteger& first, const BigInteger& second);
    friend bool operator==(const BigInteger& first, const BigInteger& second);
    friend bool operator!=(const BigInteger& first, const BigInteger& second);

    BigInteger& operator++() &;
    BigInteger operator++(int) &;
    BigInteger operator-() const;

    friend std::basic_istream<char>& operator>> (std::basic_istream<char>& in, BigInteger& to);
    friend std::basic_ostream<char>& operator<< (std::basic_ostream<char>& out, const BigInteger& from);

    explicit operator bool() const { return *this != 0;}


    std::string toString() const;


private:
    std::vector<uint32_t> data;
    bool sign = false;
    uint32_t digit_minus(uint32_t value1, uint32_t value2, uint32_t& in_mind);
    void division_rec(BigInteger& dividend, BigInteger& dividor, const BigInteger& true_div, BigInteger& result);
    BigInteger division(BigInteger &dividend, const BigInteger &other);
};

void normalize(std::vector<uint32_t>& data) {

    for (uint32_t i = 0; i < data.size() - 1; ++i) {
        data[i + 1] += data[i] / base;
        data[i] %= base;
    }
    while (data.back()/base != 0) {
        data.push_back(data.back()/base);
        data[data.size() - 2] %= base;
    }
    int i = data.size() - 1;
    while (i > 0 && data[i] == 0) {
        --i;
    }
    data.resize(i + 1);
}

BigInteger::BigInteger(int32_t value) : data(1) {
    data[0] = ::abs(value);
    normalize(data);
    sign = value < 0;
}

BigInteger BigInteger::abs() const {
    BigInteger result(*this);
    result.sign = false;
    return result;
}

BigInteger& BigInteger::operator+=(const BigInteger& other) & {
    if (!(sign^other.sign)) {
        uint32_t i = 0;
        for (; i < other.data.size() && i < data.size(); ++i) {
            //data[i] = digit_plus(data[i], other.data[i], overflow);
            data[i] += other.data[i];
        }
        if (i == data.size()) {
            while (i < other.data.size()) {
                data.push_back(other.data[i]);
                ++i;
            }
        }
        normalize(data);
    }
    else if (other.sign){
        return *this-=(other.abs());
    }
    else if (sign) {
        sign = false;
        *this-=other;
        sign = !sign;
        return *this;
    }

    return *this;
}

BigInteger operator+(const BigInteger& first, const BigInteger& second) {
    BigInteger result(first);
    result += second;
    return result;
}

BigInteger& BigInteger::operator++() & {
    *this += 1;
    return *this;
}

BigInteger BigInteger::operator++(int) & {
    BigInteger extra = *this;
    *this += 1;
    return extra;
}

BigInteger BigInteger::operator-() const {
    if (*this == 0)
        return (BigInteger(*this));
    BigInteger result(*this);
    result.sign = !sign;
    return result;
}

uint32_t BigInteger::digit_minus(uint32_t value1, uint32_t value2, uint32_t& in_mind) {
    uint32_t result;
    if (value1 >= in_mind + value2) {
        result = value1 - in_mind - value2;
        in_mind = 0;
    }
    else {
        result = base + value1 - in_mind - value2;
        in_mind = 1;
    }
    return result;
}


BigInteger& BigInteger::operator-=(const BigInteger& other) & {
    uint32_t in_the_mind = 0;
    if (!(sign^other.sign)) {
        if (abs() >= other.abs()) {
            uint32_t i = 0;
            for (; i < other.data.size(); ++i) {
                data[i] = digit_minus(data[i], other.data[i], in_the_mind);
            }
            while (i < data.size()) {
                data[i] = digit_minus(data[i], 0, in_the_mind);
                ++i;
            }
        }
        else {
            BigInteger tmp(other);
            uint32_t i = 0;
            for (; i < data.size(); ++i) {
                tmp.data[i] = digit_minus(tmp.data[i], data[i], in_the_mind);
            }
            while (i < tmp.data.size()) {
                tmp.data[i] = digit_minus(tmp.data[i], 0, in_the_mind);
                ++i;
            }
            tmp.sign = !tmp.sign;
            *this = tmp;
        }

    }
    else if (other.sign){
        *this+=(other.abs());
    }
    else if (sign) {
        sign = false;
        *this+=other;
        sign = !sign;
    }
    int i = data.size() - 1;
    while (i > 0 && data[i] == 0) {
        --i;
    }
    data.resize(i + 1);
    return *this;
}

BigInteger operator-(const BigInteger &first, const BigInteger &second) {
    BigInteger result(first);
    result -= second;
    return result;
}

bool operator<(const BigInteger &first, const BigInteger &second) {
    if (first.data.size() < second.data.size())
        return true;
    if (first.data.size() > second.data.size())
        return false;

    for (int i = first.data.size() - 1; i >= 0; --i) {
        if (first.data[i] < second.data[i])
            return true;
        if (first.data[i] > second.data[i])
            return false;
    }
    return false;
}

bool operator>(const BigInteger &first, const BigInteger &second) {
    return !(first <= second);
}

bool operator<=(const BigInteger &first, const BigInteger &second) {
    return (first < second) || (first == second);
}


bool operator>=(const BigInteger &first, const BigInteger &second) {
    return !(first < second);
}

bool operator==(const BigInteger &first, const BigInteger &second) {
    return !(first < second) && !(second < first);
}

bool operator!=(const BigInteger &first, const BigInteger &second) {
    return !(first == second);
}

std::vector<uint32_t> karatsuba_mul_rec(const std::vector<uint32_t>& left, const std::vector<uint32_t>& right) {
    if (left.size() == 1) {
        std::vector<uint32_t> result(2, 0);
        result[0] = left[0] * right[0];
        return result;
    }
    std::vector<uint32_t> result(left.size() * 2, 0);
    int k = left.size()/2;

    std::vector<uint32_t> left_first {left.begin(), left.begin() + k};
    std::vector<uint32_t> left_second {left.begin() + k, left.end()};
    std::vector<uint32_t> right_first {right.begin(), right.begin() + k};
    std::vector<uint32_t> right_second {right.begin() + k, right.end()};

    std::vector<uint32_t> sum_left(k);
    std::vector<uint32_t> sum_right(k);

    for (int i = 0; i < k; ++i) {
        sum_left[i] = left_first[i] + left_second[i];
        sum_right[i] = right_first[i] + right_second[i];
    }

    auto result_left = karatsuba_mul_rec(left_first, right_first);
    auto result_right = karatsuba_mul_rec(left_second, right_second);
    auto result_sum = karatsuba_mul_rec(sum_left, sum_right);

    for (int i = 0; i < k * 2; ++i) {
        assert(result_sum[i] >= result_left[i] + result_right[i]);
        result_sum[i] -= (result_left[i] + result_right[i]);
    }

    for (auto i = 0; i < k * 2; ++i) {
        result[i] = result_left[i];
    }

    for (auto i = k * 2; i < k * 4; ++i) {
        result[i] = result_right[i - k * 2];
    }

    for (auto i = k; i < k * 3; ++i) {
        result[i] += result_sum[i - k];
    }
    return result;
}



std::vector<uint32_t> karatsuba_mul(const std::vector<uint32_t>& left, const std::vector<uint32_t>& right) {
    int max_length = std::max(left.size(), right.size());
    int k = 1;
    while(max_length > k) {
        k *= 2;
    }

    std::vector<uint32_t> new_left(k, 0);
    std::vector<uint32_t> new_right(k, 0);

    for (uint32_t i = 0; i < left.size(); ++i) {
        new_left[i] = left[i];
    }
    for (uint32_t i = 0; i < right.size(); ++i) {
        new_right[i] = right[i];
    }

    auto result = karatsuba_mul_rec(new_left, new_right);
    normalize(result);
    return result;

}

BigInteger &BigInteger::operator*=(const BigInteger& other) & {
    if (data.size() == 0 || other.data.size() == 0) {
        sign = false;
        data = std::vector<uint32_t>(1, 0);
    }

    if (*this == other) {
        BigInteger tmp = other;
        data = karatsuba_mul(data, tmp.data);
        sign = false;
        return *this;
    }

    data = karatsuba_mul(data, other.data);
    sign = sign ^ other.sign;
    return *this;
}

BigInteger operator*(const BigInteger& first, const BigInteger& second) {
    BigInteger result(first);
    result *= second;
    return result;
}

void BigInteger::division_rec(BigInteger& dividend, BigInteger& divider, const BigInteger& true_div, BigInteger& result) {

    uint32_t current_res = 0;
    while (dividend >= divider) {
        dividend -= divider;
        ++current_res;
    }
    result *= base;
    result += current_res;

    if (dividend < true_div) {
        while(divider != true_div) {
            divider.data.erase(divider.data.begin());
            result *= base;
        }
        return;
    }
    else {
        if (divider > true_div)
            divider.data.erase(divider.data.begin());
        while (dividend < divider) {
            divider.data.erase(divider.data.begin());
            result *= base;
        }
    }
    division_rec(dividend, divider, true_div, result);
}


BigInteger BigInteger::division(BigInteger &dividend, const BigInteger &other) {
    BigInteger divider;
    divider.data = std::vector<uint32_t>(data.size(), 0);
    for (uint32_t i = 0; i < other.data.size(); ++i) {
        divider.data[divider.data.size() - 1 - i] = other.data[other.data.size() - 1 - i];
    }
    BigInteger result;
    division_rec(dividend, divider, other, result);
    return result;
}

BigInteger& BigInteger::operator/=(const BigInteger &other) & {
    if (other == 0) {
        throw std::invalid_argument("division by zero");
    }
    if (*this == other) {
        *this = BigInteger(1);
        return *this;
    }
    if (abs() < other.abs()) {
        *this = 0;
        sign = false;
        return *this;
    }
    bool is_signed = sign;
    sign = false;
    *this = division(*this, other);
    sign = is_signed ^ other.sign;
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger &other) & {
    if (*this == other) {
        *this = BigInteger(0);
        return *this;
    }


    if (this->abs() < other.abs())
        return *this;
    division(*this, other);
    return *this;
}

BigInteger operator/(const BigInteger &first, const BigInteger &second) {
    BigInteger tmp(first);
    tmp /= second;
    return tmp;
}

BigInteger operator%(const BigInteger &first, const BigInteger &second) {
    BigInteger tmp(first);
    tmp %= second;
    return tmp;
}

std::basic_istream<char>& operator>>(std::basic_istream<char>& in, BigInteger& to) {
    to = BigInteger();
    char current_digit = in.get();
    bool is_signed = false;
    if (current_digit == '-')
        is_signed = true;
    else {
        if (in.eof() || current_digit < '0' || current_digit > '9')
            return in;
        to *= 10;
        to += (current_digit - '0');
    }
    while (true) {
        current_digit = in.get();


        if (in.eof() || current_digit < '0' || current_digit > '9') {
            break;
        }
        to *= 10;
        to += (current_digit - '0');
    }
    to.sign = is_signed;
    return in;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& out, const BigInteger& from) {
    out << from.toString();
    return out;
}

std::string BigInteger::toString() const {
    std::string result;
    if (data.size() == 1 && data[0] == 0) {
        return "0";
    }
    if (sign)
        result += '-';
    for (int i = data.size() - 1; i >=0; --i) {
        int cur = data[i];

        std::string current = "0000";
        int k = 0;
        while (cur != 0) {
            char last_dig = static_cast<char>(cur%10);
            current[k] = last_dig + '0';
            ++k;
            cur/=10;
        }
        for (int j = current.length() - 1; j >= 0; --j) {
            result += current[j];
        }
    }
    int i = 0;
    while (result[i] == '0' || result[i] == '-') {
        ++i;
    }
    if (result[0] == '-') {
        return '-' + result.substr(i);
    }
    else {
        return result.substr(i);
    }


}

















