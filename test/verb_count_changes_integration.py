"""Real-timer verb delta/reset check using an isolated synthetic server (~90s).
Usage: python3 test/verb_count_changes_integration.py /path/to/count-enabled/moo
"""
import importlib.util
import json
from pathlib import Path
import sys
import time
spec = importlib.util.spec_from_file_location('fixture', Path(__file__).with_name('benchmark_integration.py'))
fixture = importlib.util.module_from_spec(spec)
spec.loader.exec_module(fixture)
server = fixture.Server()
sock, stream = server.connect()
path = server.path / 'toaststunt.log.verb_counts'
def evaluate(code):
    return fixture.evaluate(stream, code)
def wait_snapshot(key, total):
    deadline = time.monotonic() + 38
    while time.monotonic() < deadline:
        lines = path.read_text().splitlines()
        if lines:
            record = json.loads(lines[-1])
            if record['counts'].get(key) == total:
                return record
        time.sleep(.05)
    raise AssertionError('expected verb snapshot not published')
try:
    obj = evaluate('return create($nothing);').removeprefix('{1, ').removesuffix('}')
    assert evaluate(f'add_verb({obj},{{player,"xd","change_probe"}},{{"this","none","this"}}); return set_verb_code({obj},"change_probe",{{"return 42;"}});') == '{1, {}}'
    key = obj + ':change_probe'
    assert evaluate(f'for i in [1..1000]; {obj}:change_probe(); endfor; return 1;') == '{1, 1}'
    first = wait_snapshot(key, 1000)
    assert first['changes'][key] is None
    assert evaluate(f'for i in [1..50]; {obj}:change_probe(); endfor; return 1;') == '{1, 1}'
    second = wait_snapshot(key, 1050)
    assert second['changes'][key] == 50
    assert second['session'] == first['session']
    assert second['generation'] == first['generation']
    assert evaluate('return clear_verb_counts();') == '{1, 0}'
    reset = json.loads(path.read_text().splitlines()[-1])
    assert reset['counts'] == {} and reset['changes'] == {}
    assert reset['generation'] == first['generation'] + 1
    assert evaluate(f'return {obj}:change_probe();') == '{1, 42}'
    third = wait_snapshot(key, 1)
    assert third['changes'][key] is None
    assert third['generation'] == reset['generation']
    print('PASS: actual 1000 -> 1050 calls yields change 50; append history and reset generation preserved; missing previous top-50 key is unknown')
finally:
    stream.close(); sock.close(); server.close()
