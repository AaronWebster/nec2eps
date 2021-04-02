# nec2eps - Converts NEC files to EPS.
# https://github.com/AaronWebster/nec2eps

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "nec2eps",
    srcs = ["nec2eps.cc"],
    deps = [
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)
