"""Synthetic builtin snapshot tests (about 90 seconds when ON).
Usage: python3 test/builtin_counts_integration.py BINARY ON|OFF
Never connects to an existing server or database.
"""
import importlib.util
import json
import os
from pathlib import Path
import sys
import time

binary, setting = sys.argv[1:]
sys.argv = [sys.argv[0], binary]
spec = importlib.util.spec_from_file_location('benchmark_fixture', Path(__file__).with_name('benchmark_integration.py'))
fixture = importlib.util.module_from_spec(spec)
spec.loader.exec_module(fixture)

server = fixture.Server()
sock, stream = server.connect()
path = server.path / 'toaststunt.log.builtin_counts'

def evaluate(code):
    return fixture.evaluate(stream, code)

def snapshot_after(previous=None):
    deadline = time.monotonic() + 22
    while time.monotonic() < deadline:
        if path.exists() and path.is_file():
            # Every observed file must be one complete JSON object, never partial.
            raw = path.read_bytes()
            value = json.loads(raw)
            assert len(raw.splitlines()) == 1
            assert len(raw) < 100000
            if previous is None or value['ts'] != previous['ts']:
                assert value['pid'] == server.process.pid
                assert value['interval_seconds'] == 15
                assert value['mode'] == 'cumulative' and value['unit'] == 'calls'
                if previous:
                    assert 14 <= value['ts'] - previous['ts'] <= 21
                return value
        time.sleep(.03)
    raise AssertionError('snapshot did not advance at the 15-second cadence')

try:
    if setting == 'OFF':
        assert evaluate('return sqrt(4.0);') == '{1, 2.0}'
        time.sleep(17)
        assert not path.exists()
        assert not list(server.path.glob('*.builtin_counts*'))
        print('PASS OFF: server works, no snapshot or temporary file after reporting interval')
    else:
        first = snapshot_after()
        assert all(v is None for v in first["changes"].values())
        assert not Path(str(path)+'.tmp').exists()
        assert evaluate('sqrt(4.0); sqrt(9.0); call_function("sqrt",16.0); call_function("call_function","sqrt",25.0); `call_function("sqrt","bad") ! E_TYPE\'; `call_function("sqrt",@{}) ! E_ARGS\'; `call_function("missing_builtin_test") ! E_INVARG\'; call_function("suspend",0.01); return 1;') == '{1, 1}'
        second = snapshot_after(first)
        for name, delta in [('sqrt', 6), ('call_function', 7), ('suspend', 1)]:
            assert second['counts'][name]-first['counts'][name] == delta, (name, first, second)
            assert second['changes'][name] == delta
        third = snapshot_after(second)
        assert third['counts']['sqrt'] == second['counts']['sqrt']
        assert third['changes']['sqrt'] == 0
        assert len(third['counts']) == len(first['counts'])
        assert not Path(str(path)+'.tmp').exists()

        # Force temp-file creation failure, retain the last good snapshot, and
        # demonstrate interpreter availability and automatic next-tick recovery.
        temporary = Path(str(path)+'.tmp')
        temporary.mkdir()
        assert evaluate('return sqrt(4.0);') == '{1, 2.0}'
        time.sleep(16)
        assert json.loads(path.read_text()) == third
        assert evaluate('return 42;') == '{1, 42}'
        assert 'BUILTIN_COUNTS: opening snapshot temporary file' in (server.path/'console.log').read_text()
        temporary.rmdir()
        recovered = snapshot_after()
        deadline = time.monotonic()+22
        while recovered['ts'] == third['ts'] and time.monotonic() < deadline:
            time.sleep(.05)
            recovered = json.loads(path.read_text())
        assert recovered['ts'] > third['ts']
        assert recovered['changes']['sqrt'] == 1
        assert not temporary.exists()
        assert len(list(server.path.glob('*.builtin_counts*'))) == 1
        print('PASS ON: exact counts, nested wrapper/resume semantics, 15-second cadence, bounded atomic replacement, I/O failure preservation and recovery')
finally:
    stream.close()
    sock.close()
    server.close()
