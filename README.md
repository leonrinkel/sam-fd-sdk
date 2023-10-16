# sam-fd-sdk

## Initialization

Make sure you have a proper Zephyr development environment according to the official documentation.

```sh
west init -m https://git.leon.fyi/sam-fd-sdk --mr main sam-fd-workspace # or
west init -m https://github.com/leonrinkel/sam-fd-sdk --mr main sam-fd-workspace
cd sam-fd-workspace
west update
```

## Building

```sh
west build --sysbuild -b sam_fd sam-fd-sdk/app
```

## Flashing

### Using a Debugger

```sh
west flash --runner jlink # or
west flash --runner pyocd
```

### Via Serial Recovery

Serial recovery mode can be entered by running a command like the following immediately after resetting.

```sh
mcumgr \
    --conntype serial \
    --connstring dev=/dev/cu.usbserial-110,baud=115200 \
    image upload -e build/app/zephyr/zephyr.signed.bin
```

## License

Please see [LICENSE](LICENSE). The software side of this project is based on [Zephyr](https://www.zephyrproject.org) which is mostly licensed under the [Apache-2.0](http://www.apache.org/licenses/LICENSE-2.0) license.
