import time
import statistics
from appSend import send_payload
from appRecieve import receive_payload

def run_benchmark(num_messages=100):
    round_trip_times = []
    total_start_time = time.perf_counter()

    for i in range(num_messages):
        start_time = time.perf_counter()
        send_payload()
        receive_payload()
        end_time = time.perf_counter()
        round_trip_times.append((end_time - start_time) * 1000)  # Convert to milliseconds

    total_time = (time.perf_counter() - total_start_time) * 1000  # Convert to milliseconds

    print("\nBenchmark Results:")
    print(f"Total messages sent: {num_messages}")
    print(f"Total time: {total_time:.4f} ms")
    print(f"Average round-trip time: {statistics.mean(round_trip_times):.4f} ms")
    print(f"Min round-trip time: {min(round_trip_times):.4f} ms")
    print(f"Max round-trip time: {max(round_trip_times):.4f} ms")
    print(f"Throughput: {(num_messages / total_time * 1000):.2f} messages/second")

if __name__ == "__main__":
    run_benchmark() 