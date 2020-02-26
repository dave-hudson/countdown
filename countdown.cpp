/*
 * countdown.cpp
 *
 * Solves the Countdown numbers game (from UK Channel 4).
 */
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

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

/*
 * Countdown computation class.
 */
class countdown {
public:
    countdown(int target, const std::vector<int> tiles) {
        _target = target;
        _tiles = tiles;
        _closest = 0;
        _fewest_steps = 7;
        _rounds = 0;
    };

    auto compute() -> void {
        permute_all(_tiles);
    }

    auto get_best_steps() -> const std::vector<step> {
        return _best_steps;
    }

    auto get_closest() -> int {
        return _closest;
    }

    auto get_rounds() -> int {
        return _rounds;
    }

private:
    std::vector<int> _tiles;            // The tiles we have to use
    int _target;                        // The number are we trying to hit
    int _closest;                       // The closest we've got to the target number
    int _rounds;                        // The number of rounds we do to reach our result
    int _fewest_steps;                  // Smallest number of steps so far
    std::vector<step> _working_steps;   // The set of working steps we have in any given iteration
    std::vector<step> _best_steps;      // Best set of steps that we found

    auto permute_add(const std::vector<int> &v) -> void;
    auto permute_subtract(const std::vector<int> &v) -> void;
    auto permute_multiply(const std::vector<int> &v) -> void;
    auto permute_divide(const std::vector<int> &v) -> void;
    auto permute_common(const std::vector<int> &v, int new_val, operator_type op, int i, int j) -> void;

    /*
     * Run all the possible permutations for the current input vector.
     */
    auto permute_all(const std::vector<int> &v) -> void {
        _rounds++;

        permute_add(v);
        permute_subtract(v);
        permute_multiply(v);
        permute_divide(v);
    }
};

/*
 * Starting grid.
 */
int starting_grid[] = {
    100, 75, 50, 25,
    10, 10, 9, 9, 8, 8, 7, 7, 6, 6,
    5, 5, 4, 4, 3, 3, 2, 2, 1, 1
};

/*
 * Handle the common operations associated with any permutation.
 */
auto countdown::permute_common(const std::vector<int> &v, int new_val, operator_type op, int i, int j) -> void {
    auto sz = static_cast<int>(v.size());

    _working_steps.emplace_back(step(new_val, op, v[i], v[j]));

    if (new_val == _target) {
        _best_steps = _working_steps;
        _fewest_steps = static_cast<int>(_working_steps.size());
        _closest = new_val;
    }

    /*
     * If another iteration can still result in a shorter solution than the best
     * we've found so far then proceed.
     */
    if ((_fewest_steps - 1) > static_cast<int>(_working_steps.size())) {
        /*
         * If we have more than 2 values in our input vector then we can iterate further,
         * combining our new result with all unused inputs.
         */
        if (sz > 2) {
            std::vector<int> r;
            r.reserve(sz - 1);
            r.emplace_back(new_val);

            for (auto k = 0; k < sz; ++k) {
                if ((k != i) && (k != j)) {
                    r.emplace_back(v[k]);
                }
            }

            permute_all(r);
            r.clear();
        }

        if (abs(_target - new_val) < abs(_target - _closest)) {
            _best_steps = _working_steps;
            _closest = new_val;
        }
    }

    _working_steps.pop_back();
}

/*
 * Run permutations of an input vector for addition.
 */
auto countdown::permute_add(const std::vector<int> &v) -> void {
    auto sz = static_cast<int>(v.size());

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
            permute_common(v, new_val, operator_type::add, i, j);
        }
    }
}

/*
 * Run permutations of an input vector for subtraction.
 */
auto countdown::permute_subtract(const std::vector<int> &v) -> void {
    auto sz = static_cast<int>(v.size());

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

            permute_common(v, new_val, operator_type::subtract, i, j);
        }
    }
}

/*
 * Run permutations of an input vector for multiplication.
 */
auto countdown::permute_multiply(const std::vector<int> &v) -> void {
    auto sz = static_cast<int>(v.size());

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
            permute_common(v, new_val, operator_type::multiply, i, j);
        }
    }
}

/*
 * Run permutations of an input vector for division.
 */
auto countdown::permute_divide(const std::vector<int> &v) -> void {
    auto sz = static_cast<int>(v.size());

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

            permute_common(v, new_val, operator_type::divide, i, j);
        }
    }
}

/*
 * Output a step vector.
 */
auto dump_steps(const std::vector<step> &s) -> void {
    auto sz = static_cast<int>(s.size());
    for (auto i = 0; i < sz; ++i) {
        auto &st = s[i];
        std::cout << st._operand1;

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

        std::cout << st._operand2 << " = " << st._result << '\n';
    }

    std::cout << '\n';
}

/*
 * Entry point.
 */
auto main(int argc, char **argv) -> int {
    std::vector<int> v;

    /*
     * Randomize our tiles.
     */
    srandom(time(NULL));

    std::cout << "Numbers are:";

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

        std::cout << " " << v[i];
    }

    /*
     * Pick a random target in the range 101 to 999.
     */
    auto target = (random() % 899) + 101;

    std::cout << ", target is: " << target << "\n\n";

    /*
     * Set up the problem and compute the best solution.
     */
    countdown c(target, v);
    c.compute();

    /*
     * Output the results.
     */
    std::cout << "after: ";
    std::cout << c.get_rounds();
    std::cout << " rounds, ";

    auto closest = c.get_closest();
    if (closest == target) {
        std::cout << "solved:\n\n";
    } else {
        std::cout << abs(target - closest) << " away:\n\n";
    }

    dump_steps(c.get_best_steps());

    return 0;
}

