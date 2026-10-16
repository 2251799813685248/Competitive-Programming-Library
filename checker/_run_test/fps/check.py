#!/usr/bin/env python3
"""Run FPS regression and locally generated Library Checker cases. No downloads."""
import argparse
import pathlib
import subprocess
import tempfile
import time

ROOT = pathlib.Path(__file__).resolve().parents[3]
DEFAULT_CASES = ROOT.parent / 'test/library-checker-problems'
PROBLEMS = [
    'convolution_mod', 'inv_of_formal_power_series', 'log_of_formal_power_series',
    'exp_of_formal_power_series', 'pow_of_formal_power_series',
    'composition_of_formal_power_series_large',
    'compositional_inverse_of_formal_power_series_large',
    'product_of_polynomial_sequence', 'polynomial_taylor_shift',
    'multipoint_evaluation', 'polynomial_interpolation',
]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases', type=pathlib.Path, default=DEFAULT_CASES)
    parser.add_argument('--compiler', default='g++-14')
    parser.add_argument('--problem', action='append', choices=PROBLEMS)
    parser.add_argument('--scalar', action='store_true')
    parser.add_argument('--regression-only', action='store_true')
    parser.add_argument('--sanitize', action='store_true')
    args = parser.parse_args()
    flags = ['-std=c++23', '-O2' if args.sanitize else '-O3']
    if args.scalar:
        flags += ['-DFPS_DISABLE_SIMD']
    if args.sanitize:
        flags += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-g']
    flags += ['-I' + str(p) for p in sorted((ROOT / 'Library').iterdir()) if p.is_dir()]
    with tempfile.TemporaryDirectory(prefix='fps-check-') as tmp:
        tmp = pathlib.Path(tmp)
        binary = tmp / 'solution'

        def compile_source(source):
            subprocess.run([args.compiler, *flags, str(source), '-o', str(binary)], check=True)

        compile_source(pathlib.Path(__file__).with_name('regression.cpp'))
        subprocess.run([str(binary)], check=True)
        if args.regression_only:
            return
        count = 0
        for name in args.problem or PROBLEMS:
            matches = list(args.cases.glob('*/' + name + '/info.toml'))
            if len(matches) != 1:
                raise RuntimeError('Missing or ambiguous generated problem: ' + name)
            problem = matches[0].parent
            cases = sorted((problem / 'in').glob('*.in'))
            if not cases or not (problem / 'checker').is_file():
                raise RuntimeError('Generate cases and checker first: ' + str(problem))
            compile_source(ROOT / 'checker/math' / (name + '.cpp'))
            worst = (0, '')
            for case in cases:
                output = tmp / 'actual.out'
                start = time.perf_counter()
                with case.open('rb') as fin, output.open('wb') as fout:
                    subprocess.run([str(binary)], stdin=fin, stdout=fout, check=True, timeout=60)
                elapsed = 1000 * (time.perf_counter() - start)
                # Process startup and I/O are included. Use benchmark.cpp for compute timing.
                worst = max(worst, (elapsed, case.name))
                result = subprocess.run([
                    str(problem / 'checker'), str(case), str(output),
                    str(problem / 'out' / (case.stem + '.out')),
                ], capture_output=True, text=True)
                if result.returncode:
                    raise RuntimeError(name + '/' + case.name + ': ' + result.stderr)
            count += len(cases)
            print(f'{name}: {len(cases)}/{len(cases)} AC; max process wall {worst[0]:.1f} ms ({worst[1]})', flush=True)
        print(f'All {count} Library Checker cases passed.', flush=True)


if __name__ == '__main__':
    main()
