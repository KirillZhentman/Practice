#include <iostream>
#include <iomanip>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

const int MAX_USERS_COUNT = 100000;
const int MAX_PAGES_COUNT = 1000;
class ReadingMotivator {
public:
	ReadingMotivator() :
		users_pages_(MAX_USERS_COUNT + 1),
		pages_count_(MAX_PAGES_COUNT + 1) {

		for (auto& element : users_pages_) {
			element = -1;
		}
		for (auto& page : pages_count_) {
			page = 0;
		}
	}

	void Read(int user, int page) {

		if (users_pages_[user] >= 0) { --pages_count_[users_pages_[user]]; }
		else { ++users_counter_; }
		users_pages_[user] = page;
		++pages_count_[page];
	}

	double Cheer(int user) {

		if (users_pages_[user] == -1) { return 0.0; }
		if (users_counter_ == 1) { return 1.0; }
		int week_readers = accumulate(pages_count_.begin(), next(pages_count_.begin(), users_pages_[user]), 0);
		return (week_readers * 1.0 / (users_counter_ * 1.0 - 1));
	}

private:
	vector<int> users_pages_;
	vector<int> pages_count_;
	int users_counter_ = 0;
};

int main() {
	cout << setprecision(6);
	int requests_count;
	cin >> requests_count;
	ReadingMotivator motivator;
	for (int i = 0; i < requests_count; ++i) {
		string request;
		cin >> request;
		if (request == "READ"s) {
			int user, pages;
			cin >> user >> pages;
			motivator.Read(user, pages);
		}
		else if (request == "CHEER"s) {
			int user;
			cin >> user;
			cout << motivator.Cheer(user) << endl;
		}
	}
	return 0;
}