import itertools
from pathlib import Path
import random
import shutil
import subprocess
import tempfile
import unittest

from pip_solver import PIPSolver, PIP_generate_cpp, PIP_generate_py, PIPUnboundedError


def python_function(solver, name="solve"):
    namespace = {}
    exec(solver.python_source(name), namespace)
    return namespace[name]


def brute_force(problem, parameters, limit=4):
    A, B, c, d = problem
    if any(p < 0 for p in parameters):
        return None
    best = None
    for x in itertools.product(range(limit + 1), repeat=len(c)):
        if all(sum(a * xi for a, xi in zip(row, x))
               <= sum(b * p for b, p in zip(rhs, parameters)) + offset
               for row, rhs, offset in zip(A, B, d)):
            value = sum(ci * xi for ci, xi in zip(c, x))
            best = value if best is None else max(best, value)
    return best


def random_problems():
    rng = random.Random(20261020)
    for _ in range(20):
        # The first two rows enforce 0 <= x0,x1 <= 4, making enumeration exact.
        A, B, d = [[1, 0], [0, 1]], [[0, 0], [0, 0]], [4, 4]
        for _ in range(3):
            A.append([rng.randint(-3, 3) for _ in range(2)])
            B.append([rng.randint(-2, 2) for _ in range(2)])
            d.append(rng.randint(-4, 4))
        yield A, B, [rng.randint(-3, 3) for _ in range(2)], d


