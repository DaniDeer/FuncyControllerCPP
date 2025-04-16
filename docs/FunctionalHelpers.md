# Functional Helpers

Functional helpers are based on implemented functors and monads.

## Overview

`IO<T>` / `IO<void>` and `Either<T, E>`:

- `pure(T)`: wraps a value T into `IO<T>`
- `unit()`: returns an empty `IO<void>` with no side-effect
- `liftIO(IO<T>)`: lifts an `IO<T>` to `IO<Either<T, E>>` assuming it is always successful (`Right`)
- `liftIOLeft(IO<E>)`: lifts an `IO<E>` to `IO<Either<T, E>>` with an error on the `Left`
- `liftVoidIO()`: lifts an `IO<void>` to `IO<Either<void, E>>` (right)
- `liftIOtoEither()`: wraps an `IO<T>` into `IO<Either<T, E>>` based on predicates, applying error-checking logic
- `flatMapIOEither()` (TaskEither): Unwrap the `IO<Either<T, E>>`, inspect the `<Either<T, E>`. If it's a Right, call f() (which returns another `IO<Either<T, E>>`. If it's a Left, short-circuit and return that inside a new IO. Chains `IO<Either<T, E>>` functions.
- `mapIOEither(IO<Either<T, E>>)`: transforms the Right value inside `IO<Either<T, E>>` using (T -> U)
- `mapLeftIOEither(IO<Either<T, E>>)`: transforms the Left value inside `IO<Either<T, E>>` using (E -> F). This is useful for error handling, where you want to transform the error type.

Missing:

- `liftVoidIOLeft()`: lifts an `IO<void>` to `IO<Either<void, E>>` (left)
- `IO<T>` / `IO<void>` and `Maybe<T, E>`?
- Async helpers? AsyncIO?
