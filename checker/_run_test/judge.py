import argparse
import os
import subprocess
import sys
import tempfile
import time
from pathlib import Path

def get_problem_dir(base_dir, problem_name):
    for root, dirs, files in os.walk(base_dir):
        if problem_name in dirs:
            info_path = Path(root) / problem_name / "info.toml"
            if info_path.exists():
                return Path(root) / problem_name
    return None

def compile_source(source_path, exec_path):
    print(f"Compiling {source_path} ...", file=sys.stderr)
    compile_cmd = [
        "g++-14", "-std=c++23",
        "-I/home/a1024/cpp/Competitive-Programming-Library/Library/array_datastructure",
        "-I/home/a1024/cpp/Competitive-Programming-Library/Library/general_algorithm",
        "-I/home/a1024/cpp/Competitive-Programming-Library/Library/graph",
        "-I/home/a1024/cpp/Competitive-Programming-Library/Library/math",
        "-I/home/a1024/cpp/Competitive-Programming-Library/Library/set_map",
        "-I/home/a1024/cpp/Competitive-Programming-Library/Library/string_algorithm",
        "-O3", "-Wall", "-Wextra",
        str(source_path), "-o", str(exec_path)
    ]
    result = subprocess.run(compile_cmd)
    return result.returncode == 0

def judge():
    parser = argparse.ArgumentParser(description="Library Checker Judge")
    parser.add_argument("problem", help="Problem name (e.g. aplusb)")
    parser.add_argument("source", help="Path to the source code to test")
    parser.add_argument("--timeout", type=float, default=5.0, help="Timeout in seconds for each testcase")
    parser.add_argument("d")
    args = parser.parse_args()

    # Paths
    project_root = Path("/home/a1024/cpp")
    test_dir = project_root / "test"
    lc_repo_dir = test_dir / "library_checker_auto_test" / "library-checker-problems"

    source_path = Path(args.source).resolve()
    if not source_path.exists():
        print(f"Error: Source file '{args.source}' does not exist.")
        sys.exit(1)

    problem_dir = get_problem_dir(lc_repo_dir, args.problem)
    if not problem_dir:
        print(f"Error: Problem '{args.problem}' not found in library-checker-problems.")
        sys.exit(1)

    # 1. Compile the source code
    with tempfile.TemporaryDirectory() as tmpdir:
        tmp_dir_path = Path(tmpdir)
        exec_path = tmp_dir_path / "solution"
        
        if not compile_source(source_path, exec_path):
            print("\033[93mCE (Compile Error)\033[0m")
            sys.exit(1)

        # 2. Generate test cases
        print(f"Generating testcases for {args.problem}...", file=sys.stderr)
        gen_cmd = ["python3", "generate.py", "-p", args.problem]
        gen_result = subprocess.run(gen_cmd, cwd=lc_repo_dir, capture_output=True)
        if gen_result.returncode != 0:
            print(f"Error: Failed to generate testcases.\n{gen_result.stderr.decode()}", file=sys.stderr)
            sys.exit(1)
            
        in_dir = problem_dir / "in"
        expected_out_dir = problem_dir / "out"
        checker_path = problem_dir / "checker"

        if not checker_path.exists():
            print("Error: checker missing. generation failed?", file=sys.stderr)
            sys.exit(1)
            
        testcases = sorted([f for f in in_dir.iterdir() if f.is_file()])
        if not testcases:
            print("Error: No testcases found.", file=sys.stderr)
            sys.exit(1)

        print(f"--- Testing {args.source} on {args.problem} ---", file=sys.stderr)
        all_ac = True
        show_detail = args.d == "d"
        if args.d != "d" and args.d != "nd":
            print("Unknown args. Use \"d\" or \"nd\"")
            sys.exit(1)

        testcases_len = len(testcases)
        for i, in_file in enumerate(testcases, 1):
            testcase_name = in_file.name
            out_name = testcase_name.replace(".in", ".out")
            expected_out_file = expected_out_dir / out_name
            actual_out_file = tmp_dir_path / out_name
            
            if not show_detail:
                sys.stdout.write(f"\r{i}/{testcases_len}")
                sys.stdout.flush()

            # Run solution
            start_time = time.time()
            try:
                with open(in_file, "r") as fin, open(actual_out_file, "w") as fout:
                    run_result = subprocess.run(
                        [str(exec_path)],
                        stdin=fin, stdout=fout, stderr=subprocess.DEVNULL,
                        timeout=args.timeout
                    )
                elapsed_time = time.time() - start_time
                
                if run_result.returncode != 0:
                    msg = f"{testcase_name}: \033[93mRE (Runtime Error)\033[0m - code {run_result.returncode} [{elapsed_time:.3f}s]"
                    if show_detail:
                        print(msg)
                    else:
                        sys.stdout.write(f"\r{msg}\033[K\n")
                        sys.stdout.flush()
                    all_ac = False
                    continue
                    
            except subprocess.TimeoutExpired:
                msg = f"{testcase_name}: \033[93mTLE (Time Limit Exceeded)\033[0m [{args.timeout:.3f}s]"
                if show_detail:
                    print(msg)
                else:
                    sys.stdout.write(f"\r{msg}\033[K\n")
                    sys.stdout.flush()
                all_ac = False
                continue
                
            # Run checker
            # ./checker <in_file> <actual_out_file> <expected_out_file>
            checker_cmd = [str(checker_path), str(in_file), str(actual_out_file), str(expected_out_file)]
            checker_result = subprocess.run(checker_cmd, capture_output=True)
            
            if checker_result.returncode == 0:
                if show_detail:
                    print(f"{testcase_name}: \033[92mAC (Accepted)\033[0m [{elapsed_time:.3f}s]")
            else:
                msg = f"{testcase_name}: \033[91mWA (Wrong Answer)\033[0m [{elapsed_time:.3f}s]"
                if show_detail:
                    print(msg)
                else:
                    sys.stdout.write(f"\r{msg}\033[K\n")
                    sys.stdout.flush()
                all_ac = False

        if not show_detail:
            print()

        if all_ac:
            print("Result: \033[92mAll AC!\033[0m")
        else:
            print("Result: \033[91mSome testcases failed.\033[0m")

if __name__ == "__main__":
    judge()
