#!/usr/bin/python3

# Adds PlatformIO post-processing to merge all the ESP flash images into a single image.

import os
import shutil

Import("env", "projenv")

board_config = env.BoardConfig()
firmware_bin = "${BUILD_DIR}/${PROGNAME}.bin"
legacy_merged_bin = "${BUILD_DIR}/${PROGNAME}-merged.bin"


def get_merged_bin_path(env):
    overridden_path = os.environ.get("MERGED_BIN_PATH")
    if overridden_path:
        return overridden_path

    pio_env = env.subst("$PIOENV")
    if not pio_env.startswith("TinyLora_"):
        return legacy_merged_bin

    firmware_roles = (
        "companion_radio_ble",
        "companion_radio_usb",
        "room_server",
        "repeater",
    )
    for role in firmware_roles:
        suffix = f"_{role}"
        if pio_env.endswith(suffix):
            board_name = pio_env[: -len(suffix)]
            filename = f"{board_name}_MeshCore_firmware_{role}_factory.bin"
            return os.path.join("${BUILD_DIR}", filename)

    return os.path.join("${BUILD_DIR}", f"{pio_env}_MeshCore_firmware_factory.bin")


def merge_bin_action(source, target, env):
    merged_bin = get_merged_bin_path(env)
    flash_images = [
        *env.Flatten(env.get("FLASH_EXTRA_IMAGES", [])),
        "$ESP32_APP_OFFSET",
        source[0].get_abspath(),
    ]
    merge_cmd = " ".join(
        [
            '"$PYTHONEXE"',
            '"$OBJCOPY"',
            "--chip",
            board_config.get("build.mcu", "esp32"),
            "merge_bin",
            "-o",
            merged_bin,
            "--flash_mode",
            board_config.get("build.flash_mode", "dio"),
            "--flash_freq",
            "${__get_board_f_flash(__env__)}",
            "--flash_size",
            board_config.get("upload.flash_size", "4MB"),
            *flash_images,
        ]
    )
    if env.Execute(merge_cmd) != 0:
        return 1

    merged_path = env.subst(merged_bin)
    legacy_path = env.subst(legacy_merged_bin)
    if os.path.normcase(os.path.abspath(merged_path)) != os.path.normcase(os.path.abspath(legacy_path)):
        shutil.copyfile(merged_path, legacy_path)
        print(f"Compatibility copy: {legacy_path}")

    return 0


env.AddCustomTarget(
    name="mergebin",
    dependencies=firmware_bin,
    actions=merge_bin_action,
    title="Merge binary",
    description="Build combined image",
    always_build=True,
)
