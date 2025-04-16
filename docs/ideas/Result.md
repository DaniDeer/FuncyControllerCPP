# Result pattern

See in the already implemented `Either<T, E>` pattern:

### Bonus: Alias Types

```cpp
using Result<T> = Either<T, String>;
```

Now you can return `Result<int>`, `Result<float>`, etc., without repeating `Either<T, String>` everywhere.

ToDo: Extend this with a full `Result<T>` type like in Rust?