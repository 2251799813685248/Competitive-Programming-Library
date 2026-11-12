"""Generate exact Python/C++ functions for max{c*x : A*x <= B*p+d, x,p>=0}.

islpy is required only while generating code. See README.md for the API.
"""

from __future__ import annotations

import keyword
import re
from numbers import Integral
from pathlib import Path

import islpy as isl

__all__ = ["PIPSolver", "PIP_generate_py", "PIP_generate_cpp", "PIPUnboundedError"]


class PIPUnboundedError(ValueError):
    """The objective is unbounded on the nonempty feasible parameter domain."""


def _vector(values, label):
    result = []
    for value in values:
        if isinstance(value, bool) or not isinstance(value, Integral):
            raise ValueError(f"{label} must contain integers (not bool/float)")
        result.append(int(value))
    return result


def _linear(coefficients, names):
    return " + ".join(f"{a}*{name}" for a, name in zip(coefficients, names) if a) or "0"


_CPP_KEYWORDS = set("""
alignas alignof and and_eq asm atomic_cancel atomic_commit atomic_noexcept auto
bitand bitor bool break case catch char char8_t char16_t char32_t class compl
concept const consteval constexpr constinit const_cast continue co_await co_return
co_yield decltype default delete do double dynamic_cast else enum explicit export
extern false float for friend goto if inline int long mutable namespace new noexcept
not not_eq nullptr operator or or_eq private protected public reflexpr register
reinterpret_cast requires return short signed sizeof static static_assert static_cast
struct switch synchronized template this thread_local throw true try typedef typeid
typename union unsigned using virtual void volatile wchar_t while xor xor_eq
""".split())


def _check_name(name):
    if (not isinstance(name, str) or not re.fullmatch(r"[A-Za-z][A-Za-z0-9_]*", name)
            or keyword.iskeyword(name) or name in _CPP_KEYWORDS
            or name in {"std", "boost"} or "__" in name):
        raise ValueError("function_name must be an identifier valid in Python and C++")


_CPP_INTEGER_TYPES = {
    "cpp_int": "boost::multiprecision::cpp_int",
    "boost::multiprecision::cpp_int": "boost::multiprecision::cpp_int",
    "__int128": "__int128",
    "long long": "long long",
}


def _cpp_constant(value, integer_type, namespace):
    integer = f"{namespace}::Integer"
    if integer_type == "boost::multiprecision::cpp_int":
        return f'{integer}("{value}")'
    bits = 128 if integer_type == "__int128" else 64
    if not -(1 << (bits - 1)) <= value < (1 << (bits - 1)):
        raise OverflowError(f"Generated constant {value} is outside the {integer_type} range")

    def positive(number):
        if number < (1 << 64):
            return f"static_cast<{integer}>({number}ULL)"
        high, low = divmod(number, 1 << 64)
        return (f"((static_cast<{integer}>({high}ULL) << 64)"
                f" | static_cast<{integer}>({low}ULL))")

    # Avoid forming the unrepresentable positive magnitude of the signed minimum.
    if value < 0:
        return f"(-{positive(-value - 1)} - {integer}(1))"
    return positive(value)


