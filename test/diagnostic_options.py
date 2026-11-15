"""Isolated option checks. Usage: python3 diagnostic_options.py BINARY COUNTS BENCH
COUNTS/BENCH are ON or OFF. Count-enabled runs wait for a real 30-second flush.
"""
import importlib.util
import json
import os
from pathlib import Path
import sys
import time

binary, counts, benchmark = sys.argv[1:]
counts, benchmark = counts == 'ON', benchmark == 'ON'
sys.argv = [sys.argv[0], binary]
spec = importlib.util.spec_from_file_location('bench', Path(__file__).with_name('benchmark_integration.py'))
bench = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bench)

# Test both existing trace modes independently of the new build switches.
for full_trace in (False, True):
    os.environ['TOAST_TRACE_FULL_CALLS'] = '1' if full_trace else '0'
    server = bench.Server()
    try:
        sock, stream = server.connect()
        try:
            def evaluate(code):
                return bench.evaluate(stream, code)
            present = evaluate('return `function_info("benchmark_call") ! E_INVARG => 0\';')
            assert (present != '{1, 0}') == benchmark, present
            assert (evaluate('return `function_info("clear_verb_counts") ! E_INVARG => 0\';') != '{1, 0}') == counts
            obj = evaluate('return create($nothing);').removeprefix('{1, ').removesuffix('}')
            assert evaluate(f'add_verb({obj}, {{player,"xd","diagnostic_probe"}}, {{"this","none","this"}}); return set_verb_code({obj},"diagnostic_probe",{{"return 42;"}});') == '{1, {}}'
            assert evaluate(f'return {obj}:diagnostic_probe();') == '{1, 42}'
            if benchmark:
                assert evaluate(f'return benchmark_call({obj},"diagnostic_probe",{{}})["result"];') == '{1, 42}'
            else:
                assert 'E_INVARG' in evaluate(f'return call_function("benchmark_call",{obj},"diagnostic_probe",{{}});')
            events = [json.loads(line) for line in (server.path/'toaststunt.log.trace').read_text().splitlines()]
            assert events, 'trace disabled unexpectedly'
            assert any(e['fullverb'] == 'diagnostic_probe' for e in events) == full_trace
            if not full_trace:
                assert all(e['parent_span_id'] is None for e in events)
            log = server.path/'toaststunt.log.verb_counts'
            assert log.exists() == counts
            if counts and not full_trace:
                deadline = time.monotonic()+40
                while time.monotonic() < deadline:
                    lines = log.read_text().splitlines()
                    if lines:
                        snapshot = json.loads(lines[-1])
                        assert snapshot['mode'] == 'cumulative' and snapshot['window_seconds'] == 30
                        assert snapshot['counts'][f'{obj}:diagnostic_probe'] == (2 if benchmark else 1)
                        assert len(snapshot['counts']) <= 50
                        break
                    time.sleep(.1)
                else:
                    raise AssertionError('no count snapshot within 40 seconds')
        finally:
            stream.close()
            sock.close()
    finally:
        server.close()
print(f'PASS counts={counts} benchmark={benchmark}: availability, logging, both trace modes')
