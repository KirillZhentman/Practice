#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>


using namespace std;

class Domain {

public:
    Domain() = delete;
    // конструктор должен позволять конструирование из string, с сигнатурой определитесь сами
    Domain(const string& name)
    {
        for (auto i = name.rbegin(); i != name.rend(); ++i) {
            name_ += *i;
        }
        name_ += "."s;
    }

    // разработайте operator==
    bool operator==(const Domain& other) const {
        return name_ == other.name_;
    }

    bool operator<(const Domain& other) const {
        return lexicographical_compare(name_.begin(), name_.end(), other.name_.begin(), other.name_.end());
    }
    
    // разработайте метод IsSubdomain, принимающий другой домен и возвращающий true, если this его поддомен
    bool IsSubdomain(const Domain& other) const {
        if (other.name_.size() >= name_.size()) { return false; }
        return name_.substr(0, other.name_.size()) == other.name_;
    }

private:

    string name_;
};

template <typename It>
class DomainChecker {
public:
    // конструктор должен принимать список запрещённых доменов через пару итераторов
    DomainChecker(It begin, It end)
    {
        for (auto i = begin; i != end; ++i) {
            bad_domains_.push_back(*i);
        }
        sort(bad_domains_.begin(), bad_domains_.end(), [](const Domain& l, const Domain& r) {return l < r; });
        auto last = unique(bad_domains_.begin(), bad_domains_.end(), [](const Domain& l, const Domain& r) {return r.IsSubdomain(l); });
        bad_domains_.erase(last, bad_domains_.end());
    }

    vector<Domain> Get() const {
        return bad_domains_;
    }

    // разработайте метод IsForbidden, возвращающий true, если домен запрещён
    bool IsForbidden(const Domain& domain) {
        if (bad_domains_.empty()) { return false; }

        auto bad_domain = upper_bound(bad_domains_.begin(), bad_domains_.end(), domain);
        if (bad_domain == bad_domains_.begin()) { return false; }
        else {
            return (domain.IsSubdomain(*(bad_domain - 1)) || domain == *(bad_domain - 1));
        }
    }

private:
    vector<Domain> bad_domains_;
};

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

// разработайте функцию ReadDomains, читающую заданное количество доменов из стандартного входа
const std::vector<Domain> ReadDomains(istream& input, size_t number) {
    std::vector<Domain> result;

    for (size_t i = 0; i < number; ++i) {
        string name;
        getline(input, name);
        Domain new_domain(name);
        result.push_back(new_domain);
    }

    return result;
}

void TestDomainCheckerSize() {
    vector<Domain> test_vector = { Domain("abc.com"s), Domain("g.dec.com"s), Domain("com"s),
    Domain("yandex.ru"s), Domain("google.ru"s), Domain("ru"s) };
    DomainChecker test(test_vector.begin(), test_vector.end());
    assert(test.Get().size() == 2);
}

void TestAllowGoodDomains() {
    istringstream input("abc.com g.dec.com yandex.ru google.ru b"s);
    vector<Domain> test_vector;
    string test_domain;
    while (getline(input, test_domain, ' ')) {
        test_vector.push_back(Domain(test_domain));
    }
    DomainChecker test(test_vector.begin(), test_vector.end());
    test_vector.clear();
    istringstream request("com acb.com dec.com yandex.com ru.google bb"s);
    while (getline(request, test_domain, ' ')) {
        test_vector.push_back(Domain(test_domain));
    }
    for (const auto& rqst : test_vector) {
        assert(!test.IsForbidden(rqst));
    }
}

void TestCatchBadDomains() {
    istringstream input("abc.com g.dec.com yandex.ru google.ru b"s);
    vector<Domain> test_vector;
    string test_domain;
    while (getline(input, test_domain, ' ')) {
        test_vector.push_back(Domain(test_domain));
    }
    DomainChecker test(test_vector.begin(), test_vector.end());
    test_vector.clear();
    istringstream request("abc.com e.r.f.g.dec.com yandex.yandex.ru not.google.ru a.c.d.b b"s);
    while (getline(request, test_domain, ' ')) {
        test_vector.push_back(Domain(test_domain));
    }
    for (const auto& rqst : test_vector) {
        assert(test.IsForbidden(rqst));
    }
}

int main() {
    TestDomainCheckerSize();
    TestAllowGoodDomains();
    TestCatchBadDomains();
    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
    
    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}