class _Emitter:
    """Translate isl AST nodes, preserving integer division semantics."""

    def __init__(self, language, namespace="", integer_type="boost::multiprecision::cpp_int"):
        self.language = language
        self.namespace = namespace
        self.integer_type = integer_type

    def emit(self, expr):
        kind = expr.get_type()
        if kind == isl.ast_expr_type.int:
            value = expr.get_val().to_python()
            return (str(value) if self.language == "py"
                    else _cpp_constant(value, self.integer_type, self.namespace))
        if kind == isl.ast_expr_type.id:
            return expr.get_id().get_name()
        if kind != isl.ast_expr_type.op:
            raise NotImplementedError(f"Unsupported isl expression: {expr}")
        op = expr.get_op_type()
        args = [self.emit(expr.get_op_arg(j)) for j in range(expr.get_op_n_arg())]
        ops = isl.ast_expr_op_type
        binary = {
            ops.add: "+", ops.sub: "-", ops.mul: "*", ops.eq: "==",
            ops.le: "<=", ops.lt: "<", ops.ge: ">=", ops.gt: ">",
            ops.and_: "and" if self.language == "py" else "&&",
            ops.and_then: "and" if self.language == "py" else "&&",
            ops.or_: "or" if self.language == "py" else "||",
            ops.or_else: "or" if self.language == "py" else "||",
        }
        if op in binary:
            return "(" + f" {binary[op]} ".join(args) + ")"
        if op == ops.minus:
            return f"(-{args[0]})"
        if op in (ops.min, ops.max):
            name = "min" if op == ops.min else "max"
            if self.language == "py":
                return f"_pip_builtins.{name}({', '.join(args)})" if len(args) > 1 else args[0]
            return f"std::{name}<{self.namespace}::Integer>({{{', '.join(args)}}})"
        if op in (ops.cond, ops.select):
            a, b, c = args
            if self.language == "py":
                return f"({b} if {a} else {c})"
            return f"({a} ? {self.namespace}::Integer({b}) : {self.namespace}::Integer({c}))"
        if op in (ops.div, ops.fdiv_q, ops.pdiv_q, ops.pdiv_r, ops.zdiv_r):
            a, b = args
            if self.language == "py":
                if op in (ops.fdiv_q, ops.pdiv_q):
                    return f"({a} // {b})"
                if op == ops.div:
                    return f"_pip_trunc_div({a}, {b})"
                if op == ops.pdiv_r:
                    return f"({a} % {b})"
                return f"({a} - _pip_trunc_div({a}, {b}) * {b})"
            name = {
                ops.div: "trunc_div", ops.fdiv_q: "floor_div", ops.pdiv_q: "floor_div",
                ops.pdiv_r: "floor_mod", ops.zdiv_r: "trunc_mod",
            }[op]
            return f"{self.namespace}::{name}({a}, {b})"
        raise NotImplementedError(f"Unsupported isl AST operator: {op}")


