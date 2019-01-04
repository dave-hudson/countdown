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
bool find_all_exact = true;

/*
 * Result enum.
 */
enum class permute_result {
    match,                              // We got an exact match
    close,                              // We have our current closest match
    nothing                             // This path has yielded nothing new
};

/*
 * Operator type enum.
 */
enum class operator_type {
    add,
    subtract,
    multiply,
    divide
};

/*
 * Computation step recording.
 */
class step {
public:
    int _result;
    operator_type _op_type;
    int _operand1;
    int _operand2;

    step(int result, operator_type op_type, int operand1, int operand2) {
        _result = result;
        _op_type = op_type;
        _operand1 = operand1;
        _operand2 = operand2;
    }
};

auto permute_add(std::vector<step> &s, const std::vector<int> &v) -> permute_result;
auto permute_subtract(std::vector<step> &s, const std::vector<int> &v) -> permute_result;
auto permute_multiply(std::vector<step> &s, const std::vector<int> &v) -> permute_result;
auto permute_divide(std::vector<step> &s, const std::vector<int> &v) -> permute_result;

/*
 * Output a step vector.
 */
auto dump_steps(const std::vector<step> &s, const std::string &str) {
    auto sz = s.size();
    std::cout << str << ": ";

    for (auto i = 0; i < sz; ++i) {
        if (i != 0) {
            std::cout << ", ";
        }

        auto &st = s[i];
        std::cout << st._result << " = " << st._operand1;

        switch (st._op_type) {
        case operator_type::add:
            std::cout << " + ";
            break;

        case operator_type::subtract:
            std::cout << " - ";
            break;

        case operator_type::multiply:
            std::cout << " * ";
            break;

        case operator_type::divide:
            std::cout << " / ";
            break;
        }

        std::cout << st._operand2;
    }

    std::cout << '\n';
}

/*
 * Run all the possible permutations for the current input vector.
 */
auto permute_all(std::vector<step> &s, const std::vector<int> &v) -> permute_result {
    permute_result ret = permute_result::nothing;

    auto pa = permute_add(s, v);
    if (pa == permute_result::match) {
        if (!find_all_exact) {
            return permute_result::match;
        }

        ret = permute_result::match;
    }

    if (pa == permute_result::close) {
        ret = permute_result::close;
    }

    auto ps = permute_subtract(s, v);
    if (ps == permute_result::match) {
        if (!find_all_exact) {
            return permute_result::match;
        }

        ret = permute_result::match;
    }

    if (ps == permute_result::close) {
        ret = permute_result::close;
    }

    auto pm = permute_multiply(s, v);
    if (pm == permute_result::match) {
        if (!find_all_exact) {
            return permute_result::match;
        }

        ret = permute_result::match;
    }

    if (pm == permute_result::close) {
        ret = permute_result::close;
    }

    auto pd = permute_divide(s, v);
    if (pd == permute_result::match) {
        if (!find_all_exact) {
            return permute_result::match;
        }

        ret = permute_result::match;
    }

    if (pd == permute_result::close) {
        ret = permute_result::close;
    }

    return ret;
}

/*
 * Run permutations of an input vector for addition.
 */
auto permute_add(std::vector<step> &s, const std::vector<int> &v) -> permute_result {
    auto sz = v.size();
    permute_result ret = permute_result::nothing;
    std::vector<int> r;

    /*
     * We want to find all the permutations of additions within the input vector.
     * Addition is commutative so (a + b) = (b + a), meaning we don't need to examine
     * any scenarios we've already seen.
     */
    for (auto i = 0; i < (sz - 1); ++i) {
        for (auto j = i + 1; j < sz; ++j) {
            /*
             * Compute a permutation, and check if it results in an exact match.
             */
            auto new_val = v[i] + v[j];
            s.emplace_back(step(new_val, operator_type::add, v[i], v[j]));

            if (new_val == target) {
                dump_steps(s, "exact");
                if (!find_all_exact) {
                    return permute_result::match;
                }

                closest = new_val;
                ret = permute_result::match;
            }

            /*
             * If we have more than 2 values in our input vector then we can iterate further,
             * combining our new result with all unused inputs.
             */
            if (sz > 2) {
                r.reserve(sz - 1);
                r.emplace_back(new_val);

                for (auto k = 0; k < sz; ++k) {
                    if ((k != i) && (k != j)) {
                        r.emplace_back(v[k]);
                    }
                }

                auto pa = permute_all(s, r);
                if (pa == permute_result::match) {
                    if (!find_all_exact) {
                        return permute_result::match;
                    }
                }

                r.clear();
            }

            if (abs(target - new_val) < abs(target - closest)) {
                dump_steps(s, "close");
                closest = new_val;
                ret = permute_result::close;
            }

            s.pop_back();
        }
    }

    return ret;
}

/*
 * Run permutations of an input vector for subtraction.
 */
