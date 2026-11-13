#!/usr/bin/env python3
"""Run native SQL ownership regressions against a temporary local PostgreSQL."""

import json
import os
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile


def run(command, **kwargs):
    return subprocess.run(command, check=True, **kwargs)


def main():
    if len(sys.argv) != 2:
        sys.exit("Usage: python3 test/run_sql_ownership.py <LeakCheck build directory>")
    root = Path(__file__).resolve().parent.parent
    build = Path(sys.argv[1]).resolve()
    cache = (build / "CMakeCache.txt").read_text()
    if "CMAKE_BUILD_TYPE:STRING=LeakCheck\n" not in cache:
        sys.exit("Use a LeakCheck build with CMAKE_EXPORT_COMPILE_COMMANDS=ON")

    # Reuse the server's compile/link settings and objects. Replace SQL with
    # the test translation unit and rename server main in a temporary copy.
    commands = json.loads((build / "compile_commands.json").read_text())
    entry = next(c for c in commands if Path(c["file"]) == root / "src/sql.cc")
    compile_args = entry.get("arguments") or shlex.split(entry["command"])
    link_args = shlex.split((build / "CMakeFiles/moo.dir/link.txt").read_text())
    pg_bin = Path(subprocess.check_output(["pg_config", "--bindir"], text=True).strip())

    with tempfile.TemporaryDirectory(prefix="toaststunt-sql-") as temp:
        temp = Path(temp)
        test_obj = temp / "sql_ownership.o"
        server_obj = temp / "server-no-main.o"
        executable = temp / "sql-ownership"
        compile_args[compile_args.index("-o") + 1] = str(test_obj)
        compile_args[compile_args.index("-c") + 1] = str(root / "test/sql_ownership.cc")
        run(compile_args, cwd=entry["directory"])

        server_source = next(a for a in link_args if a.endswith("/src/server.cc.o"))
        run(["objcopy", "--redefine-sym", "main=toaststunt_server_main",
             str(build / server_source), str(server_obj)])
        link_args = [str(test_obj) if a.endswith("/src/sql.cc.o") else
                     str(server_obj) if a == server_source else a for a in link_args]
        link_args[link_args.index("-o") + 1] = str(executable)
        run(link_args, cwd=build)

        data = temp / "data"
        socket = temp / "socket"
        socket.mkdir()
        run([str(pg_bin / "initdb"), "-D", str(data), "-A", "trust", "--no-locale"],
            stdout=subprocess.DEVNULL)
        # No TCP listener; this database and socket are inside a private directory.
        try:
            run([str(pg_bin / "pg_ctl"), "-D", str(data), "-l", str(temp / "postgres.log"),
                 "-o", f"-h '' -k {socket} -p 55483", "-w", "start"],
                stdout=subprocess.DEVNULL)
        except subprocess.CalledProcessError:
            print((temp / "postgres.log").read_text(), file=sys.stderr)
            raise
        try:
            conninfo = f"host={socket} port=55483 dbname=postgres"
            env = os.environ.copy()
            # Require all sanitizer checks, independently of caller settings.
            env["ASAN_OPTIONS"] = "detect_leaks=1:leak_check_at_exit=0"
            env["LSAN_OPTIONS"] = ""
            # The private database uses initdb's default owner, the current user.
            for name in list(env):
                if name.startswith("PG"):
                    del env[name]
            for mode in ("success", "syntax-error", "commit-error", "direct-commit-error"):
                run([str(executable), conninfo, mode, "5"], env=env)
            log = (temp / "postgres.log").read_text()
            if "STATEMENT:  COMMIT" not in log:
                sys.exit("Expected the PostgreSQL log to confirm failure during COMMIT")
            print("PostgreSQL log confirmed the deferred constraint failed during COMMIT.")
        finally:
            run([str(pg_bin / "pg_ctl"), "-D", str(data), "-m", "immediate", "-w", "stop"],
                stdout=subprocess.DEVNULL)


if __name__ == "__main__":
    main()
