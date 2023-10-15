# bootloader

## Building

```sh
cd bootloader/mcuboot/boot/zephyr
west build -b sam_fd -p always -- \
    -DCONFIG_SINGLE_APPLICATION_SLOT=y \
    -DCONFIG_BOOT_SIGNATURE_TYPE_NONE=y \
    -DCONFIG_BOOT_VALIDATE_SLOT0=n \
    -DCONFIG_MAIN_STACK_SIZE=1024 \
    -DCONFIG_MCUBOOT_SERIAL=y \
    -DCONFIG_CONSOLE=n \
    -DCONFIG_UART_CONSOLE=n \
    -DCONFIG_BOOT_SERIAL_WAIT_FOR_DFU=y \
    -DCONFIG_BOOT_SERIAL_WAIT_FOR_DFU_TIMEOUT=500 \
    -DCONFIG_BOOT_ERASE_PROGRESSIVELY=y
```

## Flashing

```sh
west flash --runner jlink # or
west flash --runner pyocd
```
