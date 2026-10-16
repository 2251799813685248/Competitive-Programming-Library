#!/usr/bin/env python3
"""Compare complete expanded submissions with Library Checker's C++ flags.

The local compiler/CPU may differ from the judge. Results are not judge times.
"""
import argparse
import json
import os
from pathlib import Path
import resource
import statistics
import subprocess
import tempfile
import time

ROOT = Path(__file__).resolve().parents[3]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--before', required=True, type=Path)
    parser.add_argument('--after', required=True, type=Path)
    parser.add_argument('--compiler', default='g++-14')
    parser.add_argument('--runs', type=int, default=7)
    parser.add_argument('--cpu', type=int, help='Pin this process and both submissions to one Linux CPU')
    parser.add_argument('--cases', type=Path, default=ROOT.parent/'test/library-checker-problems/convolution/convolution_mod')
    parser.add_argument('--case', action='append', help='Case stem; repeatable. Default: all generated cases.')
    parser.add_argument('--output', type=Path, default=Path('convolution-comparison.json'))
    args = parser.parse_args()
    if args.runs < 1:
        parser.error('--runs must be positive')
    if args.cpu is not None:
        os.sched_setaffinity(0,{args.cpu})
    flags = ['-std=c++23', '-O2', '-march=native', '-DEVAL', '-DONLINE_JUDGE']
    cases = [args.cases/'in'/(name+'.in') for name in args.case] if args.case else sorted((args.cases/'in').glob('*.in'))
    if not cases:
        parser.error('No generated input cases')
    report = {'compiler': subprocess.check_output([args.compiler,'--version'],text=True).splitlines()[0],
              'flags': flags, 'cpu': args.cpu, 'runs': args.runs, 'cases': {}}
    with tempfile.TemporaryDirectory(prefix='convolution-compare-') as work:
        work = Path(work)
        for name in ['before','after']:
            subprocess.run([args.compiler,*flags,str(getattr(args,name)),'-o',str(work/name)],check=True)
        for case in cases:
            samples = {'before': [], 'after': []}
            for run in range(args.runs+1):
                for name in (['before','after'] if run%2 else ['after','before']):
                    output = work/(name+'.out')
                    with case.open('rb') as fin, output.open('wb') as fout:
                        usage0 = resource.getrusage(resource.RUSAGE_CHILDREN)
                        start = time.perf_counter()
                        # No timeout polling: Popen.wait(timeout=...) sleeps in
                        # growing intervals and distorts short wall-time samples.
                        subprocess.run([str(work/name)],stdin=fin,stdout=fout,check=True)
                        wall = (time.perf_counter()-start)*1000
                        usage1 = resource.getrusage(resource.RUSAGE_CHILDREN)
                    cpu = (usage1.ru_utime+usage1.ru_stime-usage0.ru_utime-usage0.ru_stime)*1000
                    if run: samples[name].append({'wall_ms':wall,'cpu_ms':cpu})
                    else:
                        result = subprocess.run([str(args.cases/'checker'),str(case),str(output),
                                                 str(args.cases/'out'/(case.stem+'.out'))],capture_output=True,text=True)
                        if result.returncode: raise RuntimeError(name+'/'+case.name+': '+result.stderr)
            report['cases'][case.stem] = {name: {
                'median': {metric:statistics.median(s[metric] for s in values) for metric in ['wall_ms','cpu_ms']},
                'samples': values} for name,values in samples.items()}
            print(case.stem,{name:result['median'] for name,result in report['cases'][case.stem].items()},flush=True)
        args.output.write_text(json.dumps(report,indent=2)+'\n')
        print('Wrote',args.output)


if __name__ == '__main__':
    main()
