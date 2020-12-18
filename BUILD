genrule(
    name = "env-set",
    srcs = ["env-set.c"],
    outs = ["env-set"],
    binary = True,
    cmd = '"$TOOL" -o "$OUT" -std=gnu11 -Wall -Werror -g -static $SRC',
    tools = [
        "x86_64-linux-musl-gcc",
    ],
    visibility = ["PUBLIC"],
)
