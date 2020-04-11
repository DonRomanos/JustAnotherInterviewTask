from cpt.packager import ConanMultiPackager
import subprocess

if __name__ == "__main__":
    docker_setup_command = "conan remote add catchorg https://api.bintray.com/conan/catchorg/Catch2 && conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan"

    builder = ConanMultiPackager(
        username = "donromanos",
        build_policy = "outdated",
        docker_entry_script = docker_setup_command,
        # build configurations
        archs = ["x86_64"] #  limit to 64-bit only
    )
    builder.add_common_builds(pure_c=False)
    builder.remove_build_if(lambda build: build.settings["compiler.libcxx"] == "libstdc++")
    for settings, options, env_vars, build_requires, reference in builder.items:
        settings["compiler.cppstd"] = "17"
    builder.run()