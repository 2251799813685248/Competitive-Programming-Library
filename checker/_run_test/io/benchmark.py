#!/usr/bin/env python3
"""Compare buffered fastio with unsynchronized iostream on identical convolution."""
import argparse
import json
from pathlib import Path
import statistics
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[3]
HERE = Path(__file__).resolve().parent


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', default='g++-14')
    parser.add_argument('--input', type=Path, default=ROOT.parent/'test/library-checker-problems/convolution/convolution_mod/in/max_random_00.in')
    parser.add_argument('--runs', type=int, default=9)
    args = parser.parse_args()
    if args.runs < 1:
        parser.error('--runs must be positive')
    flags = ['-std=c++23', '-O2', '-DFASTIO_PROFILE']
    flags += ['-I'+str(p) for p in sorted((ROOT/'Library').iterdir()) if p.is_dir()]
    results = {'iostream': [], 'fastio': []}
    with tempfile.TemporaryDirectory(prefix='fastio-bench-') as work:
        work = Path(work)
        for name in results:
            extra = ['-DFASTIO_USE_IOSTREAM'] if name == 'iostream' else []
            subprocess.run([args.compiler, *flags, *extra, str(HERE/'convolution.cpp'), '-o', str(work/name)], check=True)
        for run in range(args.runs+1):
            names = list(results) if run%2 else list(reversed(results))
            for name in names:
                with args.input.open('rb') as fin, (work/'actual.out').open('wb') as fout:
                    p = subprocess.run([str(work/name)], stdin=fin, stdout=fout, stderr=subprocess.PIPE, check=True)
                if run:  # One warm-up per implementation.
                    results[name].append(list(map(float,p.stderr.split())))
    columns = ['setup_ms', 'input_ms', 'convolution_ms', 'output_ms', 'total_ms']
    report = {name: {'median': dict(zip(columns,map(statistics.median,zip(*samples)))), 'samples': samples}
              for name,samples in results.items()}
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
