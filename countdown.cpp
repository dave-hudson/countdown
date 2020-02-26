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

    step(int result = 0, operator_type op_type = operator_type::add, int operand1 = 0, int operand2 = 0) {
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
    countdown(int target, int tiles[6]) {
        _target = target;
        for (auto i = 0; i < 6; i++) {
            _tiles[i] = tiles[i];
        }
        _closest = 0;
        _fewest_steps = 7;
        _rounds = 0;
        _working_steps_sz = 0;
        _best_steps_sz = 0;
    };

    auto compute() -> void {
        permute_all(_tiles, 6);
    }

    auto get_best_steps() -> const std::vector<step> {
        std::vector<step> s;
        for (auto i = 0; i < _best_steps_sz; ++i) {
            s.emplace_back(_best_steps[i]);
        }
        return s;
    }

    auto get_closest() -> int {
        return _closest;
    }

    auto get_rounds() -> int {
        return _rounds;
    }

private:
    int _tiles[6];                      // The tiles we have to use
    int _target;                        // The number are we trying to hit
    int _closest;                       // The closest we've got to the target number
    int _rounds;                        // The number of rounds we do to reach our result
    int _fewest_steps;                  // Smallest number of steps so far
    step _working_steps[6];             // The set of working steps we have in any given iteration
    int _working_steps_sz;              // Number of steps in our working set of steps
    step _best_steps[6];                // Best set of steps that we found
    int _best_steps_sz;                 // Number of steps in the best solution we currently have

    inline auto permute_add(const int v[6], int v_sz) -> void;
    inline auto permute_subtract(const int v[6], int v_sz) -> void;
    inline auto permute_multiply(const int v[6], int v_sz) -> void;
    inline auto permute_divide(const int v[6], int v_sz) -> void;
    auto permute_common(int new_val, operator_type op, const int v[6], int v_sz, int i, int j) -> void;

    /*
     * Run all the possible permutations for the current input vector.
     */
    auto permute_all(const int v[6], int v_sz) -> void {
        _rounds++;

        permute_add(v, v_sz);
        permute_subtract(v, v_sz);
        permute_multiply(v, v_sz);
        permute_divide(v, v_sz);
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
auto countdown::permute_common(int new_val, operator_type op, const int v[6], int v_sz, int i, int j) -> void {
    _working_steps[_working_steps_sz++] = step(new_val, op, v[i], v[j]);

    if (abs(_target - new_val) < abs(_target - _closest)) {
        for (auto i = 0; i < _working_steps_sz; ++i) {
            _best_steps[i] = _working_steps[i];
        }
        _best_steps_sz = _working_steps_sz;
        _closest = new_val;

        if (new_val == _target) {
            _fewest_steps = _working_steps_sz;
            _working_steps_sz--;
            return;
        }
    }

    /*
     * If another iteration can still result in a shorter solution than the best
     * we've found so far then proceed.
     */
    if ((_fewest_steps - 1) > _working_steps_sz) {
        /*
         * If we have more than 2 values in our input vector then we can iterate further,
         * combining our new result with all unused inputs.
         */
        if (v_sz > 2) {
            int new_v[6];
            auto idx = 0;
            new_v[idx++] = new_val;

            for (auto k = 0; k < v_sz; ++k) {
                if ((k != i) && (k != j)) {
                    new_v[idx++] = v[k];
                }
            }

            permute_all(new_v, v_sz - 1);
        }
    }

    _working_steps_sz--;
}

/*
 * Run permutations of an input array for addition.
 */
auto countdown::permute_add(const int v[6], int v_sz) -> void {
    /*
     * We want to find all the permutations of additions within the input array.
     * Addition is commutative so (a + b) = (b + a), meaning we don't need to examine
     * any scenarios we've already seen.
     */
    for (auto i = 0; i < (v_sz - 1); ++i) {
        for (auto j = i + 1; j < v_sz; ++j) {
            /*
             * Compute a permutation, and check if it results in an exact match.
             */
            auto new_val = v[i] + v[j];
            permute_common(new_val, operator_type::add, v, v_sz, i, j);
        }
    }
}

/*
 * Run permutations of an input array for subtraction.
 */
auto countdown::permute_subtract(const int v[6], int v_sz) -> void {
    for (auto i = 0; i < v_sz; ++i) {
        for (auto j = 0; j < v_sz; ++j) {
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

            permute_common(new_val, operator_type::subtract, v, v_sz, i, j);
        }
    }
}

/*
 * Run permutations of an input array for multiplication.
 */
auto countdown::permute_multiply(const int v[6], int v_sz) -> void {
    /*
     * We want to find all the permutations of multiplications within the input array.
     * Multiplication is commutative so (a * b) = (b * a), meaning we don't need to examine
     * any scenarios we've already seen.
     */
    for (auto i = 0; i < (v_sz - 1); ++i) {
        /*
         * Multiplying by 1 doesn't get us anywhere.
         */
        if (v[i] == 1) {
            continue;
        }

        for (auto j = i + 1; j < v_sz; ++j) {
            /*
             * Multiplying by 1 doesn't get us anywhere.
             */
            if (v[j] == 1) {
                continue;
            }

            auto new_val = v[i] * v[j];
            permute_common(new_val, operator_type::multiply, v, v_sz, i, j);
        }
    }
}

/*
 * Run permutations of an input array for division.
 */
auto countdown::permute_divide(const int v[6], int v_sz) -> void {
    for (auto i = 0; i < v_sz; ++i) {
        for (auto j = 0; j < v_sz; ++j) {
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

            permute_common(new_val, operator_type::divide, v, v_sz, i, j);
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
    int v[6];

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

        v[i] = starting_grid[time];
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

