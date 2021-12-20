#include <vector>
#include <iostream>
#include <cassert>
#include <climits>
#include <exception>

using SolutionMap = std::vector<std::vector<int>>;

int solve(SolutionMap& sols, int stores, int nuts) {
    assert(nuts >= 1);

    if (nuts == 1)
        return stores;

    if (stores == 0)
        return 0;

    if (sols[stores][nuts] >= 0)
        return sols[stores][nuts];

    int sol = INT_MAX;
    for (int throw_pos = 0; throw_pos < stores; ++throw_pos)
        sol = std::min(sol, 1 + std::max(solve(sols, throw_pos, nuts-1), solve(sols, stores - throw_pos - 1, nuts)));

    sols[stores][nuts] = sol;
    return sol;
}

int solve(int stores, int nuts) {
    if (nuts < 1)
        throw std::invalid_argument("nuts < 1");
    if (stores < 0)
        throw std::invalid_argument("stores < 0");

    SolutionMap solmap(stores + 1, std::vector<int>(nuts + 1, -1));
    int rc = solve(solmap, stores, nuts);
    return rc;
}

int main(int argc, char** argv) try {
    // some test cases
    assert(solve(1, 1) == 1);
    assert(solve(2, 1) == 2);
    assert(solve(1, 2) == 1);
    assert(solve(15, 1) == 15);
    assert(solve(2, 2) == 2);
    assert(solve(3, 2) == 2);
    assert(solve(4, 2) == 3);
    assert(solve(5, 2) == 3);
    assert(solve(6, 2) == 3);
    assert(solve(7, 2) == 4);
    assert(solve(8, 2) == 4);
    assert(solve(9, 2) == 4);
    assert(solve(10, 2) == 4);
    assert(solve(12, 2) == 5);
    assert(solve(13, 2) == 5);
    assert(solve(14, 2) == 5);
    assert(solve(15, 2) == 5);

    int stores, nuts;
    std::cin >> stores;
    std::cin >> nuts;
    std::cout << solve(stores, nuts) << "\n";
    return 0;
} catch(const std::exception& x) {
    std::cerr << x.what() << "\n";
    return 1;
}
