# Building and installing Sindome's ToastStunt server

## Ubuntu 26.04

Build on the machine that will run the MOO. The instructions below use the existing
`~/toaststunt` checkout and `build` directory. They assume the MOO is **offline**
and the previous executable and production database have already been backed up;
no running MOO PID is needed. Building does not start the server.

Use **libpqxx 7.10.7**, built as a shared library with matching headers, installed
privately under `~/toaststunt/.deps/libpqxx-7.10.7`. This is the PostgreSQL **C++**
wrapper; Ubuntu's `libpq5`/`libpq-dev` provide its C dependency. Do not substitute a
library-only upgrade or install this wrapper over `/usr` or `/usr/local`.

The Ubuntu 26.04 libpqxx `7.10.0-2build1` package reproduced a double-free during
checkpoint-child exit and normal shutdown. With matched 7.10.7 headers and shared
library, the original checkpoint `exit(!success)` passed repeated checkpoints,
SQL activity, saved-database reload and normal shutdown. See the
[upstream shutdown issue](https://github.com/jtv/libpqxx/issues/1195) and
[7.10.7 release](https://github.com/jtv/libpqxx/releases/tag/7.10.7).

Execute each stage separately. The subshells use `set -euo pipefail` so a failure
stops that stage without changing the interactive shell's options. Stop on an
error and inspect it before proceeding. Do not append `|| true` to a build stage.

### 1. Install prerequisites and record existing settings

```bash
(
    set -euo pipefail
    sudo apt-get update
    sudo apt-get install \
        build-essential cmake bison gperf perl curl python3 pkgconf \
        ripgrep binutils netcat-openbsd \
        libpq-dev libpq5 libpcre2-dev nettle-dev libargon2-dev \
        libsqlite3-dev libaspell-dev libcurl4-openssl-dev libssl-dev \
        libexpat1-dev
)
```

`libpcre2-dev` is required for `pcre2.h`. If it was absent during configuration,
install it and rerun the **complete CMake configuration in step 3**, not just the
compilation command. The Sindome SQL/server sources require PCRE2 headers.

Inspect the checkout, compilers and previous configuration before clearing its
cache. A new checkout without a previous build can skip the cache inspection.

```bash
cd ~/toaststunt
git status --short --branch
cmake --version
g++ --version
pg_config --version
rg '^(CMAKE_BUILD_TYPE|CMAKE_C_COMPILER|CMAKE_CXX_COMPILER|CMAKE_TOOLCHAIN_FILE|CMAKE_C_FLAGS|CMAKE_CXX_FLAGS|CMAKE_EXE_LINKER_FLAGS|ONLY_32_BITS|USE_JEMALLOC):' \
    build/CMakeCache.txt
```

The commands below reproduce the verified native settings: `/usr/bin/cc` and
`/usr/bin/c++`, Release ToastStunt, 64-bit integers and jemalloc disabled. Preserve
any intentional production differences in flags, toolchain, dependency paths and
`src/include/options.h`. Use a compatible, consistent C++ compiler/toolchain for
both libpqxx and ToastStunt. Review CMake's optional-feature detection as well;
missing development packages must not silently remove features you depend on.

### 2. Build and privately install libpqxx 7.10.7

Use fresh source so old in-source build files or generated headers in an existing
`libpqxx` directory cannot contaminate the build. Everything stays in this checkout.
The initial guards deliberately stop if these versioned paths already exist;
inspect an existing installation rather than overwriting it blindly.

```bash
(
    set -euo pipefail
    cd ~/toaststunt
    pqxx_prefix="$PWD/.deps/libpqxx-7.10.7"

    test ! -e "$pqxx_prefix"
    test ! -L "$pqxx_prefix"
    test ! -e .deps/src/libpqxx-7.10.7
    test ! -e .deps/build/libpqxx-7.10.7
    mkdir -p .deps/src .deps/build

    curl --fail --location \
        https://codeload.github.com/jtv/libpqxx/tar.gz/refs/tags/7.10.7 \
        -o .deps/src/libpqxx-7.10.7.tar.gz

    printf '%s\n' \
        '8c09cf20d57695621376f34d6da24c2c40c28408ab3b7ca92dc661ff9ad4b906  .deps/src/libpqxx-7.10.7.tar.gz' \
        | sha256sum --check -

    tar -xzf .deps/src/libpqxx-7.10.7.tar.gz -C .deps/src

    cmake -S .deps/src/libpqxx-7.10.7 \
        -B .deps/build/libpqxx-7.10.7 \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_CXX_COMPILER=/usr/bin/c++ \
        -DBUILD_SHARED_LIBS=ON \
        -DSKIP_BUILD_TEST=ON \
        -DBUILD_TEST=OFF \
        -DBUILD_DOC=OFF \
        -DCMAKE_INSTALL_LIBDIR=lib \
        -DCMAKE_INSTALL_PREFIX="$pqxx_prefix"

    cmake --build .deps/build/libpqxx-7.10.7 -j 2
    test ! -e "$pqxx_prefix"
    test ! -L "$pqxx_prefix"
    cmake --install .deps/build/libpqxx-7.10.7

    rg 'PQXX_VERSION ' "$pqxx_prefix/include/pqxx/version.hxx"
    ls -l "$pqxx_prefix/lib/libpqxx"*
)
```

Require checksum `OK`, header version `7.10.7`, and a shared `libpqxx-7.10.so` with
its `libpqxx.so` symlink. The checksum identifies the upstream tag archive tested
in this investigation. No `sudo` installation or `ldconfig` is required. Upstream
libpqxx tests are disabled by these build options; that is not a claim they ran.

### 3. Configure and compile the MOO in the existing build directory

`--fresh` clears cached configuration, including the old aggregate
`PQXX_LIBRARIES` and `PQXX_INCLUDE_DIRECTORIES` values. The complete command is
important: setting only one library path in an old cache can retain stale headers
or dependencies. Record any custom settings before using `--fresh`.

```bash
(
    set -euo pipefail
    cd ~/toaststunt
    pqxx_prefix="$PWD/.deps/libpqxx-7.10.7"

    cmake --fresh -S . -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=/usr/bin/cc \
        -DCMAKE_CXX_COMPILER=/usr/bin/c++ \
        -DONLY_32_BITS=OFF \
        -DUSE_JEMALLOC=OFF \
        -DPQXX_LIBRARY="$pqxx_prefix/lib/libpqxx.so" \
        -DPQXX_HEADER_PATH="$pqxx_prefix/include" \
        -DCMAKE_BUILD_RPATH="$pqxx_prefix/lib" \
        -DCMAKE_INSTALL_RPATH="$pqxx_prefix/lib"

    cmake --build build -j 2

    readelf -d build/moo | rg 'NEEDED|RUNPATH|RPATH'
    env -u LD_LIBRARY_PATH -u LD_PRELOAD ldd build/moo
)
```

Require `Found PCRE2` and a PostgreSQL-wrapper discovery line pointing at the
private `.deps/libpqxx-7.10.7/lib/libpqxx.so`. Compilation must finish with
`Built target moo`. Check that all previously required optional features were
found. `Found PostgreSQL` alone is insufficient if it selected an old wrapper.

### 4. Verify headers and runtime library resolution

```bash
cd ~/toaststunt
rg 'pqxx' build/CMakeFiles/moo.dir/src/sql.cc.o.d
rg 'PQXX_(LIBRARY|HEADER_PATH|LIBRARIES|INCLUDE_DIRECTORIES)' build/CMakeCache.txt
```

The compiler dependencies must use `.deps/libpqxx-7.10.7/include/pqxx`, not old
`/usr/include/pqxx`, `/usr/local/include/pqxx` or generated headers from another
source tree. The dynamic output in step 3 must show:

```text
NEEDED:  libpqxx-7.10.so
RUNPATH: /home/ubuntu/toaststunt/.deps/libpqxx-7.10.7/lib
libpqxx-7.10.so => /home/ubuntu/toaststunt/.deps/libpqxx-7.10.7/lib/libpqxx-7.10.so
```

The home-directory component reflects the actual build account. `libpq.so.5`
should continue resolving to the intended system library. There must be no
`not found` entries. The SONAME `libpqxx-7.10.so` does not identify the patch version;
check the matching installed header, explicit paths and runtime resolution together.

Keep the private install prefix permanently while this binary is in use, and
ensure the MOO service account can traverse its parent directories and read the
library. Do not commit `.deps` build artifacts. Review the actual startup
process's `LD_LIBRARY_PATH`/`LD_PRELOAD`, which can interfere with RUNPATH selection.

`./build/moo --version` intentionally exits with status **1** after printing the
version. It is not a zero-exit smoke test and will stop a plain `set -e` block.
It also does not exercise database loading, SQL or checkpoint completion.

### 5. Test checkpointing, reload and normal shutdown before starting production

Use only the small `test/Test.db` fixture, never a second live production instance.
The fixture permits wizard logins, so bind it explicitly to localhost and disable
IPv6. In one terminal, start this temporary server:

```bash
(
    set -euo pipefail
    cd ~/toaststunt
    test ! -e build/checkpoint-smoke.db
    env -u LD_LIBRARY_PATH -u LD_PRELOAD \
        ./build/moo -4 127.0.0.1 --no-ipv6 -O \
        test/Test.db build/checkpoint-smoke.db 19898
)
```

If a previous test left that output file, inspect it and choose a different
**disposable test output** filename. Do not substitute a production database.
Port 19898 must be unused; a bind failure is a reason to select another test port,
not to stop an unrelated process. The `-O` option disables MOO outbound connections.

In a second terminal, connect:

```bash
nc 127.0.0.1 19898
```

Enter these lines individually. The two setup commands add a completion logger to
this test database so checkpoint success is verified from its callback argument:

```moo
connect wizard
; add_verb(#0, {#3, "rxd", "checkpoint_finished"}, {"this", "none", "this"});
; set_verb_code(#0, "checkpoint_finished", {"server_log(\"SMOKE CHECKPOINT SUCCESS=\" + tostr(args[1]));"});
; dump_database();
```

The first terminal should show `SMOKE CHECKPOINT SUCCESS=1`, without a double-free
or other crash. A `finished` file-writing message alone is not proof that the
checkpoint child exited successfully. Repeat `; dump_database();` several times,
waiting for each success callback before starting the next checkpoint.

Then request normal shutdown in the second terminal:

```moo
; shutdown("checkpoint smoke test complete");
```

Immediately after the test server exits, run `echo $?` in the first terminal.
Require status **0**, with no cleanup crash. Then reload the saved test database:

```bash
(
    set -euo pipefail
    cd ~/toaststunt
    test ! -e build/checkpoint-smoke-reloaded.db
    env -u LD_LIBRARY_PATH -u LD_PRELOAD \
        ./build/moo -4 127.0.0.1 --no-ipv6 -O \
        build/checkpoint-smoke.db build/checkpoint-smoke-reloaded.db 19898
)
```

Reconnect with `nc`, enter `connect wizard`, repeat a checkpoint, and shut down
normally again. Require successful loading, a success callback and exit status 0.
This fixture smoke test does not replace application-specific SQL and database
validation. It does not start or modify a PostgreSQL service or use production SQL
credentials. SQL checks must use explicitly selected test resources.

### 6. Start production and keep rollback data separate

After the test server is stopped and checks pass, use the established production
startup procedure. Confirm it launches the newly built `~/toaststunt/build/moo`
and retains access to the private library directory. No service name, production
database filename or stop/start command is assumed by this guide. Review
`restart.sh` before using it: it promotes/rotates databases and logs.

Once production is running, identify its new main PID and inspect
`/proc/<PID>/exe` and `/proc/<PID>/maps` to confirm the actual executable and private
libpqxx library. Trigger a checkpoint through the normal MOO mechanism and verify
its success notification and logs, then check ordinary application SQL behavior.

The old executable supports a binary rollback; it does not roll back MOO database,
external SQL or file changes. Those require a separate consistent recovery point.

## Version metadata

Normal CMake configuration generates `version_options.h` using
`src/include/version_opt_gen.pl`. If it needs to be regenerated manually, run
`perl ../src/include/version_opt_gen.pl` **from `~/toaststunt/build`** so the output
stays in the build directory.

## Sindome files and EFS

The EFS system should be mounted at `/sindome/files` and `/sindome/library`.
Store the library gzip separately; `/sindome/files/books/library` should symlink
to `/sindome/library` so books are not duplicated in `/files` backups.

The MOO's `db/files` path should point to `/sindome/files`. After verifying the
mount and that the destination does not already exist, the existing layout uses:

```bash
ln -s /sindome/files /home/ubuntu/toaststunt/db/files
```
