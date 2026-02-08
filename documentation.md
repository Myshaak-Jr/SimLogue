# SimLogue Documentation
### Structure
All source/header files are either in the `src/`, there are the source files that belong to the project or in `libs`, there are 3rd party libraries. Source and header files are not separated.

In the root `src/` should be only `main.cpp` and files that don't have much to do with the circuit simulation logic, like `settings.cpp` and of course subfolders.

The `src/lingebra/` is for all the linear algebra matrix manipulation math stuff.

And the `src/circuit/` hosts all the files related to the circuit simulation itself.

---
### CMakeLists.txt
For now there is only the root one, that lists all the `.cpp` files and has just one target that builds the executable. In the `CMakePresets.json` there are 3 configurations: one for Windows Visual Studio and two for Linux: Debug and Release.

It also supports building with half the floating point precision by turning the option `SIMLOGUE_HIGH_PRECISION` off.

---
### Scalar definition
The scalar type is either a double or a float depending on the build options. It also defines custom numerical literals for kilo, mega, giga, mili, micro, nano and pico, with the value being mutliplied accordingly. There is also the literar `_s` which just converts the value to the scalar type, I have yet to convert all literars to `_s`.

---
### Utilities
The `util.h` and `util.cpp` host function and constants that don't have a better place to reside in.

Because I am a huge $\tau=2\pi$ fan there is `scalar tau` defined which I then use.

There are also functions to compute the integer square root in $\mathcal{O}(N\log N)$ using binary search. Both floor and ceil versions are present.

There is also a helper `make_timestamp()` function that will return a current time and date string for naming files.

---
### Circuit
This is the main module. The main class is `Circuit`.

That class owns and stores all parts, nodes and scopes as in vectors of unique pointers.

**Parts:**
Every part derives from the `Part` base class. There are also helper classes `NPinPart<N>` that work as a base for a part with N pins. It is not mandatory to use that as parts base, the code checks for example for number of pins using the `Part` interface, not by whether it derives `NPinPart<2>`.

Every part stores standard pointers to each and every node it is connected to.

Every part has three methods to operate with its assigned matrix rows:
- `num_needed_matrix_rows()` tells the number of the rows required
- `set_first_matrix_row_id(size_t first_row_id)` is used to tell the part its first row index
- `get_first_matrix_row_id()` is then used to retrieve that first index, before assigning returns a zero
When the part doesn't require any rows, the `num_needed_matrix_rows()` is zero and `get_first_matrix_row_id()` returns always a zero.

Every part has a name, that is given to it usually in the constructor. You can retrieve it using part `.get_name()` 

The general workflow with parts and their connections is by using so called pins.

You can get the current that is flowing between two pins of this part using `part.get_current_between(const ConstPin &a, const ConstPin &b)`. **For the two pin parts, the current is measured positively from pin(0) to pin(1).** For many two and single pin part there is the optimisation of not even checking whether a and b are its and just returning the current through the part. This however becomes essential when dealing with higher pin-count parts.

**Pins:**
There are two structs defining a pin: `struct Pin` and `struct ConstPin`.

You can get pins from parts using `part.pin(size_t id)` or using the pins name `part.pin(const std::string &pinname)`. 

When accessing the pins by their name use just the pin name part, without the `MY_PART.`. When the part does not have the pin with the specified name a `std::out_of_range` exception is thrown. 

Each has two overloads depending on whether the class is currently constant on not, it will therefore return either a `Pin` or a `ConstPin` respectively.

Both pin structs have the same members:
- `size_t pin_id` - that is the numerical id of the pin 
- `Node *node` - this is the node that the pin is attached to
- `Part *owner` - this is the part that the pins corresponds to
- `std::string name` - used for debugging and printing info, I will consider using string views instead, because it's cheaper

`example_pin.owner->pin(example_pin.pin_id) == example_pin`

The only difference is that `ConstPin` has `const Node *` and `const Part *` and thus you cannot modify the part nor the node through it.

You can create `ConstPin` from `Pin` but not vice-verse.

**Nodes:**
Node is just a struct that stores it's `node_id`, whether it is connected to ground and its current voltage.

The `node_id` is used as a row index in the MNA matrix.

##### The circuit interface:
All parts and nodes and scopes should be created through the circuit class.

The scopes are created using `scope_voltage` and `scope current` methods. 

