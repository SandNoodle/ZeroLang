# Soul Language Grammar BNF

### Types

```
primitive_type ::= 'i32' | 'f32' | 'i64' | 'f64' | 'str'

```

### Variables

```
variable_declaration ::= <let> [ <mut> ] <identifier> : <type_identifier> '=' <expression> ';'
```

#### Example

```rust
let mut my_mutable_variable = ...;
```

### Structs

```
struct_declaration ::= <struct> <identifier> '{' <identifier> : <type_identifier> [ ',' ... ] '}'
```

#### Example
```c
struct ExampleStruct {
    x : i32,
    y : f64,
    z : str,
}
```