import argparse
import json
import os
import subprocess
import sys
from pathlib import Path

def judge_genre():
    parser = argparse.ArgumentParser(description="Run Library Checker tests for a specific genre")
    parser.add_argument("genre", help="Genre to test (e.g. fps, floor, matrix). Use 'all' to test all genres.")
    parser.add_argument("d")
    args = parser.parse_args()

    if args.d != "nd" and args.d != "d":
        print("Unknown args. Use \"d\" or \"nd\"")
        sys.exit(1)

    project_root = Path(__file__).resolve().parent.parent.parent.parent
    lib_checker_dir = project_root / "Competitive-Programming-Library" / "checker"
    info_json_path = lib_checker_dir / "_run_test" / "info.json"
    judge_py_path = project_root / "test" / "judge.py"

    if not info_json_path.exists():
        print(f"Error: {info_json_path} not found.")
        sys.exit(1)

    with open(info_json_path, "r") as f:
        try:
            info = json.load(f)
        except json.JSONDecodeError:
            print("Error: info.json is not valid JSON.")
            sys.exit(1)

    genres_to_test = []
    if args.genre == "all":
        genres_to_test = list(info.keys())
    else:
        if args.genre not in info:
            print(f"Error: Genre '{args.genre}' not found in info.json.")
            print(f"Available genres: {', '.join(info.keys())}")
            sys.exit(1)
        genres_to_test = [args.genre]

    # Find all cpp files in Library_checker
    cpp_files = list(lib_checker_dir.rglob("*.cpp"))

    all_success = True

    for genre in genres_to_test:
        print(f"========== Testing genre: {genre} ==========")
        items = info[genre]
        for item in items:
            # Find matching cpp file
            matches = [f for f in cpp_files if item in f.name]
            if not matches:
                print(f"Warning: No source file found for '{item}' in {lib_checker_dir}")
                all_success = False
                continue
            
            # Assume first match is the target
            source_file = matches[0]
            # Problem name is the filename without extension
            problem_name = source_file.stem

            print(f"\n--- Running judge for problem: {problem_name} ---")
            if args.d == "d":
                print(f"Source: {source_file}")
            
            cmd = ["python3", str(judge_py_path), problem_name, str(source_file)]
            if args.d == "d":
                cmd.append("d")
            elif args.d == "nd":
                cmd.append("nd")
                
            result = subprocess.run(cmd)

            if result.returncode != 0:
                all_success = False
                print(f"-> Failed: {problem_name}\n")
            else:
                print(f"-> Passed: {problem_name}\n")
        print(f"========== Finished testing genre: {genre} ==========\n")

    if all_success:
        print("\033[92mAll tests for the specified genre(s) PASSED!\033[0m")
        sys.exit(0)
    else:
        print("\033[91mSome tests FAILED.\033[0m")
        sys.exit(1)

if __name__ == "__main__":
    judge_genre()