class PIPSolver:
    """Compile an integer PIP to a piecewise quasi-affine objective value.

    A: m x n, B: m x k, c: n, d: m. Parameters are named p1,...,pk.
    Empty B (no constraints) implies k=0. Generated functions return None /
    std::nullopt outside the feasible domain, including negative parameters.
    Nonempty problems with an unbounded objective raise PIPUnboundedError here.
    """

    def __init__(self, A, B, c, d):
        A = [_vector(row, "A") for row in A]
        B = [_vector(row, "B") for row in B]
        c, d = _vector(c, "c"), _vector(d, "d")
        m, n = len(A), len(c)
        if len(B) != m or len(d) != m:
            raise ValueError("A, B and d must have the same number of rows")
        if any(len(row) != n for row in A):
            raise ValueError("Each row of A must have len(c) columns")
        k = len(B[0]) if m else 0
        if any(len(row) != k for row in B):
            raise ValueError("B must be rectangular")

        self.parameter_names = tuple(f"p{j + 1}" for j in range(k))
        xs = [f"x{j}" for j in range(n)]
        ctx = isl.Context()
        prefix = f"[{', '.join(self.parameter_names)}] -> "
        conditions = [f"{p} >= 0" for p in self.parameter_names]
        conditions += [f"{x} >= 0" for x in xs]
        conditions += [f"{_linear(a, xs)} <= ({_linear(b, self.parameter_names)}) + ({di})"
                       for a, b, di in zip(A, B, d)]
        conditions.append(f"v = {_linear(c, xs)}")
        body = " and ".join(conditions)
        if xs:
            body = f"exists ({', '.join(xs)} : {body})"
        objectives = isl.Set.read_from_str(ctx, prefix + "{ [v] : " + body + " }")

        # With fixed A,c, an improving recession direction works for every
        # feasible p. Scaling a rational ray gives an integer ray, so this
        # integer feasibility test also detects rational improving directions.
        if not objectives.is_empty() and n:
            ray_conditions = [f"{x} >= 0" for x in xs]
            ray_conditions += [f"{_linear(a, xs)} <= 0" for a in A]
            ray_conditions.append(f"{_linear(c, xs)} >= 1")
            rays = isl.Set.read_from_str(ctx, "{ [" + ", ".join(xs) + "] : "
                                         + " and ".join(ray_conditions) + " }")
            if not rays.is_empty():
                raise PIPUnboundedError("c^T x is unbounded on the feasible parameter domain")

        # Optimize the scalar objective, rather than lexicographically maximizing x.
        self.optimum = objectives.lexmax_pw_multi_aff().get_pw_aff(0).coalesce()
        build = isl.AstBuild.from_context(isl.Set.read_from_str(ctx, prefix + "{ : }"))
        self._pieces = []

        def add_piece(domain, affine):
            self._pieces.append((build.expr_from_set(domain), build.expr_from_pw_aff(
                isl.PwAff.from_aff(affine).intersect_domain(domain))))

        self.optimum.foreach_piece(add_piece)

    def python_source(self, function_name="solve"):
        """Return standalone Python source (arbitrary precision integers)."""
        _check_name(function_name)
        emit = _Emitter("py").emit
        lines = ["# Generated by pip_solver (https://github.com/2251799813685248/Competitive-Programming-Library); islpy is not needed at runtime.",
                 f"def {function_name}({', '.join(self.parameter_names)}):",
                 '    """Return the maximum objective, or None if infeasible."""',
                 "    import builtins as _pip_builtins",
                 "    import operator as _pip_operator",
                 "    def _pip_trunc_div(a, b):",
                 "        q = _pip_builtins.abs(a) // _pip_builtins.abs(b)",
                 "        return -q if (a < 0) != (b < 0) else q"]
        for p in self.parameter_names:
            lines.append(f"    {p} = _pip_operator.index({p})")
        for condition, value in self._pieces:
            lines += [f"    if {emit(condition)}:", f"        return {emit(value)}"]
        lines += ["    return None", ""]
        return "\n".join(lines)

    def cpp_source(self, function_name="solve", integer_type="cpp_int"):
        """Return C++17 source using cpp_int (default), __int128, or long long.

        Fixed-width types require all runtime intermediate values to fit the type.
        Out-of-range constants raise OverflowError during code generation.
        """
        _check_name(function_name)
        if not isinstance(integer_type, str) or integer_type not in _CPP_INTEGER_TYPES:
            raise ValueError("integer_type must be 'cpp_int', '__int128', or 'long long'")
        integer_type = _CPP_INTEGER_TYPES[integer_type]
        ns = f"pip_detail_{function_name}"
        emit = _Emitter("cpp", ns, integer_type).emit
        lines = [f"// Generated by pip_solver (https://github.com/2251799813685248/Competitive-Programming-Library); integer type: {integer_type}.",
                 "#include <algorithm>", "#include <optional>"]
        if integer_type == "boost::multiprecision::cpp_int":
            lines.append("#include <boost/multiprecision/cpp_int.hpp>")
        else:
            lines.append("// All inputs and intermediate values must fit the selected integer type.")
        lines += ["", f"namespace {ns} {{", f"using Integer = {integer_type};",
                 "inline Integer trunc_div(const Integer& a, const Integer& b) { return a / b; }",
                 "inline Integer floor_div(const Integer& a, const Integer& b) {",
                 "    Integer q = a / b;",
                 "    if (a % b != 0 && ((a < 0) != (b < 0))) --q;",
                 "    return q;", "}",
                 "inline Integer floor_mod(const Integer& a, const Integer& b) {",
                 "    return a - floor_div(a, b) * b;", "}",
                 "inline Integer trunc_mod(const Integer& a, const Integer& b) { return a % b; }",
                 f"}} // namespace {ns}", ""]
        params = ", ".join(f"const {ns}::Integer& {p}" for p in self.parameter_names)
        lines.append(f"inline std::optional<{ns}::Integer> {function_name}({params}) {{")
        for condition, value in self._pieces:
            lines.append(f"    if ({emit(condition)}) return {ns}::Integer({emit(value)});")
        lines += ["    return std::nullopt;", "}", ""]
        return "\n".join(lines)

    def generate_py(self, output_path="pip_generated.py", function_name="solve"):
        path = Path(output_path)
        res = self.python_source(function_name)
        path.write_text(res, encoding="utf-8")
        return res

    def generate_cpp(self, output_path="pip_generated.cpp", function_name="solve",
                     integer_type="cpp_int"):
        path = Path(output_path)
        res = self.cpp_source(function_name, integer_type)
        path.write_text(res, encoding="utf-8")
        return res


def PIP_generate_py(A, B, c, d, output_path="pip_generated.py", function_name="solve"):
    """Solve the PIP and write a Python function; return the generated source."""
    return PIPSolver(A, B, c, d).generate_py(output_path, function_name)


def PIP_generate_cpp(A, B, c, d, output_path="pip_generated.cpp", function_name="solve",
                     integer_type="cpp_int"):
    """Write C++17 source with the selected integer type; return the source."""
    return PIPSolver(A, B, c, d).generate_cpp(output_path, function_name, integer_type)
