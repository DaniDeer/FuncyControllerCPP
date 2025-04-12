#include <Arduino.h> // Requires Arduino framework context
#include <vector> // Required for std::vector

// My functional programming lib
#include <FuncyControllerCPP.hpp> // Include the main library header
using namespace funcy_controller_cpp; // Use the library's namespace to avoid qualifying every type

Maybe<int> findFirstEven(const std::vector<int>& numbers, size_t index = 0) {
  // Base case: if we've exhausted the list, return Nothing
  if (index >= numbers.size()) {
    return Maybe<int>::Nothing();
  }

  // Recursive case: check the current element
  if (numbers[index] % 2 == 0) {
    return Maybe<int>::Just(numbers[index]);
  }

  // Recur for the next element
  return findFirstEven(numbers, index + 1);
}

void testMaybePatternBasic(const std::vector<int>& numbers) {
  Maybe<int> result = findFirstEven(numbers);

  result.match(
    [](int value) { Serial.println("Found even number: " + String(value)); },
    []() { Serial.println("No even number found"); }
  );
}

// Pure Function yields int
int sumEvenNumberOrZero(const std::vector<int>& numbers, int additionalValue) {
    Maybe<int> result = findFirstEven(numbers);

    // Use fold to process the Maybe value
    return result.fold(
        [](int value) { return value; },        // if it's Just, return the value
        []() { return 0; }                      // if it's Nothing, return 0
    ) + additionalValue;
}

void testMaybePatternWithFold(const std::vector<int>& numbers, int additionalValue) {
    // Process the numbers and print the result
    int result = sumEvenNumberOrZero(numbers, additionalValue);
    Serial.println("Result: " + String(result));  // Output should be 16 (6 + 10)
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:

  int additionalValue = 10;
  std::vector<int> numbers = {1, 3, 5, 6};
  testMaybePatternBasic(numbers); // -> Found even number: 6
  testMaybePatternWithFold(numbers, additionalValue); // -> Result: 16


  std::vector<int> numbersNothing = {1, 3, 5, 7};
  testMaybePatternBasic(numbersNothing); // -> No even number found
  testMaybePatternWithFold(numbersNothing, additionalValue); // -> Result: 10

  delay(1000);
}
