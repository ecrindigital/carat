
run example="playground":
    xmake f --enable_profiling=n -y -q && xmake -y -q && xmake run {{example}}_example

dev example="playground":
    xmake f --enable_profiling=y -m debug -y -q && xmake -y && xmake run {{example}}_example

test:
    xmake -y -q && xmake run tests

clean:
    xmake clean --all
