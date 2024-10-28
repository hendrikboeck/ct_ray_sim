# CT Ray Simulation

CT Ray Simulation is a C++ project that simulates computed tomography (CT) scans by generating projections of a given density map image at various angles.

## Features

- Load and process density map images.
- Simulate CT projections at specified angles.
- Save the resulting projection images.
- Detailed logging with `spdlog`.

## Installation

1. **Clone the repository**:

```sh
git clone https://github.com/hendrikboeck/ct_ray_sim.git
cd ct_ray_sim
```

2. **Set up the development environment**:

Ensure you have [Nix](https://nixos.org/nix/) installed, then run:
```sh
direnv allow
```

3. **Build the project**:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -Bbuild .
cmake --build build
```

## Usage

Run the simulation with the following command:
```sh
build/ct_ray_sim --inputPath <path_to_image> --outputPath <output_directory> --angles <number_of_angles>
```

Example:
```sh
./ct_ray_sim --inputPath images/sample.png --outputPath output --angles 360
```

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

Feel free to customize the README further as needed.