# ABS SCPI Driver
TCP, UDP, and RS-485 SCPI driver for ABS.

TODO: requirements, documentation, etc...

## Example Usage

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
    std::cout << "Serial number: " << idn->serial_number << '\n';
    std::cout << "Version: " << idn->version << '\n';
  } else {
    std::cerr << "Failed to get device info: "
        << bci::abs::ErrorMessage(idn.error()) << '\n';
    return 1;
  }
}
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

target_link_libraries(my_program PRIVATE bci::abs-scpi)
```