auto permute_subtract(std::vector<step> &s, const std::vector<int> &v) -> permute_result {
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

            /*
             * If our subtraction results in a zero value then it's a dead end.  Similarly,
             * any negative value can always be handled as a positive one but with a
             * subtraction rather than an addition.
             */
            if (new_val <= 0) {
                continue;
            }

            /*
             * If our subtraction results in the same value as our subtrahend then we've also
             * hit a dead end because we've not introduced any useful new intermediate value.
             */
            if (new_val == v[j]) {
                continue;
            }

            s.emplace_back(step(new_val, operator_type::subtract, v[i], v[j]));

            if (new_val == target) {
                dump_steps(s, "exact");
                if (!find_all_exact) {
                    return permute_result::match;
                }

                closest = new_val;
                ret = permute_result::match;
            }

            /*
             * If we have more than 2 values in our input vector then we can iterate further,
             * combining our new result with all unused inputs.
             */
            if (sz > 2) {
                r.reserve(sz - 1);
                r.emplace_back(new_val);

                for (auto k = 0; k < sz; ++k) {
                    if ((k != i) && (k != j)) {
                        r.emplace_back(v[k]);
                    }
                }

                auto pa = permute_all(s, r);
                if (pa == permute_result::match) {
                    if (!find_all_exact) {
                        return permute_result::match;
                    }
                }

                r.clear();
            }

            if (abs(target - new_val) < abs(target - closest)) {
                dump_steps(s, "close");
                closest = new_val;
                ret = permute_result::close;
            }

            s.pop_back();
        }
    }

    return ret;
}

/*
 * Run permutations of an input vector for multiplication.
 */
auto permute_multiply(std::vector<step> &s, const std::vector<int> &v) -> permute_result {
    auto sz = v.size();
    permute_result ret = permute_result::nothing;
    std::vector<int> r;

    /*
     * We want to find all the permutations of multiplications within the input vector.
     * Multiplication is commutative so (a * b) = (b * a), meaning we don't need to examine
     * any scenarios we've already seen.
     */
    for (auto i = 0; i < (sz - 1); ++i) {
        /*
         * Multiplying by 1 doesn't get us anywhere.
         */
        if (v[i] == 1) {
            continue;
        }

        for (auto j = i + 1; j < sz; ++j) {
            /*
             * Multiplying by 1 doesn't get us anywhere.
             */
            if (v[j] == 1) {
                continue;
            }

            auto new_val = v[i] * v[j];
            s.emplace_back(step(new_val, operator_type::multiply, v[i], v[j]));

            if (new_val == target) {
                dump_steps(s, "exact");
                if (!find_all_exact) {
                    return permute_result::match;
                }

                closest = new_val;
                ret = permute_result::match;
            }

            /*
             * If we have more than 2 values in our input vector then we can iterate further,
             * combining our new result with all unused inputs.
             */
            if (sz > 2) {
                r.reserve(sz - 1);
                r.emplace_back(new_val);

                for (auto k = 0; k < sz; ++k) {
                    if ((k != i) && (k != j)) {
                        r.emplace_back(v[k]);
                    }
                }

                auto pa = permute_all(s, r);
                if (pa == permute_result::match) {
                    if (!find_all_exact) {
                        return permute_result::match;
                    }
                }

                r.clear();
            }

            if (abs(target - new_val) < abs(target - closest)) {
                dump_steps(s, "close");
                closest = new_val;
                ret = permute_result::close;
            }

            s.pop_back();
        }
    }

    return ret;
}

/*
 * Run permutations of an input vector for division.
 */
auto permute_divide(std::vector<step> &s, const std::vector<int> &v) -> permute_result {
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

            /*
             * Dividing by 1 doesn't get us anywhere.
             */
            if (v[j] == 1) {
                continue;
            }

            auto new_val = v[i] / v[j];

            /*
             * If our division does not result in an exact quotient then we can't make
             * any more progress down this particular path.
             */
            if (v[i] % v[j]) {
                continue;
            }

            /*
             * If our division results in the same value as our divisor then we've also
             * hit a dead end because we've not introduced any useful new intermediate value.
             */
            if (new_val == v[j]) {
                continue;
            }

            s.emplace_back(step(new_val, operator_type::divide, v[i], v[j]));

            if (new_val == target) {
                dump_steps(s, "exact");
                if (!find_all_exact) {
                    return permute_result::match;
                }

                closest = new_val;
                ret = permute_result::match;
            }

            /*
             * If we have more than 2 values in our input vector then we can iterate further,
             * combining our new result with all unused inputs.
             */
            if (sz > 2) {
                r.reserve(sz - 1);
                r.emplace_back(new_val);

                for (auto k = 0; k < sz; ++k) {
                    if ((k != i) && (k != j)) {
                        r.emplace_back(v[k]);
                    }
                }

                auto pa = permute_all(s, r);
                if (pa == permute_result::match) {
                    if (!find_all_exact) {
                        return permute_result::match;
                    }
                }

                r.clear();
            }

            if (abs(target - new_val) < abs(target - closest)) {
                dump_steps(s, "close");
                closest = new_val;
                ret = permute_result::close;
            }

            s.pop_back();
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

    /*
     * In trivial cases we just have the right number in our starting set.
     */
    for (auto i = 0; i < sz; ++i) {
        if (target == v[i]) {
            std::cout << "exact: " << target << " = " << v[i] << '\n';
            return 0;
        }
    }

    std::vector<step> s;
    auto pa = permute_all(s, v);
    std::cout << '\n';
    if (pa == permute_result::match) {
        return 0;
    }

    return 1;
}

