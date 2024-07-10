#include <algorithm>
#include <cassert>
#include <iostream>
#include <ranges>
#include <span>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std::literals;

class Domain {

public:
    Domain() = delete;
    Domain(std::string&& name) :
        name_(std::move(name))
    {
        std::reverse(name_.begin(), name_.end());
        name_ += "."s;
    }

    bool operator==(const Domain& other) const noexcept {
        return name_ == other.name_;
    }

    bool operator<(const Domain& other) const noexcept {
        return std::lexicographical_compare(name_.begin(), name_.end(), other.name_.begin(), other.name_.end());
    }

    bool IsSubdomain(const Domain& other) const noexcept {
        return name_.starts_with(other.name_);
    }

private:

    std::string name_;
};

template <typename It>
class DomainChecker {
public:
    DomainChecker(It begin, It end)
    {
        for (auto i = begin; i != end; ++i) {
            bad_domains_.push_back(*i);
        }
        std::ranges::sort(bad_domains_.begin(), bad_domains_.end(), [](const Domain& l, const Domain& r) {return l < r; });
        auto last = std::ranges::unique(bad_domains_.begin(), bad_domains_.end(), [](const Domain& l, const Domain& r) {return r.IsSubdomain(l); });
        bad_domains_.erase(last.begin(), last.end());
    }

    std::span<const Domain> Get() const {
        return std::span(bad_domains_);
    }

    bool IsForbidden(const Domain& domain) const {
        if (bad_domains_.empty()) {
            return false;
        }
        auto bad_domain = std::ranges::upper_bound(bad_domains_, domain, [](const Domain& l, const Domain& r) {return l < r; });
        if (bad_domain == bad_domains_.begin()) {
            return false;
        }
        else {
            return (domain.IsSubdomain(*(bad_domain - 1)) || domain == *(bad_domain - 1));
        }
    }

private:
    std::vector<Domain> bad_domains_;
};

template <typename Number>
Number ReadNumberOnLine(std::istream& input) {
    std::string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

const std::vector<Domain> ReadDomains(std::istream& input, size_t number) {
    std::vector<Domain> result;

    for (size_t i = 0; i < number; ++i) {
        std::string name;
        getline(input, name);
        result.push_back(Domain{ move(name) });
    }

    return result;
}

void TestDomainCheckerSize() {
    std::vector<Domain> test_vector = { Domain("abc.com"s), Domain("g.dec.com"s), Domain("com"s),
    Domain("yandex.ru"s), Domain("google.ru"s), Domain("ru"s) };
    DomainChecker test(test_vector.begin(), test_vector.end());
    assert(test.Get().size() == 2);
}

void TestAllowGoodDomains() {
    std::istringstream input("abc.com g.dec.com yandex.ru google.ru b"s);
    std::vector<Domain> test_vector;
    std::string test_domain;
    while (getline(input, test_domain, ' ')) {
        test_vector.push_back(Domain(move(test_domain)));
    }
    DomainChecker test(test_vector.begin(), test_vector.end());
    test_vector.clear();
    std::istringstream request("com acb.com dec.com yandex.com ru.google bb"s);
    while (getline(request, test_domain, ' ')) {
        test_vector.push_back(Domain(move(test_domain)));
    }
    for (const auto& rqst : test_vector) {
        assert(!test.IsForbidden(rqst));
    }
}

void TestCatchBadDomains() {
    std::istringstream input("abc.com g.dec.com yandex.ru google.ru b"s);
    std::vector<Domain> test_vector;
    std::string test_domain;
    while (getline(input, test_domain, ' ')) {
        test_vector.push_back(Domain(move(test_domain)));
    }
    DomainChecker test(test_vector.begin(), test_vector.end());
    test_vector.clear();
    std::istringstream request("abc.com e.r.f.g.dec.com yandex.yandex.ru not.google.ru a.c.d.b b"s);
    while (getline(request, test_domain, ' ')) {
        test_vector.push_back(Domain(move(test_domain)));
    }
    for (const auto& rqst : test_vector) {
        assert(test.IsForbidden(rqst));
    }
}

int main() {
    TestDomainCheckerSize();
    TestAllowGoodDomains();
    TestCatchBadDomains();
    const std::vector<Domain> forbidden_domains = ReadDomains(std::cin, ReadNumberOnLine<size_t>(std::cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(std::cin, ReadNumberOnLine<size_t>(std::cin));
    for (const Domain& domain : test_domains) {
        std::cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << std::endl;
    }
}