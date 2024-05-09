load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test", "cc_binary")

cc_library(
    name="bptree",
    hdrs = [
        "bptree.hpp",
    ],
)

cc_test(
    name = "test",
    srcs = [
        "bptree_test.cpp",
        # "main_test.cpp",
    ],
    deps = [
        "@com_google_googletest//:gtest_main",
        ":bptree",
    ],
)

cc_binary (
    name = "bench",
    srcs = [
        "bptree_benchmark.cpp",
    ],
    deps = [
        "@com_google_benchmark//:benchmark_main",
        ":bptree",
    ],
)
