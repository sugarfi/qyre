# qyre

qyre (pronounced 'queer', 'qwire', or 'cthulhu') is a simple
hobby os that you can read about [here](https://sugarfi.dev/qyre).

## building

clone [limine](https://github.com/limine-bootloader/limine) and build it. copy the resulting `limine-install` file
somewhere into your `PATH`, and copy `limine.sys` into the `boot` directory of this code. install `echfs-utils` from
[here](https://github.com/echfs/echfs). then run `make`.

## todo / issues

it turns out the allocator uses a *lot* of stack space so if anybody has a fix please do submit it
