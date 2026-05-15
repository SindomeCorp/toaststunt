Introduction
============

ToastStunt comes with the ability to support multiple SQL server types. 
Each client requires the appropriate library to be installed when cmake 
is executed.

As of this version of the SQL implementation, the following types are
supported:

* PostgreSQL
* SQLiteV3

This document explains how to build and deploy with SQL.

How SQL can be used
===================

No part of ToastStunt natively uses SQL for storage or propagation.
Installing or enabling SQL will not move away from the flatfile database
format, and no connection strings are hardcoded into ToastStunt.

Instead, SQL can be used to augment data storage and indexing
outside of the MOO. Doing so can allow that data to be accessed by
external applications (websites, other services), and because of the
non-blocking nature of SQL support, can be used to offload that data
out of memory.

For example, a ticket system could be used that has an in game interface, 
or a website interface.

A pose system that records poses and has a website interface for
viewing them is another example, where storing that data in MOO
may cause performance complications.

Compiling with SQL
==================

To enable SQL support with ToastStunt, it is first necessary to
install the related client libraries. These libraries are listed below in
the relevant subsections depending on which server types you wish to 
support.

As many or as few libraries may be installed as desired. It possible
to enable all server types, or disable all of them.

After the client libraries are installed, it's only necessary to
run cmake as you would normally on ToastStunt and compile!

PostgreSQL
----------

### **Debian/Ubuntu/WSL**
`libpqxx` from distro packages may be too old or incompatible with newer toolchains.
Build a modern `7.x` release manually.
```bash
sudo apt update
sudo apt install -y libpq-dev libpq5 cmake g++ make

cd /tmp
rm -rf libpqxx
git clone https://github.com/jtv/libpqxx.git
cd libpqxx
git checkout 7.10.1
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
sudo cmake --install build
sudo ldconfig
```
Notes:
- Avoid `7.6.x` with CMake 4.x; those tags can fail to configure.
- If `7.10.1` is unavailable, use the newest `7.x` tag:
  `git tag -l "7.*" | tail -n 20`

### **Gentoo**
```bash
git clone https://github.com/jtv/libpqxx.git
cd libpqxx
git checkout 7.10.1
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
sudo cmake --install build
```

### **FreeBSD**
```bash
git clone https://github.com/jtv/libpqxx.git
cd libpqxx
git checkout 7.10.1
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(sysctl -n hw.ncpu)"
sudo cmake --install build
```

SQLiteV3
--------

### **Debian/Ubuntu/WSL**
As of 3/28/22 this was: 3.27.2
```bash
apt-get install libsqlite3-dev
```

### **Gentoo**
```bash
emerge dev-db/sqlite
```

### **FreeBSD**
```bash
pkg install sqlite3 
```
