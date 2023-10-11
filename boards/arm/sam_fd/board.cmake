# Copyright (c) 2021 Nordic Semiconductor ASA
# Copyright (c) 2023 Leon Rinkel
# SPDX-License-Identifier: Apache-2.0

board_runner_args(jlink "--device=atsamc21e16a-ant" "--speed=4000")
board_runner_args(pyocd "--target=atsamc21e16a" "--frequency=4000000")

include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
