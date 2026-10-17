#!/usr/bin/env python3
"""Check fastio boundaries and locally generated Library Checker cases."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[3]
HERE = Path(__file__).resolve().parent
PROBLEMS = {
    'convolution_mod': ('convolution.cpp', []),
    'many_aplusb': ('many_aplusb.cpp', []),
    'many_aplusb_128bit': ('many_aplusb.cpp', ['-DFASTIO_INT128']),
}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', default='g++-14')
    parser.add_argument('--cases', type=Path, default=ROOT.parent/'test/library-checker-problems')
    parser.add_argument('--regression-only', action='store_true')
    parser.add_argument('--sanitize', action='store_true')
    args = parser.parse_args()
    flags = ['-std=c++23', '-O2', '-Wall', '-Wextra']
    flags += ['-I'+str(p) for p in sorted((ROOT/'Library').iterdir()) if p.is_dir()]
    env = os.environ.copy()
    if args.sanitize:
        flags += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-g']
        env.setdefault('ASAN_OPTIONS', 'detect_leaks=0')
        env.setdefault('UBSAN_OPTIONS', 'halt_on_error=1')
    with tempfile.TemporaryDirectory(prefix='fastio-check-') as work:
        work = Path(work)
        binary = work/'test'
        def compile_source(source, extra=()):
            subprocess.run([args.compiler, *flags, *extra, str(source), '-o', str(binary)], check=True)
        compile_source(HERE/'regression.cpp')
        subprocess.run([str(binary)], check=True, env=env)
        # A standalone header, inline global instances across translation units,
        # and real pipe input with a token ending exactly at EOF.
        (work/'one.cpp').write_text('#include <fastio.hpp>\nfastio::FastInput* reader() { return &fastio::in; }\n')
        (work/'two.cpp').write_text('''#include <fastio.hpp>
fastio::FastInput* reader();
int main() {
    long long x;
    if (reader()!=&fastio::in || !fastio::read(x)) return 1;
    return fastio::print(x) ? 0 : 1;
}
''')
        compile_source(work/'one.cpp', ['-std=c++20', str(work/'two.cpp')])
        result = subprocess.run([str(binary)], input=b'-9223372036854775808', capture_output=True, check=True, env=env)
        assert result.stdout == b'-9223372036854775808\n'
        print('Standalone header, multiple translation units, pipe input: passed', flush=True)
        if args.regression_only:
            return
        count = 0
        for name, (source, extra) in PROBLEMS.items():
            matches = list(args.cases.glob('*/'+name+'/info.toml'))
            if len(matches) != 1:
                raise RuntimeError('Missing or ambiguous problem: '+name)
            problem = matches[0].parent
            cases = sorted((problem/'in').glob('*.in'))
            if not cases or not (problem/'checker').is_file():
                raise RuntimeError('Generate cases and checker first: '+str(problem))
            compile_source(HERE/source, extra)
            output = work/'actual.out'
            for case in cases:
                with case.open('rb') as fin, output.open('wb') as fout:
                    subprocess.run([str(binary)], stdin=fin, stdout=fout, check=True, timeout=60, env=env)
                result = subprocess.run([str(problem/'checker'), str(case), str(output),
                                         str(problem/'out'/(case.stem+'.out'))], capture_output=True, text=True)
                if result.returncode:
                    raise RuntimeError(name+'/'+case.name+': '+result.stderr)
            count += len(cases)
            print(f'{name}: {len(cases)}/{len(cases)} AC', flush=True)
        print(f'All {count} Library Checker cases passed.', flush=True)


if __name__ == '__main__':
    main()
