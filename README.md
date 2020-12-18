# env-set

env-set is a small utility to launch another program on a clean environment,
especially where /usr/bin/env may not exist.

Use it as follows:

`env-set [KEY=VALUE] [KEY=VALUE] -- [command] [args...]`

Where `KEY=VALUE` pairs are the environment variables to set
(if you don't set any, you launch the command in a completely clean environment,
like what `env -i` does) and `command/args...` is the command and
arguments to use.

`--` is mandatory to separate environment variables and commands.

If you set the environment variable `PATH` as part of calling `env-set`,
`env-set` will proactively use that `PATH` to search for the command to run.

Example:

`env-set HOME=/ PATH=/usr/bin:/tools/bin LC_ALL=POSIX -- bash build.sh`