class PIPTests(unittest.TestCase):
    def test_weighted_objective_instead_of_lexicographic_x(self):
        solve = python_function(PIPSolver([[1, 1]], [[1]], [1, 10], [0]))
        for p in range(8):
            self.assertEqual(solve(p), 10 * p)

    def test_floors_and_piecewise_minimum(self):
        solve = python_function(PIPSolver([[2], [1]], [[1, 0], [0, 1]], [3], [0, 0]))
        for p, q in itertools.product(range(10), repeat=2):
            self.assertEqual(solve(p, q), 3 * min(p // 2, q))

    def test_congruence_and_infeasible_domain(self):
        solve = python_function(PIPSolver([[2], [-2]], [[1], [-1]], [1], [0, 0]))
        for p in range(-3, 15):
            self.assertEqual(solve(p), p // 2 if p >= 0 and p % 2 == 0 else None)

    def test_negative_objective_and_shifted_floors(self):
        solve = python_function(PIPSolver([[-3]], [[-1]], [-2], [5]))
        for p in range(20):
            self.assertEqual(solve(p), -2 * max(0, (p - 3) // 3))

    def test_unbounded_variables_with_bounded_objective(self):
        self.assertEqual(python_function(PIPSolver([], [], [-1, 0], []))(), 0)
        solve = python_function(PIPSolver([[1, 0]], [[1]], [2, 0], [0]))
        self.assertEqual(solve(5), 10)

    def test_unbounded_objective(self):
        for problem in [([], [], [1], []), ([[-1]], [[1]], [2], [0]),
                        ([[1, -1]], [[1]], [1, 1], [0])]:
            with self.subTest(problem=problem), self.assertRaises(PIPUnboundedError):
                PIPSolver(*problem)
        # A feasible p can be far from zero; checking a few p values is insufficient.
        with self.assertRaises(PIPUnboundedError):
            PIPSolver([[0]], [[1]], [1], [-1000000])
        # An empty feasible set has no unbounded optimum.
        self.assertIsNone(python_function(PIPSolver([[0]], [[0]], [1], [-1]))(0))

    def test_zero_parameters_and_zero_variables(self):
        self.assertEqual(python_function(PIPSolver([[2]], [[]], [3], [9]))(), 12)
        self.assertEqual(python_function(PIPSolver([], [], [], []))(), 0)
        solve = python_function(PIPSolver([[]], [[1]], [], [-2]))
        self.assertIsNone(solve(1))
        self.assertEqual(solve(2), 0)

    def test_invalid_input(self):
        for problem in [([[1, 2]], [[1]], [1], [0]),
                        ([[1]], [], [1], [0]),
                        ([[1]], [[1]], [1], []),
                        ([[1], [1]], [[1], []], [1], [0, 0]),
                        ([[1.0]], [[1]], [1], [0]),
                        ([[1]], [[1]], [True], [0])]:
            with self.subTest(problem=problem), self.assertRaises(ValueError):
                PIPSolver(*problem)
        solver = PIPSolver([[1]], [[1]], [1], [0])
        for name in ["class", "int", "a-b", "1solve", "a__b", "std", "boost"]:
            with self.assertRaises(ValueError):
                solver.python_source(name)
        with self.assertRaises(TypeError):
            python_function(solver)(1.5)

    def test_file_generation(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            problem = ([[2]], [[1]], [3], [0])
            py = PIP_generate_py(*problem, root / "generated.py", "answer")
            cpp = PIP_generate_cpp(*problem, root / "generated.cpp", "answer")
            namespace = {}
            exec(py, namespace)
            self.assertEqual(namespace["answer"](7), 9)
            self.assertIn("answer(", cpp)
            self.assertEqual((root / "generated.py").read_text(), py)
            self.assertEqual((root / "generated.cpp").read_text(), cpp)
            with self.assertRaises(PIPUnboundedError):
                PIP_generate_py([], [], [1], [], root / "generated.py")
            self.assertIn("def answer", (root / "generated.py").read_text())
            for integer_type in ("cpp_int", "__int128", "long long"):
                source = PIP_generate_cpp(*problem, root / "native.cpp", "answer", integer_type)
                self.assertEqual((root / "native.cpp").read_text(), source)
                self.assertEqual(source, PIPSolver(*problem).cpp_source("answer", integer_type))

    def test_cpp_integer_type_validation(self):
        solver = PIPSolver([[1]], [[1]], [1], [0])
        self.assertEqual(solver.cpp_source(), solver.cpp_source(integer_type="cpp_int"))
        self.assertEqual(solver.cpp_source(), solver.cpp_source(
            integer_type="boost::multiprecision::cpp_int"))
        for integer_type in ("int", "int128", "", None):
            with self.subTest(integer_type=integer_type), self.assertRaises(ValueError):
                solver.cpp_source(integer_type=integer_type)
        for integer_type, bits in (("long long", 64), ("__int128", 128)):
            for value in (1 << (bits - 1), -(1 << (bits - 1)) - 1):
                solver = PIPSolver([[-1], [1]], [[], []], [value], [-1, 1])
                with self.subTest(integer_type=integer_type, value=value):
                    with self.assertRaises(OverflowError):
                        solver.cpp_source(integer_type=integer_type)

    def test_random_problems_against_enumeration(self):
        for problem in random_problems():
            solve = python_function(PIPSolver(*problem))
            for ps in itertools.product(range(-1, 5), repeat=2):
                with self.subTest(problem=problem, parameters=ps):
                    self.assertEqual(solve(*ps), brute_force(problem, ps))

    @unittest.skipUnless(shutil.which("g++"), "g++ is required for C++ integration")
    def test_cpp_compilation_and_execution(self):
        for integer_type in ("cpp_int", "__int128", "long long"):
            with self.subTest(integer_type=integer_type):
                self._check_cpp_compilation_and_execution(integer_type)

    def _check_cpp_compilation_and_execution(self, integer_type):
        cases = [
            (([[2], [1]], [[1, 0], [0, 1]], [3], [0, 0]),
             list(itertools.product(range(-1, 7), repeat=2))),
            (([[2], [-2]], [[1], [-1]], [1], [0, 0]), [(p,) for p in range(-1, 12)]),
            (([[-3]], [[-1]], [-2], [5]), [(p,) for p in range(-1, 20)]),
            (([], [], [-1, 0], []), [()]),
            (([], [], [], []), [()]),
            (([[0]], [[0]], [1], [-1]), [(0,)]),
        ]
        if integer_type == "cpp_int":
            cases.append((([[1]], [[10**50]], [10**60], [0]), [(10**40,)]))
        if integer_type != "long long":
            cases.append((([[1]], [[2**100]], [3], [0]), [(7,)]))
        bits = 128 if integer_type == "__int128" else 64
        for value in (-(1 << (bits - 1)), (1 << (bits - 1)) - 1):
            cases.append((([[-1], [1]], [[], []], [value], [-1, 1]), [()]))
        cases += [(problem, list(itertools.product(range(-1, 5), repeat=2)))
                  for problem in random_problems()]
        sources, calls, expected = [], [], []
        for j, (problem, parameters) in enumerate(cases):
            solver = PIPSolver(*problem)
            name = {0: "min", 2: "abs"}.get(j, f"solve{j}")
            solve = python_function(solver, name)
            source = solver.cpp_source(name, integer_type)
            if integer_type != "cpp_int":
                self.assertNotIn("boost", source)
            sources.append(source)
            for ps in parameters:
                value = solve(*ps)
                expected.append("infeasible" if value is None else str(value))
                args = ", ".join(f'parse<pip_detail_{name}::Integer>("{p}")' for p in ps)
                calls.append(f"    print({name}({args}));")
        harness = """
#include <iostream>
#include <string>
#include <type_traits>
template<class Integer> Integer parse(const char* text) {
    bool negative = *text == '-';
    if (negative) ++text;
    Integer value = 0;
    for (; *text; ++text) {
        if (negative) value = value * 10 - (*text - '0');
        else value = value * 10 + (*text - '0');
    }
    return value;
}
template<class Integer> void print(const std::optional<Integer>& value) {
    if (!value) { std::cout << "infeasible\\n"; return; }
    if constexpr (std::is_same_v<Integer, __int128>) {
        unsigned __int128 magnitude = static_cast<unsigned __int128>(*value);
        if (*value < 0) { std::cout << '-'; magnitude = 0 - magnitude; }
        std::string digits;
        do { digits += char('0' + magnitude % 10); magnitude /= 10; } while (magnitude);
        std::reverse(digits.begin(), digits.end());
        std::cout << digits;
    } else {
        std::cout << *value;
    }
    std::cout << '\\n';
}
int main() {
""" + "\n".join(calls) + "\n}\n"
        with tempfile.TemporaryDirectory() as directory:
            source, executable = Path(directory) / "test.cpp", Path(directory) / "test"
            source.write_text("\n".join(sources) + harness)
            subprocess.run(["g++", "-std=c++17", "-O0", "-Wall", "-Wextra",
                            "-fsanitize=undefined", "-fno-sanitize-recover=undefined",
                            str(source), "-o", str(executable)], check=True,
                           capture_output=True, text=True, timeout=90)
            result = subprocess.run([str(executable)], check=True, capture_output=True,
                                    text=True, timeout=30)
            self.assertEqual(result.stdout.splitlines(), expected)


if __name__ == "__main__":
    unittest.main()
