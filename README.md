# SimLogue
SimLogue is an analogue circuit simulator. The goal is to simulate modular synths in real-time.

---
### Features
- Currently supports these components:
	- voltage (single and double pin) and current sources
	- linear inductors, capacitors and resistors
	- switches
- Voltage and current scopes
- Rendering scope graphs and exporting the data to csv
- Loading circuits from .simlog files

---
### Usage
`simlogue [options] circuit_file duration`
Options:
- `-v, --version` - Show version information
- `-h, --help` - Show the help message
- `-r, --samplerate <freq>` - Sets the sample rate in Hz (default: `44100`)
- `-e, --export-tables` - Exports the scope tables
- `-t, --tables <path>` - Path to generated CSV tables (default: `./tables/`)
- `-g, --show-graphs` - Displays the scope graphs after run

`duration` is in seconds, and it represents the simulation time. So when the duration is `5` and the sample rate is `1000`, the simulation will produce `5000` samples.

---
### Requirements
- You need to have [gnuplot](http://gnuplot.info/) installed to render the graphs

---
### Build
#### On Linux
Using cmake in the CLI
```bash
git clone https://github.com/Myshaak-Jr/SimLogue.git
cd SimLogue

cmake --preset linux-release
cmake --build --preset build-linux-release
```
Resulting executable will be ./out/build/linux-release/simlogue.

#### On Windows
Using Visual Studio, open it as a CMake project, select Release and build all.
Resulting executable will be ./out/build/windows-vs/Release/simlogue.exe

---
### The simlog Language
A simple scripting language to build the circuits.
#### Features:

**Comments:**
Comment using C-style comments `// this will be commented`, `/* this is comment */`

**Components:**
Create a new component by writing:
`<component> <name>[: <value> ...]`

**List of available components:**
- `capacitor`
- `current_source`
- `inductor`
- `resistor`
- `switch` - doesn't need the value
- `voltage_source` - single pin version
- `voltage_source_2P` - two pin version

**Names:**
All names must be in the format: `[A-Za-z_][A-Za-z0-9_]*`
Examples: `V1`, `R5`, `VSource_25V`

Values:
All values consist of a value and a unit, like `5V`, `6.3m_Am`, `68_uF`, etc.
The cannot be a space between the value and the unit, you can use an underscore.

**Units:**
- Current - A, Am
- Voltage - V
- Resistance - Ohm, Ω
- Capacitance - F
- Inductance - H
- Time - s, min
- Frequency - Hz
- Angle - rad, deg, grad, °

You can use multipliers like `E, P, T, G, M, k, m, u, n, p, f, a` between the value and unit.

When using mutlipliers you can put '_' between them and the unit itself for readability: `k_Ohm` or in the future when for example units of length get added to differentiate between milli inches and minutes: `m_in` vs `min`.

That being said, when no underscore is present in the unit name, the parser will first try to parse as a whole unit name, and if it fails to do so it will proceed to parse the first letter as a multiplier. 

Units such as `mdeg` or `Grad` (giga radian) are technically supported although not recommended, `Grad` can be especially confusing with `grad` being a real unit. 

**Connections:**
Parts can be connected by simply writing: `V1 - R1`
This will connect `V1` to `R1`, since `V1` is a single-pin part, the parser will automatically select that pin. And since the `R1` is a two-pin part and you are connecting from the left, it will connect to the `.a` pin.
You can also chain them `V1 - S1 - R1 - C1 - GND`.
This is the same as:
```
V1.a - S1.a
S1.b - R1.a
R1.b - C1.a
C1.b - GND.a
```
Unless the parts are single- or two- pin, the names between the `-` must be pin names.

**Pin names:**
Pin names consist of a part name and the pin name itself separated by a dot.
For basic parts the pin names are indexed by letters: `a` for the first pin, `b` for the second one, etc.
Single-pin part names can be used directly as their pin name.

**Scopes:**
Scopes can be added by writing: `scope <quantity> (of <two-pin-part> | between <pin-name> and <pin-name>)`

Quantity can be either `voltage` and `current`.
When using `of`, the part name must have two exactly pins.
When using `scope current between`, the two pins must belong to the same part.

**Scheduling switches:**
Switched can be scheduled by writing: `turn (on|off) <switch-name> at <time>`

The corresponding switch will then set its state to the specified one when the simulation time reaches `<time>`. The time is specified using the `<value>` format with the unit being `s`.

****
### Technology
- The simulator uses the [MNA](https://spinningnumbers.org/assets/MNA75.pdf) approach.
- Currently I use gaussian elimination to solve the system
- The graphs are rendered using [Sciplot](https://sciplot.github.io/)

---
### Future plans
- Use sparse matrices and LU factorization with precalculated pivoting, the method Part::gen_matrix_entries is prepared to generate the entries for the sparse matrix.
- Create a multi-circuit system, that can be connected using buffered voltage inputs and outputs, every circuit will have its own matrix and thread.
- Make it real-time and export directly to the audio buffer.
