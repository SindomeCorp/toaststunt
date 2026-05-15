# Ubuntu build dependencies

```bash
sudo apt update
sudo apt install -y \
  build-essential bison gperf cmake g++ make perl pkg-config wget ca-certificates \
  libsqlite3-dev libaspell-dev libcurl4-openssl-dev libargon2-dev libssl-dev \
  libexpat1-dev libcrypt-dev nettle-dev libpq-dev libpq5
```

# PCRE (manual install for newer Ubuntu where libpcre3-dev is unavailable)

```bash
cd /tmp
wget -O pcre-8.45.tar.gz "https://sourceforge.net/projects/pcre/files/pcre/8.45/pcre-8.45.tar.gz/download"
tar -xzf pcre-8.45.tar.gz
cd pcre-8.45
./configure --prefix=/usr/local
make -j"$(nproc)"
sudo make install
sudo ldconfig
```

# SQL
Follow steps in README.SQL.md

# Version
You may need to manually run perl /src/include/version_opt_gen.pl to generate your version_options.h

# Configure and build ToastStunt

From the repository root:

```bash
rm -rf build
cmake -S . -B build \
  -DPCRE_INCLUDE_DIR=/usr/local/include \
  -DPCRE_LIBRARY=/usr/local/lib/libpcre.so
cmake --build build -j"$(nproc)"
```

If your `libpcre.so` is in `lib64`, use:
`-DPCRE_LIBRARY=/usr/local/lib64/libpcre.so`

# Files
The EFS system should be /sindome/files and /sindome/library

The library gzip should be stored somewhere. /sindome/files/books/library should symlink to /sindome/library so that we don't have books saved in any /files backup.

The /files folder needs to be symlinked in /toaststunt/db/files -> /sindome/files/

1. ln -s /sindome/files /home/ubuntu/toaststunt/db/files
