/*
 * A simple moving average buffer.
 */

#ifndef MOVING_AVG_H_
#define MOVING_AVG_H_

template<typename T, size_t S, typename ST>
class MovingAvg {
public:
  MovingAvg() {
    // Populate buffer with zeros.
    for (int i = 0; i < S; i++) {
      circularBuffer[i] = static_cast<T>(0);
    }
    writeIndex = 0;
    sum = static_cast<ST>(0);
  }

  void push(T value) {
    if (isnan(value) || isinf(value)) {
      return;
    }
    // Store oldest value in a temp variable before overwriting it
    T oldestValue = circularBuffer[writeIndex];
    circularBuffer[writeIndex] = value;

    // Increment pointer
    writeIndex++;
    writeIndex = (writeIndex >= S) ? 0 : writeIndex;

    // Update size
    bufferSize++;
    bufferSize = (bufferSize > S)? S : bufferSize;

    // Update sum
    sum += value - oldestValue;
  }

  float getMovingAvg() {
    if (bufferSize == 0) {
      return 0.0;
    }
    return sum / static_cast<float>(bufferSize);
  }

private:
  T circularBuffer[S];
  int writeIndex;
  int bufferSize;
  ST sum;
};

#endif // MOVING_AVG_H_
