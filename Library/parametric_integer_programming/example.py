"""Run from this directory to generate solution.py and solution.cpp."""

from pip_solver import PIPSolver


def main():
    solver = PIPSolver(A=[[2], [1]], B=[[1, 0], [0, 1]], c=[3], d=[0, 0])
    print(solver.generate_py("solution.py"))
    print(solver.generate_cpp("solution.cpp"))


if __name__ == "__main__":
    main()
