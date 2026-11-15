"""Isolated synthetic integration tests; never connects to an existing server.
Usage: python3 test/benchmark_integration.py /absolute/path/to/new/moo
"""
import json, os, pathlib, signal, socket, subprocess, sys, tempfile, time, unittest
BINARY = pathlib.Path(sys.argv.pop(1)).resolve()
ROOT = pathlib.Path(__file__).resolve().parents[1]

class Server:
    def __init__(self, binary=BINARY, db=None):
        self.binary = pathlib.Path(binary)
        self.tmp = tempfile.TemporaryDirectory(prefix="benchmark-call-")
        self.path = pathlib.Path(self.tmp.name)
        self.process = None
        self.start(db if db is not None else ROOT / "test/Test.db")
    def start(self, db):
        with socket.socket() as s:
            s.bind(("127.0.0.1", 0)); self.port = s.getsockname()[1]
        self.log = open(self.path / "console.log", "ab")
        self.process = subprocess.Popen([str(self.binary), "-4", "127.0.0.1", "-6", "::1",
            str(db), str(self.path / "out.db"), str(self.port)], cwd=self.path, stdout=self.log, stderr=self.log)
        for _ in range(100):
            if self.process.poll() is not None:
                raise RuntimeError((self.path / "console.log").read_text())
            try:
                s = socket.create_connection(("127.0.0.1", self.port), timeout=.1); s.close(); return
            except OSError: time.sleep(.05)
        raise RuntimeError("test server startup timed out")
    def connect(self, role="wizard"):
        s = socket.create_connection(("127.0.0.1", self.port), timeout=15)
        f = s.makefile("rw", buffering=1)
        f.write("connect " + role + "\n"); f.flush()
        return s, f
    def stop(self):
        if self.process and self.process.poll() is None:
            self.process.send_signal(signal.SIGTERM)
            try: self.process.wait(timeout=10)
            except subprocess.TimeoutExpired: self.process.kill(); self.process.wait()
        self.log.close()
        output=(self.path / "console.log").read_text(errors="replace")
        if self.process.returncode: print("exit:", self.process.returncode, output[-12000:], file=sys.stderr)
        if "ERROR: AddressSanitizer" in output or "runtime error:" in output:
            raise RuntimeError(output[-12000:])
    def close(self):
        self.stop(); self.tmp.cleanup()

def evaluate(f, code):
    f.write("; " + code + "\n"); f.flush()
    lines=[]; active=False
    while True:
        line=f.readline()
        if not line: raise RuntimeError("server closed connection")
        line=line.strip()
        if line == "-=!-^-!=-": active=True
        elif line == "-=!-v-!=-" and active: return "\n".join(lines)
        elif active: lines.append(line)

class BenchmarkTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls): cls.server=Server()
    @classmethod
    def tearDownClass(cls): cls.server.close()
    def setUp(self):
        self.s,self.f=self.server.connect()
        self.obj=evaluate(self.f,"return create($nothing);").removeprefix("{1, ").removesuffix("}")
        self.assertTrue(self.obj.startswith("#"),self.obj)
    def tearDown(self): self.f.close(); self.s.close()
    def code(self,name,lines):
        text='return set_verb_code(%s, %s, {%s});' % (self.obj,json.dumps(name),','.join(map(json.dumps,lines)))
        evaluate(self.f,'add_verb(%s, {player, "xd", %s}, {"this", "none", "this"}); return 0;' % (self.obj,json.dumps(name)))
        self.assertEqual(evaluate(self.f,text),"{1, {}}")
    def check(self,code): self.assertEqual(evaluate(self.f,code).splitlines()[-1],"{1, 1}")
    def test_results_and_permissions(self):
        self.code("value",["return args[1];"])
        self.check('for v in ({0, E_DIV, {1, "x"}, ["a" -> 2]}); r=benchmark_call(%s,"value",{v}); if (r["result"] != v || r["status"] != "complete" || r["reason"] != "" || r["ticks"] < 1 || r["cpu_seconds"] < 0.0); return 0; endif; endfor; return 1;' % self.obj)
        s,f=self.server.connect("programmer")
        try: self.assertIn("E_PERM",evaluate(f,'return benchmark_call(%s,"value",{1});' % self.obj))
        finally: f.close(); s.close()
        self.assertIn("E_TYPE",evaluate(self.f,'return benchmark_call(42,"value",{});'))
        self.assertIn("E_VERBNF",evaluate(self.f,'return benchmark_call(%s,"missing",{});' % self.obj))
    def test_tick_count_and_extended_opcode(self):
        self.code("plain",["return 0;"])
        self.code("caught",['try', 'raise(E_DIV);', 'except e (E_DIV)', 'return 0;', 'endtry'])
        # Disassembler marks exactly the opcode classes charged by the VM.
        self.check('r=benchmark_call(%s,"plain",{}); n=0; for line in (disassemble(%s,"plain")); if (index(line," * ")); n=n+1; endif; endfor; return r["ticks"] == n ? 1 | 0;' % (self.obj,self.obj))
        self.check('a=benchmark_call(%s,"plain",{}); b=benchmark_call(%s,"caught",{}); return b["status"] == "complete" && b["ticks"] > a["ticks"] ? 1 | 0;' % (self.obj,self.obj))
    def test_nested_yields_and_forks(self):
        self.code("inner",['suspend(0);', 'return 17;'])
        self.code("outer",['r=benchmark_call(this,"inner",{});','fork (0)', 'for i in [1..1000]; x=i*i; endfor;', 'endfork', 'suspend(0);', 'return r;'])
        self.check('r=benchmark_call(%s,"outer",{}); return r["status"] == "complete" && r["result"]["status"] == "complete" && r["result"]["result"] == 17 && r["ticks"] > r["result"]["ticks"] ? 1 | 0;' % self.obj)
    def test_wrapped_and_anonymous(self):
        self.code("value",["suspend(0);", "return 9;"])
        self.check('r=call_function("benchmark_call",%s,"value",{}); a=create(%s,1); q=benchmark_call(a,"value",{}); return r["result"] == 9 && q["result"] == 9 && q["status"] == "complete" ? 1 | 0;' % (self.obj,self.obj))
    def test_unrelated_cpu_excluded(self):
        self.code("wait",['suspend(0.25);','return 23;'])
        self.check('fork (0); t=ftime(1); while (ftime(1)-t < 0.12); value=string_hash("work"); if (ticks_left() < 2000); suspend(0); endif; endwhile; endfork; start=ftime(1); r=benchmark_call(%s,"wait",{}); return r["status"] == "complete" && r["result"] == 23 && ftime(1)-start >= 0.2 && r["cpu_seconds"] < 0.08 ? 1 | 0;' % self.obj)
    def test_errors_and_finally(self):
        self.code("bad",['try','raise(E_DIV);','finally','x=5;','endtry'])
        self.code("ok",['return 0;'])
        self.check("""v=`benchmark_call(%s,"bad",{}) ! E_DIV => 42'; r=benchmark_call(%s,"ok",{}); return v == 42 && r["result"] == 0 && r["status"] == "complete" ? 1 | 0;""" % (self.obj,self.obj))
    def test_suspended_kill_wrapped(self):
        self.code("wait",['suspend(10);','return 1;'])
        for call in ['benchmark_call(%s,"wait",{})', 'call_function("call_function","benchmark_call",%s,"wait",{})']:
            self.check('fork id (0); '+(call % self.obj)+'; endfork; suspend(0.05); kill_task(id); return 1;')
    def test_failed_suspension(self):
        self.code("limited",['add_property(player,"queued_task_limit",0,{player,""});',"r=`suspend(0) ! E_QUOTA => 5';",'delete_property(player,"queued_task_limit");','return r;'])
        self.check('r=benchmark_call(%s,"limited",{}); return r["result"] == 5 && r["status"] == "complete" ? 1 | 0;' % self.obj)
    def test_synchronous_native_cpu(self):
        self.code("native",['s="x";', 'for i in [1..18]; s=s+s; endfor;', 'for i in [1..80]; string_hash(s); endfor;', 'return 8;'])
        self.check('r=benchmark_call(%s,"native",{}); return r["result"] == 8 && r["cpu_seconds"] > 0.0001 && r["status"] == "complete" ? 1 | 0;' % self.obj)
    def test_abort_cleanup_and_tick_quota(self):
        self.code("kill",['kill_task(task_id());'])
        self.code("ticks",['while (1); x=1; endwhile'])
        self.code("ok",['return 7;'])
        self.check('add_property(%s,"done",0,{player,""}); fork (0); benchmark_call(%s,"kill",{}); %s.done=1; endfork; suspend(0.02); return %s.done == 0 ? 1 | 0;' % ((self.obj,)*4))
        # Direct and benchmarked tasks must both exhaust the same background quota.
        self.check('add_property($server_options,"bg_ticks",100,{player,""}); load_server_options(); fork (0); %s:ticks(); %s.done=2; endfork; fork (0); benchmark_call(%s,"ticks",{}); %s.done=3; endfork; suspend(0.05); delete_property($server_options,"bg_ticks"); load_server_options(); r=benchmark_call(%s,"ok",{}); return %s.done == 0 && r["status"] == "complete" ? 1 | 0;' % ((self.obj,)*6))
    def test_worker_cpu_excluded(self):
        self.code("hash",['set_thread_mode(args[1]);', 'return argon2("password", "12345678", 3, 8192, 1);'])
        self.check('a=benchmark_call(%s,"hash",{0}); b=benchmark_call(%s,"hash",{1}); return a["status"] == "complete" && b["status"] == "complete" && a["result"] == b["result"] && a["cpu_seconds"] > b["cpu_seconds"] ? 1 | 0;' % (self.obj,self.obj))
    def test_seconds_timeout_cleanup(self):
        self.code("slow",['while (1); string_hash("work"); endwhile'])
        self.check('add_property(%s,"done",0,{player,""}); add_property($server_options,"bg_ticks",2147483647,{player,""}); add_property($server_options,"bg_seconds",1,{player,""}); load_server_options(); fork (0); benchmark_call(%s,"slow",{}); %s.done=1; endfork; suspend(1.2); delete_property($server_options,"bg_ticks"); delete_property($server_options,"bg_seconds"); load_server_options(); return %s.done == 0 ? 1 | 0;' % ((self.obj,)*4))
    def test_z_restart_incomplete(self):
        self.code("wait",['suspend(100);','return 31;'])
        self.code("nested",['return call_function("benchmark_call",this,"wait",{});'])
        self.code("quick",['return 45;'])
        self.check('add_property(#0,"benchmark_error_seen",0,{player,""}); fork id (0); try; benchmark_call(%s,"wait",{}); except e (E_DIV); #0.benchmark_error_seen=1; endtry; endfork; add_property(#0,"benchmark_error_task",id,{player,""}); add_property(#0,"benchmark_kills",{},{player,""}); return 1;' % self.obj)
        for call in ['benchmark_call(%s,"wait",{})', 'call_function("call_function","benchmark_call",%s,"wait",{})']:
            self.check('fork id (0); '+(call % self.obj)+'; endfork; #0.benchmark_kills={@#0.benchmark_kills,id}; return 1;')
        self.check('add_property(#0,"benchmark_result",0,{player,""}); fork id (0); #0.benchmark_result=call_function("benchmark_call",%s,"nested",{}); endfork; add_property(#0,"benchmark_task",id,{player,""}); suspend(0.05); dump_database(); return 1;' % self.obj)
        out=self.server.path / "out.db"
        for _ in range(100):
            if out.exists(): break
            time.sleep(.05)
        self.assertTrue(out.exists())
        checkpoint=self.server.path / "checkpoint.db"
        checkpoint.write_bytes(out.read_bytes())
        malformed=self.server.path / "malformed.db"
        marker=b"benchmark_call continuation v1 incomplete"
        parts=checkpoint.read_bytes().split(marker)
        self.assertGreater(len(parts),2)
        # Corrupt each continuation in turn, including ones after an already
        # loaded outer scope, to exercise partial-load cleanup and wrappers.
        for damaged in range(len(parts)-1):
            payload=parts[0]
            for i,part in enumerate(parts[1:]):
                payload+=(b"benchmark_call continuation v9 incomplete" if i == damaged else marker)+part
            malformed.write_bytes(payload)
            rejected=subprocess.run([str(BINARY), "-4", "127.0.0.1", "-6", "::1", str(malformed), str(self.server.path / "reject.db"), "0"], cwd=self.server.path, capture_output=True, timeout=5)
            self.assertNotEqual(rejected.returncode,0)
            self.assertIn(b"Bad saved state",rejected.stderr)
            self.assertNotIn(b"ERROR: AddressSanitizer",rejected.stderr)
            self.assertNotIn(b"runtime error:",rejected.stderr)
        if os.environ.get("BENCHMARK_OLD_BINARY"):
            rejected=subprocess.run([os.environ["BENCHMARK_OLD_BINARY"], "-4", "127.0.0.1", "-6", "::1", str(checkpoint), str(self.server.path / "old-reject.db"), "0"], cwd=self.server.path, capture_output=True, timeout=5)
            self.assertNotEqual(rejected.returncode,0)
            self.assertIn(b"Unknown built-in function",rejected.stderr)
        # The original process's scope is still complete after checkpointing.
        self.check('resume(#0.benchmark_task); suspend(0.05); return #0.benchmark_result["status"] == "complete" ? 1 | 0;')
        self.f.close(); self.s.close(); self.server.stop(); self.server.start(checkpoint)
        self.s,self.f=self.server.connect()
        previous=out.stat().st_mtime_ns
        self.check('dump_database(); return 1;')
        for _ in range(100):
            if out.stat().st_mtime_ns != previous: break
            time.sleep(.05)
        self.assertNotEqual(out.stat().st_mtime_ns,previous)
        checkpoint.write_bytes(out.read_bytes())
        self.f.close(); self.s.close(); self.server.stop(); self.server.start(checkpoint)
        self.s,self.f=self.server.connect()
        self.check('for id in (#0.benchmark_kills); kill_task(id); endfor; resume(#0.benchmark_error_task,E_DIV); suspend(0.05); return #0.benchmark_error_seen == 1 ? 1 | 0;')
        self.check('r=benchmark_call(%s,"quick",{}); return r["status"] == "complete" && r["result"] == 45 ? 1 | 0;' % self.obj)
        self.check('resume(#0.benchmark_task); suspend(0.05); r=#0.benchmark_result; return r["status"] == "incomplete" && r["reason"] == "restart" && r["ticks"] == E_NACC && r["cpu_seconds"] == E_NACC && r["result"]["reason"] == "restart" && r["result"]["result"] == 31 ? 1 | 0;')


        # Once all benchmark continuations have completed or been cancelled,
        # a clean shutdown checkpoint must load in a benchmark-OFF binary.
        if os.environ.get("BENCHMARK_OLD_BINARY"):
            self.f.close(); self.s.close(); self.server.stop()
            off = Server(binary=os.environ["BENCHMARK_OLD_BINARY"], db=out)
            try:
                sock, stream = off.connect()
                try:
                    self.assertEqual(evaluate(stream, "return 42;"), "{1, 42}")
                finally:
                    stream.close(); sock.close()
            finally:
                off.close()

if __name__ == "__main__": unittest.main()
