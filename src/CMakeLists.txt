# Set sources

set(Srcdir src)

set(Root_Srcs
    ${Srcdir}/bptree.hpp
)

set(Complete_Srcs
    ${Root_Srcs}
    ${Srcdir}/main.cpp

    PARENT_SCOPE
)

# Test sources
set(Tests_Srcs
    ${Root_Srcs}
    ${Srcdir}/bptree_test.cpp
    ${Srcdir}/main_test.cpp

    PARENT_SCOPE
)

# Benchmark sources
set(Benchmark_Srcs
    ${Root_Srcs}
    ${Srcdir}/bptree_benchmark.cpp
    ${Srcdir}/main_benchmark.cpp

    PARENT_SCOPE
)