The parts are created using the `PartT *add_part<PartT>(...args)` method.

The nodes are created automatically through the `connect(a,b)` method, where `a` and `b` are the two pins that are to be connected.  It creates the nodes only when it needs to.

---
#### MNA:
MNA stands for Modified Nodal Analysis and it is the main algorithm that is used for setting up the circuit equation matrix.

It works by parts and nodes writing their equations in the matrix.

Every node and every part that needs its current solved because it or other part depends on it have their own row.

This works by parts stamping their equivalent resistor and current source values into the matrix on the positions of their nodes. You can read the basic specific stamps and more in [this paper](https://spinningnumbers.org/assets/MNA75.pdf).

---
#### Update in three steps:
**1. Building the matrix**
My very naïve approach is that every I build the matrix anew every frame. This is highly inefficient, but it is a working proof of concept. A better approach is discussed at the end of this document.

Every node gets assigned its row id at the very start of the building process. Then we assign additional rows to the parts that require it, every part gets a continuous block of indices starting at its first matrix row id. 

Then we prepare a list of all matrix entries in the triplet format. We stamp every part by generating its matrix entries and appending it to the list. The matrix is created with the size corresponding to the total number of rows needed and the list gets converted to the matrix by adding each triplet value to the corresponding coordinate. This is to make it easier to use sparse matrices in the future.

You can see the `struct StampParams` in the code, it exists to make the addition of new part update parameters easier. It currently consists of the ground pin, timestep and its inverse, current simulation step and current simulation time.

**2. Solving the system**
Each frame after the matrix is built the right-hand-side (RHS) vector is created, and each part stamps its RHS values into it using `.stamp_rhs_entries(std::vector<scalar>& rhs, const StampParams &params)`. This vector is then converted to `lingebra::Vector` and passed alongside the matrix into the gaussian elimination function.

**3. Updating the parts**
The resulting values are then distributed into the nodes' voltages as well as to the adequate parts.

Every part has its own update function which also takes the stamp parameters as an argument. This is for example for specific part scheduling and other stuff. It is empty by default.

---
### Scopes
Scopes are used to measure voltages and currents in the circuit. They record either the current between two pins of the same part using the `part.get_current_between(a, b)` method or the voltage between two pins each frame.

There is a base `Scope` class that and derived `VoltageScope` and `CurrentScope` classes which each implement different `record` functions. Furthermore, those derived classes each specify the name of the variable they measure to be then used in naming the column/axis/table.

The scope stores the values in `std::vectors`, `values[i]` holds the measured value at step $i$ and `times[i]` stores the relative time at step $i$. 

The user can choose to export those values using the `-e, --export-tables` flag, the export location is then specified by the user using `-t, --tables <path>`.

The `export_path` is created from the specified export location followed by a directory named using the current timestamp. The program will automatically create a copy of each exported table in `<export_path>/latest/`.

Each scopes data will be exported into a csv table named using the scope type and its underlining pin names with two columns corresponding to the times and values respectively.

Additionally, each scope has the ability to render its values as a graph using the sciplot library. The user can choose to do so using the `-g, --show_graphs` flag.

---
### Parts
Every part type is derived from the `Part` base class.
Currently, there are these part types available:
1. `VoltageSource`
   A single-pin dc voltage source, the negative pin is automatically connected to ground. Takes the voltage as a parameter in the constructor.
2. `VoltageSource2P`
   A two-pin version of the previous part. `pin(0)` is positive and `pin(1)` is negative pole.
3. `AcVoltageSource` 
   Same as the Voltage source but ac. Frequency, amplitude and phase need to be specified, updates its voltage in the update method each frame.
4. `AcVoltageSource2P`
   A two-pin version of the previous part. `pin(0)` is positive and `pin(1)` is negative pole.
5. `CurrentSource`
   A current source, specified current flows from `pin(0)` to `pin(1)`.
6. `Capacitor`
   Basic linear capacitor.
7. `Resistor`
   Basic linear resistor.
8. `Inductor`
   Basic linear inductor.
9. `Switch`
10. `OpAmp`
   Operational amplifier.

**Switches:**
The switch basically works as a variable resistor, switching between 0 and high resistance without changing the circuit topology. The high resistance is $10\,M\Omega$. Sadly when you do not want to change the topology in the middle of the run you can either have zero resistance when on and finite resistance off or 0 conductance off and finite conductance on, I will need to test which is better in the future.

The switch can be scheduled using its built-in event scheduling system. It uses the standard priority queue to enqueue and retrieve the events. The `schedule_on(size_t step)` and `schedule_off(size_t step)` are used for this task. 

`std::priority_queue<T>` does not ensure stability. As a result, when two events get scheduled to the same step the pop order is unspecified.

**Op Amps**
Those are implemented using switching states betweens `Linear`, where it behaves like an ideal linear amplifier and `SatHigh` and `SatLow` where it behaves like a voltage source.

There is a small hysteresis (1e-3) between the state switching to minimize rapid oscillations.

---

### The simlog language interpreter
The simlog language is a scripting language for creating circuits, it is designed to be easily readable by non-programmers.

The language is interpreted by the `Interpreter` class.

Each circuit class owns an interpreter, which is created in the circuits constructor, and that interpreter can only work with its parent circuit. 

The constructor is private and should be only called by the parent circuit object and **after the ground node has been created**.  

The interpreter has a simple interface consisting of two methods:
- `execute(std::istream &in)`/`execute(const std::string &script)`
  This executes the text in the input stream/string as simlog language line by line.
- `parse_value(std::string_view value_string, std::string_view where)`
  This static method is for parsing values like `485.89_mA`, its the only public parse method, because it is used also while parsing the command line arguments. `where` is used to give more adequate error messages.

Whenever the interpreter encounters an error, it throws `ParseError` with the error type and information.

There are those keywords:
- one for every part type: `capacitor`, `current_source`, `inductor`, `resistor`, `switch`, `voltage_source`, `voltage_source_2P`, `ac_voltage_source`, `ac_voltage_source_2P`
- `scope`
- `turn`

#### Parsing Values
`parse_value` returns a `struct Value` which holds the value itself and the quantity type of which the value is. The `Value` struct has a default value of `0.0` and `Quantity::Voltage` for the program to be able to default-construct it.

Physical quantities are defined in the `quantity.h` in the same directory as the interpreter source files.

It also defines the `UnitEntry` and `UnitInfo` structs, those are used when converting quantities to units and vice-versa. `UnitEntry` consolidates the unit and `UnitInfo` into one struct for a more readable unit lookup table. In the `UnitInfo` struct, every unit specifies of which quantity it is and its ratio to the base unit. 

Units include utf-8 characters like ° and Ω.

When parsing the value, the function first finds the biggest block of letter from the start that can all form a valid number, so it takes into account even multiple dots etc. It then tries to convert it into a scalar value using the standard library. If it fails to do so, it throws a syntax error. All underscores from that block are discarded in the process.

When it has the value the remainder forms the unit. In this unit part there is allowed to be only one more underscore. If its there it directly separates the unit multiplier from the unit itself and the program tries to parse them accordingly, it then multiplies the value but the correct ratio and returns it alongside the correct quantity deduced from the unit.

If there is no underscore in the unit part, it first tries to treat the whole remainder as a unit, it this fails, then it treats the first letter (either a one byte or two because of the utf-8 encoding) as the multiplier and the rest as a unit. When it succeeds it either variant it returns the correctly scaled value, otherwise it throws a syntax error.

#### Parsing Part Names
`parse_part` returns a `Part *` to a part with a specified name. It first checks if the name is valid. Each name is `r"[a-bA-B_]\w*"` but the checker is implemented without regex to boost the performance.

The part pointers are stored in a hash table. When you try to retrieve a part that has not been created it throws a name error.

#### Parsing Pin Names
Parsing pin names is a little bit more tricky than plain part names, because part name can be sometimes treated as a pin name itself. 

The `parse_pin` method takes the `pinname` itself as an argument alongside the current line index, again for printing error messages and arguments for parsing two-pin parts as pins.

Each normal pin name comes in the form of a part name followed by a dot followed by the pin name itself. 

For basic N-pin-parts the pin names are the first N letters of the lower-case English alphabet. In the general case the same predicate applies as for the part names.

The parser will try to find this dot. If it succeeds and the dot position isn't on the edge of the string it splits the string and finds the correct part using the first half and the `parse_part` and then checks the pin name and tries to get the pin from the part using it while catching the `std::out_of_range` error generated by the `.pin(pinname)` method and eventually returning it or throwing its own name error.

When there is no dot and the part is single-pin, it returns the `pin(0)`.

When there is no dot and the part is two-pin and the flag `support_twopin` is `true`, it returns the `pin(twopin_part_pin_id)`. This is for parsing connections.

#### Line Processing
The input simlog code is interpreted line-by-line. The line is firstly tokenized and then there is a decision tree that resolves it.

#### Tokenization:
Tokenization separates the source into words using white spaces as separators, while symbols: `:`,`-`,`,` are always treated as separate tokens.

Comments are discarded in this process. There is a member flag `bool parsing_comment` which makes parsing multiline comments persistent between line executions.

The output is a vector of string views as tokens.

#### Execution:
When there are tokens to process, the interpreter decides what to do depending on the first one. The first token is always either a keyword or the whole line specifies connection, therefore we can just branch the program using it and the "else" statement will parse connections.

The `curr_token` states which token is currently being processed. When the next token should be processed, the `curr_token` is increased and the interpreter shall firstly check whether such token exists, otherwise it is a syntax error. 

**Part definition:**
When the keyword is a part type, the interpreter immediately tries to create the part of the corresponding type. This is done using the `add_basic_part` method (basic because it supports only those parts whose constructors have just the name and scalar arguments).

The method takes the tokens, `curr_token`, current line index and `part_type_name` for error messages and then an array of $N$ `ParamInfo` structs called `constructor_signature`. 

The `constructor_signature` array is an image of the parts constructor, but instead of types it declares the quantity type. It can also be empty, in that case the constructor takes just the name.

The `ParamInfo` struct consolidates the quantity type and whether it has a default value, optionally the value itself of the related parameter. It has two constructors, either just the quantity or the quantity and the default value, the `has_default_value` will get resolved automatically. This makes it easier to write the signature, for example: `std::array<ParamInfo, 3>{ Frequency, Voltage, { Angle, 0.0 } }`.

The function then parses and check the part name and assures that the part doesn't yet exist, otherwise it is a part redefinition, which is an error.

Then all the constructor argument tokens are gathered and converted into an array of $N$ `Value` structs. If there happens to be more than $N$ values, it is automatically a syntax error. The function also takes into account the correct separator for each value token.

Because in the simlog script the constructor arguments of different quantities can be in an arbitrary order, the function then tries to find the correct value for each argument, it uses the constructor signature to do so, while not reusing the same arguments twice.

It then constructs the part and puts it in the part hash table.

**Scope definition:**
The scope definition is a decision tree where it checks the remaining tokens and allows a few simple sentences to be written: `scope (voltage|current) (of <two-pin part name>|between <pin_name_a> and <pin_name_b>)`. Where the for the current scope the two pins must have the same owner.

**Switch scheduling:**
The `turn` keyword is for the switch scheduling. It works in a similar way to the scope definition, it is also a simple decision tree. This time you write sentences in the form: `turn (on|off) <switch name> at <time value>`.

**Connecting the parts:**
When the first token is not a keyword, than the line declares part connections.

Every other token in the list should be a pin name and the rest `-` separators. The function checks for that and for every pair of pin names connected by the dash it creates the connections.

This time it also enables the `support_twopin` flag in the `parse_pin` and the for such parts is 1 for parts on the left of the dash and 0 for the ones on the right, this way you can link two pin parts in series.

---
### Lingebra (Linear Algebra Module)
All members of the lingebra module should be put into the lingebra namespace.

There is currently just one file `lingebra.h`.

**Warning:** gauss elimination and string representations currently don't work for ModInts, because I didn't have use for them.

#### `ModInt<size_t N>`
The `ModInt<N>` class isn't really useful in the circuit simulation, but it made testing matrices easier. It uses modulo arithmetics to work with integers.

For prime values of `N`, it supports finding the inverse using the Extended Euclidian algorithm and division.

#### `field` concept
A concept tries to represent mathematical field. It states that the type can be zero, can be one and those two are different. Furthermore it states that it can be multiplied, divised, added and substracted.
#### `Vector<field F>`
Variable length vector that supports additional mathematical operations.

The template argument F is the type that the vector stores and works with.

**Constructors**
- `Vector()` - empty
- `Vector(size_t size, const F &value)`
- `explicit Vector(size_t size)` - filled with zeros
- `Vector{a, b, c}` - using initializer list
- from iterators

**Basic API**
- get the vector dimension using `.dim()`
- to get the value in $\mathcal{O}(1)$ use `operator[index]`
- you can use `.assign(size_t new_size, const F &value)` to resize and fill with value
- use `.clear()` to fill the vector with zeros
- `.repr()` creates a string representation with floats being formatted to 3 decimal places

**Operations**
- You can add vectors per element using `+` and `+=`
- Use `vec1 * vec2` to compute the dot product
- You can scale the vector using a scalar `F` and operators `/` and `*`
#### `Matrix<field F>`
A dense matrix class.

The template argument F is the type that the matrix stores and works with.

`m` is the number of rows and `n` is the number of columns.

**Constructors**
- `Matrix()` - empty
- `Matrix(size_t m, size_t n, const F &value)`
- `explicit Matrix(size_t m, size_t n)` - filled with zeros
- `Matrix{{...},{...},...}` - initializer list of lists, the inner lists must have the same lengths
- `Matrix(vector<vector<F>>)`

**Basic API**
- get the matrix dimension using `.m()` and `.n()`
- to get the value in $\mathcal{O}(1)$ use `operator(row, col)`
- you can use `.assign(size_t new_m, size_t new_n, const F &value)` to resize and fill with value
- use `.clear()` to fill the vector with zeros
- `.rows()` direct access to the underlying `std::vector`
- `.repr()` creates a string representation with floats being formatted to 3 decimal places
- `.swap_rows(a,b)`
- `.is_square()` returns `.m() == .n()`

**Operations**
- You can add matrices per element using `+` and `+=`
- Use `mat1 * mat2` to compute the matrix product
- You can scale the matrix using a scalar `F` and operators `/` and `*`

#### Matrix Vector multiplication
You can multiply matrices and vectors.

#### Solving a system of equations
You can use `void solve_gaussian_elimination(Matrix<F> &A, Vector<F> &b);` to solve a system $A\vec{x}=\vec{b}$ in $\mathcal{O}(N^{3})$.

`b.dim()` has to be equal to `A.n()`.

It will throw `singular_matrix_exception` when encountering singular matrix. Or `std::runtime_error` when `b.dim() != A.n()`

It will modify the input matrix and the resulting vector $\vec{x}$ is stored in place of the input vector $\vec{b}$.

---
# The Future
### Building and solving the matrix better
Doing that every frame is of course a huge performance issue, with the matrix being mostly constant throughout the run making it almost pointless. I will replace this approach with a more performance friendly one in the future.

Firstly, I will use a sparse matrix built only once at the beginning of the run. This can lower the time complexity quite significantly because the number of entries $\in \mathcal{O}(P)$, where $P$ is the number of parts. I will use LU factorisation once with the attention to pivoting to keep it sparse and then just build the RHS vectors each frame.

I realised that when I sort the nodes by their connected part count I can improve the sparsity without some specialized pivoting algorithm. The output nonzero entries footprint resembles the problem and the related diagram discussed in the book *Circuit Simulation (Farid N. Najm)* on the page 114 & 115. This of course pivots just the node part of the MNA matrix. Also this pivoting is just for sparsity and accuracy should be taken into account as well.

This approach currently assumes that the matrix is constant the whole time. However, that usually isn't the case. I shall solve it as follows:

Each part will specify whether or not it requires changing the matrix every frame and the entries that change will be in the lowest bottom right corner of the matrix. This way I can reuse most of the LU factorisation from the beginning. The program will remember on which row it stopped doing the factorisation of the constant part and it will just compute the changing part each frame.

I will of course need to pivot that changing part, this can be precomputed.

There could however arise an issue when encounter a close-to-zero value previously selected as an pivot. That could happen because of the way I plan to do the LU factorisation. I will first perform a symbolic one, creating the non-zero entries in the matrix at the start of the run for both the static and dynamic parts. I will then fill the placeholder entries with the computed values without further pivoting. As a result, a zero pivot may occur in the latter process.

I will have to come up with a system robust enough to handle the earlier stated problem. I am thinking some "panic button" that the program can press in order to get a replacement for a specific small pivot. Maybe having multiple available entries for every pivot with precomputed differences of the resulting non-zero footprint?  

### Simlog Syntax Proposals
There is a file in the examples that is named `syntax_proposals.simlog`. You can find some syntactic sugar there which I will try to implement including part multi-declaration, easier syntax for parallel branches and more.
