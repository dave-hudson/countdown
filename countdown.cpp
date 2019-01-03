/*
 * countdown.cpp
 */
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

/*
 * Starting grid.
 */
int starting_grid[] = {
    100, 75, 50, 25,
    10, 10, 9, 9, 8, 8, 7, 7, 6, 6,
    5, 5, 4, 4, 3, 3, 2, 2, 1, 1
};

int puzzle[6];

int target;
int closest;

/*
 * Result enum.
 */
enum class permute_result {
    match,
    close,
    nothing
};

auto permute_add(const std::vector<int> &v) -> permute_result;
auto permute_subtract(const std::vector<int> &v) -> permute_result;
auto permute_multiply(const std::vector<int> &v) -> permute_result;
auto permute_divide(const std::vector<int> &v) -> permute_result;

/*
 * Run all the possible permutations for the current input vector.
 */
auto permute_all(const std::vector<int> &v) -> permute_result {
    permute_result ret = permute_result::nothing;

    auto pa = permute_add(v);
    if (pa == permute_result::match) {
        return permute_result::match;
    }

    if (pa == permute_result::close) {
        ret = permute_result::close;
    }

    auto ps = permute_subtract(v);
    if (ps == permute_result::match) {
        return permute_result::match;
    }

    if (ps == permute_result::close) {
        ret = permute_result::close;
    }

    auto pm = permute_multiply(v);
    if (pm == permute_result::match) {
        return permute_result::match;
    }

    if (pm == permute_result::close) {
        ret = permute_result::close;
    }

    auto pd = permute_divide(v);
    if (pd == permute_result::match) {
        return permute_result::match;
    }

    if (pd == permute_result::close) {
        ret = permute_result::close;
    }

    return ret;
}

/*
 * Run permutations of an input vector for addition.
 */
auto permute_add(const std::vector<int> &v) -> permute_result {
    auto sz = v.size();
    permute_result ret = permute_result::nothing;
    std::vector<int> r;

    for (auto i = 0; i < sz; ++i) {
        for (auto j = 0; j < sz; ++j) {
            /*
             * We can't use the same tile or intermediate value twice.
             */
            if (i == j) {
                continue;
            }

            auto new_val = v[i] + v[j];
            if (new_val == target) {
                std::cout << "\nexact: " << new_val << " = " << v[i] << " + " << v[j] << '\n';
                return permute_result::match;
            }

            /*
             * If our subtraction results in a zero value then it's a dead end.
             */
            if (new_val == 0) {
                break;
            }

            /*
             * If we have more than 2 values then we can iterate further.
             */
            if (sz > 2) {
                r.reserve(sz - 1);
                r.emplace_back(new_val);

                for (auto k = 0; k < sz; ++k) {
                    if ((k != i) && (k != j)) {
                        r.emplace_back(v[k]);
                    }
                }

                auto pa = permute_all(r);
                if (pa != permute_result::nothing) {
                    std::cout << new_val << " = " << v[i] << " + " << v[j] << '\n';
                    return pa;
                }

                r.clear();
            }

            if (abs(target - new_val) < abs(target - closest)) {
                std::cout << "\nclose: " << new_val << " = " << v[i] << " + " << v[j] << '\n';
                closest = new_val;
                ret = permute_result::close;
            }
        }
    }

    return ret;
}

/*
 * Run permutations of an input vector for subtraction.
 */
auto permute_subtract(const std::vector<int> &v) -> permute_result {
    auto sz = v.size();
    permute_result ret = permute_result::nothing;
    std::vector<int> r;

    for (auto i = 0; i < sz; ++i) {
        for (auto j = 0; j < sz; ++j) {
            /*
             * We can't use the same tile or intermediate value twice.
             */
            if (i == j) {
                continue;
            }

            auto new_val = v[i] - v[j];
            if (new_val == target) {
                std::cout << "\nexact: " << new_val << " = " << v[i] << " - " << v[j] << '\n';
                return permute_result::match;
            }

            /*
             * If our subtraction results in a zero value then it's a dead end.  Similarly,
             * any negative value can always be handled as a positive one but with a
             * subtraction rather than an addition.
             */
            if (new_val <= 0) {
                break;
            }

            /*
             * If we have more than 2 values then we can iterate further.
             */
            if (sz > 2) {
                r.reserve(sz - 1);
                r.emplace_back(new_val);

                for (auto k = 0; k < sz; ++k) {
                    if ((k != i) && (k != j)) {
                        r.emplace_back(v[k]);
                    }
                }

                auto pa = permute_all(r);
                if (pa != permute_result::nothing) {
                    std::cout << new_val << " = " << v[i] << " - " << v[j] << '\n';
                    return pa;
                }

                r.clear();
            }

            if (abs(target - new_val) < abs(target - closest)) {
                std::cout << "\nclose: " << new_val << " = " << v[i] << " - " << v[j] << '\n';
                closest = new_val;
                ret = permute_result::close;
            }
        }
    }

    return ret;
}

