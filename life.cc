#include <vector>
#include <iostream>
#include <chrono>
#include <thread>

class Game {
public:
    Game(int x, int y):
        field_(x, std::vector<int>(y, ' '))
    {}

    void set(int x, int y) {
        field_[x % field_.size()][y % field_[0].size()] = '*';
    }

    void render(std::ostream& out) const {
        for (const auto& r: field_) {
            for (const char c: r)
                out << c;
            out << "\n";
        }
    }

    void next() {
        auto future = field_;
        for (auto& r: future)
            for (auto& c: r)
                c = ' ';

        for (int x = 0; x < field_.size(); ++x)
            for (int y = 0; y < field_[x].size(); ++y)
                switch (count_neighb(x, y)) {
                case 0:
                case 1:
                    future[x][y] = ' ';
                    break;
                case 2:
                    future[x][y] = field_[x][y];
                    break;
                case 3:
                    future[x][y] = '*';
                    break;
                default:
                    future[x][y] = ' ';
                    break;
                }

        std::swap(field_, future);
    }

private:
    int count_neighb(int x, int y) const {
        int n_nb = 0;
        for (auto mod: {
            std::make_pair(0, 1),
            std::make_pair(1, 0),
            std::make_pair(0, -1),
            std::make_pair(-1, 0),
            std::make_pair(1, 1),
            std::make_pair(-1, -1),
            std::make_pair(1, -1),
            std::make_pair(-1, 1)
        }) {
            int x_n = (x + mod.first + field_.size()) % field_.size();
            int y_n = (y + mod.second + field_[0].size()) % field_[0].size();
            if (field_[x_n][y_n] == '*')
                ++n_nb;
        }
        return n_nb;
    }

    std::vector<std::vector<int>> field_;
};

int main(int argc, char** argv) try {
    Game g{std::stoi(argv[1]), std::stoi(argv[2])};
    for (int i = 0; i < std::stoi(argv[3]); ++i)
        g.set(rand(), rand());

    for (;; std::this_thread::sleep_for(std::chrono::milliseconds(100))) {
        g.next();
        g.render(std::cout);
        std::cout << "=============\n";
    }
    return 0;
} catch (const std::exception& x) {
    std::cerr << x.what() << "\n";
    return 0;
}
