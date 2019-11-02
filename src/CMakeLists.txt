# Set sources

set(Srcdir src)

set(Root_Srcs
    ${Srcdir}/bptree.hpp
    ${Srcdir}/bptnode.hpp
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
    ${Srcdir}/bptnode_test.cpp
    ${Srcdir}/main_test.cpp

    PARENT_SCOPE
)