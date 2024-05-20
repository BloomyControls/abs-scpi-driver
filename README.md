# ABS SCPI Driver

C/C++ driver for communicating with a Bloomy Controls Advanced Battery Simulator
(ABS) via SCPI over TCP, UDP, and RS-485.

## Features

- TCP, UDP, RS-485, and UDP multicast
- Modern, native C++20 library
- Supports Windows and Linux
- Implements all SCPI commands and queries supported by the ABS
- Automatic device discovery over UDP multicast and RS-485 to quickly find and
  identify devices
- Exception-less error handling\*
- C wrapper (`include/bci/abs/CInterface.h`) for use in C and other languages
- Easy inclusion in CMake projects

\* Exceptions are not handled internally unless it is necessary to do so. Any
exceptions thrown by this library are considered critical exceptional
situations, such as a failure to allocate memory.

## Example Usage (C++)

This example uses a UDP driver to query the device's information (equivalent to
a `*IDN?` over SCPI). This library makes use of Sy Brand's excellent `expected`
library to provide exception-less error handling. If this library throws an
exception, it is considered an exceptional event, and is generally expected to
be unrecoverable.

Note that the UDP driver's `Open()` function also has an overload which takes
the local IP address to bind to. If you have multiple NICs, you probably want to
use this option.

```c++
#include <bci/abs/ScpiClient.h>
#include <bci/abs/UdpDriver.h>

#include <iostream>

int main(void) {
  using ec = bci::abs::ErrorCode;

  auto driver{std::make_shared<bci::abs::drivers::UdpDriver>()};
  bci::abs::ScpiClient client{driver};

  auto ret = driver->Open("192.168.1.70");
  if (ret != ec::kSuccess) {
    std::cerr << "Failed to open device: " << bci::abs::ErrorMessage(ret)
        << '\n';
    return 1;
  }

  if (auto idn = client.GetDeviceInfo()) {
    std::cout << "Part number: " << idn->part_number << '\n';
    std::cout << "Serial number: " << idn->serial << '\n';
    std::cout << "Version: " << idn->version << '\n';
  } else {
    std::cerr << "Failed to get device info: "
        << bci::abs::ErrorMessage(idn.error()) << '\n';
    return 1;
  }
}
```

## Example Usage (C)

This example implements the same logic as the C++ example above using the
C interface.

```c
#include <bci/abs/CInterface.h>

#include <stdio.h>
#include <string.h>

static void FormatErr(const char* msg, int ec) {
  fprintf(stderr, "%s: %s (%d)\n", msg, AbsScpiClient_ErrorMessage(ec), ec);
}

int main(void) {
  int ret = 1;
  int ec;
  AbsScpiClientHandle handle = NULL;
  AbsDeviceInfo idn;

  ec = AbsScpiClient_Init(&handle);
  if (ec < 0) {
    FormatErr("failed to initialize client", ec);
    goto cleanup;
  }

  ec = AbsScpiClient_OpenUdp(handle, "192.168.1.70", NULL);
  if (ec < 0) {
    FormatErr("failed to open device", ec);
    goto cleanup;
  }

  ec = AbsScpiClient_GetDeviceInfo(handle, &idn);
  if (ec < 0) {
    FormatErr("failed to get device info", ec);
    goto cleanup;
  }

  printf("Part number: %s\n", idn.part_number);
  printf("Serial number: %s\n", idn.serial);
  printf("Version: %s\n", idn.version);

  ret = 0;

cleanup:
  AbsScpiClient_Destroy(&handle);

  return ret;
}
```

## Building

To build the driver as a shared library (.DLL or .so), first configure CMake,
then execute the build system. This project can also be built with Visual
Studio.

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
cmake --build build
```

## Adding to a CMake Project

In your CMakeLists.txt, fetch and link with the driver:

```cmake
FetchContent_Declare(abs-driver
  GIT_REPOSITORY https://github.com/BloomyControls/abs-scpi-driver.git
  GIT_TAG v1.0.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(abs-driver)

target_link_libraries(my_program PRIVATE abs::scpi)
```

## Dependencies

This library does not have any runtime dependencies aside from the standard
C/C++ runtime libraries on your system. However, it does have some build
dependencies. If these are installed on your system, CMake will find and use
them if your installed versions are compatible. Otherwise, it will automatically
fetch and build them as part of the build. The build dependencies are:

- fmt v10.2.1
- Boost v1.81.0
- FastFloat v6.1.1

These are all linked statically, regardless of the build configuration.

## License

Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.

This software is distributed under the BSD-3-clause license. See the LICENSE
file or <https://opensource.org/license/BSD-3-Clause> for details.
