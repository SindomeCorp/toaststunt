# Install expat

sudo apt-get update
sudo apt-get install -y libexpat1-dev

# SQL
Follow steps in README.SQL.md

# Version
You may need to manually run perl /src/include/version_opt_gen.pl to generate your version_options.h

# Files
The EFS system should be /sindome/files and /sindome/library

The library gzip should be stored somewhere. /sindome/files/books/library should symlink to /sindome/library so that we don't have books saved in any /files backup.

The /files folder needs to be symlinked in /toaststunt/db/files -> /sindome/files/

1. ln -s /sindome/files /home/ubuntu/toaststunt/db/files

