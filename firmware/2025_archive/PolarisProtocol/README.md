# Protobuf protocol for Polaris
This PlatformIO project can be used to generate and test a Protobuf based binary protocol for Polaris.

# Running the tests
```shell
pio run -t exec
```

# Generated files
The protocol files are generated when running the tests and the output is
located at `.pio/build/buoy_data/nanopb/generated-src`. To use the protocol
in a project, the `buoy_data.pb.h` and `buoy_data.pb.c` files can be copied
over and a dependency on Nanopb added.