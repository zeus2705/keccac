% SHA3-256(1) SHA3-256 1.0
% Antoine REMY & Stanislas MEDRANO
% October 2002

# NAME
SHA3-256 - compute SHA3-256 message digest

# SYNOPSIS
**SHA3-256** [OPTION]... [FILE]...

# DESCRIPTION
**SHA3-256** Print SHA256 (256-bit) checksums. With no FILE, or when FILE is -, read standard input.

# OPTIONS
**-h**, **--help**
: Display an help message

# EXAMPLE
**SHA3-256 | SHA3-256 -**
: Hash the standard input until an **EOF** or an **EOL** then display the hash in hexadecimal

**SHA3-256 -h | SHA3-256 --help**
: Display the software usage message and exit.

**SHA3-256 file_name**
: compute SHA3-256 and display the result in hexadecimal

# EXIT VALUES
**0**
: Sucess

**1**
: Invalid file name

# BUGS
None


# COPYRIGHT
Copyright © 2020 Free Software Foundation, Inc.  License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the extent permitted by law.

# AUTHOR
Written by Antoine REMY and Stanislas MEDRANO

# SEE ALSO
Full documentation on the SHA3-256 algorithm https://keccak.team/keccak_specs_summary.html
