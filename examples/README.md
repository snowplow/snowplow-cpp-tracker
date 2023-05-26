# Example app for the C++ tracker

The app tracks a large number of events to a given collector.

## Installation

The app uses the tracker library imported using a CMake target.

### Build the tracker library

#### 1. Install Conan

First, install the Conan package manager.
On macOS, you can install via Homebrew: `brew install conan`

#### 2. Install dependencies using Conan

```sh
# cd into the root of this repository
cd snowplow-cpp-tracker

# install the dependencies into the `install` folder
conan install examples -b missing -of install/cmake -pr:b=default -pr:h=default -s build_type=Debug -g=CMakeDeps -o system_curl_uuid=False
```

#### 3. Build the tracker project

This will build the tracker into the `build-tracker` folder.
It will install then install the tracker in the `install` folder set using the `DCMAKE_INSTALL_PREFIX` attribute.

```sh
cmake -S . -B build-tracker -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Debug -DSNOWPLOW_USE_EXTERNAL_JSON=1 -DSNOWPLOW_USE_EXTERNAL_SQLITE=0 -DSNOWPLOW_BUILD_TESTS=0 -DSNOWPLOW_BUILD_EXAMPLE=0 -DSNOWPLOW_BUILD_PERFORMANCE=0 -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=1 -DBUILD_SHARED_LIBS=1
```

#### 4. Install the tracker library

```sh
cmake --build build-tracker --target install --config Debug
```

### Build the example app

#### 1. Prepare the build folder

Make sure that `DCMAKE_INSTALL_PREFIX` points to the folder that you chose to install the tracker library above.

```sh
# cd into the root of this repository
cd snowplow-cpp-tracker

cmake -S examples -B build-app -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Debug -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=1
```

#### 2. Compile the app

```sh
cmake --build build-app --config Debug
```

## Usage

Run the app from the `build-app` folder given the collector URL:

```sh
cd build-app

./snowplow-example http://localhost:9090
```