/*
 * Run permutations of an input vector for multiplication.
 */
auto permute_multiply(const std::vector<int> &v) -> permute_result {
    auto sz = v.size();
    permute_result ret = permute_result::nothing;
    std::vector<int> r;

    for (auto i = 0; i < sz; ++i) {
        for (auto j = 0; j < sz; ++j) {
            /*
             * We can't use the same tile or intermediate value twice.
             */
            if (i == j) {
                continue;
            }

            auto new_val = v[i] * v[j];
            if (new_val == target) {
                std::cout << "\nexact: " << new_val << " = " << v[i] << " * " << v[j] << '\n';
                return permute_result::match;
            }

            /*
             * If we have more than 2 values then we can iterate further.
             */
            if (sz > 2) {
                r.reserve(sz - 1);
                r.emplace_back(new_val);

                for (auto k = 0; k < sz; ++k) {
                    if ((k != i) && (k != j)) {
                        r.emplace_back(v[k]);
                    }
                }

                auto pa = permute_all(r);
                if (pa != permute_result::nothing) {
                    std::cout << new_val << " = " << v[i] << " * " << v[j] << '\n';
                    return pa;
                }

                r.clear();
            }

            if (abs(target - new_val) < abs(target - closest)) {
                std::cout << "\nclose: " << new_val << " = " << v[i] << " * " << v[j] << '\n';
                closest = new_val;
                ret = permute_result::close;
            }
        }
    }

    return ret;
}

/*
 * Run permutations of an input vector for division.
 */
auto permute_divide(const std::vector<int> &v) -> permute_result {
    auto sz = v.size();
    permute_result ret = permute_result::nothing;
    std::vector<int> r;

    for (auto i = 0; i < sz; ++i) {
        for (auto j = 0; j < sz; ++j) {
            /*
             * We can't use the same tile or intermediate value twice.
             */
            if (i == j) {
                continue;
            }

            auto new_val = v[i] / v[j];

            /*
             * If our division does not result in an exact quotient then we can't make
             * any more progress down this particular path.
             */
            if (v[i] % v[j]) {
                break;
            }

            if (new_val == target) {
                std::cout << "\nexact: " << new_val << " = " << v[i] << " / " << v[j] << '\n';
                return permute_result::match;
            }

            /*
             * If we have more than 2 values then we can iterate further.
             */
            if (sz > 2) {
                r.reserve(sz - 1);
                r.emplace_back(new_val);

                for (auto k = 0; k < sz; ++k) {
                    if ((k != i) && (k != j)) {
                        r.emplace_back(v[k]);
                    }
                }

                auto pa = permute_all(r);
                if (pa != permute_result::nothing) {
                    std::cout << new_val << " = " << v[i] << " / " << v[j] << '\n';
                    return pa;
                }

                r.clear();
            }

            if (abs(target - new_val) < abs(target - closest)) {
                std::cout << "\nclose: " << new_val << " = " << v[i] << " / " << v[j] << '\n';
                closest = new_val;
                ret = permute_result::close;
            }
        }
    }

    return ret;
}

/*
 * Select the numbers for the game.
 */
auto setup_puzzle() -> std::vector<int> {
    std::vector<int> v;

    /*
     * Randomize our tiles.
     */
    srandom(time(NULL));

    /*
     * Pick a random target.
     */
    target = random() % 1000;
    closest = 0;

    /*
     * Pick 6 unique random tiles.
     */
    for (auto i = 0; i < 6; ++i) {
        int time;
        do {
            time = random() % 24;
        } while (starting_grid[time] < 0);

        v.emplace_back(starting_grid[time]);
        starting_grid[time] = -1;
    }

    return v;
}

/*
 * Entry point.
 */
auto main(int argc, char **argv) -> int {
    auto v = setup_puzzle();
    std::cout << target << " : ";

    auto sz = v.size();
    for (auto i = 0; i < sz; ++i) {
        std::cout << v[i] << " ";
    }

    std::cout << "\n\n";

    for (auto i = 0; i < sz; ++i) {
        if (target == v[i]) {
            std::cout << target << " = " << v[i] << '\n';
            return 0;
        }
    }

    if (permute_all(v) == permute_result::match) {
        return 0;
    }

    return 1;
}

