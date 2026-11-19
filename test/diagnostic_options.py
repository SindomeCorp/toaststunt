"""Isolated option checks. Usage: python3 diagnostic_options.py BINARY VERBS BENCH BUILTINS LOGS
All four settings are ON or OFF. Runs check a real 30-second publication interval.
"""
import importlib.util
import json
import os
from pathlib import Path
import sys
import time

binary, *settings = sys.argv[1:]
assert len(settings) == 4 and all(setting in ('ON', 'OFF') for setting in settings)
counts, benchmark, builtins, logs = (setting == 'ON' for setting in settings)
sys.argv = [sys.argv[0], binary]
spec = importlib.util.spec_from_file_location('bench', Path(__file__).with_name('benchmark_integration.py'))
bench = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bench)

# Enable full tracing once even with logging off to verify that it cannot override the build flag.
for full_trace in (False, True):
    os.environ['TOAST_TRACE_FULL_CALLS'] = '1' if full_trace else '0'
    started = time.monotonic()
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
            for name, enabled in [
                ('verb_call_counts', counts),
                ('builtin_call_counts', builtins),
                ('clear_builtin_count', builtins),
            ]:
                present = evaluate(f"return `function_info({json.dumps(name)}) ! E_INVARG => 0';")
                assert (present != '{1, 0}') == enabled, (name, present)
                if enabled:
                    other_sock, other_stream = server.connect('programmer')
                    try:
                        assert 'E_PERM' in bench.evaluate(other_stream, f'return {name}();')
                    finally:
                        other_stream.close()
                        other_sock.close()

            if builtins:
                assert evaluate(
                    'before=builtin_call_counts(); sqrt(4.0); sqrt(9.0); '
                    'after=builtin_call_counts(); return after["sqrt"]-before["sqrt"];'
                ) == '{1, 2}'

            trace = server.path/'toaststunt.log.trace'
            log = server.path/'toaststunt.log.verb_counts'
            builtin_log = server.path/'toaststunt.log.builtin_counts'
            assert trace.exists() == logs
            assert log.exists() == (counts and logs)
            if logs:
                events = [json.loads(line) for line in trace.read_text().splitlines()]
                assert events, 'trace enabled but empty'
                assert any(e['fullverb'] == 'diagnostic_probe' for e in events) == full_trace
                if not full_trace:
                    assert all(e['parent_span_id'] is None for e in events)
            if logs and counts and not full_trace:
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
            if counts:
                assert evaluate(
                    'clear_verb_counts(); '
                    f'for i in [1..60]; name=tostr("counter_probe_",i); '
                    f'add_verb({obj},{{player,"xd",name}},{{"this","none","this"}}); '
                    f'set_verb_code({obj},name,{{"return 42;"}}); {obj}:(name)(); endfor; '
                    f'{obj}:counter_probe_60(); {obj}:counter_probe_60(); '
                    'return length(verb_call_counts()) == 50 && length(verb_call_counts(60)) == 60 '
                    '&& length(verb_call_counts(1000000000)) == 60 && verb_call_counts(0) == {} '
                    f'&& equal(verb_call_counts(3), {{{{"{obj}:counter_probe_60",3}},'
                    f'{{"{obj}:counter_probe_1",1}},{{"{obj}:counter_probe_10",1}}}});'
                ) == '{1, 1}', 'verb query limits and ordering'
                assert evaluate(
                    'size=value_bytes(verb_call_counts(50)); '
                    'add_property($server_options,"max_list_value_bytes",size,{player,""}); '
                    'add_property($server_options,"max_concat_catchable",1,{player,""}); '
                    'load_server_options(); exact=length(verb_call_counts(50)); '
                    '$server_options.max_list_value_bytes=size-1; load_server_options(); '
                    "over=`verb_call_counts(50) ! E_QUOTA => 1'; "
                    'smaller=length(verb_call_counts(49)); '
                    'delete_property($server_options,"max_list_value_bytes"); '
                    'delete_property($server_options,"max_concat_catchable"); load_server_options(); '
                    'return {exact,over,smaller};'
                ) == '{1, {50, 1, 49}}', 'verb query byte limit'
                assert 'E_INVARG' in evaluate('return verb_call_counts(-1);')
                assert 'E_TYPE' in evaluate('return verb_call_counts("50");')
                assert 'E_ARGS' in evaluate('return verb_call_counts(@{1,2});')
                assert evaluate(
                    f'clear_verb_counts(); empty=length(verb_call_counts()); '
                    f'{obj}:diagnostic_probe(); return {{empty,verb_call_counts()[1][2]}};'
                ) == '{1, {0, 1}}'

            if not full_trace:
                if not logs or not builtins:
                    # A disabled build must stay quiet past both snapshot timers.
                    time.sleep(max(0, 32 - (time.monotonic() - started)))
                    assert not builtin_log.exists()
                else:
                    deadline = time.monotonic()+22
                    while not builtin_log.exists() and time.monotonic() < deadline:
                        time.sleep(.1)
                    assert builtin_log.exists()
                if not logs:
                    assert not trace.exists()
                    assert not log.exists()
            if builtins:
                previous = json.loads(builtin_log.read_text()) if builtin_log.exists() else None
                assert 'E_ARGS' in evaluate('return clear_builtin_count(@{1});')
                if counts:
                    assert evaluate(
                        'verbs=verb_call_counts(); clear_builtin_count(); '
                        'return equal(verbs, verb_call_counts());'
                    ) == '{1, 1}'
                assert evaluate(
                    'sqrt(4.0); reset=clear_builtin_count(); before=builtin_call_counts(); '
                    'sqrt(9.0); after=builtin_call_counts(); '
                    'return {reset, before["sqrt"], before["clear_builtin_count"], '
                    'before["builtin_call_counts"], after["sqrt"]};'
                ) == '{1, {0, 0, 0, 1, 1}}'
                if logs:
                    snapshot = json.loads(builtin_log.read_text())
                    assert snapshot['counts'] and all(total == 0 for total in snapshot['counts'].values())
                    assert all(delta is None for delta in snapshot['changes'].values())
                    if previous:
                        assert snapshot['session'] == previous['session']
                        assert snapshot['generation'] == previous['generation'] + (2 if counts else 1)
                else:
                    assert not builtin_log.exists()
        finally:
            stream.close()
            sock.close()
    finally:
        server.close()
print(f'PASS verbs={counts} builtins={builtins} benchmark={benchmark} logs={logs}: live queries, permissions, snapshots and trace modes')
